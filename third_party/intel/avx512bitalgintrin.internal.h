#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
# error "Never use <avx512bitalgintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVX512BITALGINTRIN_H_INCLUDED
#define _AVX512BITALGINTRIN_H_INCLUDED
#if !defined (__AVX512BITALG__) || !defined (__EVEX512__)
#pragma GCC push_options
#pragma GCC target("avx512bitalg,evex512")
#define __DISABLE_AVX512BITALG__
#endif
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_popcnt_epi8 (__m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountb_v64qi ((__v64qi) __A);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_popcnt_epi16 (__m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountw_v32hi ((__v32hi) __A);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_popcnt_epi8 (__m512i __W, __mmask64 __U, __m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountb_v64qi_mask ((__v64qi) __A,
        (__v64qi) __W,
        (__mmask64) __U);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_popcnt_epi8 (__mmask64 __U, __m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountb_v64qi_mask ((__v64qi) __A,
      (__v64qi)
      _mm512_setzero_si512 (),
      (__mmask64) __U);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_popcnt_epi16 (__m512i __W, __mmask32 __U, __m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountw_v32hi_mask ((__v32hi) __A,
       (__v32hi) __W,
       (__mmask32) __U);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_popcnt_epi16 (__mmask32 __U, __m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountw_v32hi_mask ((__v32hi) __A,
      (__v32hi)
      _mm512_setzero_si512 (),
      (__mmask32) __U);
}
extern __inline __mmask64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_bitshuffle_epi64_mask (__m512i __A, __m512i __B)
{
  return (__mmask64) __builtin_ia32_vpshufbitqmb512_mask ((__v64qi) __A,
       (__v64qi) __B,
       (__mmask64) -1);
}
extern __inline __mmask64
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_bitshuffle_epi64_mask (__mmask64 __M, __m512i __A, __m512i __B)
{
  return (__mmask64) __builtin_ia32_vpshufbitqmb512_mask ((__v64qi) __A,
       (__v64qi) __B,
       (__mmask64) __M);
}
#ifdef __DISABLE_AVX512BITALG__
#undef __DISABLE_AVX512BITALG__
#pragma GCC pop_options
#endif
#endif
#endif
