#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avxvnniintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVXVNNIINTRIN_H_INCLUDED
#define _AVXVNNIINTRIN_H_INCLUDED
#if !defined(__AVXVNNI__)
#pragma GCC push_options
#pragma GCC target("avxvnni")
#define __DISABLE_AVXVNNIVL__
#endif
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpbusd_avx_epi32(__m256i __A, __m256i __B, __m256i __C)
{
  return (__m256i) __builtin_ia32_vpdpbusd_v8si ((__v8si) __A,
         (__v8si) __B,
         (__v8si) __C);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpbusd_avx_epi32(__m128i __A, __m128i __B, __m128i __C)
{
  return (__m128i) __builtin_ia32_vpdpbusd_v4si ((__v4si) __A,
       (__v4si) __B,
       (__v4si) __C);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpbusds_avx_epi32(__m256i __A, __m256i __B, __m256i __C)
{
  return (__m256i) __builtin_ia32_vpdpbusds_v8si ((__v8si) __A,
        (__v8si) __B,
        (__v8si) __C);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpbusds_avx_epi32(__m128i __A,__m128i __B,__m128i __C)
{
  return (__m128i) __builtin_ia32_vpdpbusds_v4si ((__v4si) __A,
        (__v4si) __B,
        (__v4si) __C);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpwssd_avx_epi32(__m256i __A,__m256i __B,__m256i __C)
{
  return (__m256i) __builtin_ia32_vpdpwssd_v8si ((__v8si) __A,
       (__v8si) __B,
       (__v8si) __C);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpwssd_avx_epi32(__m128i __A,__m128i __B,__m128i __C)
{
  return (__m128i) __builtin_ia32_vpdpwssd_v4si ((__v4si) __A,
       (__v4si) __B,
       (__v4si) __C);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpwssds_avx_epi32(__m256i __A,__m256i __B,__m256i __C)
{
  return (__m256i) __builtin_ia32_vpdpwssds_v8si ((__v8si) __A,
        (__v8si) __B,
        (__v8si) __C);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpwssds_avx_epi32(__m128i __A,__m128i __B,__m128i __C)
{
  return (__m128i) __builtin_ia32_vpdpwssds_v4si ((__v4si) __A,
        (__v4si) __B,
        (__v4si) __C);
}
#ifdef __DISABLE_AVXVNNIVL__
#undef __DISABLE_AVXVNNIVL__
#pragma GCC pop_options
#endif
#endif
#endif
