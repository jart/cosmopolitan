/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/blocksigs.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/extend.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"

static char *mapend;
static size_t maptotal;

static void *__zipos_mmap_space(size_t mapsize) {
  char *start;
  size_t offset;
  _unassert(mapsize);
  offset = maptotal;
  maptotal += mapsize;
  start = (char *)kMemtrackZiposStart;
  if (!mapend) mapend = start;
  mapend = _extend(start, maptotal, mapend, MAP_PRIVATE,
                   kMemtrackZiposStart + kMemtrackZiposSize);
  return start + offset;
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
      h->next = 0;
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
    h->mapsize = mapsize;
    pthread_mutex_init(&h->lock, 0);
  }
  return h;
}

static int __zipos_mkfd(int minfd) {
  int e, fd;
  static bool demodernize;
  if (!demodernize) {
    e = errno;
    if ((fd = __sys_fcntl(2, F_DUPFD_CLOEXEC, minfd)) != -1) {
      return fd;
    } else if (errno == EINVAL) {
      demodernize = true;
      errno = e;
    } else {
      return fd;
    }
  }
  if ((fd = __sys_fcntl(2, F_DUPFD, minfd)) != -1) {
    __sys_fcntl(fd, F_SETFD, FD_CLOEXEC);
  }
  return fd;
}

static int __zipos_setfd(int fd, struct ZiposHandle *h, unsigned flags,
                         int mode) {
  int want = fd;
  atomic_compare_exchange_strong_explicit(
      &g_fds.f, &want, fd + 1, memory_order_release, memory_order_relaxed);
  g_fds.p[fd].kind = kFdZip;
  g_fds.p[fd].handle = (intptr_t)h;
  g_fds.p[fd].flags = flags | O_CLOEXEC;
  g_fds.p[fd].mode = mode;
  g_fds.p[fd].extra = 0;
  __fds_unlock();
  return fd;
}

static int __zipos_load(struct Zipos *zipos, size_t cf, unsigned flags,
                        int mode) {
  size_t lf;
  size_t size;
  int rc, fd, minfd;
  struct ZiposHandle *h;
  lf = GetZipCfileOffset(zipos->map + cf);
  _npassert((ZIP_LFILE_MAGIC(zipos->map + lf) == kZipLfileHdrMagic));
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
  h->pos = 0;
  h->cfile = cf;
  h->size = size;
  if (!IsTiny() && h->mem &&
      crc32_z(0, h->mem, h->size) != ZIP_LFILE_CRC32(zipos->map + lf)) {
    h->mem = 0;
    eio();
  }
  if (h->mem) {
    minfd = 3;
    __fds_lock();
  TryAgain:
    if (IsWindows() || IsMetal()) {
      if ((fd = __reservefd_unlocked(-1)) != -1) {
        return __zipos_setfd(fd, h, flags, mode);
      }
    } else if ((fd = __zipos_mkfd(minfd)) != -1) {
      if (__ensurefds_unlocked(fd) != -1) {
        if (g_fds.p[fd].kind) {
          sys_close(fd);
          minfd = fd + 1;
          goto TryAgain;
        }
        return __zipos_setfd(fd, h, flags, mode);
      }
      sys_close(fd);
    }
    __fds_unlock();
  }
  __zipos_free(zipos, h);
  return -1;
}

/**
 * Loads compressed file from αcτµαlly pδrταblε εxεcµταblε object store.
 *
 * @param uri is obtained via __zipos_parseuri()
 * @asyncsignalsafe
 * @threadsafe
 */
int __zipos_open(const struct ZiposUri *name, unsigned flags, int mode) {
  int rc;
  ssize_t cf;
  struct Zipos *zipos;
  if (_weaken(pthread_testcancel_np) &&
      (rc = _weaken(pthread_testcancel_np)())) {
    errno = rc;
    return -1;
  }
  BLOCK_SIGNALS;
  if ((flags & O_ACCMODE) == O_RDONLY) {
    if ((zipos = __zipos_get())) {
      if ((cf = __zipos_find(zipos, name)) != -1) {
        rc = __zipos_load(zipos, cf, flags, mode);
      } else {
        rc = enoent();
      }
    } else {
      rc = enoexec();
    }
  } else {
    rc = einval();
  }
  ALLOW_SIGNALS;
  return rc;
}
