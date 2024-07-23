#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <avxvnniint8vlintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVXVNNIINT8INTRIN_H_INCLUDED
#define _AVXVNNIINT8INTRIN_H_INCLUDED
#if !defined(__AVXVNNIINT8__)
#pragma GCC push_options
#pragma GCC target("avxvnniint8")
#define __DISABLE_AVXVNNIINT8__
#endif
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpbssd_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpbssd128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpbssds_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpbssds128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpbsud_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpbsud128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpbsuds_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpbsuds128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpbuud_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpbuud128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpbuuds_epi32 (__m128i __W, __m128i __A, __m128i __B)
{
  return (__m128i)
    __builtin_ia32_vpdpbuuds128 ((__v4si) __W, (__v4si) __A, (__v4si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpbssd_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpbssd256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpbssds_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpbssds256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpbsud_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpbsud256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpbsuds_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpbsuds256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpbuud_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpbuud256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpbuuds_epi32 (__m256i __W, __m256i __A, __m256i __B)
{
  return (__m256i)
    __builtin_ia32_vpdpbuuds256 ((__v8si) __W, (__v8si) __A, (__v8si) __B);
}
#ifdef __DISABLE_AVXVNNIINT8__
#undef __DISABLE_AVXVNNIINT8__
#pragma GCC pop_options
#endif
#endif
#endif
