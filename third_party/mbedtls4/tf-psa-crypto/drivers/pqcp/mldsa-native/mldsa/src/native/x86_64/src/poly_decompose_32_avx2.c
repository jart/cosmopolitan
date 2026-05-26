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
 *
 * The algorithm for Decompose(r) (more specifically the handling for the
 * wrap-around cases) are modified. See the "Reference" section in the comments
 * below for a more detailed comparison.
 */

#include "../../../common.h"

#if defined(MLD_ARITH_BACKEND_X86_64_DEFAULT) && \
    !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)

#include <immintrin.h>
#include <stdint.h>
#include <string.h>
#include "arith_native_x86_64.h"
#include "consts.h"

/*
 * Reference: The reference implementation has the input polynomial as a
 *            separate argument that may be aliased with either of the outputs.
 *            Removing the aliasing eases CBMC proofs.
 */
void mld_poly_decompose_32_avx2(int32_t *a1, int32_t *a0)
{
  unsigned int i;
  __m256i f, f0, f1, t;
  const __m256i q_bound = _mm256_set1_epi32(31 * ((MLDSA_Q - 1) / 32));
  /* check-magic: 1025 == floor(2**22 / 4092) */
  const __m256i v = _mm256_set1_epi32(1025);
  const __m256i alpha = _mm256_set1_epi32(2 * ((MLDSA_Q - 1) / 32));
  const __m256i off = _mm256_set1_epi32(127);
  const __m256i shift = _mm256_set1_epi32(512);

  for (i = 0; i < MLDSA_N / 8; i++)
  {
    f = _mm256_load_si256((__m256i *)&a0[8 * i]);

    /* check-magic: 4092 == intdiv(2 * intdiv(MLDSA_Q - 1, 32), 128) */
    /*
     * Compute f1 = round-(f / (2*GAMMA2)) as round-(f / (128B)) =
     * round-(ceil(f / 128) / B) where B = 2*GAMMA2 / 128 = 4092. See
     * mld_decompose() in mldsa/src/rounding.h for more details.
     *
     * range: 0 <= f <= Q-1 = 32*GAMMA2 = 16*128*B
     */

    /* Compute f1' = ceil(f / 128) as floor((f + 127) >> 7) */
    f1 = _mm256_add_epi32(f, off);
    f1 = _mm256_srli_epi32(f1, 7);
    /*
     * range: 0 <= f1' <= (Q-1)/128 = 16B
     *
     * Also, f1' <= (Q-1)/128 = 2^16 - 2^6 < 2^16 ensures that the odd-index
     * 16-bit lanes are all 0, so no bits will be dropped in the input of the
     * _mm256_mulhi_epu16() below.
     */

    /*
     * Compute f1 = round-(f1' / B) ≈ round(f1' * 1025 / 2^22). This is exact
     * for 0 <= f1' < 2^16. Note that half is rounded down since 1025 / 2^22 ≲
     * 1 / 4092.
     *
     * round(f1' * 1025 / 2^22) is in turn computed in 2 steps as
     * round(floor(f1' * 1025 / 2^16) / 2^6). The mulhi computes f1'' =
     * floor(f1' * 1025 / 2^16). As for the next step f1 = round(f1'' / 2^6),
     * because AVX2 doesn't have rounding right-shift (e.g. urshr in Neon), we
     * simulate it using mulhrs with a power of 2, in this case mulhrs(f1'',
     * 2^9) = round(f1'' * 2^9 / 2^15). (Note that the denominator is 2^15,
     * not 2^16 as in mulhi.)
     */
    f1 = _mm256_mulhi_epu16(f1, v);
    /*
     * range: 0 <= f1'' < floor(2^16 * 1025 / 2^16) = 1025
     *
     * Because 0 <= f1'' < 2^15, the multiplication in mulhrs is unsigned, that
     * is, no erroneous sign-extension occurs.
     */
    f1 = _mm256_mulhrs_epi16(f1, shift);
    /*
     * range: 0 <= f1 = round-(f1' / B) <= round-(16B / B) = 16
     *
     * Note that the odd-index 16-bit lanes are still all 0 right now, so
     * reinterpreting f1 as 8 lanes of int32_t (as done in the following) does
     * not affect its value.
     */

    /*
     * If f1 = 16, i.e. f > 31*GAMMA2, proceed as if f' = f - Q was given
     * instead. (For f = 31*GAMMA2 + 1 thus f' = -GAMMA2, we still round it to 0
     * like other "wrapped around" cases.)
     *
     * Reference: They handle wrap-around in a somewhat convoluted way. Most
     *            notably, they compute remainder f0 with quotient f1 that's
     *            already wrapped around, so is off by q (instead of by 1) from
     *            what it should be ultimately. They detect the need for
     *            correction by checking if f0 is abnormally large.
     *
     *            Our approach is closer to Algorithm 36 in the specification,
     *            in that we compute f0 normally and correct f1, f0 in the way
     *            they prescribed. The only real difference is that we check for
     *            wrap-around by examining f directly, instead of some other
     *            intermediates computed from it.
     */

    /* Check for wrap-around */
    t = _mm256_cmpgt_epi32(f, q_bound);

    /* Compute remainder f0 */
    f0 = _mm256_mullo_epi32(f1, alpha);
    f0 = _mm256_sub_epi32(f, f0);
    /*
     * range: -GAMMA2 < f0 <= GAMMA2
     *
     * This holds since f1 = round-(f / (2*GAMMA2)) was computed exactly.
     */

    /* If wrap-around is required, set f1 = 0 and f0 -= 1 */
    f1 = _mm256_andnot_si256(t, f1);
    f0 = _mm256_add_epi32(f0, t);
    /* range: 0 <= f1 <= 15, -GAMMA2 <= f0 <= GAMMA2 */

    _mm256_store_si256((__m256i *)&a1[8 * i], f1);
    _mm256_store_si256((__m256i *)&a0[8 * i], f0);
  }
}

#else /* MLD_ARITH_BACKEND_X86_64_DEFAULT && !MLD_CONFIG_MULTILEVEL_NO_SHARED \
       */

MLD_EMPTY_CU(avx2_poly_decompose_32)

#endif /* !(MLD_ARITH_BACKEND_X86_64_DEFAULT && \
          !MLD_CONFIG_MULTILEVEL_NO_SHARED) */
