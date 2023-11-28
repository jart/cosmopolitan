#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avx512ifmavlintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVX512IFMAVLINTRIN_H_INCLUDED
#define _AVX512IFMAVLINTRIN_H_INCLUDED
#if !defined(__AVX512VL__) || !defined(__AVX512IFMA__)
#pragma GCC push_options
#pragma GCC target("avx512ifma,avx512vl")
#define __DISABLE_AVX512IFMAVL__
#endif
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_madd52lo_epu64 (__m128i __X, __m128i __Y, __m128i __Z)
{
  return (__m128i) __builtin_ia32_vpmadd52luq128_mask ((__v2di) __X,
             (__v2di) __Y,
             (__v2di) __Z,
             (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_madd52hi_epu64 (__m128i __X, __m128i __Y, __m128i __Z)
{
  return (__m128i) __builtin_ia32_vpmadd52huq128_mask ((__v2di) __X,
             (__v2di) __Y,
             (__v2di) __Z,
             (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_madd52lo_epu64 (__m256i __X, __m256i __Y, __m256i __Z)
{
  return (__m256i) __builtin_ia32_vpmadd52luq256_mask ((__v4di) __X,
             (__v4di) __Y,
             (__v4di) __Z,
             (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_madd52hi_epu64 (__m256i __X, __m256i __Y, __m256i __Z)
{
  return (__m256i) __builtin_ia32_vpmadd52huq256_mask ((__v4di) __X,
             (__v4di) __Y,
             (__v4di) __Z,
             (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_madd52lo_epu64 (__m128i __W, __mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_vpmadd52luq128_mask ((__v2di) __W,
             (__v2di) __X,
             (__v2di) __Y,
             (__mmask8) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_madd52hi_epu64 (__m128i __W, __mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_vpmadd52huq128_mask ((__v2di) __W,
             (__v2di) __X,
             (__v2di) __Y,
             (__mmask8) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_madd52lo_epu64 (__m256i __W, __mmask8 __M, __m256i __X,
       __m256i __Y)
{
  return (__m256i) __builtin_ia32_vpmadd52luq256_mask ((__v4di) __W,
             (__v4di) __X,
             (__v4di) __Y,
             (__mmask8) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_madd52hi_epu64 (__m256i __W, __mmask8 __M, __m256i __X,
       __m256i __Y)
{
  return (__m256i) __builtin_ia32_vpmadd52huq256_mask ((__v4di) __W,
             (__v4di) __X,
             (__v4di) __Y,
             (__mmask8) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_madd52lo_epu64 (__mmask8 __M, __m128i __X, __m128i __Y, __m128i __Z)
{
  return (__m128i) __builtin_ia32_vpmadd52luq128_maskz ((__v2di) __X,
       (__v2di) __Y,
       (__v2di) __Z,
       (__mmask8) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_madd52hi_epu64 (__mmask8 __M, __m128i __X, __m128i __Y, __m128i __Z)
{
  return (__m128i) __builtin_ia32_vpmadd52huq128_maskz ((__v2di) __X,
       (__v2di) __Y,
       (__v2di) __Z,
       (__mmask8) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_madd52lo_epu64 (__mmask8 __M, __m256i __X, __m256i __Y, __m256i __Z)
{
  return (__m256i) __builtin_ia32_vpmadd52luq256_maskz ((__v4di) __X,
       (__v4di) __Y,
       (__v4di) __Z,
       (__mmask8) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_madd52hi_epu64 (__mmask8 __M, __m256i __X, __m256i __Y, __m256i __Z)
{
  return (__m256i) __builtin_ia32_vpmadd52huq256_maskz ((__v4di) __X,
       (__v4di) __Y,
       (__v4di) __Z,
       (__mmask8) __M);
}
#ifdef __DISABLE_AVX512IFMAVL__
#undef __DISABLE_AVX512IFMAVL__
#pragma GCC pop_options
#endif
#endif
#endif
