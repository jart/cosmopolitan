/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/* References
 * ==========
 *
 * - [REF_AVX2]
 *   CRYSTALS-Dilithium optimized AVX2 implementation
 *   Bai, Ducas, Kiltz, Lepoint, Lyubashevsky, Schwabe, Seiler, Stehlé
 *   https://github.com/pq-crystals/dilithium/tree/master/avx2
 */

/*
 * This file is derived from the public domain
 * AVX2 Dilithium implementation @[REF_AVX2].
 */

#include "../../../common.h"

#if defined(MLD_ARITH_BACKEND_X86_64_DEFAULT) && \
    !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)

#include <immintrin.h>
#include <stdint.h>
#include "arith_native_x86_64.h"

void mld_polyz_unpack_19_avx2(int32_t *r, const uint8_t *a)
{
  unsigned int i;
  __m256i f;
  __m128i low, high;

  const __m256i shufbidx = _mm256_set_epi8(
      -1, 31, 30, 29, -1, 29, 28, 27, -1, 26, 25, 24, -1, 24, 23, 22, -1, 9, 8,
      7, -1, 7, 6, 5, -1, 4, 3, 2, -1, 2, 1, 0);
  /* Equivalent to _mm256_set_epi32(4, 0, 4, 0, 4, 0, 4, 0) */
  const __m256i srlvdidx = _mm256_set1_epi64x((uint64_t)4 << 32);
  const __m256i mask = _mm256_set1_epi32(0xFFFFF);
  const __m256i gamma1 = _mm256_set1_epi32((1 << 19));

  for (i = 0; i < MLDSA_N / 8; i++)
  {
    /* Load bytes 0..15 into low 128-bit vector */
    low = _mm_loadu_si128((__m128i *)&a[20 * i]);
    /* Load bytes 4..19 into high 128-bit vector */
    high = _mm_loadu_si128((__m128i *)&a[20 * i + 4]);
    /* Combine into 256-bit vector */
    f = _mm256_inserti128_si256(_mm256_castsi128_si256(low), high, 1);

    /* Shuffling 8-bit lanes
     *
     * ┌─ Indices 0-9 into low 128-bit half ───────────────────────────────────┐
     * │ Shuffle: [-1, 9, 8, 7, -1, 7, 6, 5, -1, 4, 3, 2, -1, 2, 1, 0]         │
     * │ Result:  [0, byte9, byte8, byte7, ..., 0, byte2, byte1, byte0]        │
     * └───────────────────────────────────────────────────────────────────────┘
     *
     * ┌─ Indices 16-31 into high 128-bit half ────────────────────────────────┐
     * │ Shuffle: [-1,31, 30, 29, -1,29, 28, 27, -1,26, 25, 24, -1,24, 23, 22] │
     * │ Result:  [0, byte19, byte18, byte17, ..., 0, byte12, byte11, byte10]  │
     * └───────────────────────────────────────────────────────────────────────┘
     */
    f = _mm256_shuffle_epi8(f, shufbidx);

    /* Keep only 20 out of 24 bits in each 32-bit lane */
    /* Bits   0..23     16..39    40..63    56..79
     *        80..103   96..119   120..143  136..159 */
    f = _mm256_srlv_epi32(f, srlvdidx);
    /* Bits   0..23     20..39    40..63    60..79
     *        80..103   100..119  120..143  140..159 */
    f = _mm256_and_si256(f, mask);
    /* Bits   0..19     20..39    40..59    60..79
     *        80..99    100..119  120..139  140..159 */

    /* Map [0, 1, ..., 2^20-1] to [2^19, 2^19-1, ..., -2^19+1] */
    f = _mm256_sub_epi32(gamma1, f);

    _mm256_store_si256((__m256i *)&r[8 * i], f);
  }
}

#else /* MLD_ARITH_BACKEND_X86_64_DEFAULT && !MLD_CONFIG_MULTILEVEL_NO_SHARED \
       */

MLD_EMPTY_CU(avx2_polyz_unpack_19)

#endif /* !(MLD_ARITH_BACKEND_X86_64_DEFAULT && \
          !MLD_CONFIG_MULTILEVEL_NO_SHARED) */
