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
#include "libc/calls/blocksigs.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/extend.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/zip.internal.h"
#include "third_party/zlib/zconf.h"
#include "third_party/zlib/zlib.h"

static bool __zipos_method_is_supported(int method) {
  switch (method) {
    case kZipCompressionNone:
    case kZipCompressionDeflate:
      return true;
    default:
      return false;
  }
}

static void *__zipos_mmap_space(struct Zipos *zipos, size_t mapsize) {
  char *start;
  size_t offset;
  offset = zipos->maptotal;
  zipos->maptotal += mapsize;
  start = (char *)kMemtrackZiposStart;
  if (!zipos->mapend) zipos->mapend = start;
  zipos->mapend = _extend(start, zipos->maptotal, zipos->mapend, MAP_PRIVATE,
                          kMemtrackZiposStart + kMemtrackZiposSize);
  return start + offset;
}

void __zipos_free(struct ZiposHandle *h) {
  if (!h) return;
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
      h->next = 0;
      break;
    }
    ph = &h->next;
  }
  if (!h) {
    h = __zipos_mmap_space(zipos, mapsize);
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

static int __zipos_load_fd(struct Zipos *zipos, int cf, int fd,
                           struct ZiposUri *name,
                           struct ZiposHandle **out_handle) {
  uint64_t size;
  struct ZiposHandle *h;
  if (cf == ZIPOS_SYNTHETIC_DIRECTORY) {
    size = name->len;
    if (!(h = __zipos_alloc(zipos, size + 1))) return -1;
    if (size) memcpy(h->data, name->path, size);
    h->data[size] = 0;
  } else {
    uint8_t lfile[kZipLfileHdrMinSize];
    uint64_t lf = GetZipCfileOffset(zipos->cdir + cf);
    int compressed = ZIP_CFILE_COMPRESSIONMETHOD(zipos->cdir + cf);
    if (!__zipos_method_is_supported(compressed) ||
        pread(fd, lfile, kZipLfileHdrMinSize, lf) != kZipLfileHdrMinSize ||
        ZIP_LFILE_MAGIC(lfile) != kZipLfileHdrMagic) {
      return eio();  // this corruption
    }
    size = GetZipCfileUncompressedSize(zipos->cdir + cf);
    if (!(h = __zipos_alloc(zipos, size))) return -1;
    uint64_t off = lf + ZIP_LFILE_HDRSIZE(lfile);
    if (!compressed) {
      if (pread(fd, h->data, size, off) != size) {
        __zipos_free(h);
        return eio();
      }
    } else {
      struct ZiposHandle *h2;
      uint64_t compsize = GetZipCfileCompressedSize(zipos->cdir + cf);
      if (!(h2 = __zipos_alloc(zipos, compsize))) {
        __zipos_free(h);
        return -1;
      }
      if (pread(fd, h2->data, compsize, off) != compsize ||
          __inflate(h->data, size, h2->data, compsize)) {
        __zipos_free(h2);
        __zipos_free(h);
        return eio();
      }
      __zipos_free(h2);
    }
  }
  h->pos = 0;
  h->cfile = cf;
  h->size = size;
  *out_handle = h;
  return 0;
}

static int __zipos_load(struct Zipos *zipos, int cf, int flags,
                        struct ZiposUri *name) {
  int fd;
  if ((fd = openat(AT_FDCWD, zipos->progpath,
                   O_RDONLY | O_CLOEXEC |
                       (flags & (O_NONBLOCK | O_RANDOM | O_SEQUENTIAL)),
                   0)) != -1) {
    struct ZiposHandle *h = 0;
    if (__zipos_load_fd(zipos, cf, fd, name, &h) != -1) {
      if (!IsWindows() && !IsMetal()) {
        // unix doesn't use the g_fds table by default, so we need to
        // explicitly ensure an entry exists for our new open()d file
        __ensurefds(fd);
      }
      // turn it from a file fd to a zip fd
      // layer the handle on windows so it can be restored on close
      h->handle = g_fds.p[fd].handle;
      g_fds.p[fd].kind = kFdZip;
      g_fds.p[fd].handle = (intptr_t)h;
      g_fds.p[fd].flags &= ~O_CLOEXEC;
      g_fds.p[fd].flags |= flags & O_CLOEXEC;
      return fd;
    } else {
      close(fd);
      return -1;
    }
  } else {
    return -1;
  }
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
  if ((flags & ~(O_CLOEXEC | O_NONBLOCK | O_NOFOLLOW | O_NOCTTY | O_DIRECTORY |
                 O_NOATIME | O_RANDOM | O_SEQUENTIAL))) {
    return einval();
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
  int cf;
  if ((cf = __zipos_find(zipos, name)) == -1) {
    return -1;
  }
  if (cf != ZIPOS_SYNTHETIC_DIRECTORY) {
    int mode = GetZipCfileMode(zipos->cdir + cf);
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
