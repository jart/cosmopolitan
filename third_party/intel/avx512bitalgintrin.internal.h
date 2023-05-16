#if !defined _IMMINTRIN_H_INCLUDED
#error \
    "Never use <avx512bitalgintrin.h> directly; include <x86intrin.h> instead."
#endif

#ifndef _AVX512BITALGINTRIN_H_INCLUDED
#define _AVX512BITALGINTRIN_H_INCLUDED

#ifndef __AVX512BITALG__
#pragma GCC push_options
#pragma GCC target("avx512bitalg")
#define __DISABLE_AVX512BITALG__
#endif /* __AVX512BITALG__ */

__funline __m512i _mm512_popcnt_epi8(__m512i __A) {
  return (__m512i)__builtin_ia32_vpopcountb_v64qi((__v64qi)__A);
}

__funline __m512i _mm512_popcnt_epi16(__m512i __A) {
  return (__m512i)__builtin_ia32_vpopcountw_v32hi((__v32hi)__A);
}

#ifdef __DISABLE_AVX512BITALG__
#undef __DISABLE_AVX512BITALG__
#pragma GCC pop_options
#endif /* __DISABLE_AVX512BITALG__ */

#if !defined(__AVX512BITALG__) || !defined(__AVX512BW__)
#pragma GCC push_options
#pragma GCC target("avx512bitalg,avx512bw")
#define __DISABLE_AVX512BITALGBW__
#endif /* __AVX512VLBW__ */

__funline __m512i _mm512_mask_popcnt_epi8(__m512i __A, __mmask64 __U,
                                        __m512i __B) {
  return (__m512i)__builtin_ia32_vpopcountb_v64qi_mask(
      (__v64qi)__A, (__v64qi)__B, (__mmask64)__U);
}

__funline __m512i _mm512_maskz_popcnt_epi8(__mmask64 __U, __m512i __A) {
  return (__m512i)__builtin_ia32_vpopcountb_v64qi_mask(
      (__v64qi)__A, (__v64qi)_mm512_setzero_si512(), (__mmask64)__U);
}
__funline __m512i _mm512_mask_popcnt_epi16(__m512i __A, __mmask32 __U,
                                         __m512i __B) {
  return (__m512i)__builtin_ia32_vpopcountw_v32hi_mask(
      (__v32hi)__A, (__v32hi)__B, (__mmask32)__U);
}

__funline __m512i _mm512_maskz_popcnt_epi16(__mmask32 __U, __m512i __A) {
  return (__m512i)__builtin_ia32_vpopcountw_v32hi_mask(
      (__v32hi)__A, (__v32hi)_mm512_setzero_si512(), (__mmask32)__U);
}

__funline __mmask64 _mm512_bitshuffle_epi64_mask(__m512i __A, __m512i __B) {
  return (__mmask64)__builtin_ia32_vpshufbitqmb512_mask(
      (__v64qi)__A, (__v64qi)__B, (__mmask64)-1);
}

__funline __mmask64 _mm512_mask_bitshuffle_epi64_mask(__mmask64 __M, __m512i __A,
                                                    __m512i __B) {
  return (__mmask64)__builtin_ia32_vpshufbitqmb512_mask(
      (__v64qi)__A, (__v64qi)__B, (__mmask64)__M);
}

#ifdef __DISABLE_AVX512BITALGBW__
#undef __DISABLE_AVX512BITALGBW__
#pragma GCC pop_options
#endif /* __DISABLE_AVX512BITALGBW__ */

#if !defined(__AVX512BITALG__) || !defined(__AVX512VL__) || \
    !defined(__AVX512BW__)
#pragma GCC push_options
#pragma GCC target("avx512bitalg,avx512vl,avx512bw")
#define __DISABLE_AVX512BITALGVLBW__
#endif /* __AVX512VLBW__ */

__funline __m256i _mm256_mask_popcnt_epi8(__m256i __A, __mmask32 __U,
                                        __m256i __B) {
  return (__m256i)__builtin_ia32_vpopcountb_v32qi_mask(
      (__v32qi)__A, (__v32qi)__B, (__mmask32)__U);
}

__funline __m256i _mm256_maskz_popcnt_epi8(__mmask32 __U, __m256i __A) {
  return (__m256i)__builtin_ia32_vpopcountb_v32qi_mask(
      (__v32qi)__A, (__v32qi)_mm256_setzero_si256(), (__mmask32)__U);
}

