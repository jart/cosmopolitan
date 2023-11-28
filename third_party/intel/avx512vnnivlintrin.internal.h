#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avx512vnnivlintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVX512VNNIVLINTRIN_H_INCLUDED
#define _AVX512VNNIVLINTRIN_H_INCLUDED
#if !defined(__AVX512VL__) || !defined(__AVX512VNNI__)
#pragma GCC push_options
#pragma GCC target("avx512vnni,avx512vl")
#define __DISABLE_AVX512VNNIVL__
#endif
#define _mm256_dpbusd_epi32(A, B, C) ((__m256i) __builtin_ia32_vpdpbusd_v8si ((__v8si) (A), (__v8si) (B), (__v8si) (C)))
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_dpbusd_epi32 (__m256i __A, __mmask8 __B, __m256i __C, __m256i __D)
{
  return (__m256i)__builtin_ia32_vpdpbusd_v8si_mask ((__v8si)__A, (__v8si) __C,
      (__v8si) __D, (__mmask8)__B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_dpbusd_epi32 (__mmask8 __A, __m256i __B, __m256i __C, __m256i __D)
{
  return (__m256i)__builtin_ia32_vpdpbusd_v8si_maskz ((__v8si)__B,
    (__v8si) __C, (__v8si) __D, (__mmask8)__A);
}
#define _mm_dpbusd_epi32(A, B, C) ((__m128i) __builtin_ia32_vpdpbusd_v4si ((__v4si) (A), (__v4si) (B), (__v4si) (C)))
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_dpbusd_epi32 (__m128i __A, __mmask8 __B, __m128i __C, __m128i __D)
{
  return (__m128i)__builtin_ia32_vpdpbusd_v4si_mask ((__v4si)__A, (__v4si) __C,
      (__v4si) __D, (__mmask8)__B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_dpbusd_epi32 (__mmask8 __A, __m128i __B, __m128i __C, __m128i __D)
{
  return (__m128i)__builtin_ia32_vpdpbusd_v4si_maskz ((__v4si)__B,
    (__v4si) __C, (__v4si) __D, (__mmask8)__A);
}
#define _mm256_dpbusds_epi32(A, B, C) ((__m256i) __builtin_ia32_vpdpbusds_v8si ((__v8si) (A), (__v8si) (B), (__v8si) (C)))
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_dpbusds_epi32 (__m256i __A, __mmask8 __B, __m256i __C, __m256i __D)
{
  return (__m256i)__builtin_ia32_vpdpbusds_v8si_mask ((__v8si)__A,
    (__v8si) __C, (__v8si) __D, (__mmask8)__B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_dpbusds_epi32 (__mmask8 __A, __m256i __B, __m256i __C,
        __m256i __D)
{
  return (__m256i)__builtin_ia32_vpdpbusds_v8si_maskz ((__v8si)__B,
    (__v8si) __C, (__v8si) __D, (__mmask8)__A);
}
#define _mm_dpbusds_epi32(A, B, C) ((__m128i) __builtin_ia32_vpdpbusds_v4si ((__v4si) (A), (__v4si) (B), (__v4si) (C)))
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_dpbusds_epi32 (__m128i __A, __mmask8 __B, __m128i __C, __m128i __D)
{
  return (__m128i)__builtin_ia32_vpdpbusds_v4si_mask ((__v4si)__A,
    (__v4si) __C, (__v4si) __D, (__mmask8)__B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_dpbusds_epi32 (__mmask8 __A, __m128i __B, __m128i __C, __m128i __D)
{
  return (__m128i)__builtin_ia32_vpdpbusds_v4si_maskz ((__v4si)__B,
    (__v4si) __C, (__v4si) __D, (__mmask8)__A);
}
#define _mm256_dpwssd_epi32(A, B, C) ((__m256i) __builtin_ia32_vpdpwssd_v8si ((__v8si) (A), (__v8si) (B), (__v8si) (C)))
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_dpwssd_epi32 (__m256i __A, __mmask8 __B, __m256i __C, __m256i __D)
{
  return (__m256i)__builtin_ia32_vpdpwssd_v8si_mask ((__v8si)__A, (__v8si) __C,
      (__v8si) __D, (__mmask8)__B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_dpwssd_epi32 (__mmask8 __A, __m256i __B, __m256i __C, __m256i __D)
{
  return (__m256i)__builtin_ia32_vpdpwssd_v8si_maskz ((__v8si)__B,
    (__v8si) __C, (__v8si) __D, (__mmask8)__A);
}
#define _mm_dpwssd_epi32(A, B, C) ((__m128i) __builtin_ia32_vpdpwssd_v4si ((__v4si) (A), (__v4si) (B), (__v4si) (C)))
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_dpwssd_epi32 (__m128i __A, __mmask8 __B, __m128i __C, __m128i __D)
{
  return (__m128i)__builtin_ia32_vpdpwssd_v4si_mask ((__v4si)__A, (__v4si) __C,
      (__v4si) __D, (__mmask8)__B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_dpwssd_epi32 (__mmask8 __A, __m128i __B, __m128i __C, __m128i __D)
{
  return (__m128i)__builtin_ia32_vpdpwssd_v4si_maskz ((__v4si)__B,
    (__v4si) __C, (__v4si) __D, (__mmask8)__A);
}
#define _mm256_dpwssds_epi32(A, B, C) ((__m256i) __builtin_ia32_vpdpwssds_v8si ((__v8si) (A), (__v8si) (B), (__v8si) (C)))
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_dpwssds_epi32 (__m256i __A, __mmask8 __B, __m256i __C, __m256i __D)
{
  return (__m256i)__builtin_ia32_vpdpwssds_v8si_mask ((__v8si)__A,
    (__v8si) __C, (__v8si) __D, (__mmask8)__B);
}
extern __inline __m256i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_dpwssds_epi32 (__mmask8 __A, __m256i __B, __m256i __C,
       __m256i __D)
{
  return (__m256i)__builtin_ia32_vpdpwssds_v8si_maskz ((__v8si)__B,
    (__v8si) __C, (__v8si) __D, (__mmask8)__A);
}
#define _mm_dpwssds_epi32(A, B, C) ((__m128i) __builtin_ia32_vpdpwssds_v4si ((__v4si) (A), (__v4si) (B), (__v4si) (C)))
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_dpwssds_epi32 (__m128i __A, __mmask8 __B, __m128i __C, __m128i __D)
{
  return (__m128i)__builtin_ia32_vpdpwssds_v4si_mask ((__v4si)__A,
    (__v4si) __C, (__v4si) __D, (__mmask8)__B);
}
extern __inline __m128i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_dpwssds_epi32 (__mmask8 __A, __m128i __B, __m128i __C, __m128i __D)
{
  return (__m128i)__builtin_ia32_vpdpwssds_v4si_maskz ((__v4si)__B,
    (__v4si) __C, (__v4si) __D, (__mmask8)__A);
}
#ifdef __DISABLE_AVX512VNNIVL__
#undef __DISABLE_AVX512VNNIVL__
#pragma GCC pop_options
#endif
#endif
#endif
