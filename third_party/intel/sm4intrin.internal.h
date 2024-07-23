#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <sm4intrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _SM4INTRIN_H_INCLUDED
#define _SM4INTRIN_H_INCLUDED
#ifndef __SM4__
#pragma GCC push_options
#pragma GCC target("sm4")
#define __DISABLE_SM4__
#endif
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sm4key4_epi32 (__m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_vsm4key4128 ((__v4si) __A, (__v4si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_sm4key4_epi32 (__m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_vsm4key4256 ((__v8si) __A, (__v8si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sm4rnds4_epi32 (__m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_vsm4rnds4128 ((__v4si) __A, (__v4si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_sm4rnds4_epi32 (__m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_vsm4rnds4256 ((__v8si) __A, (__v8si) __B);
}
#ifdef __DISABLE_SM4__
#undef __DISABLE_SM4__
#pragma GCC pop_options
#endif
#endif
#endif
