#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <sm3intrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _SM3INTRIN_H_INCLUDED
#define _SM3INTRIN_H_INCLUDED
#ifndef __SM3__
#pragma GCC push_options
#pragma GCC target("sm3")
#define __DISABLE_SM3__
#endif
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sm3msg1_epi32 (__m128i __A, __m128i __B, __m128i __C)
{
  return (__m128i) __builtin_ia32_vsm3msg1 ((__v4si) __A, (__v4si) __B,
         (__v4si) __C);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sm3msg2_epi32 (__m128i __A, __m128i __B, __m128i __C)
{
  return (__m128i) __builtin_ia32_vsm3msg2 ((__v4si) __A, (__v4si) __B,
         (__v4si) __C);
}
#ifdef __OPTIMIZE__
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sm3rnds2_epi32 (__m128i __A, __m128i __B, __m128i __C, const int __D)
{
  return (__m128i) __builtin_ia32_vsm3rnds2 ((__v4si) __A, (__v4si) __B,
          (__v4si) __C, __D);
}
#else
#define _mm_sm3rnds2_epi32(A, B, C, D) ((__m128i) __builtin_ia32_vsm3rnds2 ((__v4si) (A), (__v4si) (B), (__v4si) (C), (int) (D)))
#endif
#ifdef __DISABLE_SM3__
#undef __DISABLE_SM3__
#pragma GCC pop_options
#endif
#endif
#endif
