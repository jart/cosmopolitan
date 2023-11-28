#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _AMMINTRIN_H_INCLUDED
#define _AMMINTRIN_H_INCLUDED
#include "third_party/intel/pmmintrin.internal.h"
#ifndef __SSE4A__
#pragma GCC push_options
#pragma GCC target("sse4a")
#define __DISABLE_SSE4A__
#endif
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_stream_sd (double * __P, __m128d __Y)
{
  __builtin_ia32_movntsd (__P, (__v2df) __Y);
}
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_stream_ss (float * __P, __m128 __Y)
{
  __builtin_ia32_movntss (__P, (__v4sf) __Y);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_extract_si64 (__m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_extrq ((__v2di) __X, (__v16qi) __Y);
}
#ifdef __OPTIMIZE__
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_extracti_si64 (__m128i __X, unsigned const int __I, unsigned const int __L)
{
  return (__m128i) __builtin_ia32_extrqi ((__v2di) __X, __I, __L);
}
#else
#define _mm_extracti_si64(X, I, L) ((__m128i) __builtin_ia32_extrqi ((__v2di)(__m128i)(X), (unsigned int)(I), (unsigned int)(L)))
#endif
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_insert_si64 (__m128i __X,__m128i __Y)
{
  return (__m128i) __builtin_ia32_insertq ((__v2di)__X, (__v2di)__Y);
}
#ifdef __OPTIMIZE__
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_inserti_si64(__m128i __X, __m128i __Y, unsigned const int __I, unsigned const int __L)
{
  return (__m128i) __builtin_ia32_insertqi ((__v2di)__X, (__v2di)__Y, __I, __L);
}
#else
#define _mm_inserti_si64(X, Y, I, L) ((__m128i) __builtin_ia32_insertqi ((__v2di)(__m128i)(X), (__v2di)(__m128i)(Y), (unsigned int)(I), (unsigned int)(L)))
#endif
#ifdef __DISABLE_SSE4A__
#undef __DISABLE_SSE4A__
#pragma GCC pop_options
#endif
#endif
#endif
