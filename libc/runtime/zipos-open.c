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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/intrin/fds.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"
#include "libc/zip.h"

struct ZiposHandle *__zipos_keep(struct ZiposHandle *h) {
  atomic_fetch_add_explicit(&h->refs, 1, memory_order_relaxed);
  return h;
}

void __zipos_drop(struct ZiposHandle *h) {
  if (atomic_fetch_sub_explicit(&h->refs, 1, memory_order_release))
    return;
  atomic_thread_fence(memory_order_acquire);
  munmap((char *)h, h->mapsize);
}

static struct ZiposHandle *__zipos_alloc(struct Zipos *zipos, size_t size) {
  size_t mapsize;
  struct ZiposHandle *h;
  mapsize = sizeof(struct ZiposHandle) + size;
  if ((h = mmap(0, mapsize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
                -1, 0)) != MAP_FAILED) {
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
    if (!(h = __zipos_alloc(zipos, size + 1)))
      return -1;
    if (size)
      memcpy(h->data, name->path, size);
    h->data[size] = 0;
    h->mem = h->data;
  } else {
    lf = GetZipCfileOffset(zipos->map + cf);
    size = GetZipLfileUncompressedSize(zipos->map + lf);
    switch (ZIP_LFILE_COMPRESSIONMETHOD(zipos->map + lf)) {
      case kZipCompressionNone:
        if (!(h = __zipos_alloc(zipos, 0)))
          return -1;
        h->mem = ZIP_LFILE_CONTENT(zipos->map + lf);
        break;
      case kZipCompressionDeflate:
        if (!(h = __zipos_alloc(zipos, size)))
          return -1;
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
  h->size = size;
  if (h->mem) {
    minfd = 3;
    __fds_lock();
  TryAgain:
    if (IsWindows() || IsMetal()) {
      if ((fd = __reservefd_unlocked(-1)) != -1)
        return __zipos_setfd(fd, h, flags);
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
  if (oldfd == newfd)
    return;
  BLOCK_SIGNALS;
  BLOCK_CANCELATION;
  __fds_lock();
  if (__isfdkind(newfd, kFdZip)) {
    __zipos_drop((struct ZiposHandle *)(intptr_t)g_fds.p[newfd].handle);
    if (!__isfdkind(oldfd, kFdZip))
      bzero(g_fds.p + newfd, sizeof(*g_fds.p));
  }
  if (__isfdkind(oldfd, kFdZip)) {
    __zipos_keep((struct ZiposHandle *)(intptr_t)g_fds.p[oldfd].handle);
    __ensurefds_unlocked(newfd);
    g_fds.p[newfd] = g_fds.p[oldfd];
  }
  __fds_unlock();
  ALLOW_CANCELATION;
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
      (flags & O_ACCMODE) != O_RDONLY)
    return erofs();

  // get the zipos global singleton
  struct Zipos *zipos;
  if (!(zipos = __zipos_get()))
    return enoexec();

  // most open() calls are due to languages path searching assets. the
  // majority of these calls will return ENOENT or ENOTDIR. we need to
  // perform two extremely costly sigprocmask() calls below. thanks to
  // zipos being a read-only filesystem, we can avoid it in many cases
  ssize_t cf;
  if ((cf = __zipos_find(zipos, name)) == -1)
    return -1;
  if (flags & O_EXCL)
    return eexist();
  if (cf != ZIPOS_SYNTHETIC_DIRECTORY) {
    int mode = GetZipCfileMode(zipos->map + cf);
    if ((flags & O_DIRECTORY) && !S_ISDIR(mode))
      return enotdir();
    if (!(mode & 0444))
      return eacces();
  }

  // now do the heavy lifting
  BLOCK_SIGNALS;
  BLOCK_CANCELATION;
  rc = __zipos_load(zipos, cf, flags, name);
  ALLOW_CANCELATION;
  ALLOW_SIGNALS;
  return rc;
}
