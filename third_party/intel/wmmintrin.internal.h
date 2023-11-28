#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _WMMINTRIN_H_INCLUDED
#define _WMMINTRIN_H_INCLUDED
#include "third_party/intel/emmintrin.internal.h"
#if !defined(__AES__) || !defined(__SSE2__)
#pragma GCC push_options
#pragma GCC target("aes,sse2")
#define __DISABLE_AES__
#endif
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesdec_si128 (__m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_aesdec128 ((__v2di)__X, (__v2di)__Y);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesdeclast_si128 (__m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_aesdeclast128 ((__v2di)__X,
       (__v2di)__Y);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesenc_si128 (__m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_aesenc128 ((__v2di)__X, (__v2di)__Y);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesenclast_si128 (__m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_aesenclast128 ((__v2di)__X, (__v2di)__Y);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aesimc_si128 (__m128i __X)
{
  return (__m128i) __builtin_ia32_aesimc128 ((__v2di)__X);
}
#ifdef __OPTIMIZE__
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_aeskeygenassist_si128 (__m128i __X, const int __C)
{
  return (__m128i) __builtin_ia32_aeskeygenassist128 ((__v2di)__X, __C);
}
#else
#define _mm_aeskeygenassist_si128(X, C) ((__m128i) __builtin_ia32_aeskeygenassist128 ((__v2di)(__m128i)(X), (int)(C)))
#endif
#ifdef __DISABLE_AES__
#undef __DISABLE_AES__
#pragma GCC pop_options
#endif
#if !defined(__PCLMUL__) || !defined(__SSE2__)
#pragma GCC push_options
#pragma GCC target("pclmul,sse2")
#define __DISABLE_PCLMUL__
#endif
#ifdef __OPTIMIZE__
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_clmulepi64_si128 (__m128i __X, __m128i __Y, const int __I)
{
  return (__m128i) __builtin_ia32_pclmulqdq128 ((__v2di)__X,
      (__v2di)__Y, __I);
}
#else
#define _mm_clmulepi64_si128(X, Y, I) ((__m128i) __builtin_ia32_pclmulqdq128 ((__v2di)(__m128i)(X), (__v2di)(__m128i)(Y), (int)(I)))
#endif
#ifdef __DISABLE_PCLMUL__
#undef __DISABLE_PCLMUL__
#pragma GCC pop_options
#endif
#endif
#endif
