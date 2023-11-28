#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <vpclmulqdqintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _VPCLMULQDQINTRIN_H_INCLUDED
#define _VPCLMULQDQINTRIN_H_INCLUDED
#if !defined(__VPCLMULQDQ__) || !defined(__AVX512F__)
#pragma GCC push_options
#pragma GCC target("vpclmulqdq,avx512f")
#define __DISABLE_VPCLMULQDQF__
#endif
#ifdef __OPTIMIZE__
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_clmulepi64_epi128 (__m512i __A, __m512i __B, const int __C)
{
  return (__m512i) __builtin_ia32_vpclmulqdq_v8di ((__v8di)__A,
        (__v8di) __B, __C);
}
#else
#define _mm512_clmulepi64_epi128(A, B, C) ((__m512i) __builtin_ia32_vpclmulqdq_v8di ((__v8di)(__m512i)(A), (__v8di)(__m512i)(B), (int)(C)))
#endif
#ifdef __DISABLE_VPCLMULQDQF__
#undef __DISABLE_VPCLMULQDQF__
#pragma GCC pop_options
#endif
#if !defined(__VPCLMULQDQ__) || !defined(__AVX__)
#pragma GCC push_options
#pragma GCC target("vpclmulqdq,avx")
#define __DISABLE_VPCLMULQDQ__
#endif
#ifdef __OPTIMIZE__
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_clmulepi64_epi128 (__m256i __A, __m256i __B, const int __C)
{
  return (__m256i) __builtin_ia32_vpclmulqdq_v4di ((__v4di)__A,
         (__v4di) __B, __C);
}
#else
#define _mm256_clmulepi64_epi128(A, B, C) ((__m256i) __builtin_ia32_vpclmulqdq_v4di ((__v4di)(__m256i)(A), (__v4di)(__m256i)(B), (int)(C)))
#endif
#ifdef __DISABLE_VPCLMULQDQ__
#undef __DISABLE_VPCLMULQDQ__
#pragma GCC pop_options
#endif
#endif
#endif
