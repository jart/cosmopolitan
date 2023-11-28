#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <avx512vp2intersectintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVX512VP2INTERSECTINTRIN_H_INCLUDED
#define _AVX512VP2INTERSECTINTRIN_H_INCLUDED
#if !defined(__AVX512VP2INTERSECT__)
#pragma GCC push_options
#pragma GCC target("avx512vp2intersect")
#define __DISABLE_AVX512VP2INTERSECT__
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_2intersect_epi32 (__m512i __A, __m512i __B, __mmask16 *__U,
    __mmask16 *__M)
{
  __builtin_ia32_2intersectd512 (__U, __M, (__v16si) __A, (__v16si) __B);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_2intersect_epi64 (__m512i __A, __m512i __B, __mmask8 *__U,
    __mmask8 *__M)
{
  __builtin_ia32_2intersectq512 (__U, __M, (__v8di) __A, (__v8di) __B);
}
#ifdef __DISABLE_AVX512VP2INTERSECT__
#undef __DISABLE_AVX512VP2INTERSECT__
#pragma GCC pop_options
#endif
#endif
#endif
