#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
# error "Never use <vaesintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef __VAESINTRIN_H_INCLUDED
#define __VAESINTRIN_H_INCLUDED
#if !defined(__VAES__) || !defined(__AVX__)
#pragma GCC push_options
#pragma GCC target("vaes,avx")
#define __DISABLE_VAES__
#endif
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_aesdec_epi128 (__m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_vaesdec_v32qi ((__v32qi) __A, (__v32qi) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_aesdeclast_epi128 (__m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_vaesdeclast_v32qi ((__v32qi) __A,
        (__v32qi) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_aesenc_epi128 (__m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_vaesenc_v32qi ((__v32qi) __A, (__v32qi) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_aesenclast_epi128 (__m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_vaesenclast_v32qi ((__v32qi) __A,
        (__v32qi) __B);
}
#ifdef __DISABLE_VAES__
#undef __DISABLE_VAES__
#pragma GCC pop_options
#endif
#if !defined(__VAES__) || !defined(__AVX512F__)
#pragma GCC push_options
#pragma GCC target("vaes,avx512f")
#define __DISABLE_VAESF__
#endif
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_aesdec_epi128 (__m512i __A, __m512i __B)
{
  return (__m512i)__builtin_ia32_vaesdec_v64qi ((__v64qi) __A, (__v64qi) __B);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_aesdeclast_epi128 (__m512i __A, __m512i __B)
{
  return (__m512i)__builtin_ia32_vaesdeclast_v64qi ((__v64qi) __A,
          (__v64qi) __B);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_aesenc_epi128 (__m512i __A, __m512i __B)
{
  return (__m512i)__builtin_ia32_vaesenc_v64qi ((__v64qi) __A, (__v64qi) __B);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_aesenclast_epi128 (__m512i __A, __m512i __B)
{
  return (__m512i)__builtin_ia32_vaesenclast_v64qi ((__v64qi) __A,
          (__v64qi) __B);
}
#ifdef __DISABLE_VAESF__
#undef __DISABLE_VAESF__
#pragma GCC pop_options
#endif
#endif
#endif
