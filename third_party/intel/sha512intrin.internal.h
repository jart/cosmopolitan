#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <sha512intrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _SHA512INTRIN_H_INCLUDED
#define _SHA512INTRIN_H_INCLUDED
#ifndef __SHA512__
#pragma GCC push_options
#pragma GCC target("sha512")
#define __DISABLE_SHA512__
#endif
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_sha512msg1_epi64 (__m256i __A, __m128i __B)
{
  return (__m256i) __builtin_ia32_vsha512msg1 ((__v4di) __A, (__v2di) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_sha512msg2_epi64 (__m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_vsha512msg2 ((__v4di) __A, (__v4di) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_sha512rnds2_epi64 (__m256i __A, __m256i __B, __m128i __C)
{
  return (__m256i) __builtin_ia32_vsha512rnds2 ((__v4di) __A, (__v4di) __B,
      (__v2di) __C);
}
#ifdef __DISABLE_SHA512__
#undef __DISABLE_SHA512__
#pragma GCC pop_options
#endif
#endif
#endif
