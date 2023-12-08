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
#include "libc/macros.internal.h"
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
          bzero((*t)[y % 3], sizeof(short) * xn);
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
