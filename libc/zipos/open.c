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
#include "ape/relocations.h"
#include "libc/assert.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/undeflate.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"
#include "third_party/zlib/zlib.h"

static int __zipos_inflate_fast(struct ZiposHandle *h, uint8_t *data,
                                size_t size) {
  int rc;
  z_stream zs;
  zs.opaque = &h;
  zs.next_in = data;
  zs.avail_in = size;
  zs.total_in = size;
  zs.next_out = h->freeme;
  zs.avail_out = h->size;
  zs.total_out = h->size;
  zs.zfree = Z_NULL;
  zs.zalloc = Z_NULL;
  if (inflateInit2(&zs, -MAX_WBITS) == Z_OK) {
    switch (inflate(&zs, Z_NO_FLUSH)) {
      case Z_STREAM_END:
        rc = 0;
        break;
      case Z_MEM_ERROR:
        rc = enomem();
        break;
      case Z_DATA_ERROR:
        rc = eio();
        break;
      case Z_NEED_DICT:
        rc = enotsup(); /* TODO(jart): Z_NEED_DICT */
        break;
      default:
        abort();
    }
    inflateEnd(&zs);
  } else {
    rc = enomem();
  }
  return rc;
}

static int __zipos_inflate_tiny(struct ZiposHandle *h, uint8_t *data,
                                size_t size) {
  struct DeflateState ds;
  return undeflate(h->freeme, h->size, data, size, &ds) != -1 ? 0 : eio();
}

static int __zipos_load(struct Zipos *zipos, size_t cf, unsigned flags,
                        int mode) {
  size_t lf;
  int rc, fd;
  size_t size;
  uint8_t *data;
  struct ZiposHandle *h;
  lf = GetZipCfileOffset(zipos->map + cf);
  if (!IsTiny() &&
      (ZIP_LFILE_MAGIC(zipos->map + lf) != kZipLfileHdrMagic ||
       (ZIP_LFILE_COMPRESSIONMETHOD(zipos->map + lf) != kZipCompressionNone &&
        ZIP_LFILE_COMPRESSIONMETHOD(zipos->map + lf) !=
            kZipCompressionDeflate))) {
    return eio();
  }
  if (!(h = calloc(1, sizeof(*h)))) return -1;
  h->cfile = cf;
  h->size = GetZipLfileUncompressedSize(zipos->map + lf);
  if (ZIP_LFILE_COMPRESSIONMETHOD(zipos->map + lf)) {
    if ((h->freeme = malloc(h->size))) {
      data = ZIP_LFILE_CONTENT(zipos->map + lf);
      size = GetZipLfileCompressedSize(zipos->map + lf);
      if (IsTiny()) {
        rc = __zipos_inflate_tiny(h, data, size);
      } else {
        rc = __zipos_inflate_fast(h, data, size);
      }
      if (rc != -1) {
        h->mem = h->freeme;
      }
    }
  } else {
    h->mem = ZIP_LFILE_CONTENT(zipos->map + lf);
  }
  if (!IsTiny() && h->mem &&
      crc32_z(0, h->mem, h->size) != ZIP_LFILE_CRC32(zipos->map + lf)) {
    errno = EIO;
    h->mem = NULL;
  }
  if (h->mem) {
    if ((fd = dup(2)) != -1) {
      if (__ensurefds(fd) != -1) {
        h->handle = g_fds.p[fd].handle;
        g_fds.p[fd].kind = kFdZip;
        g_fds.p[fd].handle = (intptr_t)h;
        g_fds.p[fd].flags = flags | O_CLOEXEC;
        return fd;
      }
      close(fd);
    }
  }
  free(h->freeme);
  free(h);
  return -1;
}

/**
 * Loads compressed file from αcτµαlly pδrταblε εxεcµταblε object store.
 *
 * @param uri is obtained via __zipos_parseuri()
 * @note don't call open() from signal handlers
 */
int __zipos_open(const struct ZiposUri *name, unsigned flags, int mode) {
  int fd;
  ssize_t cf;
  sigset_t oldmask;
  struct Zipos *zipos;
  if ((flags & O_ACCMODE) != O_RDONLY) return einval();
  if ((zipos = __zipos_get())) {
    if ((cf = __zipos_find(zipos, name)) != -1) {
      fd = __zipos_load(zipos, cf, flags, mode);
      ZTRACE("__zipos_open(%.*s)", name->len, name->path);
    } else {
      ZTRACE("__zipos_open(%.*s) enoent", name->len, name->path);
      fd = enoent();
    }
  } else {
    fd = enoexec();
    ZTRACE("__zipos_open(%.*s) enoexec", name->len, name->path);
  }
  return fd;
}
