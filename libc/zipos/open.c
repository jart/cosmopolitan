/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/relocations.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/conv/sizemultiply.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/rbx.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/str/undeflate.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.h"
#include "third_party/zlib/zlib.h"

static int __zipos_inflate_fast(struct ZiposHandle *h, uint8_t *data,
                                size_t size) {
  int rc;
  z_stream zs;
  zs.opaque = &h;
  zs.next_in = data;
  zs.avail_in = size;
  zs.total_in = size;
  zs.next_out = h->mem;
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
  return undeflate(h->mem, h->size, data, size, &ds) != -1 ? 0 : eio();
}

static int __zipos_load(size_t cf, unsigned flags, int mode) {
  int fd;
  size_t lf;
  struct ZiposHandle *h;
  lf = ZIP_CFILE_OFFSET(&_base[0] + cf);
  assert(ZIP_LFILE_MAGIC(&_base[0] + lf) == kZipLfileHdrMagic);
  assert(ZIP_LFILE_COMPRESSIONMETHOD(&_base[0] + lf) == kZipCompressionNone ||
         ZIP_LFILE_COMPRESSIONMETHOD(&_base[0] + lf) == kZipCompressionDeflate);
  if ((fd = createfd()) == -1) return -1;
  if (!(h = calloc(1, sizeof(*h)))) return -1;
  h->cfile = cf;
  if ((h->size = ZIP_LFILE_UNCOMPRESSEDSIZE(&_base[0] + lf))) {
    if (ZIP_LFILE_COMPRESSIONMETHOD(&_base[0] + lf)) {
      assert(ZIP_LFILE_COMPRESSEDSIZE(&_base[0] + lf));
      h->mapsize = ROUNDUP(h->size + FRAMESIZE, FRAMESIZE);
      if ((h->map = mapanon(h->mapsize)) != MAP_FAILED) {
        h->mem = h->map + FRAMESIZE / 2;
        if ((IsTiny() ? __zipos_inflate_tiny : __zipos_inflate_fast)(
                h, ZIP_LFILE_CONTENT(&_base[0] + lf),
                ZIP_LFILE_COMPRESSEDSIZE(&_base[0] + lf)) == -1) {
          fd = -1;
        }
      } else {
        fd = -1;
      }
    } else {
      h->mem = ZIP_LFILE_CONTENT(&_base[0] + lf);
    }
  }
  if (!IsTiny() && fd != -1 &&
      crc32_z(0, h->mem, h->size) != ZIP_LFILE_CRC32(&_base[0] + lf)) {
    fd = eio();
  }
  if (fd != -1) {
    g_fds.p[fd].kind = kFdZip;
    g_fds.p[fd].handle = (intptr_t)h;
    g_fds.p[fd].flags = flags;
  } else {
    __zipos_close(h);
  }
  return fd;
}

/**
 * Loads compressed file from αcτµαlly pδrταblε εxεcµταblε object store.
 *
 * @param uri is obtained via __zipos_parseuri()
 * @asyncsignalsafe
 */
int __zipos_open(const struct ZiposUri *name, unsigned flags, int mode) {
  int fd;
  ssize_t cf;
  sigset_t oldmask;
  assert((flags & O_ACCMODE) == O_RDONLY);
  sigprocmask(SIG_BLOCK, &kSigsetFull, &oldmask);
  if ((cf = __zipos_find(name)) != -1) {
    fd = __zipos_load(cf, flags, mode);
  } else {
    fd = enoent();
  }
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
  return fd;
}
