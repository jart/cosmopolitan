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
#include "libc/intrin/pcmpgtb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/intrin/punpckhbw.h"
#include "libc/intrin/punpcklbw.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"

/**
 * Transcodes UTF-8 to UTF-16.
 *
 * @param dst is output buffer
 * @param dstsize is shorts in dst
 * @param src is NUL-terminated UTF-8 input string
 * @return ax shorts written excluding nul
 * @return dx index of character after nul word in src
 */
axdx_t tprecode8to16(char16_t *dst, size_t dstsize, const char *src) {
  axdx_t r;
  unsigned n;
  uint64_t w;
  wint_t x, y;
  uint8_t v1[16], v2[16], vz[16];
  r.ax = 0;
  r.dx = 0;
  for (;;) {
    if (!IsTiny() && !((uintptr_t)(src + r.dx) & 15)) {
      /* 34x speedup for ascii */
      memset(vz, 0, 16);
      while (r.ax + 16 < dstsize) {
        memcpy(v1, src + r.dx, 16);
        pcmpgtb((int8_t *)v2, (int8_t *)v1, (int8_t *)vz);
        if (pmovmskb(v2) != 0xFFFF) break;
        punpcklbw(v2, v1, vz);
        punpckhbw(v1, v1, vz);
        memcpy(dst + r.ax + 0, v2, 16);
        memcpy(dst + r.ax + 8, v1, 16);
        r.ax += 16;
        r.dx += 16;
      }
    }
    x = src[r.dx++] & 0xff;
    if (ThomPikeCont(x)) continue;
    if (!isascii(x)) {
      n = ThomPikeLen(x);
      x = ThomPikeByte(x);
      while (--n) {
        if ((y = src[r.dx++] & 0xff)) {
          x = ThomPikeMerge(x, y);
        } else {
          x = 0;
          break;
        }
      }
    }
    if (!x) break;
    w = EncodeUtf16(x);
    while (w && r.ax + 1 < dstsize) {
      dst[r.ax++] = w & 0xFFFF;
      w >>= 16;
    }
  }
  if (r.ax < dstsize) {
    dst[r.ax] = 0;
  }
  return r;
}
