#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avx512bf16vlintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVX512BF16VLINTRIN_H_INCLUDED
#define _AVX512BF16VLINTRIN_H_INCLUDED
#if !defined(__AVX512VL__) || !defined(__AVX512BF16__)
#pragma GCC push_options
#pragma GCC target("avx512bf16,avx512vl")
#define __DISABLE_AVX512BF16VL__
#endif
typedef short __v16bh __attribute__ ((__vector_size__ (32)));
typedef short __v8bh __attribute__ ((__vector_size__ (16)));
typedef short __m256bh __attribute__ ((__vector_size__ (32), __may_alias__));
typedef short __m128bh __attribute__ ((__vector_size__ (16), __may_alias__));
typedef unsigned short __bfloat16;
extern __inline __m256bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtne2ps_pbh (__m256 __A, __m256 __B)
{
  return (__m256bh)__builtin_ia32_cvtne2ps2bf16_v16hi(__A, __B);
}
extern __inline __m256bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtne2ps_pbh (__m256bh __A, __mmask16 __B, __m256 __C, __m256 __D)
{
  return (__m256bh)__builtin_ia32_cvtne2ps2bf16_v16hi_mask(__C, __D, __A, __B);
}
extern __inline __m256bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtne2ps_pbh (__mmask16 __A, __m256 __B, __m256 __C)
{
  return (__m256bh)__builtin_ia32_cvtne2ps2bf16_v16hi_maskz(__B, __C, __A);
}
extern __inline __m128bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtne2ps_pbh (__m128 __A, __m128 __B)
{
  return (__m128bh)__builtin_ia32_cvtne2ps2bf16_v8hi(__A, __B);
}
extern __inline __m128bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtne2ps_pbh (__m128bh __A, __mmask8 __B, __m128 __C, __m128 __D)
{
  return (__m128bh)__builtin_ia32_cvtne2ps2bf16_v8hi_mask(__C, __D, __A, __B);
}
extern __inline __m128bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtne2ps_pbh (__mmask8 __A, __m128 __B, __m128 __C)
{
  return (__m128bh)__builtin_ia32_cvtne2ps2bf16_v8hi_maskz(__B, __C, __A);
}
extern __inline __m128bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtneps_pbh (__m256 __A)
{
  return (__m128bh)__builtin_ia32_cvtneps2bf16_v8sf(__A);
}
extern __inline __m128bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtneps_pbh (__m128bh __A, __mmask8 __B, __m256 __C)
{
  return (__m128bh)__builtin_ia32_cvtneps2bf16_v8sf_mask(__C, __A, __B);
}
extern __inline __m128bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtneps_pbh (__mmask8 __A, __m256 __B)
{
  return (__m128bh)__builtin_ia32_cvtneps2bf16_v8sf_maskz(__B, __A);
}
extern __inline __m128bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtneps_pbh (__m128 __A)
{
  return (__m128bh)__builtin_ia32_cvtneps2bf16_v4sf(__A);
}
extern __inline __m128bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtneps_pbh (__m128bh __A, __mmask8 __B, __m128 __C)
{
  return (__m128bh)__builtin_ia32_cvtneps2bf16_v4sf_mask(__C, __A, __B);
}
extern __inline __m128bh
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtneps_pbh (__mmask8 __A, __m128 __B)
{
  return (__m128bh)__builtin_ia32_cvtneps2bf16_v4sf_maskz(__B, __A);
}
extern __inline __m256
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dpbf16_ps (__m256 __A, __m256bh __B, __m256bh __C)
{
  return (__m256)__builtin_ia32_dpbf16ps_v8sf(__A, __B, __C);
}
extern __inline __m256
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_dpbf16_ps (__m256 __A, __mmask8 __B, __m256bh __C, __m256bh __D)
{
  return (__m256)__builtin_ia32_dpbf16ps_v8sf_mask(__A, __C, __D, __B);
}
extern __inline __m256
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_dpbf16_ps (__mmask8 __A, __m256 __B, __m256bh __C, __m256bh __D)
{
  return (__m256)__builtin_ia32_dpbf16ps_v8sf_maskz(__B, __C, __D, __A);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_dpbf16_ps (__m128 __A, __m128bh __B, __m128bh __C)
{
  return (__m128)__builtin_ia32_dpbf16ps_v4sf(__A, __B, __C);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_dpbf16_ps (__m128 __A, __mmask8 __B, __m128bh __C, __m128bh __D)
{
  return (__m128)__builtin_ia32_dpbf16ps_v4sf_mask(__A, __C, __D, __B);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_dpbf16_ps (__mmask8 __A, __m128 __B, __m128bh __C, __m128bh __D)
{
  return (__m128)__builtin_ia32_dpbf16ps_v4sf_maskz(__B, __C, __D, __A);
}
extern __inline __bfloat16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtness_sbh (float __A)
{
  __v4sf __V = {__A, 0, 0, 0};
  __v8hi __R = __builtin_ia32_cvtneps2bf16_v4sf_mask ((__v4sf)__V,
        (__v8hi)_mm_undefined_si128 (), (__mmask8)-1);
  return __R[0];
}
extern __inline __m128
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpbh_ps (__m128bh __A)
{
  return (__m128)_mm_castsi128_ps ((__m128i)_mm_slli_epi32 (
  (__m128i)_mm_cvtepi16_epi32 ((__m128i)__A), 16));
}
extern __inline __m256
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtpbh_ps (__m128bh __A)
{
  return (__m256)_mm256_castsi256_ps ((__m256i)_mm256_slli_epi32 (
  (__m256i)_mm256_cvtepi16_epi32 ((__m128i)__A), 16));
}
extern __inline __m128
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtpbh_ps (__mmask8 __U, __m128bh __A)
{
  return (__m128)_mm_castsi128_ps ((__m128i)_mm_slli_epi32 (
  (__m128i)_mm_maskz_cvtepi16_epi32 (
  (__mmask8)__U, (__m128i)__A), 16));
}
extern __inline __m256
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtpbh_ps (__mmask8 __U, __m128bh __A)
{
  return (__m256)_mm256_castsi256_ps ((__m256i)_mm256_slli_epi32 (
  (__m256i)_mm256_maskz_cvtepi16_epi32 (
  (__mmask8)__U, (__m128i)__A), 16));
}
extern __inline __m128
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtpbh_ps (__m128 __S, __mmask8 __U, __m128bh __A)
{
  return (__m128)_mm_castsi128_ps ((__m128i)_mm_mask_slli_epi32 (
  (__m128i)__S, (__mmask8)__U, (__m128i)_mm_cvtepi16_epi32 (
  (__m128i)__A), 16));
}
extern __inline __m256
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtpbh_ps (__m256 __S, __mmask8 __U, __m128bh __A)
{
  return (__m256)_mm256_castsi256_ps ((__m256i)_mm256_mask_slli_epi32 (
  (__m256i)__S, (__mmask8)__U, (__m256i)_mm256_cvtepi16_epi32 (
  (__m128i)__A), 16));
}
#ifdef __DISABLE_AVX512BF16VL__
#undef __DISABLE_AVX512BF16VL__
#pragma GCC pop_options
#endif
#endif
#endif
