#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avx512fp16vlintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef __AVX512FP16VLINTRIN_H_INCLUDED
#define __AVX512FP16VLINTRIN_H_INCLUDED
#if !defined(__AVX512VL__) || !defined(__AVX512FP16__) || defined (__EVEX512__)
#pragma GCC push_options
#pragma GCC target("avx512fp16,avx512vl,no-evex512")
#define __DISABLE_AVX512FP16VL__
#endif
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_set1_ps (float __F)
{
  return __extension__ (__m128)(__v4sf){ __F, __F, __F, __F };
}
extern __inline __m256 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_set1_ps (float __A)
{
  return __extension__ (__m256){ __A, __A, __A, __A,
     __A, __A, __A, __A };
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_and_si128 (__m128i __A, __m128i __B)
{
  return (__m128i) ((__v2du)__A & (__v2du)__B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_and_si256 (__m256i __A, __m256i __B)
{
  return (__m256i) ((__v4du)__A & (__v4du)__B);
}
extern __inline __m128
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_castph_ps (__m128h __a)
{
  return (__m128) __a;
}
extern __inline __m256
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_castph_ps (__m256h __a)
{
  return (__m256) __a;
}
extern __inline __m128d
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_castph_pd (__m128h __a)
{
  return (__m128d) __a;
}
extern __inline __m256d
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_castph_pd (__m256h __a)
{
  return (__m256d) __a;
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_castph_si128 (__m128h __a)
{
  return (__m128i) __a;
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_castph_si256 (__m256h __a)
{
  return (__m256i) __a;
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_castps_ph (__m128 __a)
{
  return (__m128h) __a;
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_castps_ph (__m256 __a)
{
  return (__m256h) __a;
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_castpd_ph (__m128d __a)
{
  return (__m128h) __a;
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_castpd_ph (__m256d __a)
{
  return (__m256h) __a;
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_castsi128_ph (__m128i __a)
{
  return (__m128h) __a;
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_castsi256_ph (__m256i __a)
{
  return (__m256h) __a;
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_castph256_ph128 (__m256h __A)
{
  union
  {
    __m128h __a[2];
    __m256h __v;
  } __u = { .__v = __A };
  return __u.__a[0];
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_castph128_ph256 (__m128h __A)
{
  union
  {
    __m128h __a[2];
    __m256h __v;
  } __u;
  __u.__a[0] = __A;
  return __u.__v;
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_zextph128_ph256 (__m128h __A)
{
  return (__m256h) _mm256_avx512_insertf128_ps (_mm256_avx512_setzero_ps (),
      (__m128) __A, 0);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_conj_pch (__m256h __A)
{
  return (__m256h) _mm256_xor_epi32 ((__m256i) __A, _mm256_avx512_set1_epi32 (1<<31));
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_conj_pch (__m256h __W, __mmask8 __U, __m256h __A)
{
  return (__m256h) __builtin_ia32_movaps256_mask ((__v8sf)
         _mm256_conj_pch (__A),
        (__v8sf) __W,
        (__mmask8) __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_conj_pch (__mmask8 __U, __m256h __A)
{
  return (__m256h) __builtin_ia32_movaps256_mask ((__v8sf)
         _mm256_conj_pch (__A),
        (__v8sf)
         _mm256_avx512_setzero_ps (),
        (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_conj_pch (__m128h __A)
{
  return (__m128h) _mm_xor_epi32 ((__m128i) __A, _mm_avx512_set1_epi32 (1<<31));
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_conj_pch (__m128h __W, __mmask8 __U, __m128h __A)
{
  return (__m128h) __builtin_ia32_movaps128_mask ((__v4sf) _mm_conj_pch (__A),
        (__v4sf) __W,
        (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_conj_pch (__mmask8 __U, __m128h __A)
{
  return (__m128h) __builtin_ia32_movaps128_mask ((__v4sf) _mm_conj_pch (__A),
        (__v4sf) _mm_avx512_setzero_ps (),
        (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_ph (__m128h __A, __m128h __B)
{
  return (__m128h) ((__v8hf) __A + (__v8hf) __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_add_ph (__m256h __A, __m256h __B)
{
  return (__m256h) ((__v16hf) __A + (__v16hf) __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_add_ph (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return __builtin_ia32_addph128_mask (__C, __D, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_add_ph (__m256h __A, __mmask16 __B, __m256h __C, __m256h __D)
{
  return __builtin_ia32_addph256_mask (__C, __D, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_add_ph (__mmask8 __A, __m128h __B, __m128h __C)
{
  return __builtin_ia32_addph128_mask (__B, __C, _mm_setzero_ph (),
           __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_add_ph (__mmask16 __A, __m256h __B, __m256h __C)
{
  return __builtin_ia32_addph256_mask (__B, __C,
           _mm256_setzero_ph (), __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_ph (__m128h __A, __m128h __B)
{
  return (__m128h) ((__v8hf) __A - (__v8hf) __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_sub_ph (__m256h __A, __m256h __B)
{
  return (__m256h) ((__v16hf) __A - (__v16hf) __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_sub_ph (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return __builtin_ia32_subph128_mask (__C, __D, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_sub_ph (__m256h __A, __mmask16 __B, __m256h __C, __m256h __D)
{
  return __builtin_ia32_subph256_mask (__C, __D, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_sub_ph (__mmask8 __A, __m128h __B, __m128h __C)
{
  return __builtin_ia32_subph128_mask (__B, __C, _mm_setzero_ph (),
           __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_sub_ph (__mmask16 __A, __m256h __B, __m256h __C)
{
  return __builtin_ia32_subph256_mask (__B, __C,
           _mm256_setzero_ph (), __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mul_ph (__m128h __A, __m128h __B)
{
  return (__m128h) ((__v8hf) __A * (__v8hf) __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mul_ph (__m256h __A, __m256h __B)
{
  return (__m256h) ((__v16hf) __A * (__v16hf) __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_mul_ph (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return __builtin_ia32_mulph128_mask (__C, __D, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_mul_ph (__m256h __A, __mmask16 __B, __m256h __C, __m256h __D)
{
  return __builtin_ia32_mulph256_mask (__C, __D, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_mul_ph (__mmask8 __A, __m128h __B, __m128h __C)
{
  return __builtin_ia32_mulph128_mask (__B, __C, _mm_setzero_ph (),
           __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_mul_ph (__mmask16 __A, __m256h __B, __m256h __C)
{
  return __builtin_ia32_mulph256_mask (__B, __C,
           _mm256_setzero_ph (), __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_div_ph (__m128h __A, __m128h __B)
{
  return (__m128h) ((__v8hf) __A / (__v8hf) __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_div_ph (__m256h __A, __m256h __B)
{
  return (__m256h) ((__v16hf) __A / (__v16hf) __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_div_ph (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return __builtin_ia32_divph128_mask (__C, __D, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_div_ph (__m256h __A, __mmask16 __B, __m256h __C, __m256h __D)
{
  return __builtin_ia32_divph256_mask (__C, __D, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_div_ph (__mmask8 __A, __m128h __B, __m128h __C)
{
  return __builtin_ia32_divph128_mask (__B, __C, _mm_setzero_ph (),
           __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_div_ph (__mmask16 __A, __m256h __B, __m256h __C)
{
  return __builtin_ia32_divph256_mask (__B, __C,
           _mm256_setzero_ph (), __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_max_ph (__m128h __A, __m128h __B)
{
  return __builtin_ia32_maxph128_mask (__A, __B,
           _mm_setzero_ph (),
           (__mmask8) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_max_ph (__m256h __A, __m256h __B)
{
  return __builtin_ia32_maxph256_mask (__A, __B,
           _mm256_setzero_ph (),
           (__mmask16) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_max_ph (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return __builtin_ia32_maxph128_mask (__C, __D, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_max_ph (__m256h __A, __mmask16 __B, __m256h __C, __m256h __D)
{
  return __builtin_ia32_maxph256_mask (__C, __D, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_max_ph (__mmask8 __A, __m128h __B, __m128h __C)
{
  return __builtin_ia32_maxph128_mask (__B, __C, _mm_setzero_ph (),
           __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_max_ph (__mmask16 __A, __m256h __B, __m256h __C)
{
  return __builtin_ia32_maxph256_mask (__B, __C,
           _mm256_setzero_ph (), __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_ph (__m128h __A, __m128h __B)
{
  return __builtin_ia32_minph128_mask (__A, __B,
           _mm_setzero_ph (),
           (__mmask8) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_min_ph (__m256h __A, __m256h __B)
{
  return __builtin_ia32_minph256_mask (__A, __B,
           _mm256_setzero_ph (),
           (__mmask16) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_min_ph (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return __builtin_ia32_minph128_mask (__C, __D, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_min_ph (__m256h __A, __mmask16 __B, __m256h __C, __m256h __D)
{
  return __builtin_ia32_minph256_mask (__C, __D, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_min_ph (__mmask8 __A, __m128h __B, __m128h __C)
{
  return __builtin_ia32_minph128_mask (__B, __C, _mm_setzero_ph (),
           __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_min_ph (__mmask16 __A, __m256h __B, __m256h __C)
{
  return __builtin_ia32_minph256_mask (__B, __C,
           _mm256_setzero_ph (), __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_abs_ph (__m128h __A)
{
  return (__m128h) _mm_avx512_and_si128 (_mm_avx512_set1_epi32 (0x7FFF7FFF),
      (__m128i) __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_abs_ph (__m256h __A)
{
  return (__m256h) _mm256_avx512_and_si256 (_mm256_avx512_set1_epi32 (0x7FFF7FFF),
         (__m256i) __A);
}
#ifdef __OPTIMIZE
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmp_ph_mask (__m128h __A, __m128h __B, const int __C)
{
  return (__mmask8) __builtin_ia32_cmpph128_mask (__A, __B, __C,
        (__mmask8) -1);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmp_ph_mask (__mmask8 __A, __m128h __B, __m128h __C,
        const int __D)
{
  return (__mmask8) __builtin_ia32_cmpph128_mask (__B, __C, __D, __A);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmp_ph_mask (__m256h __A, __m256h __B, const int __C)
{
  return (__mmask16) __builtin_ia32_cmpph256_mask (__A, __B, __C,
         (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmp_ph_mask (__mmask16 __A, __m256h __B, __m256h __C,
        const int __D)
{
  return (__mmask16) __builtin_ia32_cmpph256_mask (__B, __C, __D,
         __A);
}
#else
#define _mm_cmp_ph_mask(A, B, C) (__builtin_ia32_cmpph128_mask ((A), (B), (C), (-1)))
#define _mm_mask_cmp_ph_mask(A, B, C, D) (__builtin_ia32_cmpph128_mask ((B), (C), (D), (A)))
#define _mm256_cmp_ph_mask(A, B, C) (__builtin_ia32_cmpph256_mask ((A), (B), (C), (-1)))
#define _mm256_mask_cmp_ph_mask(A, B, C, D) (__builtin_ia32_cmpph256_mask ((B), (C), (D), (A)))
#endif
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_sqrt_ph (__m128h __A)
{
  return __builtin_ia32_sqrtph128_mask (__A, _mm_setzero_ph (),
     (__mmask8) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_sqrt_ph (__m256h __A)
{
  return __builtin_ia32_sqrtph256_mask (__A, _mm256_setzero_ph (),
     (__mmask16) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_sqrt_ph (__m128h __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_sqrtph128_mask (__C, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_sqrt_ph (__m256h __A, __mmask16 __B, __m256h __C)
{
  return __builtin_ia32_sqrtph256_mask (__C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_sqrt_ph (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_sqrtph128_mask (__B, _mm_setzero_ph (),
     __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_sqrt_ph (__mmask16 __A, __m256h __B)
{
  return __builtin_ia32_sqrtph256_mask (__B, _mm256_setzero_ph (),
     __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_rsqrt_ph (__m128h __A)
{
  return __builtin_ia32_rsqrtph128_mask (__A, _mm_setzero_ph (),
      (__mmask8) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_rsqrt_ph (__m256h __A)
{
  return __builtin_ia32_rsqrtph256_mask (__A, _mm256_setzero_ph (),
      (__mmask16) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_rsqrt_ph (__m128h __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_rsqrtph128_mask (__C, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_rsqrt_ph (__m256h __A, __mmask16 __B, __m256h __C)
{
  return __builtin_ia32_rsqrtph256_mask (__C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_rsqrt_ph (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_rsqrtph128_mask (__B, _mm_setzero_ph (), __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_rsqrt_ph (__mmask16 __A, __m256h __B)
{
  return __builtin_ia32_rsqrtph256_mask (__B, _mm256_setzero_ph (),
      __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_rcp_ph (__m128h __A)
{
  return __builtin_ia32_rcpph128_mask (__A, _mm_setzero_ph (),
           (__mmask8) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_rcp_ph (__m256h __A)
{
  return __builtin_ia32_rcpph256_mask (__A, _mm256_setzero_ph (),
           (__mmask16) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_rcp_ph (__m128h __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_rcpph128_mask (__C, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_rcp_ph (__m256h __A, __mmask16 __B, __m256h __C)
{
  return __builtin_ia32_rcpph256_mask (__C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_rcp_ph (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_rcpph128_mask (__B, _mm_setzero_ph (), __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_rcp_ph (__mmask16 __A, __m256h __B)
{
  return __builtin_ia32_rcpph256_mask (__B, _mm256_setzero_ph (),
           __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_scalef_ph (__m128h __A, __m128h __B)
{
  return __builtin_ia32_scalefph128_mask (__A, __B,
       _mm_setzero_ph (),
       (__mmask8) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_scalef_ph (__m256h __A, __m256h __B)
{
  return __builtin_ia32_scalefph256_mask (__A, __B,
       _mm256_setzero_ph (),
       (__mmask16) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_scalef_ph (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return __builtin_ia32_scalefph128_mask (__C, __D, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_scalef_ph (__m256h __A, __mmask16 __B, __m256h __C,
         __m256h __D)
{
  return __builtin_ia32_scalefph256_mask (__C, __D, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_scalef_ph (__mmask8 __A, __m128h __B, __m128h __C)
{
  return __builtin_ia32_scalefph128_mask (__B, __C,
       _mm_setzero_ph (), __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_scalef_ph (__mmask16 __A, __m256h __B, __m256h __C)
{
  return __builtin_ia32_scalefph256_mask (__B, __C,
       _mm256_setzero_ph (),
       __A);
}
#ifdef __OPTIMIZE__
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_reduce_ph (__m128h __A, int __B)
{
  return __builtin_ia32_reduceph128_mask (__A, __B,
       _mm_setzero_ph (),
       (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_ph (__m128h __A, __mmask8 __B, __m128h __C, int __D)
{
  return __builtin_ia32_reduceph128_mask (__C, __D, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_reduce_ph (__mmask8 __A, __m128h __B, int __C)
{
  return __builtin_ia32_reduceph128_mask (__B, __C,
       _mm_setzero_ph (), __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_reduce_ph (__m256h __A, int __B)
{
  return __builtin_ia32_reduceph256_mask (__A, __B,
       _mm256_setzero_ph (),
       (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_ph (__m256h __A, __mmask16 __B, __m256h __C, int __D)
{
  return __builtin_ia32_reduceph256_mask (__C, __D, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_reduce_ph (__mmask16 __A, __m256h __B, int __C)
{
  return __builtin_ia32_reduceph256_mask (__B, __C,
       _mm256_setzero_ph (),
       __A);
}
#else
#define _mm_reduce_ph(A, B) (__builtin_ia32_reduceph128_mask ((A), (B), _mm_setzero_ph (), ((__mmask8)-1)))
#define _mm_mask_reduce_ph(A, B, C, D) (__builtin_ia32_reduceph128_mask ((C), (D), (A), (B)))
#define _mm_maskz_reduce_ph(A, B, C) (__builtin_ia32_reduceph128_mask ((B), (C), _mm_setzero_ph (), (A)))
#define _mm256_reduce_ph(A, B) (__builtin_ia32_reduceph256_mask ((A), (B), _mm256_setzero_ph (), ((__mmask16)-1)))
#define _mm256_mask_reduce_ph(A, B, C, D) (__builtin_ia32_reduceph256_mask ((C), (D), (A), (B)))
#define _mm256_maskz_reduce_ph(A, B, C) (__builtin_ia32_reduceph256_mask ((B), (C), _mm256_setzero_ph (), (A)))
#endif
#ifdef __OPTIMIZE__
  extern __inline __m128h
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
  _mm_roundscale_ph (__m128h __A, int __B)
  {
    return __builtin_ia32_rndscaleph128_mask (__A, __B,
           _mm_setzero_ph (),
           (__mmask8) -1);
  }
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_roundscale_ph (__m128h __A, __mmask8 __B, __m128h __C, int __D)
{
  return __builtin_ia32_rndscaleph128_mask (__C, __D, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_roundscale_ph (__mmask8 __A, __m128h __B, int __C)
{
  return __builtin_ia32_rndscaleph128_mask (__B, __C,
         _mm_setzero_ph (), __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_roundscale_ph (__m256h __A, int __B)
{
  return __builtin_ia32_rndscaleph256_mask (__A, __B,
         _mm256_setzero_ph (),
         (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_roundscale_ph (__m256h __A, __mmask16 __B, __m256h __C,
      int __D)
{
  return __builtin_ia32_rndscaleph256_mask (__C, __D, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_roundscale_ph (__mmask16 __A, __m256h __B, int __C)
{
  return __builtin_ia32_rndscaleph256_mask (__B, __C,
         _mm256_setzero_ph (),
         __A);
}
#else
#define _mm_roundscale_ph(A, B) (__builtin_ia32_rndscaleph128_mask ((A), (B), _mm_setzero_ph (), ((__mmask8)-1)))
#define _mm_mask_roundscale_ph(A, B, C, D) (__builtin_ia32_rndscaleph128_mask ((C), (D), (A), (B)))
#define _mm_maskz_roundscale_ph(A, B, C) (__builtin_ia32_rndscaleph128_mask ((B), (C), _mm_setzero_ph (), (A)))
#define _mm256_roundscale_ph(A, B) (__builtin_ia32_rndscaleph256_mask ((A), (B), _mm256_setzero_ph(), ((__mmask16)-1)))
#define _mm256_mask_roundscale_ph(A, B, C, D) (__builtin_ia32_rndscaleph256_mask ((C), (D), (A), (B)))
#define _mm256_maskz_roundscale_ph(A, B, C) (__builtin_ia32_rndscaleph256_mask ((B), (C), _mm256_setzero_ph (), (A)))
#endif
#ifdef __OPTIMIZE__
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
  _mm_mask_fpclass_ph_mask (__mmask8 __U, __m128h __A, const int __imm)
{
  return (__mmask8) __builtin_ia32_fpclassph128_mask ((__v8hf) __A,
            __imm, __U);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fpclass_ph_mask (__m128h __A, const int __imm)
{
  return (__mmask8) __builtin_ia32_fpclassph128_mask ((__v8hf) __A,
            __imm,
            (__mmask8) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fpclass_ph_mask (__mmask16 __U, __m256h __A, const int __imm)
{
  return (__mmask16) __builtin_ia32_fpclassph256_mask ((__v16hf) __A,
             __imm, __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fpclass_ph_mask (__m256h __A, const int __imm)
{
  return (__mmask16) __builtin_ia32_fpclassph256_mask ((__v16hf) __A,
             __imm,
             (__mmask16) -1);
}
#else
#define _mm_fpclass_ph_mask(X, C) ((__mmask8) __builtin_ia32_fpclassph128_mask ((__v8hf) (__m128h) (X), (int) (C),(__mmask8)-1))
#define _mm_mask_fpclass_ph_mask(u, X, C) ((__mmask8) __builtin_ia32_fpclassph128_mask ((__v8hf) (__m128h) (X), (int) (C),(__mmask8)(u)))
#define _mm256_fpclass_ph_mask(X, C) ((__mmask16) __builtin_ia32_fpclassph256_mask ((__v16hf) (__m256h) (X), (int) (C),(__mmask16)-1))
#define _mm256_mask_fpclass_ph_mask(u, X, C) ((__mmask16) __builtin_ia32_fpclassph256_mask ((__v16hf) (__m256h) (X), (int) (C),(__mmask16)(u)))
#endif
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_getexp_ph (__m256h __A)
{
  return (__m256h) __builtin_ia32_getexpph256_mask ((__v16hf) __A,
          (__v16hf)
          _mm256_setzero_ph (),
          (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_getexp_ph (__m256h __W, __mmask16 __U, __m256h __A)
{
  return (__m256h) __builtin_ia32_getexpph256_mask ((__v16hf) __A,
          (__v16hf) __W,
          (__mmask16) __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_getexp_ph (__mmask16 __U, __m256h __A)
{
  return (__m256h) __builtin_ia32_getexpph256_mask ((__v16hf) __A,
          (__v16hf)
          _mm256_setzero_ph (),
          (__mmask16) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_getexp_ph (__m128h __A)
{
  return (__m128h) __builtin_ia32_getexpph128_mask ((__v8hf) __A,
          (__v8hf)
          _mm_setzero_ph (),
          (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_getexp_ph (__m128h __W, __mmask8 __U, __m128h __A)
{
  return (__m128h) __builtin_ia32_getexpph128_mask ((__v8hf) __A,
          (__v8hf) __W,
          (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_getexp_ph (__mmask8 __U, __m128h __A)
{
  return (__m128h) __builtin_ia32_getexpph128_mask ((__v8hf) __A,
          (__v8hf)
          _mm_setzero_ph (),
          (__mmask8) __U);
}
#ifdef __OPTIMIZE__
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_getmant_ph (__m256h __A, _MM_MANTISSA_NORM_ENUM __B,
     _MM_MANTISSA_SIGN_ENUM __C)
{
  return (__m256h) __builtin_ia32_getmantph256_mask ((__v16hf) __A,
           (__C << 2) | __B,
           (__v16hf)
           _mm256_setzero_ph (),
           (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_getmant_ph (__m256h __W, __mmask16 __U, __m256h __A,
   _MM_MANTISSA_NORM_ENUM __B,
   _MM_MANTISSA_SIGN_ENUM __C)
{
  return (__m256h) __builtin_ia32_getmantph256_mask ((__v16hf) __A,
           (__C << 2) | __B,
           (__v16hf) __W,
           (__mmask16) __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_getmant_ph (__mmask16 __U, __m256h __A,
    _MM_MANTISSA_NORM_ENUM __B,
    _MM_MANTISSA_SIGN_ENUM __C)
{
  return (__m256h) __builtin_ia32_getmantph256_mask ((__v16hf) __A,
           (__C << 2) | __B,
           (__v16hf)
           _mm256_setzero_ph (),
           (__mmask16) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_getmant_ph (__m128h __A, _MM_MANTISSA_NORM_ENUM __B,
  _MM_MANTISSA_SIGN_ENUM __C)
{
  return (__m128h) __builtin_ia32_getmantph128_mask ((__v8hf) __A,
           (__C << 2) | __B,
           (__v8hf)
           _mm_setzero_ph (),
           (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_getmant_ph (__m128h __W, __mmask8 __U, __m128h __A,
       _MM_MANTISSA_NORM_ENUM __B,
       _MM_MANTISSA_SIGN_ENUM __C)
{
  return (__m128h) __builtin_ia32_getmantph128_mask ((__v8hf) __A,
           (__C << 2) | __B,
           (__v8hf) __W,
           (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_getmant_ph (__mmask8 __U, __m128h __A,
        _MM_MANTISSA_NORM_ENUM __B,
        _MM_MANTISSA_SIGN_ENUM __C)
{
  return (__m128h) __builtin_ia32_getmantph128_mask ((__v8hf) __A,
           (__C << 2) | __B,
           (__v8hf)
           _mm_setzero_ph (),
           (__mmask8) __U);
}
#else
#define _mm256_getmant_ph(X, B, C) ((__m256h) __builtin_ia32_getmantph256_mask ((__v16hf)(__m256h) (X), (int)(((C)<<2) | (B)), (__v16hf)(__m256h)_mm256_setzero_ph (), (__mmask16)-1))
#define _mm256_mask_getmant_ph(W, U, X, B, C) ((__m256h) __builtin_ia32_getmantph256_mask ((__v16hf)(__m256h) (X), (int)(((C)<<2) | (B)), (__v16hf)(__m256h)(W), (__mmask16)(U)))
#define _mm256_maskz_getmant_ph(U, X, B, C) ((__m256h) __builtin_ia32_getmantph256_mask ((__v16hf)(__m256h) (X), (int)(((C)<<2) | (B)), (__v16hf)(__m256h)_mm256_setzero_ph (), (__mmask16)(U)))
#define _mm_getmant_ph(X, B, C) ((__m128h) __builtin_ia32_getmantph128_mask ((__v8hf)(__m128h) (X), (int)(((C)<<2) | (B)), (__v8hf)(__m128h)_mm_setzero_ph (), (__mmask8)-1))
#define _mm_mask_getmant_ph(W, U, X, B, C) ((__m128h) __builtin_ia32_getmantph128_mask ((__v8hf)(__m128h) (X), (int)(((C)<<2) | (B)), (__v8hf)(__m128h)(W), (__mmask8)(U)))
#define _mm_maskz_getmant_ph(U, X, B, C) ((__m128h) __builtin_ia32_getmantph128_mask ((__v8hf)(__m128h) (X), (int)(((C)<<2) | (B)), (__v8hf)(__m128h)_mm_setzero_ph (), (__mmask8)(U)))
#endif
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtph_epi32 (__m128h __A)
{
  return (__m128i)
    __builtin_ia32_vcvtph2dq128_mask (__A,
          (__v4si)
          _mm_avx512_setzero_si128 (),
          (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtph_epi32 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return (__m128i)
    __builtin_ia32_vcvtph2dq128_mask (__C, ( __v4si) __A, __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtph_epi32 (__mmask8 __A, __m128h __B)
{
  return (__m128i)
    __builtin_ia32_vcvtph2dq128_mask (__B,
          (__v4si) _mm_avx512_setzero_si128 (),
          __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtph_epi32 (__m128h __A)
{
  return (__m256i)
    __builtin_ia32_vcvtph2dq256_mask (__A,
          (__v8si)
          _mm256_avx512_setzero_si256 (),
          (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtph_epi32 (__m256i __A, __mmask8 __B, __m128h __C)
{
  return (__m256i)
    __builtin_ia32_vcvtph2dq256_mask (__C, ( __v8si) __A, __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtph_epi32 (__mmask8 __A, __m128h __B)
{
  return (__m256i)
    __builtin_ia32_vcvtph2dq256_mask (__B,
          (__v8si)
          _mm256_avx512_setzero_si256 (),
          __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtph_epu32 (__m128h __A)
{
  return (__m128i)
    __builtin_ia32_vcvtph2udq128_mask (__A,
           (__v4si)
           _mm_avx512_setzero_si128 (),
           (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtph_epu32 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return (__m128i)
    __builtin_ia32_vcvtph2udq128_mask (__C, ( __v4si) __A, __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtph_epu32 (__mmask8 __A, __m128h __B)
{
  return (__m128i)
    __builtin_ia32_vcvtph2udq128_mask (__B,
           (__v4si)
           _mm_avx512_setzero_si128 (),
           __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtph_epu32 (__m128h __A)
{
  return (__m256i)
    __builtin_ia32_vcvtph2udq256_mask (__A,
           (__v8si)
           _mm256_avx512_setzero_si256 (),
           (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtph_epu32 (__m256i __A, __mmask8 __B, __m128h __C)
{
  return (__m256i)
    __builtin_ia32_vcvtph2udq256_mask (__C, ( __v8si) __A, __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtph_epu32 (__mmask8 __A, __m128h __B)
{
  return (__m256i)
    __builtin_ia32_vcvtph2udq256_mask (__B,
           (__v8si) _mm256_avx512_setzero_si256 (),
           __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttph_epi32 (__m128h __A)
{
  return (__m128i)
    __builtin_ia32_vcvttph2dq128_mask (__A,
           (__v4si) _mm_avx512_setzero_si128 (),
           (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvttph_epi32 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return (__m128i)__builtin_ia32_vcvttph2dq128_mask (__C,
           ( __v4si) __A,
           __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvttph_epi32 (__mmask8 __A, __m128h __B)
{
  return (__m128i)
    __builtin_ia32_vcvttph2dq128_mask (__B,
           (__v4si) _mm_avx512_setzero_si128 (),
           __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvttph_epi32 (__m128h __A)
{
  return (__m256i)
    __builtin_ia32_vcvttph2dq256_mask (__A,
           (__v8si)
           _mm256_avx512_setzero_si256 (),
           (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvttph_epi32 (__m256i __A, __mmask8 __B, __m128h __C)
{
  return (__m256i)
    __builtin_ia32_vcvttph2dq256_mask (__C,
           ( __v8si) __A,
           __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvttph_epi32 (__mmask8 __A, __m128h __B)
{
  return (__m256i)
    __builtin_ia32_vcvttph2dq256_mask (__B,
           (__v8si)
           _mm256_avx512_setzero_si256 (),
           __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttph_epu32 (__m128h __A)
{
  return (__m128i)
    __builtin_ia32_vcvttph2udq128_mask (__A,
     (__v4si)
     _mm_avx512_setzero_si128 (),
     (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvttph_epu32 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return (__m128i)
    __builtin_ia32_vcvttph2udq128_mask (__C,
     ( __v4si) __A,
     __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvttph_epu32 (__mmask8 __A, __m128h __B)
{
  return (__m128i)
    __builtin_ia32_vcvttph2udq128_mask (__B,
     (__v4si)
     _mm_avx512_setzero_si128 (),
     __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvttph_epu32 (__m128h __A)
{
  return (__m256i)
    __builtin_ia32_vcvttph2udq256_mask (__A,
     (__v8si)
     _mm256_avx512_setzero_si256 (), (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvttph_epu32 (__m256i __A, __mmask8 __B, __m128h __C)
{
  return (__m256i)
    __builtin_ia32_vcvttph2udq256_mask (__C,
     ( __v8si) __A,
     __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvttph_epu32 (__mmask8 __A, __m128h __B)
{
  return (__m256i)
    __builtin_ia32_vcvttph2udq256_mask (__B,
     (__v8si)
     _mm256_avx512_setzero_si256 (),
     __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtepi32_ph (__m128i __A)
{
  return __builtin_ia32_vcvtdq2ph128_mask ((__v4si) __A,
        _mm_setzero_ph (),
        (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtepi32_ph (__m128h __A, __mmask8 __B, __m128i __C)
{
  return __builtin_ia32_vcvtdq2ph128_mask ((__v4si) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtepi32_ph (__mmask8 __A, __m128i __B)
{
  return __builtin_ia32_vcvtdq2ph128_mask ((__v4si) __B,
        _mm_setzero_ph (),
        __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtepi32_ph (__m256i __A)
{
  return __builtin_ia32_vcvtdq2ph256_mask ((__v8si) __A,
        _mm_setzero_ph (),
        (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtepi32_ph (__m128h __A, __mmask8 __B, __m256i __C)
{
  return __builtin_ia32_vcvtdq2ph256_mask ((__v8si) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtepi32_ph (__mmask8 __A, __m256i __B)
{
  return __builtin_ia32_vcvtdq2ph256_mask ((__v8si) __B,
        _mm_setzero_ph (),
        __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtepu32_ph (__m128i __A)
{
  return __builtin_ia32_vcvtudq2ph128_mask ((__v4si) __A,
         _mm_setzero_ph (),
         (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtepu32_ph (__m128h __A, __mmask8 __B, __m128i __C)
{
  return __builtin_ia32_vcvtudq2ph128_mask ((__v4si) __C,
         __A,
         __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtepu32_ph (__mmask8 __A, __m128i __B)
{
  return __builtin_ia32_vcvtudq2ph128_mask ((__v4si) __B,
         _mm_setzero_ph (),
         __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtepu32_ph (__m256i __A)
{
  return __builtin_ia32_vcvtudq2ph256_mask ((__v8si) __A,
         _mm_setzero_ph (),
         (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtepu32_ph (__m128h __A, __mmask8 __B, __m256i __C)
{
  return __builtin_ia32_vcvtudq2ph256_mask ((__v8si) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtepu32_ph (__mmask8 __A, __m256i __B)
{
  return __builtin_ia32_vcvtudq2ph256_mask ((__v8si) __B,
         _mm_setzero_ph (),
         __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtph_epi64 (__m128h __A)
{
  return
    __builtin_ia32_vcvtph2qq128_mask (__A,
          _mm_avx512_setzero_si128 (),
          (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtph_epi64 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvtph2qq128_mask (__C, __A, __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtph_epi64 (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvtph2qq128_mask (__B,
        _mm_avx512_setzero_si128 (),
        __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtph_epi64 (__m128h __A)
{
  return __builtin_ia32_vcvtph2qq256_mask (__A,
        _mm256_avx512_setzero_si256 (),
        (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtph_epi64 (__m256i __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvtph2qq256_mask (__C, __A, __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtph_epi64 (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvtph2qq256_mask (__B,
        _mm256_avx512_setzero_si256 (),
        __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtph_epu64 (__m128h __A)
{
  return __builtin_ia32_vcvtph2uqq128_mask (__A,
         _mm_avx512_setzero_si128 (),
         (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtph_epu64 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvtph2uqq128_mask (__C, __A, __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtph_epu64 (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvtph2uqq128_mask (__B,
         _mm_avx512_setzero_si128 (),
         __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtph_epu64 (__m128h __A)
{
  return __builtin_ia32_vcvtph2uqq256_mask (__A,
         _mm256_avx512_setzero_si256 (),
         (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtph_epu64 (__m256i __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvtph2uqq256_mask (__C, __A, __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtph_epu64 (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvtph2uqq256_mask (__B,
         _mm256_avx512_setzero_si256 (),
         __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttph_epi64 (__m128h __A)
{
  return __builtin_ia32_vcvttph2qq128_mask (__A,
         _mm_avx512_setzero_si128 (),
         (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvttph_epi64 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvttph2qq128_mask (__C,
         __A,
         __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvttph_epi64 (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvttph2qq128_mask (__B,
         _mm_avx512_setzero_si128 (),
         __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvttph_epi64 (__m128h __A)
{
  return __builtin_ia32_vcvttph2qq256_mask (__A,
         _mm256_avx512_setzero_si256 (),
         (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvttph_epi64 (__m256i __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvttph2qq256_mask (__C,
         __A,
         __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvttph_epi64 (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvttph2qq256_mask (__B,
         _mm256_avx512_setzero_si256 (),
         __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttph_epu64 (__m128h __A)
{
  return __builtin_ia32_vcvttph2uqq128_mask (__A,
          _mm_avx512_setzero_si128 (),
          (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvttph_epu64 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvttph2uqq128_mask (__C,
          __A,
          __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvttph_epu64 (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvttph2uqq128_mask (__B,
          _mm_avx512_setzero_si128 (),
          __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvttph_epu64 (__m128h __A)
{
  return __builtin_ia32_vcvttph2uqq256_mask (__A,
          _mm256_avx512_setzero_si256 (),
          (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvttph_epu64 (__m256i __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvttph2uqq256_mask (__C,
          __A,
          __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvttph_epu64 (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvttph2uqq256_mask (__B,
          _mm256_avx512_setzero_si256 (),
          __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtepi64_ph (__m128i __A)
{
  return __builtin_ia32_vcvtqq2ph128_mask ((__v2di) __A,
        _mm_setzero_ph (),
        (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtepi64_ph (__m128h __A, __mmask8 __B, __m128i __C)
{
  return __builtin_ia32_vcvtqq2ph128_mask ((__v2di) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtepi64_ph (__mmask8 __A, __m128i __B)
{
  return __builtin_ia32_vcvtqq2ph128_mask ((__v2di) __B,
        _mm_setzero_ph (),
        __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtepi64_ph (__m256i __A)
{
  return __builtin_ia32_vcvtqq2ph256_mask ((__v4di) __A,
        _mm_setzero_ph (),
        (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtepi64_ph (__m128h __A, __mmask8 __B, __m256i __C)
{
  return __builtin_ia32_vcvtqq2ph256_mask ((__v4di) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtepi64_ph (__mmask8 __A, __m256i __B)
{
  return __builtin_ia32_vcvtqq2ph256_mask ((__v4di) __B,
        _mm_setzero_ph (),
        __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtepu64_ph (__m128i __A)
{
  return __builtin_ia32_vcvtuqq2ph128_mask ((__v2di) __A,
         _mm_setzero_ph (),
         (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtepu64_ph (__m128h __A, __mmask8 __B, __m128i __C)
{
  return __builtin_ia32_vcvtuqq2ph128_mask ((__v2di) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtepu64_ph (__mmask8 __A, __m128i __B)
{
  return __builtin_ia32_vcvtuqq2ph128_mask ((__v2di) __B,
         _mm_setzero_ph (),
         __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtepu64_ph (__m256i __A)
{
  return __builtin_ia32_vcvtuqq2ph256_mask ((__v4di) __A,
         _mm_setzero_ph (),
         (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtepu64_ph (__m128h __A, __mmask8 __B, __m256i __C)
{
  return __builtin_ia32_vcvtuqq2ph256_mask ((__v4di) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtepu64_ph (__mmask8 __A, __m256i __B)
{
  return __builtin_ia32_vcvtuqq2ph256_mask ((__v4di) __B,
         _mm_setzero_ph (),
         __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtph_epi16 (__m128h __A)
{
  return (__m128i)
    __builtin_ia32_vcvtph2w128_mask (__A,
         (__v8hi)
         _mm_avx512_setzero_si128 (),
         (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtph_epi16 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return (__m128i)
    __builtin_ia32_vcvtph2w128_mask (__C, ( __v8hi) __A, __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtph_epi16 (__mmask8 __A, __m128h __B)
{
  return (__m128i)
    __builtin_ia32_vcvtph2w128_mask (__B,
         (__v8hi)
         _mm_avx512_setzero_si128 (),
         __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtph_epi16 (__m256h __A)
{
  return (__m256i)
    __builtin_ia32_vcvtph2w256_mask (__A,
         (__v16hi)
         _mm256_avx512_setzero_si256 (),
         (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtph_epi16 (__m256i __A, __mmask16 __B, __m256h __C)
{
  return (__m256i)
    __builtin_ia32_vcvtph2w256_mask (__C, ( __v16hi) __A, __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtph_epi16 (__mmask16 __A, __m256h __B)
{
  return (__m256i)
    __builtin_ia32_vcvtph2w256_mask (__B,
         (__v16hi)
         _mm256_avx512_setzero_si256 (),
         __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtph_epu16 (__m128h __A)
{
  return (__m128i)
    __builtin_ia32_vcvtph2uw128_mask (__A,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtph_epu16 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return (__m128i)
    __builtin_ia32_vcvtph2uw128_mask (__C, ( __v8hi) __A, __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtph_epu16 (__mmask8 __A, __m128h __B)
{
  return (__m128i)
    __builtin_ia32_vcvtph2uw128_mask (__B,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtph_epu16 (__m256h __A)
{
  return (__m256i)
    __builtin_ia32_vcvtph2uw256_mask (__A,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtph_epu16 (__m256i __A, __mmask16 __B, __m256h __C)
{
  return (__m256i)
    __builtin_ia32_vcvtph2uw256_mask (__C, ( __v16hi) __A, __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtph_epu16 (__mmask16 __A, __m256h __B)
{
  return (__m256i)
    __builtin_ia32_vcvtph2uw256_mask (__B,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttph_epi16 (__m128h __A)
{
  return (__m128i)
    __builtin_ia32_vcvttph2w128_mask (__A,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvttph_epi16 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return (__m128i)
    __builtin_ia32_vcvttph2w128_mask (__C,
          ( __v8hi) __A,
          __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvttph_epi16 (__mmask8 __A, __m128h __B)
{
  return (__m128i)
    __builtin_ia32_vcvttph2w128_mask (__B,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvttph_epi16 (__m256h __A)
{
  return (__m256i)
    __builtin_ia32_vcvttph2w256_mask (__A,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvttph_epi16 (__m256i __A, __mmask16 __B, __m256h __C)
{
  return (__m256i)
    __builtin_ia32_vcvttph2w256_mask (__C,
          ( __v16hi) __A,
          __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvttph_epi16 (__mmask16 __A, __m256h __B)
{
  return (__m256i)
    __builtin_ia32_vcvttph2w256_mask (__B,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttph_epu16 (__m128h __A)
{
  return (__m128i)
    __builtin_ia32_vcvttph2uw128_mask (__A,
           (__v8hi)
           _mm_avx512_setzero_si128 (),
           (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvttph_epu16 (__m128i __A, __mmask8 __B, __m128h __C)
{
  return (__m128i)
    __builtin_ia32_vcvttph2uw128_mask (__C,
           ( __v8hi) __A,
           __B);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvttph_epu16 (__mmask8 __A, __m128h __B)
{
  return (__m128i)
    __builtin_ia32_vcvttph2uw128_mask (__B,
           (__v8hi)
           _mm_avx512_setzero_si128 (),
           __A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvttph_epu16 (__m256h __A)
{
  return (__m256i)
    __builtin_ia32_vcvttph2uw256_mask (__A,
           (__v16hi)
           _mm256_avx512_setzero_si256 (),
           (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvttph_epu16 (__m256i __A, __mmask16 __B, __m256h __C)
{
  return (__m256i)
    __builtin_ia32_vcvttph2uw256_mask (__C,
           ( __v16hi) __A,
           __B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvttph_epu16 (__mmask16 __A, __m256h __B)
{
  return (__m256i)
    __builtin_ia32_vcvttph2uw256_mask (__B,
           (__v16hi) _mm256_avx512_setzero_si256 (),
           __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtepi16_ph (__m128i __A)
{
  return __builtin_ia32_vcvtw2ph128_mask ((__v8hi) __A,
       _mm_setzero_ph (),
       (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtepi16_ph (__m128h __A, __mmask8 __B, __m128i __C)
{
  return __builtin_ia32_vcvtw2ph128_mask ((__v8hi) __C,
       __A,
       __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtepi16_ph (__mmask8 __A, __m128i __B)
{
  return __builtin_ia32_vcvtw2ph128_mask ((__v8hi) __B,
       _mm_setzero_ph (),
       __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtepi16_ph (__m256i __A)
{
  return __builtin_ia32_vcvtw2ph256_mask ((__v16hi) __A,
       _mm256_setzero_ph (),
       (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtepi16_ph (__m256h __A, __mmask16 __B, __m256i __C)
{
  return __builtin_ia32_vcvtw2ph256_mask ((__v16hi) __C,
       __A,
       __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtepi16_ph (__mmask16 __A, __m256i __B)
{
  return __builtin_ia32_vcvtw2ph256_mask ((__v16hi) __B,
       _mm256_setzero_ph (),
       __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtepu16_ph (__m128i __A)
{
  return __builtin_ia32_vcvtuw2ph128_mask ((__v8hi) __A,
        _mm_setzero_ph (),
        (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtepu16_ph (__m128h __A, __mmask8 __B, __m128i __C)
{
  return __builtin_ia32_vcvtuw2ph128_mask ((__v8hi) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtepu16_ph (__mmask8 __A, __m128i __B)
{
  return __builtin_ia32_vcvtuw2ph128_mask ((__v8hi) __B,
        _mm_setzero_ph (),
        __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtepu16_ph (__m256i __A)
{
  return __builtin_ia32_vcvtuw2ph256_mask ((__v16hi) __A,
        _mm256_setzero_ph (),
        (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtepu16_ph (__m256h __A, __mmask16 __B, __m256i __C)
{
  return __builtin_ia32_vcvtuw2ph256_mask ((__v16hi) __C, __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtepu16_ph (__mmask16 __A, __m256i __B)
{
  return __builtin_ia32_vcvtuw2ph256_mask ((__v16hi) __B,
        _mm256_setzero_ph (),
        __A);
}
extern __inline __m128d
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtph_pd (__m128h __A)
{
  return __builtin_ia32_vcvtph2pd128_mask (__A,
        _mm_avx512_setzero_pd (),
        (__mmask8) -1);
}
extern __inline __m128d
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtph_pd (__m128d __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvtph2pd128_mask (__C, __A, __B);
}
extern __inline __m128d
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtph_pd (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvtph2pd128_mask (__B, _mm_avx512_setzero_pd (), __A);
}
extern __inline __m256d
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtph_pd (__m128h __A)
{
  return __builtin_ia32_vcvtph2pd256_mask (__A,
        _mm256_avx512_setzero_pd (),
        (__mmask8) -1);
}
extern __inline __m256d
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtph_pd (__m256d __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvtph2pd256_mask (__C, __A, __B);
}
extern __inline __m256d
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtph_pd (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvtph2pd256_mask (__B,
        _mm256_avx512_setzero_pd (),
        __A);
}
extern __inline __m128
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtxph_ps (__m128h __A)
{
  return __builtin_ia32_vcvtph2psx128_mask (__A,
        _mm_avx512_setzero_ps (),
        (__mmask8) -1);
}
extern __inline __m128
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtxph_ps (__m128 __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvtph2psx128_mask (__C, __A, __B);
}
extern __inline __m128
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtxph_ps (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvtph2psx128_mask (__B, _mm_avx512_setzero_ps (), __A);
}
extern __inline __m256
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtxph_ps (__m128h __A)
{
  return __builtin_ia32_vcvtph2psx256_mask (__A,
         _mm256_avx512_setzero_ps (),
         (__mmask8) -1);
}
extern __inline __m256
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtxph_ps (__m256 __A, __mmask8 __B, __m128h __C)
{
  return __builtin_ia32_vcvtph2psx256_mask (__C, __A, __B);
}
extern __inline __m256
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtxph_ps (__mmask8 __A, __m128h __B)
{
  return __builtin_ia32_vcvtph2psx256_mask (__B,
         _mm256_avx512_setzero_ps (),
         __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtxps_ph (__m128 __A)
{
  return __builtin_ia32_vcvtps2phx128_mask ((__v4sf) __A,
         _mm_setzero_ph (),
         (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtxps_ph (__m128h __A, __mmask8 __B, __m128 __C)
{
  return __builtin_ia32_vcvtps2phx128_mask ((__v4sf) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtxps_ph (__mmask8 __A, __m128 __B)
{
  return __builtin_ia32_vcvtps2phx128_mask ((__v4sf) __B,
         _mm_setzero_ph (),
         __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtxps_ph (__m256 __A)
{
  return __builtin_ia32_vcvtps2phx256_mask ((__v8sf) __A,
         _mm_setzero_ph (),
         (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtxps_ph (__m128h __A, __mmask8 __B, __m256 __C)
{
  return __builtin_ia32_vcvtps2phx256_mask ((__v8sf) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtxps_ph (__mmask8 __A, __m256 __B)
{
  return __builtin_ia32_vcvtps2phx256_mask ((__v8sf) __B,
         _mm_setzero_ph (),
         __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpd_ph (__m128d __A)
{
  return __builtin_ia32_vcvtpd2ph128_mask ((__v2df) __A,
        _mm_setzero_ph (),
        (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtpd_ph (__m128h __A, __mmask8 __B, __m128d __C)
{
  return __builtin_ia32_vcvtpd2ph128_mask ((__v2df) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtpd_ph (__mmask8 __A, __m128d __B)
{
  return __builtin_ia32_vcvtpd2ph128_mask ((__v2df) __B,
        _mm_setzero_ph (),
        __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtpd_ph (__m256d __A)
{
  return __builtin_ia32_vcvtpd2ph256_mask ((__v4df) __A,
        _mm_setzero_ph (),
        (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtpd_ph (__m128h __A, __mmask8 __B, __m256d __C)
{
  return __builtin_ia32_vcvtpd2ph256_mask ((__v4df) __C, __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtpd_ph (__mmask8 __A, __m256d __B)
{
  return __builtin_ia32_vcvtpd2ph256_mask ((__v4df) __B,
        _mm_setzero_ph (),
        __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmaddsub_ph (__m256h __A, __m256h __B, __m256h __C)
{
  return (__m256h)__builtin_ia32_vfmaddsubph256_mask ((__v16hf)__A,
            (__v16hf)__B,
            (__v16hf)__C,
            (__mmask16)-1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fmaddsub_ph (__m256h __A, __mmask16 __U, __m256h __B,
    __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmaddsubph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask3_fmaddsub_ph (__m256h __A, __m256h __B, __m256h __C,
     __mmask16 __U)
{
  return (__m256h) __builtin_ia32_vfmaddsubph256_mask3 ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_fmaddsub_ph (__mmask16 __U, __m256h __A, __m256h __B,
     __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmaddsubph256_maskz ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmaddsub_ph (__m128h __A, __m128h __B, __m128h __C)
{
  return (__m128h)__builtin_ia32_vfmaddsubph128_mask ((__v8hf)__A,
            (__v8hf)__B,
            (__v8hf)__C,
            (__mmask8)-1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_fmaddsub_ph (__m128h __A, __mmask8 __U, __m128h __B,
        __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmaddsubph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask3_fmaddsub_ph (__m128h __A, __m128h __B, __m128h __C,
         __mmask8 __U)
{
  return (__m128h) __builtin_ia32_vfmaddsubph128_mask3 ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_fmaddsub_ph (__mmask8 __U, __m128h __A, __m128h __B,
         __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmaddsubph128_maskz ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmsubadd_ph (__m256h __A, __m256h __B, __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmsubaddph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fmsubadd_ph (__m256h __A, __mmask16 __U, __m256h __B,
    __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmsubaddph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask3_fmsubadd_ph (__m256h __A, __m256h __B, __m256h __C,
     __mmask16 __U)
{
  return (__m256h) __builtin_ia32_vfmsubaddph256_mask3 ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_fmsubadd_ph (__mmask16 __U, __m256h __A, __m256h __B,
     __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmsubaddph256_maskz ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmsubadd_ph (__m128h __A, __m128h __B, __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmsubaddph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_fmsubadd_ph (__m128h __A, __mmask8 __U, __m128h __B,
        __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmsubaddph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask3_fmsubadd_ph (__m128h __A, __m128h __B, __m128h __C,
         __mmask8 __U)
{
  return (__m128h) __builtin_ia32_vfmsubaddph128_mask3 ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_fmsubadd_ph (__mmask8 __U, __m128h __A, __m128h __B,
         __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmsubaddph128_maskz ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmadd_ph (__m256h __A, __m256h __B, __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmaddph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fmadd_ph (__m256h __A, __mmask16 __U, __m256h __B,
    __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmaddph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask3_fmadd_ph (__m256h __A, __m256h __B, __m256h __C,
     __mmask16 __U)
{
  return (__m256h) __builtin_ia32_vfmaddph256_mask3 ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_fmadd_ph (__mmask16 __U, __m256h __A, __m256h __B,
     __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmaddph256_maskz ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmadd_ph (__m128h __A, __m128h __B, __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmaddph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_fmadd_ph (__m128h __A, __mmask8 __U, __m128h __B,
        __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmaddph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask3_fmadd_ph (__m128h __A, __m128h __B, __m128h __C,
         __mmask8 __U)
{
  return (__m128h) __builtin_ia32_vfmaddph128_mask3 ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_fmadd_ph (__mmask8 __U, __m128h __A, __m128h __B,
         __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmaddph128_maskz ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fnmadd_ph (__m256h __A, __m256h __B, __m256h __C)
{
  return (__m256h) __builtin_ia32_vfnmaddph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fnmadd_ph (__m256h __A, __mmask16 __U, __m256h __B,
    __m256h __C)
{
  return (__m256h) __builtin_ia32_vfnmaddph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask3_fnmadd_ph (__m256h __A, __m256h __B, __m256h __C,
     __mmask16 __U)
{
  return (__m256h) __builtin_ia32_vfnmaddph256_mask3 ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_fnmadd_ph (__mmask16 __U, __m256h __A, __m256h __B,
     __m256h __C)
{
  return (__m256h) __builtin_ia32_vfnmaddph256_maskz ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fnmadd_ph (__m128h __A, __m128h __B, __m128h __C)
{
  return (__m128h) __builtin_ia32_vfnmaddph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_fnmadd_ph (__m128h __A, __mmask8 __U, __m128h __B,
        __m128h __C)
{
  return (__m128h) __builtin_ia32_vfnmaddph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask3_fnmadd_ph (__m128h __A, __m128h __B, __m128h __C,
         __mmask8 __U)
{
  return (__m128h) __builtin_ia32_vfnmaddph128_mask3 ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_fnmadd_ph (__mmask8 __U, __m128h __A, __m128h __B,
         __m128h __C)
{
  return (__m128h) __builtin_ia32_vfnmaddph128_maskz ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmsub_ph (__m256h __A, __m256h __B, __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmsubph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fmsub_ph (__m256h __A, __mmask16 __U, __m256h __B,
    __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmsubph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask3_fmsub_ph (__m256h __A, __m256h __B, __m256h __C,
     __mmask16 __U)
{
  return (__m256h) __builtin_ia32_vfmsubph256_mask3 ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_fmsub_ph (__mmask16 __U, __m256h __A, __m256h __B,
     __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmsubph256_maskz ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmsub_ph (__m128h __A, __m128h __B, __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmsubph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_fmsub_ph (__m128h __A, __mmask8 __U, __m128h __B,
        __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmsubph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask3_fmsub_ph (__m128h __A, __m128h __B, __m128h __C,
         __mmask8 __U)
{
  return (__m128h) __builtin_ia32_vfmsubph128_mask3 ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_fmsub_ph (__mmask8 __U, __m128h __A, __m128h __B,
         __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmsubph128_maskz ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fnmsub_ph (__m256h __A, __m256h __B, __m256h __C)
{
  return (__m256h) __builtin_ia32_vfnmsubph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) -1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fnmsub_ph (__m256h __A, __mmask16 __U, __m256h __B,
    __m256h __C)
{
  return (__m256h) __builtin_ia32_vfnmsubph256_mask ((__v16hf) __A,
             (__v16hf) __B,
             (__v16hf) __C,
             (__mmask16) __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask3_fnmsub_ph (__m256h __A, __m256h __B, __m256h __C,
     __mmask16 __U)
{
  return (__m256h) __builtin_ia32_vfnmsubph256_mask3 ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_fnmsub_ph (__mmask16 __U, __m256h __A, __m256h __B,
     __m256h __C)
{
  return (__m256h) __builtin_ia32_vfnmsubph256_maskz ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C,
       (__mmask16)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fnmsub_ph (__m128h __A, __m128h __B, __m128h __C)
{
  return (__m128h) __builtin_ia32_vfnmsubph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) -1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_fnmsub_ph (__m128h __A, __mmask8 __U, __m128h __B,
        __m128h __C)
{
  return (__m128h) __builtin_ia32_vfnmsubph128_mask ((__v8hf) __A,
             (__v8hf) __B,
             (__v8hf) __C,
             (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask3_fnmsub_ph (__m128h __A, __m128h __B, __m128h __C,
         __mmask8 __U)
{
  return (__m128h) __builtin_ia32_vfnmsubph128_mask3 ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_fnmsub_ph (__mmask8 __U, __m128h __A, __m128h __B,
         __m128h __C)
{
  return (__m128h) __builtin_ia32_vfnmsubph128_maskz ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C,
       (__mmask8)
       __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmadd_pch (__m128h __A, __m128h __B, __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmaddcph128 ((__v8hf) __A,
      (__v8hf) __B,
      (__v8hf) __C);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_fmadd_pch (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return (__m128h)
    __builtin_ia32_vfmaddcph128_mask ((__v8hf) __A,
          (__v8hf) __C,
          (__v8hf) __D, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask3_fmadd_pch (__m128h __A, __m128h __B, __m128h __C, __mmask8 __D)
{
  return (__m128h)
    __builtin_ia32_vfmaddcph128_mask3 ((__v8hf) __A,
           (__v8hf) __B,
           (__v8hf) __C, __D);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_fmadd_pch (__mmask8 __A, __m128h __B, __m128h __C, __m128h __D)
{
  return (__m128h) __builtin_ia32_vfmaddcph128_maskz ((__v8hf) __B,
            (__v8hf) __C,
            (__v8hf) __D, __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmadd_pch (__m256h __A, __m256h __B, __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmaddcph256 ((__v16hf) __A,
      (__v16hf) __B,
      (__v16hf) __C);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fmadd_pch (__m256h __A, __mmask8 __B, __m256h __C, __m256h __D)
{
  return (__m256h)
     __builtin_ia32_vfmaddcph256_mask ((__v16hf) __A,
           (__v16hf) __C,
           (__v16hf) __D, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask3_fmadd_pch (__m256h __A, __m256h __B, __m256h __C, __mmask8 __D)
{
  return (__m256h)
    __builtin_ia32_vfmaddcph256_mask3 ((__v16hf) __A,
           (__v16hf) __B,
           (__v16hf) __C, __D);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_fmadd_pch (__mmask8 __A, __m256h __B, __m256h __C, __m256h __D)
{
  return (__m256h)__builtin_ia32_vfmaddcph256_maskz ((__v16hf) __B,
           (__v16hf) __C,
           (__v16hf) __D, __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fcmadd_pch (__m128h __A, __m128h __B, __m128h __C)
{
  return (__m128h) __builtin_ia32_vfcmaddcph128 ((__v8hf) __A,
       (__v8hf) __B,
       (__v8hf) __C);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_fcmadd_pch (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return (__m128h)
     __builtin_ia32_vfcmaddcph128_mask ((__v8hf) __A,
     (__v8hf) __C,
     (__v8hf) __D, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask3_fcmadd_pch (__m128h __A, __m128h __B, __m128h __C, __mmask8 __D)
{
  return (__m128h)
    __builtin_ia32_vfcmaddcph128_mask3 ((__v8hf) __A,
     (__v8hf) __B,
     (__v8hf) __C, __D);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_fcmadd_pch (__mmask8 __A, __m128h __B, __m128h __C, __m128h __D)
{
  return (__m128h)__builtin_ia32_vfcmaddcph128_maskz ((__v8hf) __B,
            (__v8hf) __C,
            (__v8hf) __D, __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fcmadd_pch (__m256h __A, __m256h __B, __m256h __C)
{
  return (__m256h) __builtin_ia32_vfcmaddcph256 ((__v16hf) __A,
       (__v16hf) __B,
       (__v16hf) __C);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fcmadd_pch (__m256h __A, __mmask8 __B, __m256h __C, __m256h __D)
{
  return (__m256h)
     __builtin_ia32_vfcmaddcph256_mask ((__v16hf) __A,
     (__v16hf) __C,
     (__v16hf) __D, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask3_fcmadd_pch (__m256h __A, __m256h __B, __m256h __C, __mmask8 __D)
{
  return (__m256h)
    __builtin_ia32_vfcmaddcph256_mask3 ((__v16hf) __A,
     (__v16hf) __B,
     (__v16hf) __C, __D);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_fcmadd_pch (__mmask8 __A, __m256h __B, __m256h __C, __m256h __D)
{
  return (__m256h) __builtin_ia32_vfcmaddcph256_maskz ((__v16hf) __B,
             (__v16hf) __C,
             (__v16hf) __D, __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fmul_pch (__m128h __A, __m128h __B)
{
  return (__m128h) __builtin_ia32_vfmulcph128 ((__v8hf) __A, (__v8hf) __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_fmul_pch (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return (__m128h) __builtin_ia32_vfmulcph128_mask ((__v8hf) __C,
          (__v8hf) __D,
          (__v8hf) __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_fmul_pch (__mmask8 __A, __m128h __B, __m128h __C)
{
  return (__m128h) __builtin_ia32_vfmulcph128_mask ((__v8hf) __B,
          (__v8hf) __C,
          _mm_setzero_ph (),
          __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fmul_pch (__m256h __A, __m256h __B)
{
  return (__m256h) __builtin_ia32_vfmulcph256 ((__v16hf) __A,
            (__v16hf) __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fmul_pch (__m256h __A, __mmask8 __B, __m256h __C, __m256h __D)
{
  return (__m256h) __builtin_ia32_vfmulcph256_mask ((__v16hf) __C,
          (__v16hf) __D,
          (__v16hf) __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_fmul_pch (__mmask8 __A, __m256h __B, __m256h __C)
{
  return (__m256h) __builtin_ia32_vfmulcph256_mask ((__v16hf) __B,
          (__v16hf) __C,
          _mm256_setzero_ph (),
          __A);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_fcmul_pch (__m128h __A, __m128h __B)
{
  return (__m128h) __builtin_ia32_vfcmulcph128 ((__v8hf) __A,
      (__v8hf) __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_fcmul_pch (__m128h __A, __mmask8 __B, __m128h __C, __m128h __D)
{
  return (__m128h) __builtin_ia32_vfcmulcph128_mask ((__v8hf) __C,
           (__v8hf) __D,
           (__v8hf) __A, __B);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_fcmul_pch (__mmask8 __A, __m128h __B, __m128h __C)
{
  return (__m128h) __builtin_ia32_vfcmulcph128_mask ((__v8hf) __B,
           (__v8hf) __C,
           _mm_setzero_ph (),
           __A);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_fcmul_pch (__m256h __A, __m256h __B)
{
  return (__m256h) __builtin_ia32_vfcmulcph256 ((__v16hf) __A, (__v16hf) __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_fcmul_pch (__m256h __A, __mmask8 __B, __m256h __C, __m256h __D)
{
  return (__m256h) __builtin_ia32_vfcmulcph256_mask ((__v16hf) __C,
           (__v16hf) __D,
           (__v16hf) __A, __B);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_fcmul_pch (__mmask8 __A, __m256h __B, __m256h __C)
{
  return (__m256h) __builtin_ia32_vfcmulcph256_mask ((__v16hf) __B,
           (__v16hf) __C,
           _mm256_setzero_ph (),
           __A);
}
#define _MM256_REDUCE_OP(op) __m128h __T1 = (__m128h) _mm256_avx512_extractf128_pd ((__m256d) __A, 0); __m128h __T2 = (__m128h) _mm256_avx512_extractf128_pd ((__m256d) __A, 1); __m128h __T3 = (__T1 op __T2); __m128h __T4 = (__m128h) __builtin_shuffle (__T3, (__v8hi) { 4, 5, 6, 7, 0, 1, 2, 3 }); __m128h __T5 = (__T3) op (__T4); __m128h __T6 = (__m128h) __builtin_shuffle (__T5, (__v8hi) { 2, 3, 0, 1, 4, 5, 6, 7 }); __m128h __T7 = __T5 op __T6; return __T7[0] op __T7[1]
extern __inline _Float16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_reduce_add_ph (__m256h __A)
{
  _MM256_REDUCE_OP (+);
}
extern __inline _Float16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_reduce_mul_ph (__m256h __A)
{
  _MM256_REDUCE_OP (*);
}
#undef _MM256_REDUCE_OP
#define _MM256_REDUCE_OP(op) __m128h __T1 = (__m128h) _mm256_avx512_extractf128_pd ((__m256d) __A, 0); __m128h __T2 = (__m128h) _mm256_avx512_extractf128_pd ((__m256d) __A, 1); __m128h __T3 = _mm_##op (__T1, __T2); __m128h __T4 = (__m128h) __builtin_shuffle (__T3, (__v8hi) { 2, 3, 0, 1, 6, 7, 4, 5 }); __m128h __T5 = _mm_##op (__T3, __T4); __m128h __T6 = (__m128h) __builtin_shuffle (__T5, (__v8hi) { 4, 5 }); __m128h __T7 = _mm_##op (__T5, __T6); __m128h __T8 = (__m128h) __builtin_shuffle (__T7, (__v8hi) { 1, 0 }); __m128h __T9 = _mm_##op (__T7, __T8); return __T9[0]
extern __inline _Float16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_reduce_min_ph (__m256h __A)
{
  _MM256_REDUCE_OP (min_ph);
}
extern __inline _Float16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_reduce_max_ph (__m256h __A)
{
  _MM256_REDUCE_OP (max_ph);
}
#define _MM_REDUCE_OP(op) __m128h __T1 = (__m128h) __builtin_shuffle (__A, (__v8hi) { 4, 5, 6, 7, 0, 1, 2, 3 }); __m128h __T2 = (__A) op (__T1); __m128h __T3 = (__m128h) __builtin_shuffle (__T2, (__v8hi){ 2, 3, 0, 1, 4, 5, 6, 7 }); __m128h __T4 = __T2 op __T3; return __T4[0] op __T4[1]
extern __inline _Float16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_reduce_add_ph (__m128h __A)
{
  _MM_REDUCE_OP (+);
}
extern __inline _Float16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_reduce_mul_ph (__m128h __A)
{
  _MM_REDUCE_OP (*);
}
#undef _MM_REDUCE_OP
#define _MM_REDUCE_OP(op) __m128h __T1 = (__m128h) __builtin_shuffle (__A, (__v8hi) { 2, 3, 0, 1, 6, 7, 4, 5 }); __m128h __T2 = _mm_##op (__A, __T1); __m128h __T3 = (__m128h) __builtin_shuffle (__T2, (__v8hi){ 4, 5 }); __m128h __T4 = _mm_##op (__T2, __T3); __m128h __T5 = (__m128h) __builtin_shuffle (__T4, (__v8hi){ 1, 0 }); __m128h __T6 = _mm_##op (__T4, __T5); return __T6[0]
extern __inline _Float16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_reduce_min_ph (__m128h __A)
{
  _MM_REDUCE_OP (min_ph);
}
extern __inline _Float16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_reduce_max_ph (__m128h __A)
{
  _MM_REDUCE_OP (max_ph);
}
#undef _MM256_REDUCE_OP
#undef _MM_REDUCE_OP
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_blend_ph (__mmask16 __U, __m256h __A, __m256h __W)
{
  return (__m256h) __builtin_ia32_movdquhi256_mask ((__v16hi) __W,
          (__v16hi) __A,
          (__mmask16) __U);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_permutex2var_ph (__m256h __A, __m256i __I, __m256h __B)
{
  return (__m256h) __builtin_ia32_vpermi2varhi256_mask ((__v16hi) __A,
             (__v16hi) __I,
             (__v16hi) __B,
             (__mmask16)-1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_permutexvar_ph (__m256i __A, __m256h __B)
{
  return (__m256h) __builtin_ia32_permvarhi256_mask ((__v16hi) __B,
           (__v16hi) __A,
           (__v16hi)
           (_mm256_setzero_ph ()),
           (__mmask16)-1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_blend_ph (__mmask8 __U, __m128h __A, __m128h __W)
{
  return (__m128h) __builtin_ia32_movdquhi128_mask ((__v8hi) __W,
          (__v8hi) __A,
          (__mmask8) __U);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_permutex2var_ph (__m128h __A, __m128i __I, __m128h __B)
{
  return (__m128h) __builtin_ia32_vpermi2varhi128_mask ((__v8hi) __A,
             (__v8hi) __I,
             (__v8hi) __B,
             (__mmask8)-1);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_permutexvar_ph (__m128i __A, __m128h __B)
{
  return (__m128h) __builtin_ia32_permvarhi128_mask ((__v8hi) __B,
           (__v8hi) __A,
           (__v8hi)
           (_mm_setzero_ph ()),
           (__mmask8)-1);
}
extern __inline __m256h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_set1_pch (_Float16 _Complex __A)
{
  union
  {
    _Float16 _Complex __a;
    float __b;
  } __u = { .__a = __A };
  return (__m256h) _mm256_avx512_set1_ps (__u.__b);
}
extern __inline __m128h
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_pch (_Float16 _Complex __A)
{
  union
  {
    _Float16 _Complex __a;
    float __b;
  } __u = { .__a = __A };
  return (__m128h) _mm_avx512_set1_ps (__u.__b);
}
#define _mm_mul_pch(A, B) _mm_fmul_pch ((A), (B))
#define _mm_mask_mul_pch(W, U, A, B) _mm_mask_fmul_pch ((W), (U), (A), (B))
#define _mm_maskz_mul_pch(U, A, B) _mm_maskz_fmul_pch ((U), (A), (B))
#define _mm256_mul_pch(A, B) _mm256_fmul_pch ((A), (B))
#define _mm256_mask_mul_pch(W, U, A, B) _mm256_mask_fmul_pch ((W), (U), (A), (B))
#define _mm256_maskz_mul_pch(U, A, B) _mm256_maskz_fmul_pch ((U), (A), (B))
#define _mm_cmul_pch(A, B) _mm_fcmul_pch ((A), (B))
#define _mm_mask_cmul_pch(W, U, A, B) _mm_mask_fcmul_pch ((W), (U), (A), (B))
#define _mm_maskz_cmul_pch(U, A, B) _mm_maskz_fcmul_pch ((U), (A), (B))
#define _mm256_cmul_pch(A, B) _mm256_fcmul_pch ((A), (B))
#define _mm256_mask_cmul_pch(W, U, A, B) _mm256_mask_fcmul_pch ((W), (U), (A), (B))
#define _mm256_maskz_cmul_pch(U, A, B) _mm256_maskz_fcmul_pch((U), (A), (B))
#ifdef __DISABLE_AVX512FP16VL__
#undef __DISABLE_AVX512FP16VL__
#pragma GCC pop_options
#endif
#endif
#endif
