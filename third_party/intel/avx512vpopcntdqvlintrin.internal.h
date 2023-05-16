#if !defined _IMMINTRIN_H_INCLUDED
#error \
    "Never use <avx512vpopcntdqvlintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef _AVX512VPOPCNTDQVLINTRIN_H_INCLUDED
#define _AVX512VPOPCNTDQVLINTRIN_H_INCLUDED

#if !defined(__AVX512VPOPCNTDQ__) || !defined(__AVX512VL__)
#pragma GCC push_options
#pragma GCC target("avx512vpopcntdq,avx512vl")
#define __DISABLE_AVX512VPOPCNTDQVL__
#endif /* __AVX512VPOPCNTDQVL__ */

__funline __m128i _mm_popcnt_epi32(__m128i __A) {
  return (__m128i)__builtin_ia32_vpopcountd_v4si((__v4si)__A);
}

__funline __m128i _mm_mask_popcnt_epi32(__m128i __A, __mmask16 __U, __m128i __B) {
  return (__m128i)__builtin_ia32_vpopcountd_v4si_mask((__v4si)__A, (__v4si)__B,
                                                      (__mmask16)__U);
}

__funline __m128i _mm_maskz_popcnt_epi32(__mmask16 __U, __m128i __A) {
  return (__m128i)__builtin_ia32_vpopcountd_v4si_mask(
      (__v4si)__A, (__v4si)_mm_setzero_si128(), (__mmask16)__U);
}

__funline __m256i _mm256_popcnt_epi32(__m256i __A) {
  return (__m256i)__builtin_ia32_vpopcountd_v8si((__v8si)__A);
}

__funline __m256i _mm256_mask_popcnt_epi32(__m256i __A, __mmask16 __U,
                                         __m256i __B) {
  return (__m256i)__builtin_ia32_vpopcountd_v8si_mask((__v8si)__A, (__v8si)__B,
                                                      (__mmask16)__U);
}

__funline __m256i _mm256_maskz_popcnt_epi32(__mmask16 __U, __m256i __A) {
  return (__m256i)__builtin_ia32_vpopcountd_v8si_mask(
      (__v8si)__A, (__v8si)_mm256_setzero_si256(), (__mmask16)__U);
}

__funline __m128i _mm_popcnt_epi64(__m128i __A) {
  return (__m128i)__builtin_ia32_vpopcountq_v2di((__v2di)__A);
}

__funline __m128i _mm_mask_popcnt_epi64(__m128i __A, __mmask8 __U, __m128i __B) {
  return (__m128i)__builtin_ia32_vpopcountq_v2di_mask((__v2di)__A, (__v2di)__B,
                                                      (__mmask8)__U);
}

__funline __m128i _mm_maskz_popcnt_epi64(__mmask8 __U, __m128i __A) {
  return (__m128i)__builtin_ia32_vpopcountq_v2di_mask(
      (__v2di)__A, (__v2di)_mm_setzero_si128(), (__mmask8)__U);
}

__funline __m256i _mm256_popcnt_epi64(__m256i __A) {
  return (__m256i)__builtin_ia32_vpopcountq_v4di((__v4di)__A);
}

__funline __m256i _mm256_mask_popcnt_epi64(__m256i __A, __mmask8 __U,
                                         __m256i __B) {
  return (__m256i)__builtin_ia32_vpopcountq_v4di_mask((__v4di)__A, (__v4di)__B,
                                                      (__mmask8)__U);
}

__funline __m256i _mm256_maskz_popcnt_epi64(__mmask8 __U, __m256i __A) {
  return (__m256i)__builtin_ia32_vpopcountq_v4di_mask(
      (__v4di)__A, (__v4di)_mm256_setzero_si256(), (__mmask8)__U);
}

#ifdef __DISABLE_AVX512VPOPCNTDQVL__
#undef __DISABLE_AVX512VPOPCNTDQVL__
#pragma GCC pop_options
#endif /* __DISABLE_AVX512VPOPCNTDQVL__ */

#endif /* _AVX512VPOPCNTDQVLINTRIN_H_INCLUDED */