__funline __mmask32 _mm256_bitshuffle_epi64_mask(__m256i __A, __m256i __B) {
  return (__mmask32)__builtin_ia32_vpshufbitqmb256_mask(
      (__v32qi)__A, (__v32qi)__B, (__mmask32)-1);
}

__funline __mmask32 _mm256_mask_bitshuffle_epi64_mask(__mmask32 __M, __m256i __A,
                                                    __m256i __B) {
  return (__mmask32)__builtin_ia32_vpshufbitqmb256_mask(
      (__v32qi)__A, (__v32qi)__B, (__mmask32)__M);
}

#ifdef __DISABLE_AVX512BITALGVLBW__
#undef __DISABLE_AVX512BITALGVLBW__
#pragma GCC pop_options
#endif /* __DISABLE_AVX512BITALGVLBW__ */

#if !defined(__AVX512BITALG__) || !defined(__AVX512VL__)
#pragma GCC push_options
#pragma GCC target("avx512bitalg,avx512vl")
#define __DISABLE_AVX512BITALGVL__
#endif /* __AVX512VLBW__ */

__funline __mmask16 _mm_bitshuffle_epi64_mask(__m128i __A, __m128i __B) {
  return (__mmask16)__builtin_ia32_vpshufbitqmb128_mask(
      (__v16qi)__A, (__v16qi)__B, (__mmask16)-1);
}

__funline __mmask16 _mm_mask_bitshuffle_epi64_mask(__mmask16 __M, __m128i __A,
                                                 __m128i __B) {
  return (__mmask16)__builtin_ia32_vpshufbitqmb128_mask(
      (__v16qi)__A, (__v16qi)__B, (__mmask16)__M);
}

__funline __m256i _mm256_popcnt_epi8(__m256i __A) {
  return (__m256i)__builtin_ia32_vpopcountb_v32qi((__v32qi)__A);
}

__funline __m256i _mm256_popcnt_epi16(__m256i __A) {
  return (__m256i)__builtin_ia32_vpopcountw_v16hi((__v16hi)__A);
}

__funline __m128i _mm_popcnt_epi8(__m128i __A) {
  return (__m128i)__builtin_ia32_vpopcountb_v16qi((__v16qi)__A);
}

__funline __m128i _mm_popcnt_epi16(__m128i __A) {
  return (__m128i)__builtin_ia32_vpopcountw_v8hi((__v8hi)__A);
}

__funline __m256i _mm256_mask_popcnt_epi16(__m256i __A, __mmask16 __U,
                                         __m256i __B) {
  return (__m256i)__builtin_ia32_vpopcountw_v16hi_mask(
      (__v16hi)__A, (__v16hi)__B, (__mmask16)__U);
}

__funline __m256i _mm256_maskz_popcnt_epi16(__mmask16 __U, __m256i __A) {
  return (__m256i)__builtin_ia32_vpopcountw_v16hi_mask(
      (__v16hi)__A, (__v16hi)_mm256_setzero_si256(), (__mmask16)__U);
}

__funline __m128i _mm_mask_popcnt_epi8(__m128i __A, __mmask16 __U, __m128i __B) {
  return (__m128i)__builtin_ia32_vpopcountb_v16qi_mask(
      (__v16qi)__A, (__v16qi)__B, (__mmask16)__U);
}

__funline __m128i _mm_maskz_popcnt_epi8(__mmask16 __U, __m128i __A) {
  return (__m128i)__builtin_ia32_vpopcountb_v16qi_mask(
      (__v16qi)__A, (__v16qi)_mm_setzero_si128(), (__mmask16)__U);
}
__funline __m128i _mm_mask_popcnt_epi16(__m128i __A, __mmask8 __U, __m128i __B) {
  return (__m128i)__builtin_ia32_vpopcountw_v8hi_mask((__v8hi)__A, (__v8hi)__B,
                                                      (__mmask8)__U);
}

__funline __m128i _mm_maskz_popcnt_epi16(__mmask8 __U, __m128i __A) {
  return (__m128i)__builtin_ia32_vpopcountw_v8hi_mask(
      (__v8hi)__A, (__v8hi)_mm_setzero_si128(), (__mmask8)__U);
}
#ifdef __DISABLE_AVX512BITALGVL__
#undef __DISABLE_AVX512BITALGVL__
#pragma GCC pop_options
#endif /* __DISABLE_AVX512BITALGBW__ */

#endif /* _AVX512BITALGINTRIN_H_INCLUDED */
