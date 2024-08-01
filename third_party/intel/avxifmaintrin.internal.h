#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avxifmaintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVXIFMAINTRIN_H_INCLUDED
#define _AVXIFMAINTRIN_H_INCLUDED
#ifndef __AVXIFMA__
#pragma GCC push_options
#pragma GCC target("avxifma")
#define __DISABLE_AVXIFMA__
#endif
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_madd52lo_avx_epu64 (__m128i __X, __m128i __Y, __m128i __Z)
{
  return (__m128i) __builtin_ia32_vpmadd52luq128 ((__v2di) __X,
        (__v2di) __Y,
        (__v2di) __Z);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_madd52hi_avx_epu64 (__m128i __X, __m128i __Y, __m128i __Z)
{
  return (__m128i) __builtin_ia32_vpmadd52huq128 ((__v2di) __X,
        (__v2di) __Y,
        (__v2di) __Z);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_madd52lo_avx_epu64 (__m256i __X, __m256i __Y, __m256i __Z)
{
  return (__m256i) __builtin_ia32_vpmadd52luq256 ((__v4di) __X,
        (__v4di) __Y,
        (__v4di) __Z);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_madd52hi_avx_epu64 (__m256i __X, __m256i __Y, __m256i __Z)
{
  return (__m256i) __builtin_ia32_vpmadd52huq256 ((__v4di) __X,
        (__v4di) __Y,
        (__v4di) __Z);
}
#ifdef __DISABLE_AVXIFMA__
#undef __DISABLE_AVXIFMA__
#pragma GCC pop_options
#endif
#endif
#endif
