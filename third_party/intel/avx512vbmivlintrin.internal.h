#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avx512vbmivlintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVX512VBMIVLINTRIN_H_INCLUDED
#define _AVX512VBMIVLINTRIN_H_INCLUDED
#if !defined(__AVX512VL__) || !defined(__AVX512VBMI__)
#pragma GCC push_options
#pragma GCC target("avx512vbmi,avx512vl")
#define __DISABLE_AVX512VBMIVL__
#endif
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_multishift_epi64_epi8 (__m256i __W, __mmask32 __M, __m256i __X, __m256i __Y)
{
  return (__m256i) __builtin_ia32_vpmultishiftqb256_mask ((__v32qi) __X,
         (__v32qi) __Y,
         (__v32qi) __W,
         (__mmask32) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_multishift_epi64_epi8 (__mmask32 __M, __m256i __X, __m256i __Y)
{
  return (__m256i) __builtin_ia32_vpmultishiftqb256_mask ((__v32qi) __X,
         (__v32qi) __Y,
         (__v32qi)
         _mm256_setzero_si256 (),
         (__mmask32) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_multishift_epi64_epi8 (__m256i __X, __m256i __Y)
{
  return (__m256i) __builtin_ia32_vpmultishiftqb256_mask ((__v32qi) __X,
         (__v32qi) __Y,
         (__v32qi)
         _mm256_undefined_si256 (),
         (__mmask32) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_multishift_epi64_epi8 (__m128i __W, __mmask16 __M, __m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_vpmultishiftqb128_mask ((__v16qi) __X,
         (__v16qi) __Y,
         (__v16qi) __W,
         (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_multishift_epi64_epi8 (__mmask16 __M, __m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_vpmultishiftqb128_mask ((__v16qi) __X,
         (__v16qi) __Y,
         (__v16qi)
         _mm_setzero_si128 (),
         (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_multishift_epi64_epi8 (__m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_vpmultishiftqb128_mask ((__v16qi) __X,
         (__v16qi) __Y,
         (__v16qi)
         _mm_undefined_si128 (),
         (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_permutexvar_epi8 (__m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_permvarqi256_mask ((__v32qi) __B,
           (__v32qi) __A,
           (__v32qi)
           _mm256_undefined_si256 (),
           (__mmask32) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_permutexvar_epi8 (__mmask32 __M, __m256i __A,
    __m256i __B)
{
  return (__m256i) __builtin_ia32_permvarqi256_mask ((__v32qi) __B,
           (__v32qi) __A,
           (__v32qi)
           _mm256_setzero_si256 (),
           (__mmask32) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_permutexvar_epi8 (__m256i __W, __mmask32 __M, __m256i __A,
          __m256i __B)
{
  return (__m256i) __builtin_ia32_permvarqi256_mask ((__v32qi) __B,
           (__v32qi) __A,
           (__v32qi) __W,
           (__mmask32) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_permutexvar_epi8 (__m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_permvarqi128_mask ((__v16qi) __B,
           (__v16qi) __A,
           (__v16qi)
           _mm_undefined_si128 (),
           (__mmask16) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_permutexvar_epi8 (__mmask16 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_permvarqi128_mask ((__v16qi) __B,
           (__v16qi) __A,
           (__v16qi)
           _mm_setzero_si128 (),
           (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_permutexvar_epi8 (__m128i __W, __mmask16 __M, __m128i __A,
       __m128i __B)
{
  return (__m128i) __builtin_ia32_permvarqi128_mask ((__v16qi) __B,
           (__v16qi) __A,
           (__v16qi) __W,
           (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_permutex2var_epi8 (__m256i __A, __m256i __I, __m256i __B)
{
  return (__m256i) __builtin_ia32_vpermt2varqi256_mask ((__v32qi) __I
                 ,
       (__v32qi) __A,
       (__v32qi) __B,
       (__mmask32) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_permutex2var_epi8 (__m256i __A, __mmask32 __U,
    __m256i __I, __m256i __B)
{
  return (__m256i) __builtin_ia32_vpermt2varqi256_mask ((__v32qi) __I
                 ,
       (__v32qi) __A,
       (__v32qi) __B,
       (__mmask32)
       __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask2_permutex2var_epi8 (__m256i __A, __m256i __I,
     __mmask32 __U, __m256i __B)
{
  return (__m256i) __builtin_ia32_vpermi2varqi256_mask ((__v32qi) __A,
       (__v32qi) __I
                 ,
       (__v32qi) __B,
       (__mmask32)
       __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_permutex2var_epi8 (__mmask32 __U, __m256i __A,
     __m256i __I, __m256i __B)
{
  return (__m256i) __builtin_ia32_vpermt2varqi256_maskz ((__v32qi) __I
                  ,
        (__v32qi) __A,
        (__v32qi) __B,
        (__mmask32)
        __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_permutex2var_epi8 (__m128i __A, __m128i __I, __m128i __B)
{
  return (__m128i) __builtin_ia32_vpermt2varqi128_mask ((__v16qi) __I
                 ,
       (__v16qi) __A,
       (__v16qi) __B,
       (__mmask16) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_permutex2var_epi8 (__m128i __A, __mmask16 __U, __m128i __I,
        __m128i __B)
{
  return (__m128i) __builtin_ia32_vpermt2varqi128_mask ((__v16qi) __I
                 ,
       (__v16qi) __A,
       (__v16qi) __B,
       (__mmask16)
       __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask2_permutex2var_epi8 (__m128i __A, __m128i __I, __mmask16 __U,
         __m128i __B)
{
  return (__m128i) __builtin_ia32_vpermi2varqi128_mask ((__v16qi) __A,
       (__v16qi) __I
                 ,
       (__v16qi) __B,
       (__mmask16)
       __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_permutex2var_epi8 (__mmask16 __U, __m128i __A, __m128i __I,
         __m128i __B)
{
  return (__m128i) __builtin_ia32_vpermt2varqi128_maskz ((__v16qi) __I
                  ,
        (__v16qi) __A,
        (__v16qi) __B,
        (__mmask16)
        __U);
}
#ifdef __DISABLE_AVX512VBMIVL__
#undef __DISABLE_AVX512VBMIVL__
#pragma GCC pop_options
#endif
#endif
#endif
