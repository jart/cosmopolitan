#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
# error "Never use <avx5124vnniwintrin.h> directly; include <x86intrin.h> instead."
#endif
#ifndef _AVX5124VNNIWINTRIN_H_INCLUDED
#define _AVX5124VNNIWINTRIN_H_INCLUDED
#ifndef __AVX5124VNNIW__
#pragma GCC push_options
#pragma GCC target("avx5124vnniw")
#define __DISABLE_AVX5124VNNIW__
#endif
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_4dpwssd_epi32 (__m512i __A, __m512i __B, __m512i __C,
        __m512i __D, __m512i __E, __m128i *__F)
{
  return (__m512i) __builtin_ia32_vp4dpwssd ((__v16si) __B,
          (__v16si) __C,
          (__v16si) __D,
          (__v16si) __E,
          (__v16si) __A,
          (const __v4si *) __F);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_4dpwssd_epi32 (__m512i __A, __mmask16 __U, __m512i __B,
      __m512i __C, __m512i __D, __m512i __E,
      __m128i *__F)
{
  return (__m512i) __builtin_ia32_vp4dpwssd_mask ((__v16si) __B,
        (__v16si) __C,
        (__v16si) __D,
        (__v16si) __E,
        (__v16si) __A,
        (const __v4si *) __F,
        (__v16si) __A,
        (__mmask16) __U);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_4dpwssd_epi32 (__mmask16 __U, __m512i __A, __m512i __B,
       __m512i __C, __m512i __D, __m512i __E,
       __m128i *__F)
{
  return (__m512i) __builtin_ia32_vp4dpwssd_mask ((__v16si) __B,
        (__v16si) __C,
        (__v16si) __D,
        (__v16si) __E,
        (__v16si) __A,
        (const __v4si *) __F,
        (__v16si) _mm512_setzero_ps (),
        (__mmask16) __U);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_4dpwssds_epi32 (__m512i __A, __m512i __B, __m512i __C,
         __m512i __D, __m512i __E, __m128i *__F)
{
  return (__m512i) __builtin_ia32_vp4dpwssds ((__v16si) __B,
           (__v16si) __C,
           (__v16si) __D,
           (__v16si) __E,
           (__v16si) __A,
           (const __v4si *) __F);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_4dpwssds_epi32 (__m512i __A, __mmask16 __U, __m512i __B,
       __m512i __C, __m512i __D, __m512i __E,
       __m128i *__F)
{
  return (__m512i) __builtin_ia32_vp4dpwssds_mask ((__v16si) __B,
         (__v16si) __C,
         (__v16si) __D,
         (__v16si) __E,
         (__v16si) __A,
         (const __v4si *) __F,
         (__v16si) __A,
         (__mmask16) __U);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_4dpwssds_epi32 (__mmask16 __U, __m512i __A, __m512i __B,
        __m512i __C, __m512i __D, __m512i __E,
        __m128i *__F)
{
  return (__m512i) __builtin_ia32_vp4dpwssds_mask ((__v16si) __B,
         (__v16si) __C,
         (__v16si) __D,
         (__v16si) __E,
         (__v16si) __A,
         (const __v4si *) __F,
         (__v16si) _mm512_setzero_ps (),
         (__mmask16) __U);
}
#ifdef __DISABLE_AVX5124VNNIW__
#undef __DISABLE_AVX5124VNNIW__
#pragma GCC pop_options
#endif
#endif
#endif
