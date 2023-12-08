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
#include "libc/assert.h"
#include "libc/intrin/packuswb.h"
#include "libc/intrin/paddw.h"
#include "libc/intrin/palignr.h"
#include "libc/intrin/pmaddubsw.h"
#include "libc/intrin/psraw.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"

#define TAPS       8
#define RATIO      2
#define OFFSET     3
#define STRIDE     8
#define SPREAD     (STRIDE * RATIO + TAPS - OFFSET)
#define OVERLAP    (SPREAD - STRIDE * RATIO)
#define LOOKBEHIND OFFSET
#define LOOKAHEAD  (SPREAD - LOOKBEHIND)
#define SCALE      5
#define ROUND      (1 << (SCALE - 1))

/**
 * Performs 2D Motion Picture Convolution Acceleration by Leveraging SSSE3.
 *
 * @note H/T John Costella, Jean-Baptiste Joseph Fourier
 * @note RIP Huixiang Chen
 */
void *cDecimate2xUint8x8(unsigned long n, unsigned char A[n],
                         const signed char K[8]) {
  short kRound[8] = {ROUND, ROUND, ROUND, ROUND, ROUND, ROUND, ROUND, ROUND};
  signed char kMadd1[16] = {K[0], K[1], K[0], K[1], K[0], K[1], K[0], K[1],
                            K[0], K[1], K[0], K[1], K[0], K[1], K[0], K[1]};
  signed char kMadd2[16] = {K[2], K[3], K[2], K[3], K[2], K[3], K[2], K[3],
                            K[2], K[3], K[2], K[3], K[2], K[3], K[2], K[3]};
  signed char kMadd3[16] = {K[4], K[5], K[4], K[5], K[4], K[5], K[4], K[5],
                            K[4], K[5], K[4], K[5], K[4], K[5], K[4], K[5]};
  signed char kMadd4[16] = {K[6], K[7], K[6], K[7], K[6], K[7], K[6], K[7],
                            K[6], K[7], K[6], K[7], K[6], K[7], K[6], K[7]};
  unsigned char bv0[16], bv1[16], bv2[16], bv3[16];
  unsigned char in1[16], in2[16], in3[16];
  short wv0[8], wv1[8], wv2[8], wv3[8];
  unsigned long i, j, w;
  if (n >= STRIDE) {
    i = 0;
    w = (n + RATIO / 2) / RATIO;
    memset(in1, A[0], sizeof(in1));
    memset(in2, A[n - 1], 16);
    memcpy(in2, A, MIN(16, n));
    for (; i < w; i += STRIDE) {
      j = i * RATIO + 16;
      if (j + 16 <= n) {
        memcpy(in3, &A[j], 16);
      } else {
        memset(in3, A[n - 1], 16);
        if (j < n) {
          memcpy(in3, &A[j], n - j);
        }
      }
      palignr(bv0, in2, in1, 13);
      palignr(bv1, in2, in1, 15);
      palignr(bv2, in3, in2, 1);
      palignr(bv3, in3, in2, 3);
      pmaddubsw(wv0, bv0, kMadd1);
      pmaddubsw(wv1, bv1, kMadd2);
      pmaddubsw(wv2, bv2, kMadd3);
      pmaddubsw(wv3, bv3, kMadd4);
      paddw(wv0, wv0, kRound);
      paddw(wv0, wv0, wv1);
      paddw(wv0, wv0, wv2);
      paddw(wv0, wv0, wv3);
      psraw(wv0, wv0, SCALE);
      packuswb(bv2, wv0, wv0);
      memcpy(&A[i], bv2, STRIDE);
      memcpy(in1, in2, 16);
      memcpy(in2, in3, 16);
    }
  }
  return A;
}
