#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <avxvnniint16intrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVXVNNIINT16INTRIN_H_INCLUDED
#define _AVXVNNIINT16INTRIN_H_INCLUDED
#if !defined(__AVXVNNIINT16__)
#pragma GCC push_options
#pragma GCC target("avxvnniint16")
#define __DISABLE_AVXVNNIINT16__
#endif
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpwsud_avx_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpwsud128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpwsuds_avx_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpwsuds128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpwusd_avx_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpwusd128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpwusds_avx_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpwusds128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpwuud_avx_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpwuud128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpwuuds_avx_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpwuuds128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpwsud_avx_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpwsud256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpwsuds_avx_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpwsuds256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpwusd_avx_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpwusd256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpwusds_avx_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpwusds256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpwuud_avx_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpwuud256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpwuuds_avx_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpwuuds256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
#ifdef __DISABLE_AVXVNNIINT16__
#undef __DISABLE_AVXVNNIINT16__
#pragma GCC pop_options
#endif
#endif
#endif
