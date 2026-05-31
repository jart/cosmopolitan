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
#include "arith_native_x86_64.h"
#include "consts.h"

/*************************************************
 * Name:        mld_poly_caddq_avx2
 *
 * Description: For all coefficients of in/out polynomial add Q if
 *              coefficient is negative.
 *
 * Arguments:   - int32_t *r: pointer to input/output polynomial
 **************************************************/
void mld_poly_caddq_avx2(int32_t *r)
{
  unsigned int i;
  __m256i f, g;
  const __m256i q = _mm256_set1_epi32(MLDSA_Q);
  const __m256i zero = _mm256_setzero_si256();
  __m256i *rr = (__m256i *)r;

  for (i = 0; i < MLDSA_N / 8; i++)
  {
    f = _mm256_load_si256(&rr[i]);
    g = _mm256_cmpgt_epi32(zero, f);
    g = _mm256_and_si256(g, q);
    f = _mm256_add_epi32(f, g);
    _mm256_store_si256(&rr[i], f);
  }
}

#else /* MLD_ARITH_BACKEND_X86_64_DEFAULT && !MLD_CONFIG_MULTILEVEL_NO_SHARED \
       */

MLD_EMPTY_CU(avx2_reduce)

#endif /* !(MLD_ARITH_BACKEND_X86_64_DEFAULT && \
          !MLD_CONFIG_MULTILEVEL_NO_SHARED) */
