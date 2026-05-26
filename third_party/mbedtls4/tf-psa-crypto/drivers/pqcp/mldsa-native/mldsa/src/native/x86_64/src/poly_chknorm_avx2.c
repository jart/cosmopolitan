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

int mld_poly_chknorm_avx2(const int32_t *a, int32_t B)
{
  unsigned int i;
  __m256i f, t;
  const __m256i bound = _mm256_set1_epi32(B - 1);

  t = _mm256_setzero_si256();
  for (i = 0; i < MLDSA_N / 8; i++)
  {
    f = _mm256_load_si256((const __m256i *)&a[8 * i]);
    f = _mm256_abs_epi32(f);
    f = _mm256_cmpgt_epi32(f, bound);
    t = _mm256_or_si256(t, f);
  }

  return 1 - _mm256_testz_si256(t, t);
}

#else /* MLD_ARITH_BACKEND_X86_64_DEFAULT && !MLD_CONFIG_MULTILEVEL_NO_SHARED \
       */

MLD_EMPTY_CU(avx2_poly_chknorm)

#endif /* !(MLD_ARITH_BACKEND_X86_64_DEFAULT && \
          !MLD_CONFIG_MULTILEVEL_NO_SHARED) */
