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
#include "libc/macros.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "third_party/intel/immintrin.internal.h"

/**
 * Performs 2D Motion Picture Convolution Acceleration by Leveraging SSSE3.
 *
 * @note H/T John Costella, Jean-Baptiste Joseph Fourier
 * @note RIP Huixiang Chen
 */
void *cDecimate2xUint8x8(unsigned long n, unsigned char A[n],
                         const signed char K[8]) {
#ifdef __x86_64__
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
  __m128i kRound = _mm_set1_epi16(ROUND);
  __m128i kMadd1 = _mm_set_epi8(K[1], K[0], K[1], K[0], K[1], K[0], K[1], K[0],
                                K[1], K[0], K[1], K[0], K[1], K[0], K[1], K[0]);
  __m128i kMadd2 = _mm_set_epi8(K[3], K[2], K[3], K[2], K[3], K[2], K[3], K[2],
                                K[3], K[2], K[3], K[2], K[3], K[2], K[3], K[2]);
  __m128i kMadd3 = _mm_set_epi8(K[5], K[4], K[5], K[4], K[5], K[4], K[5], K[4],
                                K[5], K[4], K[5], K[4], K[5], K[4], K[5], K[4]);
  __m128i kMadd4 = _mm_set_epi8(K[7], K[6], K[7], K[6], K[7], K[6], K[7], K[6],
                                K[7], K[6], K[7], K[6], K[7], K[6], K[7], K[6]);
  __m128i bv0, bv1, bv2, bv3;
  __m128i in1, in2, in3;
  __m128i wv0, wv1, wv2, wv3;
  unsigned long i, j, w;
  if (n >= STRIDE) {
    i = 0;
    w = (n + RATIO / 2) / RATIO;
    in1 = _mm_set1_epi8(A[0]);
    in2 = _mm_set1_epi8(A[n - 1]);
    _mm_storeu_si128((__m128i *)&in2, _mm_loadu_si128((__m128i *)A));
    for (; i < w; i += STRIDE) {
      j = i * RATIO + 16;
      if (j + 16 <= n) {
        in3 = _mm_loadu_si128((__m128i *)&A[j]);
      } else {
        in3 = _mm_set1_epi8(A[n - 1]);
        if (j < n) {
          // SSSE3-compatible way to handle partial loads
          __m128i mask = _mm_loadu_si128((__m128i *)&A[j]);
          __m128i shuffle_mask = _mm_set_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7,
                                              6, 5, 4, 3, 2, 1, 0);
          __m128i index = _mm_set1_epi8(n - j);
          __m128i cmp = _mm_cmplt_epi8(shuffle_mask, index);
          in3 = _mm_or_si128(_mm_and_si128(cmp, mask),
                             _mm_andnot_si128(cmp, in3));
        }
      }
      bv0 = _mm_alignr_epi8(in2, in1, 13);
      bv1 = _mm_alignr_epi8(in2, in1, 15);
      bv2 = _mm_alignr_epi8(in3, in2, 1);
      bv3 = _mm_alignr_epi8(in3, in2, 3);
      wv0 = _mm_maddubs_epi16(bv0, kMadd1);
      wv1 = _mm_maddubs_epi16(bv1, kMadd2);
      wv2 = _mm_maddubs_epi16(bv2, kMadd3);
      wv3 = _mm_maddubs_epi16(bv3, kMadd4);
      wv0 = _mm_add_epi16(wv0, kRound);
      wv0 = _mm_add_epi16(wv0, wv1);
      wv0 = _mm_add_epi16(wv0, wv2);
      wv0 = _mm_add_epi16(wv0, wv3);
      wv0 = _mm_srai_epi16(wv0, SCALE);
      bv2 = _mm_packus_epi16(wv0, wv0);
      _mm_storel_epi64((__m128i *)&A[i], bv2);
      in1 = in2;
      in2 = in3;
    }
  }
  return A;
#else
  long h, i;
  if (n < 2)
    return A;
  unsigned char M[3 + n + 4];
  unsigned char *q = M;
  q[0] = A[0];
  q[1] = A[0];
  q[2] = A[0];
  memcpy(q + 3, A, n);
  q[3 + n + 0] = A[n - 1];
  q[3 + n + 1] = A[n - 1];
  q[3 + n + 2] = A[n - 1];
  q[3 + n + 3] = A[n - 1];
  q += 3;
  h = (n + 1) >> 1;
  for (i = 0; i < h; ++i) {
    short x0, x1, x2, x3, x4, x5, x6, x7;
    x0 = q[i * 2 - 3];
    x1 = q[i * 2 - 2];
    x2 = q[i * 2 - 1];
    x3 = q[i * 2 + 0];
    x4 = q[i * 2 + 1];
    x5 = q[i * 2 + 2];
    x6 = q[i * 2 + 3];
    x7 = q[i * 2 + 4];
    x0 *= K[0];
    x1 *= K[1];
    x2 *= K[2];
    x3 *= K[3];
    x4 *= K[4];
    x5 *= K[5];
    x6 *= K[6];
    x7 *= K[7];
    x0 += x1;
    x2 += x3;
    x4 += x5;
    x6 += x7;
    x0 += x2;
    x4 += x6;
    x0 += x4;
    x0 += 1 << 4;
    x0 >>= 5;
    A[i] = MIN(255, MAX(0, x0));
  }
  return A;
#endif
}
