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
#include "libc/bits/xmmintrin.internal.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "tool/viz/lib/convolution.h"
#include "tool/viz/lib/graphic.h"

/**
 * Blurs image.
 * @see https://en.wikipedia.org/wiki/Kernel_(image_processing)
 */
long gaussian(long yn, long xn, unsigned char img[3][yn][xn]) {
#if 0
  long rc, c, y, x, b;
  unsigned *iy, *ix;
  unsigned char(*ta)[3][xn];
  iy = convoindex(2, yn, 2);
  ix = convoindex(2, xn, 2);
  ta = memalign(32, xn * 3);
  if (ta && ix && iy) {
    iy += 2;
    ix += 2;
    for (c = 0; c < 3; ++c) {
      for (y = 0; y < yn + 3; ++y) {
        if (y >= 3) memcpy(img[c][y - 3], (*ta)[y % 3], xn);
        if (y < yn) {
          for (x = 0; x < xn; ++x) {
            b = CONVOLVE5X5(/* clang-format off */
                            15, (1 / 256.), img[c],
                            1,  4,  6,  4, 1,
                            4, 16, 24, 16, 4,
                            6, 24, 36, 24, 6,
                            4, 16, 24, 16, 4,
                            1,  4,  6,  4, 1
                            /* clang-format on */);
            (*ta)[y % 3][x] = MIN(255, MAX(0, b));
          }
        }
      }
    }
    rc = 0;
  } else {
    rc = enomem();
  }
  free(ta);
  if (ix) free(ix - 2);
  if (iy) free(iy - 2);
  return rc;
#endif
  return 0;
}
