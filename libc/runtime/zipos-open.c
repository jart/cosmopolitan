/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/extend.internal.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "libc/zip.internal.h"

#define MAX_REFS SSIZE_MAX

static char *__zipos_mapend;
static size_t __zipos_maptotal;
static pthread_mutex_t __zipos_lock_obj;

static void __zipos_wipe(void) {
  pthread_mutex_init(&__zipos_lock_obj, 0);
}

static void __zipos_lock(void) {
  pthread_mutex_lock(&__zipos_lock_obj);
}

static void __zipos_unlock(void) {
  pthread_mutex_unlock(&__zipos_lock_obj);
}

static void *__zipos_mmap_space(size_t mapsize) {
  char *start;
  size_t offset;
  unassert(mapsize);
  offset = __zipos_maptotal;
  __zipos_maptotal += mapsize;
  start = (char *)kMemtrackZiposStart;
  if (!__zipos_mapend) __zipos_mapend = start;
  __zipos_mapend = _extend(start, __zipos_maptotal, __zipos_mapend, MAP_PRIVATE,
                           kMemtrackZiposStart + kMemtrackZiposSize);
  return start + offset;
}

struct ZiposHandle *__zipos_keep(struct ZiposHandle *h) {
  size_t refs = atomic_fetch_add_explicit(&h->refs, 1, memory_order_relaxed);
  unassert(!VERY_UNLIKELY(refs > MAX_REFS));
  return h;
}

void __zipos_drop(struct ZiposHandle *h) {
  if (atomic_fetch_sub_explicit(&h->refs, 1, memory_order_release)) {
    return;
  }
  atomic_thread_fence(memory_order_acquire);
  if (IsAsan()) {
    __asan_poison((char *)h + sizeof(struct ZiposHandle),
                  h->mapsize - sizeof(struct ZiposHandle), kAsanHeapFree);
  }
  __zipos_lock();
  do h->next = h->zipos->freelist;
  while (!_cmpxchg(&h->zipos->freelist, h->next, h));
  __zipos_unlock();
}

static struct ZiposHandle *__zipos_alloc(struct Zipos *zipos, size_t size) {
  size_t mapsize;
  struct ZiposHandle *h, **ph;
  __zipos_lock();
  mapsize = sizeof(struct ZiposHandle) + size;
  mapsize = ROUNDUP(mapsize, 4096);
StartOver:
  ph = &zipos->freelist;
  while ((h = *ph)) {
    if (h->mapsize >= mapsize) {
      if (!_cmpxchg(ph, h, h->next)) goto StartOver;
      atomic_store_explicit(&h->refs, 0, memory_order_relaxed);
      break;
    }
    ph = &h->next;
  }
  if (!h) {
    h = __zipos_mmap_space(mapsize);
  }
  __zipos_unlock();
  if (IsAsan()) {
    __asan_unpoison((char *)h, sizeof(struct ZiposHandle) + size);
    __asan_poison((char *)h + sizeof(struct ZiposHandle) + size,
                  mapsize - (sizeof(struct ZiposHandle) + size),
                  kAsanHeapOverrun);
  }
  if (h) {
    h->size = size;
    h->zipos = zipos;
    h->mapsize = mapsize;
  }
  return h;
}

static int __zipos_mkfd(int minfd) {
  int fd, e = errno;
  if ((fd = __sys_fcntl(2, F_DUPFD_CLOEXEC, minfd)) != -1) {
    return fd;
  } else if (errno == EINVAL) {
    errno = e;
    return __fixupnewfd(__sys_fcntl(2, F_DUPFD, minfd), O_CLOEXEC);
  } else {
    return fd;
  }
}

static int __zipos_setfd(int fd, struct ZiposHandle *h, unsigned flags) {
  int want = fd;
  atomic_compare_exchange_strong_explicit(
      &g_fds.f, &want, fd + 1, memory_order_release, memory_order_relaxed);
  g_fds.p[fd].kind = kFdZip;
  g_fds.p[fd].handle = (intptr_t)h;
  g_fds.p[fd].flags = flags | O_CLOEXEC;
  __fds_unlock();
  return fd;
}

