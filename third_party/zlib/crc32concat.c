/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 1995-2017 Jean-loup Gailly and Mark Adler                          │
│ Copyright 2017 The Chromium Authors                                          │
│ Use of this source code is governed by the BSD-style licenses that can       │
│ be found in the third_party/zlib/LICENSE file.                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/zlib/deflate.h"
#include "third_party/zlib/internal.h"
#include "third_party/zlib/zutil.h"

asm(".ident\t\"\\n\\n\
zlib (zlib License)\\n\
Copyright 1995-2017 Jean-loup Gailly and Mark Adler\"");
asm(".include \"libc/disclaimer.inc\"");

#define GF2_DIM 32 /* dimension of GF(2) vectors (length of CRC) */

static unsigned long gf2_matrix_times(unsigned long *mat, unsigned long vec) {
  unsigned long sum;
  sum = 0;
  while (vec) {
    if (vec & 1) sum ^= *mat;
    vec >>= 1;
    mat++;
  }
  return sum;
}

static void gf2_matrix_square(unsigned long *square, unsigned long *mat) {
  int n;
  for (n = 0; n < GF2_DIM; n++) {
    square[n] = gf2_matrix_times(mat, mat[n]);
  }
}

uLong crc32_combine(uLong crc1, uLong crc2, int64_t len2) {
  int n;
  unsigned long row;
  unsigned long even[GF2_DIM]; /* even-power-of-two zeros operator */
  unsigned long odd[GF2_DIM];  /* odd-power-of-two zeros operator */

  /* degenerate case (also disallow negative lengths) */
  if (len2 <= 0) return crc1;

  /* put operator for one zero bit in odd */
  odd[0] = 0xedb88320UL; /* CRC-32 polynomial */
  row = 1;
  for (n = 1; n < GF2_DIM; n++) {
    odd[n] = row;
    row <<= 1;
  }

  /* put operator for two zero bits in even */
  gf2_matrix_square(even, odd);

  /* put operator for four zero bits in odd */
  gf2_matrix_square(odd, even);

  /* apply len2 zeros to crc1 (first square will put the operator for one
     zero byte, eight zero bits, in even) */
  do {
    /* apply zeros operator for this bit of len2 */
    gf2_matrix_square(even, odd);
    if (len2 & 1) crc1 = gf2_matrix_times(even, crc1);
    len2 >>= 1;

    /* if no more bits set, then done */
    if (len2 == 0) break;

    /* another iteration of the loop with odd and even swapped */
    gf2_matrix_square(odd, even);
    if (len2 & 1) crc1 = gf2_matrix_times(odd, crc1);
    len2 >>= 1;

    /* if no more bits set, then done */
  } while (len2 != 0);

  /* return combined crc */
  crc1 ^= crc2;
  return crc1;
}
