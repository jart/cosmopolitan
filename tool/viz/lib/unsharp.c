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
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "tool/viz/lib/convolution.h"
#include "tool/viz/lib/graphic.h"

/**
 * Enhances image detail.
 * @see https://en.wikipedia.org/wiki/Kernel_(image_processing)
 */
long unsharp(long cn, long yw, long xw, unsigned char img[cn][yw][xw], long yn,
             long xn) {
  long rc, c, y, x;
  unsigned *iy, *ix;
  short(*t)[3][xn] = xmemalign(64, sizeof(short) * xn * 3);
  iy = convoindex(2, yn, 2);
  ix = convoindex(2, xn, 2);
  if (t && ix && iy) {
    iy += 2;
    ix += 2;
    for (c = 0; c < 3; ++c) {
      for (y = 0; y < yn + 3; ++y) {
        if (y >= 3) {
          for (x = 0; x < xn; ++x) {
            img[c][y - 3][x] = MIN(255, MAX(0, (*t)[y % 3][x]));
          }
          memset((*t)[y % 3], 0, sizeof(short) * xn);
        }
        if (y < yn) {
          for (x = 0; x < xn; ++x) {
            (*t)[y % 3][x] = CONVOLVE5X5(/* clang-format off */
                    7, (-1 / 256.), img[c],
                          1,  4,   6,  4, 1,
                          4, 16,  24, 16, 4,
                          6, 24,-476, 24, 6,
                          4, 16,  24, 16, 4,
                          1,  4,   6,  4, 1
                                         /* clang-format on */);
          }
        }
      }
    }
    rc = 0;
  } else {
    rc = enomem();
  }
  free(t);
  if (ix) free(ix - 2);
  if (iy) free(iy - 2);
  return rc;
}
