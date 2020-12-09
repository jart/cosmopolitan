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
#include "libc/fmt/conv.h"
#include "libc/intrin/packsswb.h"
#include "libc/intrin/pandn.h"
#include "libc/intrin/pcmpgtw.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/str/utf16.h"

static const int16_t kDel16[8] = {127, 127, 127, 127, 127, 127, 127, 127};

/**
 * Transcodes UTF-16 to UTF-8.
 *
 * @param dst is output buffer
 * @param dstsize is bytes in dst
 * @param src is NUL-terminated UTF-16 input string
 * @return ax bytes written excluding nul
 * @return dx index of character after nul word in src
 */
axdx_t tprecode16to8(char *dst, size_t dstsize, const char16_t *src) {
  axdx_t r;
  uint64_t w;
  wint_t x, y;
  int16_t v1[8], v2[8], v3[8], vz[8];
  r.ax = 0;
  r.dx = 0;
  for (;;) {
    if (!IsTiny() && !((uintptr_t)(src + r.dx) & 15)) {
      /* 10x speedup for ascii */
      memset(vz, 0, 16);
      while (r.ax + 8 < dstsize) {
        memcpy(v1, src + r.dx, 16);
        pcmpgtw(v2, v1, vz);
        pcmpgtw(v3, v1, kDel16);
        pandn((void *)v2, (void *)v3, (void *)v2);
        if (pmovmskb((void *)v2) != 0xFFFF) break;
        packsswb((void *)v1, v1, v1);
        memcpy(dst + r.ax, v1, 8);
        r.ax += 8;
        r.dx += 8;
      }
    }
    if (!(x = src[r.dx++])) break;
    if (IsUtf16Cont(x)) continue;
    if (!IsUcs2(x)) {
      if (!(y = src[r.dx++])) break;
      x = MergeUtf16(x, y);
    }
    w = tpenc(x);
    while (w && r.ax + 1 < dstsize) {
      dst[r.ax++] = w & 0xFF;
      w >>= 8;
    }
  }
  if (r.ax < dstsize) {
    dst[r.ax] = 0;
  }
  return r;
}