static int __zipos_load(struct Zipos *zipos, size_t cf, int flags,
                        struct ZiposUri *name) {
  size_t lf;
  size_t size;
  int fd, minfd;
  struct ZiposHandle *h;
  if (cf == ZIPOS_SYNTHETIC_DIRECTORY) {
    size = name->len;
    if (!(h = __zipos_alloc(zipos, size + 1))) return -1;
    if (size) memcpy(h->data, name->path, size);
    h->data[size] = 0;
    h->mem = h->data;
  } else {
    lf = GetZipCfileOffset(zipos->map + cf);
    npassert((ZIP_LFILE_MAGIC(zipos->map + lf) == kZipLfileHdrMagic));
    size = GetZipLfileUncompressedSize(zipos->map + lf);
    switch (ZIP_LFILE_COMPRESSIONMETHOD(zipos->map + lf)) {
      case kZipCompressionNone:
        if (!(h = __zipos_alloc(zipos, 0))) return -1;
        h->mem = ZIP_LFILE_CONTENT(zipos->map + lf);
        break;
      case kZipCompressionDeflate:
        if (!(h = __zipos_alloc(zipos, size))) return -1;
        if (!__inflate(h->data, size, ZIP_LFILE_CONTENT(zipos->map + lf),
                       GetZipLfileCompressedSize(zipos->map + lf))) {
          h->mem = h->data;
        } else {
          h->mem = 0;
          eio();
        }
        break;
      default:
        return eio();
    }
  }
  atomic_store_explicit(&h->pos, 0, memory_order_relaxed);
  h->cfile = cf;
  unassert(size < SIZE_MAX);
  h->size = size;
  if (h->mem) {
    minfd = 3;
    __fds_lock();
  TryAgain:
    if (IsWindows() || IsMetal()) {
      if ((fd = __reservefd_unlocked(-1)) != -1) {
        return __zipos_setfd(fd, h, flags);
      }
    } else if ((fd = __zipos_mkfd(minfd)) != -1) {
      if (__ensurefds_unlocked(fd) != -1) {
        if (g_fds.p[fd].kind) {
          sys_close(fd);
          minfd = fd + 1;
          goto TryAgain;
        }
        return __zipos_setfd(fd, h, flags);
      }
      sys_close(fd);
    }
    __fds_unlock();
  }
  __zipos_drop(h);
  return -1;
}

void __zipos_postdup(int oldfd, int newfd) {
  if (oldfd == newfd) {
    return;
  }
  BLOCK_SIGNALS;
  __fds_lock();
  if (__isfdkind(newfd, kFdZip)) {
    __zipos_drop((struct ZiposHandle *)(intptr_t)g_fds.p[newfd].handle);
    if (!__isfdkind(oldfd, kFdZip)) {
      bzero(g_fds.p + newfd, sizeof(*g_fds.p));
    }
  }
  if (__isfdkind(oldfd, kFdZip)) {
    __zipos_keep((struct ZiposHandle *)(intptr_t)g_fds.p[oldfd].handle);
    __ensurefds_unlocked(newfd);
    g_fds.p[newfd] = g_fds.p[oldfd];
  }
  __fds_unlock();
  ALLOW_SIGNALS;
}

/**
 * Loads compressed file from αcτµαlly pδrταblε εxεcµταblε object store.
 *
 * @param uri is obtained via __zipos_parseuri()
 * @asyncsignalsafe
 */
int __zipos_open(struct ZiposUri *name, int flags) {

  // check if this thread is cancelled
  int rc;
  if (_weaken(pthread_testcancel_np) &&
      (rc = _weaken(pthread_testcancel_np)())) {
    errno = rc;
    return -1;
  }

  // validate api usage
  if ((flags & O_CREAT) ||  //
      (flags & O_TRUNC) ||  //
      (flags & O_ACCMODE) != O_RDONLY) {
    return erofs();
  }

  // get the zipos global singleton
  struct Zipos *zipos;
  if (!(zipos = __zipos_get())) {
    return enoexec();
  }

  // most open() calls are due to languages path searching assets. the
  // majority of these calls will return ENOENT or ENOTDIR. we need to
  // perform two extremely costly sigprocmask() calls below. thanks to
  // zipos being a read-only filesystem, we can avoid it in many cases
  ssize_t cf;
  if ((cf = __zipos_find(zipos, name)) == -1) {
    return -1;
  }
  if (flags & O_EXCL) {
    return eexist();
  }
  if (cf != ZIPOS_SYNTHETIC_DIRECTORY) {
    int mode = GetZipCfileMode(zipos->map + cf);
    if ((flags & O_DIRECTORY) && !S_ISDIR(mode)) {
      return enotdir();
    }
    if (!(mode & 0444)) {
      return eacces();
    }
  }

  // now do the heavy lifting
  BLOCK_SIGNALS;
  rc = __zipos_load(zipos, cf, flags, name);
  ALLOW_SIGNALS;
  return rc;
}

__attribute__((__constructor__)) static void __zipos_ctor(void) {
  __zipos_wipe();
  pthread_atfork(__zipos_lock, __zipos_unlock, __zipos_wipe);
}
