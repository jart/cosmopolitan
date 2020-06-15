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
#include "dsp/core/ks8.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "tool/viz/lib/convolution.h"
#include "tool/viz/lib/graphic.h"

#define SHARPEN(...) KS8(0, 0, 0, 0, -1, -1, 5, -1, -1, 0, 0, 0, __VA_ARGS__)

long sharpen(long cn, long yw, long xw, unsigned char p[cn][yw][xw], long yn,
             long xn) {
  long rc, c, y, x;
  short(*ta)[3][xn];
  unsigned *iy, *ix;
  if (yn >= 3 && xn > 0) {
    ta = memalign(32, sizeof(short) * xn * 3);
    iy = convoindex(1, yn, 1);
    ix = convoindex(1, xn, 1);
    if (ta && iy && ix) {
      iy += 1;
      ix += 1;
      for (c = 0; c < 3; ++c) {
        for (y = 0; y < yn + 3; ++y) {
          if (y >= 3) {
            for (x = 0; x < xn; ++x) {
              p[c][y - 3][x] = MIN(255, MAX(0, (*ta)[y % 3][x]));
            }
          }
          if (y < yn) {
            for (x = 0; x < xn; ++x) {
              (*ta)[y % 3][x] =
                  SHARPEN(p[c][iy[y - 1]][x], p[c][y][ix[x - 1]], p[c][y][x],
                          p[c][y][ix[x + 1]], p[c][iy[y + 1]][x]);
            }
          }
        }
      }
      rc = 0;
    } else {
      rc = enomem();
    }
    free(ta);
    if (ix) free(ix - 1);
    if (iy) free(iy - 1);
  } else {
    rc = einval();
  }
  return rc;
}
