#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _IMMINTRIN_H_INCLUDED
#error "Never use <avx512vlbwintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AVX512VLBWINTRIN_H_INCLUDED
#define _AVX512VLBWINTRIN_H_INCLUDED
#if !defined(__AVX512VL__) || !defined(__AVX512BW__) || defined (__EVEX512__)
#pragma GCC push_options
#pragma GCC target("avx512vl,avx512bw,no-evex512")
#define __DISABLE_AVX512VLBW__
#endif
typedef short __v16hi_u __attribute__ ((__vector_size__ (32), __may_alias__, __aligned__ (1)));
typedef short __v8hi_u __attribute__ ((__vector_size__ (16), __may_alias__, __aligned__ (1)));
typedef char __v32qi_u __attribute__ ((__vector_size__ (32), __may_alias__, __aligned__ (1)));
typedef char __v16qi_u __attribute__ ((__vector_size__ (16), __may_alias__, __aligned__ (1)));
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_set1_epi32 (int __A)
{
  return _mm_avx512_set_epi32 (__A, __A, __A, __A);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_set1_epi16 (short __A)
{
  return _mm_avx512_set_epi16 (__A, __A, __A, __A, __A, __A, __A, __A);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_set1_epi8 (char __A)
{
  return _mm_avx512_set_epi8 (__A, __A, __A, __A, __A, __A, __A, __A,
         __A, __A, __A, __A, __A, __A, __A, __A);
}
extern __inline __m256i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_set_epi16 (short __q15, short __q14, short __q13, short __q12,
    short __q11, short __q10, short __q09, short __q08,
    short __q07, short __q06, short __q05, short __q04,
    short __q03, short __q02, short __q01, short __q00)
{
  return __extension__ (__m256i)(__v16hi){
    __q00, __q01, __q02, __q03, __q04, __q05, __q06, __q07,
    __q08, __q09, __q10, __q11, __q12, __q13, __q14, __q15
  };
}
extern __inline __m256i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_set_epi8 (char __q31, char __q30, char __q29, char __q28,
    char __q27, char __q26, char __q25, char __q24,
    char __q23, char __q22, char __q21, char __q20,
    char __q19, char __q18, char __q17, char __q16,
    char __q15, char __q14, char __q13, char __q12,
    char __q11, char __q10, char __q09, char __q08,
    char __q07, char __q06, char __q05, char __q04,
    char __q03, char __q02, char __q01, char __q00)
{
  return __extension__ (__m256i)(__v32qi){
    __q00, __q01, __q02, __q03, __q04, __q05, __q06, __q07,
    __q08, __q09, __q10, __q11, __q12, __q13, __q14, __q15,
    __q16, __q17, __q18, __q19, __q20, __q21, __q22, __q23,
    __q24, __q25, __q26, __q27, __q28, __q29, __q30, __q31
  };
}
extern __inline __m256i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_set1_epi16 (short __A)
{
  return _mm256_avx512_set_epi16 (__A, __A, __A, __A, __A, __A, __A, __A,
      __A, __A, __A, __A, __A, __A, __A, __A);
}
extern __inline __m256i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_set1_epi32 (int __A)
{
  return __extension__ (__m256i)(__v8si){ __A, __A, __A, __A,
       __A, __A, __A, __A };
}
extern __inline __m256i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_set1_epi8 (char __A)
{
  return _mm256_avx512_set_epi8 (__A, __A, __A, __A, __A, __A, __A, __A,
     __A, __A, __A, __A, __A, __A, __A, __A,
     __A, __A, __A, __A, __A, __A, __A, __A,
     __A, __A, __A, __A, __A, __A, __A, __A);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_max_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_pmaxsw128 ((__v8hi)__A, (__v8hi)__B);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_min_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_pminsw128 ((__v8hi)__A, (__v8hi)__B);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_max_epu16 (__m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_pmaxuw128 ((__v8hi)__X, (__v8hi)__Y);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_min_epu16 (__m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_pminuw128 ((__v8hi)__X, (__v8hi)__Y);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_max_epi8 (__m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_pmaxsb128 ((__v16qi)__X, (__v16qi)__Y);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_min_epi8 (__m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_pminsb128 ((__v16qi)__X, (__v16qi)__Y);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_max_epu8 (__m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_pmaxub128 ((__v16qi)__A, (__v16qi)__B);
}
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avx512_min_epu8 (__m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_pminub128 ((__v16qi)__A, (__v16qi)__B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_mov_epi8 (__m256i __W, __mmask32 __U, __m256i __A)
{
  return (__m256i) __builtin_ia32_movdquqi256_mask ((__v32qi) __A,
          (__v32qi) __W,
          (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_max_epi16 (__m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_pmaxsw256 ((__v16hi)__A, (__v16hi)__B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_min_epi16 (__m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_pminsw256 ((__v16hi)__A, (__v16hi)__B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_max_epu16 (__m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_pmaxuw256 ((__v16hi)__A, (__v16hi)__B);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_min_epu16 (__m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_pminuw256 ((__v16hi)__A, (__v16hi)__B);
}
#ifdef __OPTIMIZE__
extern __inline __m256 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_insertf128_ps (__m256 __X, __m128 __Y, const int __O)
{
  return (__m256) __builtin_ia32_vinsertf128_ps256 ((__v8sf)__X,
          (__v4sf)__Y,
          __O);
}
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_extractf128_pd (__m256d __X, const int __N)
{
  return (__m128d) __builtin_ia32_vextractf128_pd256 ((__v4df)__X, __N);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_avx512_extracti128_si256 (__m256i __X, const int __M)
{
  return (__m128i) __builtin_ia32_extract128i256 ((__v4di)__X, __M);
}
#else
#define _mm256_avx512_insertf128_ps(X, Y, O) ((__m256) __builtin_ia32_vinsertf128_ps256 ((__v8sf)(__m256)(X), (__v4sf)(__m128)(Y), (int)(O)))
#define _mm256_avx512_extractf128_pd(X, N) ((__m128d) __builtin_ia32_vextractf128_pd256 ((__v4df)(__m256d)(X), (int)(N)))
#define _mm256_avx512_extracti128_si256(X, M) ((__m128i) __builtin_ia32_extract128i256 ((__v4di)(__m256i)(X), (int)(M)))
#endif
#define _MM256_AVX512_REDUCE_OPERATOR_BASIC_EPI16(op) __v8hi __T1 = (__v8hi)_mm256_avx512_extracti128_si256 (__W, 0); __v8hi __T2 = (__v8hi)_mm256_avx512_extracti128_si256 (__W, 1); __v8hi __T3 = __T1 op __T2; __v8hi __T4 = __builtin_shufflevector (__T3, __T3, 4, 5, 6, 7, 4, 5, 6, 7); __v8hi __T5 = __T3 op __T4; __v8hi __T6 = __builtin_shufflevector (__T5, __T5, 2, 3, 2, 3, 4, 5, 6, 7); __v8hi __T7 = __T5 op __T6; __v8hi __T8 = __builtin_shufflevector (__T7, __T7, 1, 1, 2, 3, 4, 5, 6, 7); __v8hi __T9 = __T7 op __T8; return __T9[0]
#define _MM256_AVX512_REDUCE_OPERATOR_MAX_MIN_EP16(op) __m128i __T1 = _mm256_avx512_extracti128_si256 (__V, 0); __m128i __T2 = _mm256_avx512_extracti128_si256 (__V, 1); __m128i __T3 = _mm_avx512_##op (__T1, __T2); __m128i __T4 = (__m128i)__builtin_shufflevector ((__v8hi)__T3, (__v8hi)__T3, 4, 5, 6, 7, 4, 5, 6, 7); __m128i __T5 = _mm_avx512_##op (__T3, __T4); __m128i __T6 = (__m128i)__builtin_shufflevector ((__v8hi)__T5, (__v8hi)__T5, 2, 3, 2, 3, 4, 5, 6, 7); __m128i __T7 = _mm_avx512_##op (__T5, __T6); __m128i __T8 = (__m128i)__builtin_shufflevector ((__v8hi)__T7, (__v8hi)__T7, 1, 1, 2, 3, 4, 5, 6, 7); __v8hi __T9 = (__v8hi)_mm_avx512_##op (__T7, __T8); return __T9[0]
#define _MM256_AVX512_REDUCE_OPERATOR_BASIC_EPI8(op) __v16qi __T1 = (__v16qi)_mm256_avx512_extracti128_si256 (__W, 0); __v16qi __T2 = (__v16qi)_mm256_avx512_extracti128_si256 (__W, 1); __v16qi __T3 = __T1 op __T2; __v16qi __T4 = __builtin_shufflevector (__T3, __T3, 8, 9, 10, 11, 12, 13, 14, 15, 8, 9, 10, 11, 12, 13, 14, 15); __v16qi __T5 = __T3 op __T4; __v16qi __T6 = __builtin_shufflevector (__T5, __T5, 4, 5, 6, 7, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15); __v16qi __T7 = __T5 op __T6; __v16qi __T8 = __builtin_shufflevector (__T7, __T7, 2, 3, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15); __v16qi __T9 = __T7 op __T8; __v16qi __T10 = __builtin_shufflevector (__T9, __T9, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15); __v16qi __T11 = __T9 op __T10; return __T11[0]
#define _MM256_AVX512_REDUCE_OPERATOR_MAX_MIN_EP8(op) __m128i __T1 = _mm256_avx512_extracti128_si256 (__V, 0); __m128i __T2 = _mm256_avx512_extracti128_si256 (__V, 1); __m128i __T3 = _mm_avx512_##op (__T1, __T2); __m128i __T4 = (__m128i)__builtin_shufflevector ((__v16qi)__T3, (__v16qi)__T3, 8, 9, 10, 11, 12, 13, 14, 15, 8, 9, 10, 11, 12, 13, 14, 15); __m128i __T5 = _mm_avx512_##op (__T3, __T4); __m128i __T6 = (__m128i)__builtin_shufflevector ((__v16qi)__T5, (__v16qi)__T5, 4, 5, 6, 7, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15); __m128i __T7 = _mm_avx512_##op (__T5, __T6); __m128i __T8 = (__m128i)__builtin_shufflevector ((__v16qi)__T7, (__v16qi)__T5, 2, 3, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15); __m128i __T9 = _mm_avx512_##op (__T7, __T8); __m128i __T10 = (__m128i)__builtin_shufflevector ((__v16qi)__T9, (__v16qi)__T9, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15); __v16qi __T11 = (__v16qi)_mm_avx512_##op (__T9, __T10); return __T11[0]
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_mov_epi8 (__mmask32 __U, __m256i __A)
{
  return (__m256i) __builtin_ia32_movdquqi256_mask ((__v32qi) __A,
          (__v32qi)
          _mm256_avx512_setzero_si256 (),
          (__mmask32) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_mov_epi8 (__m128i __W, __mmask16 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_movdquqi128_mask ((__v16qi) __A,
          (__v16qi) __W,
          (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_mov_epi8 (__mmask16 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_movdquqi128_mask ((__v16qi) __A,
          (__v16qi)
          _mm_avx512_setzero_si128 (),
          (__mmask16) __U);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_storeu_epi8 (void *__P, __m256i __A)
{
  *(__v32qi_u *) __P = (__v32qi_u) __A;
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_storeu_epi8 (void *__P, __mmask32 __U, __m256i __A)
{
  __builtin_ia32_storedquqi256_mask ((char *) __P,
         (__v32qi) __A,
         (__mmask32) __U);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_storeu_epi8 (void *__P, __m128i __A)
{
  *(__v16qi_u *) __P = (__v16qi_u) __A;
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_storeu_epi8 (void *__P, __mmask16 __U, __m128i __A)
{
  __builtin_ia32_storedquqi128_mask ((char *) __P,
         (__v16qi) __A,
         (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_loadu_epi16 (void const *__P)
{
  return (__m256i) (*(__v16hi_u *) __P);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_loadu_epi16 (__m256i __W, __mmask16 __U, void const *__P)
{
  return (__m256i) __builtin_ia32_loaddquhi256_mask ((const short *) __P,
           (__v16hi) __W,
           (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_loadu_epi16 (__mmask16 __U, void const *__P)
{
  return (__m256i) __builtin_ia32_loaddquhi256_mask ((const short *) __P,
           (__v16hi)
           _mm256_avx512_setzero_si256 (),
           (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadu_epi16 (void const *__P)
{
  return (__m128i) (*(__v8hi_u *) __P);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_loadu_epi16 (__m128i __W, __mmask8 __U, void const *__P)
{
  return (__m128i) __builtin_ia32_loaddquhi128_mask ((const short *) __P,
           (__v8hi) __W,
           (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_loadu_epi16 (__mmask8 __U, void const *__P)
{
  return (__m128i) __builtin_ia32_loaddquhi128_mask ((const short *) __P,
           (__v8hi)
           _mm_avx512_setzero_si128 (),
           (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_mov_epi16 (__m256i __W, __mmask16 __U, __m256i __A)
{
  return (__m256i) __builtin_ia32_movdquhi256_mask ((__v16hi) __A,
          (__v16hi) __W,
          (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_mov_epi16 (__mmask16 __U, __m256i __A)
{
  return (__m256i) __builtin_ia32_movdquhi256_mask ((__v16hi) __A,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_mov_epi16 (__m128i __W, __mmask8 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_movdquhi128_mask ((__v8hi) __A,
          (__v8hi) __W,
          (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_mov_epi16 (__mmask8 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_movdquhi128_mask ((__v8hi) __A,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_loadu_epi8 (void const *__P)
{
  return (__m256i) (*(__v32qi_u *) __P);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_loadu_epi8 (__m256i __W, __mmask32 __U, void const *__P)
{
  return (__m256i) __builtin_ia32_loaddquqi256_mask ((const char *) __P,
           (__v32qi) __W,
           (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_loadu_epi8 (__mmask32 __U, void const *__P)
{
  return (__m256i) __builtin_ia32_loaddquqi256_mask ((const char *) __P,
           (__v32qi)
           _mm256_avx512_setzero_si256 (),
           (__mmask32) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadu_epi8 (void const *__P)
{
  return (__m128i) (*(__v16qi_u *) __P);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_loadu_epi8 (__m128i __W, __mmask16 __U, void const *__P)
{
  return (__m128i) __builtin_ia32_loaddquqi128_mask ((const char *) __P,
           (__v16qi) __W,
           (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_loadu_epi8 (__mmask16 __U, void const *__P)
{
  return (__m128i) __builtin_ia32_loaddquqi128_mask ((const char *) __P,
           (__v16qi)
           _mm_avx512_setzero_si128 (),
           (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_blend_epi16 (__mmask8 __U, __m128i __A, __m128i __W)
{
  return (__m128i) __builtin_ia32_blendmw_128_mask ((__v8hi) __A,
          (__v8hi) __W,
          (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_blend_epi8 (__mmask16 __U, __m128i __A, __m128i __W)
{
  return (__m128i) __builtin_ia32_blendmb_128_mask ((__v16qi) __A,
          (__v16qi) __W,
          (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_blend_epi16 (__mmask16 __U, __m256i __A, __m256i __W)
{
  return (__m256i) __builtin_ia32_blendmw_256_mask ((__v16hi) __A,
          (__v16hi) __W,
          (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_blend_epi8 (__mmask32 __U, __m256i __A, __m256i __W)
{
  return (__m256i) __builtin_ia32_blendmb_256_mask ((__v32qi) __A,
          (__v32qi) __W,
          (__mmask32) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtepi16_epi8 (__m256i __A)
{
  return (__m128i) __builtin_ia32_pmovwb256_mask ((__v16hi) __A,
        (__v16qi)_mm_avx512_undefined_si128(),
        (__mmask16) -1);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtepi16_storeu_epi8 (void * __P, __mmask16 __M,__m256i __A)
{
  __builtin_ia32_pmovwb256mem_mask ((__v16qi *) __P , (__v16hi) __A, __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtepi16_epi8 (__m128i __O, __mmask16 __M, __m256i __A)
{
  return (__m128i) __builtin_ia32_pmovwb256_mask ((__v16hi) __A,
        (__v16qi) __O, __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtepi16_epi8 (__mmask16 __M, __m256i __A)
{
  return (__m128i) __builtin_ia32_pmovwb256_mask ((__v16hi) __A,
        (__v16qi)
        _mm_avx512_setzero_si128 (),
        __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsepi16_epi8 (__m128i __A)
{
  return (__m128i) __builtin_ia32_pmovswb128_mask ((__v8hi) __A,
         (__v16qi)_mm_avx512_undefined_si128(),
         (__mmask8) -1);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtsepi16_storeu_epi8 (void * __P, __mmask8 __M,__m128i __A)
{
  __builtin_ia32_pmovswb128mem_mask ((unsigned long long *) __P , (__v8hi) __A, __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtsepi16_epi8 (__m128i __O, __mmask8 __M, __m128i __A)
{
  return (__m128i) __builtin_ia32_pmovswb128_mask ((__v8hi) __A,
         (__v16qi) __O, __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtsepi16_epi8 (__mmask8 __M, __m128i __A)
{
  return (__m128i) __builtin_ia32_pmovswb128_mask ((__v8hi) __A,
         (__v16qi)
         _mm_avx512_setzero_si128 (),
         __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtsepi16_epi8 (__m256i __A)
{
  return (__m128i) __builtin_ia32_pmovswb256_mask ((__v16hi) __A,
         (__v16qi)_mm_avx512_undefined_si128(),
         (__mmask16) -1);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtsepi16_storeu_epi8 (void * __P, __mmask16 __M,__m256i __A)
{
  __builtin_ia32_pmovswb256mem_mask ((__v16qi *) __P , (__v16hi) __A, __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtsepi16_epi8 (__m128i __O, __mmask16 __M, __m256i __A)
{
  return (__m128i) __builtin_ia32_pmovswb256_mask ((__v16hi) __A,
         (__v16qi) __O, __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtsepi16_epi8 (__mmask16 __M, __m256i __A)
{
  return (__m128i) __builtin_ia32_pmovswb256_mask ((__v16hi) __A,
         (__v16qi)
         _mm_avx512_setzero_si128 (),
         __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtusepi16_epi8 (__m128i __A)
{
  return (__m128i) __builtin_ia32_pmovuswb128_mask ((__v8hi) __A,
          (__v16qi)_mm_avx512_undefined_si128(),
          (__mmask8) -1);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtusepi16_storeu_epi8 (void * __P, __mmask8 __M,__m128i __A)
{
  __builtin_ia32_pmovuswb128mem_mask ((unsigned long long *) __P , (__v8hi) __A, __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtusepi16_epi8 (__m128i __O, __mmask8 __M, __m128i __A)
{
  return (__m128i) __builtin_ia32_pmovuswb128_mask ((__v8hi) __A,
          (__v16qi) __O,
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtusepi16_epi8 (__mmask8 __M, __m128i __A)
{
  return (__m128i) __builtin_ia32_pmovuswb128_mask ((__v8hi) __A,
          (__v16qi)
          _mm_avx512_setzero_si128 (),
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cvtusepi16_epi8 (__m256i __A)
{
  return (__m128i) __builtin_ia32_pmovuswb256_mask ((__v16hi) __A,
          (__v16qi)_mm_avx512_undefined_si128(),
          (__mmask16) -1);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtusepi16_storeu_epi8 (void * __P, __mmask16 __M,__m256i __A)
{
  __builtin_ia32_pmovuswb256mem_mask ((__v16qi *) __P , (__v16hi) __A, __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtusepi16_epi8 (__m128i __O, __mmask16 __M, __m256i __A)
{
  return (__m128i) __builtin_ia32_pmovuswb256_mask ((__v16hi) __A,
          (__v16qi) __O,
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtusepi16_epi8 (__mmask16 __M, __m256i __A)
{
  return (__m128i) __builtin_ia32_pmovuswb256_mask ((__v16hi) __A,
          (__v16qi)
          _mm_avx512_setzero_si128 (),
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_broadcastb_epi8 (__m256i __O, __mmask32 __M, __m128i __A)
{
  return (__m256i) __builtin_ia32_pbroadcastb256_mask ((__v16qi) __A,
             (__v32qi) __O,
             __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_broadcastb_epi8 (__mmask32 __M, __m128i __A)
{
  return (__m256i) __builtin_ia32_pbroadcastb256_mask ((__v16qi) __A,
             (__v32qi)
             _mm256_avx512_setzero_si256 (),
             __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_set1_epi8 (__m256i __O, __mmask32 __M, char __A)
{
  return (__m256i) __builtin_ia32_pbroadcastb256_gpr_mask (__A,
          (__v32qi) __O,
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_set1_epi8 (__mmask32 __M, char __A)
{
  return (__m256i) __builtin_ia32_pbroadcastb256_gpr_mask (__A,
          (__v32qi)
          _mm256_avx512_setzero_si256 (),
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_broadcastb_epi8 (__m128i __O, __mmask16 __M, __m128i __A)
{
  return (__m128i) __builtin_ia32_pbroadcastb128_mask ((__v16qi) __A,
             (__v16qi) __O,
             __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_broadcastb_epi8 (__mmask16 __M, __m128i __A)
{
  return (__m128i) __builtin_ia32_pbroadcastb128_mask ((__v16qi) __A,
             (__v16qi)
             _mm_avx512_setzero_si128 (),
             __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_set1_epi8 (__m128i __O, __mmask16 __M, char __A)
{
  return (__m128i) __builtin_ia32_pbroadcastb128_gpr_mask (__A,
          (__v16qi) __O,
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_set1_epi8 (__mmask16 __M, char __A)
{
  return (__m128i) __builtin_ia32_pbroadcastb128_gpr_mask (__A,
          (__v16qi)
          _mm_avx512_setzero_si128 (),
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_broadcastw_epi16 (__m256i __O, __mmask16 __M, __m128i __A)
{
  return (__m256i) __builtin_ia32_pbroadcastw256_mask ((__v8hi) __A,
             (__v16hi) __O,
             __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_broadcastw_epi16 (__mmask16 __M, __m128i __A)
{
  return (__m256i) __builtin_ia32_pbroadcastw256_mask ((__v8hi) __A,
             (__v16hi)
             _mm256_avx512_setzero_si256 (),
             __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_set1_epi16 (__m256i __O, __mmask16 __M, short __A)
{
  return (__m256i) __builtin_ia32_pbroadcastw256_gpr_mask (__A,
          (__v16hi) __O,
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_set1_epi16 (__mmask16 __M, short __A)
{
  return (__m256i) __builtin_ia32_pbroadcastw256_gpr_mask (__A,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_broadcastw_epi16 (__m128i __O, __mmask8 __M, __m128i __A)
{
  return (__m128i) __builtin_ia32_pbroadcastw128_mask ((__v8hi) __A,
             (__v8hi) __O,
             __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_broadcastw_epi16 (__mmask8 __M, __m128i __A)
{
  return (__m128i) __builtin_ia32_pbroadcastw128_mask ((__v8hi) __A,
             (__v8hi)
             _mm_avx512_setzero_si128 (),
             __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_set1_epi16 (__m128i __O, __mmask8 __M, short __A)
{
  return (__m128i) __builtin_ia32_pbroadcastw128_gpr_mask (__A,
          (__v8hi) __O,
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_set1_epi16 (__mmask8 __M, short __A)
{
  return (__m128i) __builtin_ia32_pbroadcastw128_gpr_mask (__A,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_permutexvar_epi16 (__m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_permvarhi256_mask ((__v16hi) __B,
           (__v16hi) __A,
           (__v16hi)
           _mm256_avx512_setzero_si256 (),
           (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_permutexvar_epi16 (__mmask16 __M, __m256i __A,
    __m256i __B)
{
  return (__m256i) __builtin_ia32_permvarhi256_mask ((__v16hi) __B,
           (__v16hi) __A,
           (__v16hi)
           _mm256_avx512_setzero_si256 (),
           (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_permutexvar_epi16 (__m256i __W, __mmask16 __M, __m256i __A,
          __m256i __B)
{
  return (__m256i) __builtin_ia32_permvarhi256_mask ((__v16hi) __B,
           (__v16hi) __A,
           (__v16hi) __W,
           (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_permutexvar_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_permvarhi128_mask ((__v8hi) __B,
           (__v8hi) __A,
           (__v8hi)
           _mm_avx512_setzero_si128 (),
           (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_permutexvar_epi16 (__mmask8 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_permvarhi128_mask ((__v8hi) __B,
           (__v8hi) __A,
           (__v8hi)
           _mm_avx512_setzero_si128 (),
           (__mmask8) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_permutexvar_epi16 (__m128i __W, __mmask8 __M, __m128i __A,
       __m128i __B)
{
  return (__m128i) __builtin_ia32_permvarhi128_mask ((__v8hi) __B,
           (__v8hi) __A,
           (__v8hi) __W,
           (__mmask8) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_permutex2var_epi16 (__m256i __A, __m256i __I, __m256i __B)
{
  return (__m256i) __builtin_ia32_vpermt2varhi256_mask ((__v16hi) __I
                 ,
       (__v16hi) __A,
       (__v16hi) __B,
       (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_permutex2var_epi16 (__m256i __A, __mmask16 __U,
    __m256i __I, __m256i __B)
{
  return (__m256i) __builtin_ia32_vpermt2varhi256_mask ((__v16hi) __I
                 ,
       (__v16hi) __A,
       (__v16hi) __B,
       (__mmask16)
       __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask2_permutex2var_epi16 (__m256i __A, __m256i __I,
     __mmask16 __U, __m256i __B)
{
  return (__m256i) __builtin_ia32_vpermi2varhi256_mask ((__v16hi) __A,
       (__v16hi) __I
                 ,
       (__v16hi) __B,
       (__mmask16)
       __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_permutex2var_epi16 (__mmask16 __U, __m256i __A,
     __m256i __I, __m256i __B)
{
  return (__m256i) __builtin_ia32_vpermt2varhi256_maskz ((__v16hi) __I
                  ,
        (__v16hi) __A,
        (__v16hi) __B,
        (__mmask16)
        __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_permutex2var_epi16 (__m128i __A, __m128i __I, __m128i __B)
{
  return (__m128i) __builtin_ia32_vpermt2varhi128_mask ((__v8hi) __I
                 ,
       (__v8hi) __A,
       (__v8hi) __B,
       (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_permutex2var_epi16 (__m128i __A, __mmask8 __U, __m128i __I,
        __m128i __B)
{
  return (__m128i) __builtin_ia32_vpermt2varhi128_mask ((__v8hi) __I
                 ,
       (__v8hi) __A,
       (__v8hi) __B,
       (__mmask8)
       __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask2_permutex2var_epi16 (__m128i __A, __m128i __I, __mmask8 __U,
         __m128i __B)
{
  return (__m128i) __builtin_ia32_vpermi2varhi128_mask ((__v8hi) __A,
       (__v8hi) __I
                 ,
       (__v8hi) __B,
       (__mmask8)
       __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_permutex2var_epi16 (__mmask8 __U, __m128i __A, __m128i __I,
         __m128i __B)
{
  return (__m128i) __builtin_ia32_vpermt2varhi128_maskz ((__v8hi) __I
                  ,
        (__v8hi) __A,
        (__v8hi) __B,
        (__mmask8)
        __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_maddubs_epi16 (__m256i __W, __mmask16 __U, __m256i __X,
      __m256i __Y)
{
  return (__m256i) __builtin_ia32_pmaddubsw256_mask ((__v32qi) __X,
           (__v32qi) __Y,
           (__v16hi) __W,
           (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_maddubs_epi16 (__mmask16 __U, __m256i __X, __m256i __Y)
{
  return (__m256i) __builtin_ia32_pmaddubsw256_mask ((__v32qi) __X,
           (__v32qi) __Y,
           (__v16hi)
           _mm256_avx512_setzero_si256 (),
           (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_maddubs_epi16 (__m128i __W, __mmask8 __U, __m128i __X,
   __m128i __Y)
{
  return (__m128i) __builtin_ia32_pmaddubsw128_mask ((__v16qi) __X,
           (__v16qi) __Y,
           (__v8hi) __W,
           (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_maddubs_epi16 (__mmask8 __U, __m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_pmaddubsw128_mask ((__v16qi) __X,
           (__v16qi) __Y,
           (__v8hi)
           _mm_avx512_setzero_si128 (),
           (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_madd_epi16 (__m256i __W, __mmask8 __U, __m256i __A,
   __m256i __B)
{
  return (__m256i) __builtin_ia32_pmaddwd256_mask ((__v16hi) __A,
         (__v16hi) __B,
         (__v8si) __W,
         (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_madd_epi16 (__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pmaddwd256_mask ((__v16hi) __A,
         (__v16hi) __B,
         (__v8si)
         _mm256_avx512_setzero_si256 (),
         (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_madd_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
       __m128i __B)
{
  return (__m128i) __builtin_ia32_pmaddwd128_mask ((__v8hi) __A,
         (__v8hi) __B,
         (__v4si) __W,
         (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_madd_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pmaddwd128_mask ((__v8hi) __A,
         (__v8hi) __B,
         (__v4si)
         _mm_avx512_setzero_si128 (),
         (__mmask8) __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_movepi8_mask (__m128i __A)
{
  return (__mmask16) __builtin_ia32_cvtb2mask128 ((__v16qi) __A);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_movepi8_mask (__m256i __A)
{
  return (__mmask32) __builtin_ia32_cvtb2mask256 ((__v32qi) __A);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_movepi16_mask (__m128i __A)
{
  return (__mmask8) __builtin_ia32_cvtw2mask128 ((__v8hi) __A);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_movepi16_mask (__m256i __A)
{
  return (__mmask16) __builtin_ia32_cvtw2mask256 ((__v16hi) __A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_movm_epi8 (__mmask16 __A)
{
  return (__m128i) __builtin_ia32_cvtmask2b128 (__A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_movm_epi8 (__mmask32 __A)
{
  return (__m256i) __builtin_ia32_cvtmask2b256 (__A);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_movm_epi16 (__mmask8 __A)
{
  return (__m128i) __builtin_ia32_cvtmask2w128 (__A);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_movm_epi16 (__mmask16 __A)
{
  return (__m256i) __builtin_ia32_cvtmask2w256 (__A);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_test_epi8_mask (__m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_ptestmb128 ((__v16qi) __A,
      (__v16qi) __B,
      (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_test_epi8_mask (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_ptestmb128 ((__v16qi) __A,
      (__v16qi) __B, __U);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_test_epi8_mask (__m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_ptestmb256 ((__v32qi) __A,
      (__v32qi) __B,
      (__mmask32) -1);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_test_epi8_mask (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_ptestmb256 ((__v32qi) __A,
      (__v32qi) __B, __U);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_test_epi16_mask (__m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_ptestmw128 ((__v8hi) __A,
            (__v8hi) __B,
            (__mmask8) -1);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_test_epi16_mask (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_ptestmw128 ((__v8hi) __A,
            (__v8hi) __B, __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_test_epi16_mask (__m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_ptestmw256 ((__v16hi) __A,
      (__v16hi) __B,
      (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_test_epi16_mask (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_ptestmw256 ((__v16hi) __A,
      (__v16hi) __B, __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_min_epu16 (__mmask16 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pminuw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_min_epu16 (__m256i __W, __mmask16 __M, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_pminuw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_min_epu16 (__mmask8 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pminuw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_min_epu16 (__m128i __W, __mmask8 __M, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_pminuw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi) __W,
        (__mmask8) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_min_epi16 (__mmask16 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pminsw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_min_epi16 (__m256i __W, __mmask16 __M, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_pminsw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_max_epu8 (__mmask32 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pmaxub256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi)
        _mm256_avx512_setzero_si256 (),
        (__mmask32) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_max_epu8 (__m256i __W, __mmask32 __M, __m256i __A,
        __m256i __B)
{
  return (__m256i) __builtin_ia32_pmaxub256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi) __W,
        (__mmask32) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_max_epu8 (__mmask16 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pmaxub128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi)
        _mm_avx512_setzero_si128 (),
        (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_max_epu8 (__m128i __W, __mmask16 __M, __m128i __A,
     __m128i __B)
{
  return (__m128i) __builtin_ia32_pmaxub128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi) __W,
        (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_max_epi8 (__mmask32 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pmaxsb256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi)
        _mm256_avx512_setzero_si256 (),
        (__mmask32) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_max_epi8 (__m256i __W, __mmask32 __M, __m256i __A,
        __m256i __B)
{
  return (__m256i) __builtin_ia32_pmaxsb256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi) __W,
        (__mmask32) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_max_epi8 (__mmask16 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pmaxsb128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi)
        _mm_avx512_setzero_si128 (),
        (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_max_epi8 (__m128i __W, __mmask16 __M, __m128i __A,
     __m128i __B)
{
  return (__m128i) __builtin_ia32_pmaxsb128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi) __W,
        (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_min_epu8 (__mmask32 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pminub256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi)
        _mm256_avx512_setzero_si256 (),
        (__mmask32) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_min_epu8 (__m256i __W, __mmask32 __M, __m256i __A,
        __m256i __B)
{
  return (__m256i) __builtin_ia32_pminub256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi) __W,
        (__mmask32) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_min_epu8 (__mmask16 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pminub128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi)
        _mm_avx512_setzero_si128 (),
        (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_min_epu8 (__m128i __W, __mmask16 __M, __m128i __A,
     __m128i __B)
{
  return (__m128i) __builtin_ia32_pminub128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi) __W,
        (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_min_epi8 (__mmask32 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pminsb256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi)
        _mm256_avx512_setzero_si256 (),
        (__mmask32) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_min_epi8 (__m256i __W, __mmask32 __M, __m256i __A,
        __m256i __B)
{
  return (__m256i) __builtin_ia32_pminsb256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi) __W,
        (__mmask32) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_min_epi8 (__mmask16 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pminsb128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi)
        _mm_avx512_setzero_si128 (),
        (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_min_epi8 (__m128i __W, __mmask16 __M, __m128i __A,
     __m128i __B)
{
  return (__m128i) __builtin_ia32_pminsb128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi) __W,
        (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_max_epi16 (__mmask16 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pmaxsw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_max_epi16 (__m256i __W, __mmask16 __M, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_pmaxsw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_max_epi16 (__mmask8 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pmaxsw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_max_epi16 (__m128i __W, __mmask8 __M, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_pmaxsw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi) __W,
        (__mmask8) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_max_epu16 (__mmask16 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pmaxuw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_max_epu16 (__m256i __W, __mmask16 __M, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_pmaxuw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_max_epu16 (__mmask8 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pmaxuw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_max_epu16 (__m128i __W, __mmask8 __M, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_pmaxuw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi) __W,
        (__mmask8) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_min_epi16 (__mmask8 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pminsw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_min_epi16 (__m128i __W, __mmask8 __M, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_pminsw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi) __W,
        (__mmask8) __M);
}
#ifdef __OPTIMIZE__
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_alignr_epi8 (__m256i __W, __mmask32 __U, __m256i __A,
    __m256i __B, const int __N)
{
  return (__m256i) __builtin_ia32_palignr256_mask ((__v4di) __A,
         (__v4di) __B,
         __N * 8,
         (__v4di) __W,
         (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_alignr_epi8 (__mmask32 __U, __m256i __A, __m256i __B,
     const int __N)
{
  return (__m256i) __builtin_ia32_palignr256_mask ((__v4di) __A,
         (__v4di) __B,
         __N * 8,
         (__v4di)
         _mm256_avx512_setzero_si256 (),
         (__mmask32) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_alignr_epi8 (__m128i __W, __mmask16 __U, __m128i __A,
        __m128i __B, const int __N)
{
  return (__m128i) __builtin_ia32_palignr128_mask ((__v2di) __A,
         (__v2di) __B,
         __N * 8,
         (__v2di) __W,
         (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_alignr_epi8 (__mmask16 __U, __m128i __A, __m128i __B,
         const int __N)
{
  return (__m128i) __builtin_ia32_palignr128_mask ((__v2di) __A,
         (__v2di) __B,
         __N * 8,
         (__v2di)
         _mm_avx512_setzero_si128 (),
         (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_dbsad_epu8 (__m256i __A, __m256i __B, const int __imm)
{
  return (__m256i) __builtin_ia32_dbpsadbw256_mask ((__v32qi) __A,
          (__v32qi) __B,
          __imm,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_dbsad_epu8 (__m256i __W, __mmask16 __U, __m256i __A,
   __m256i __B, const int __imm)
{
  return (__m256i) __builtin_ia32_dbpsadbw256_mask ((__v32qi) __A,
          (__v32qi) __B,
          __imm,
          (__v16hi) __W,
          (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_dbsad_epu8 (__mmask16 __U, __m256i __A, __m256i __B,
    const int __imm)
{
  return (__m256i) __builtin_ia32_dbpsadbw256_mask ((__v32qi) __A,
          (__v32qi) __B,
          __imm,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_dbsad_epu8 (__m128i __A, __m128i __B, const int __imm)
{
  return (__m128i) __builtin_ia32_dbpsadbw128_mask ((__v16qi) __A,
          (__v16qi) __B,
          __imm,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_dbsad_epu8 (__m128i __W, __mmask8 __U, __m128i __A,
       __m128i __B, const int __imm)
{
  return (__m128i) __builtin_ia32_dbpsadbw128_mask ((__v16qi) __A,
          (__v16qi) __B,
          __imm,
          (__v8hi) __W,
          (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_dbsad_epu8 (__mmask8 __U, __m128i __A, __m128i __B,
        const int __imm)
{
  return (__m128i) __builtin_ia32_dbpsadbw128_mask ((__v16qi) __A,
          (__v16qi) __B,
          __imm,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          (__mmask8) __U);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmp_epi16_mask (__mmask8 __U, __m128i __X, __m128i __Y,
    const int __P)
{
  return (__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi) __X,
       (__v8hi) __Y, __P,
       (__mmask8) __U);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmp_epi16_mask (__m128i __X, __m128i __Y, const int __P)
{
  return (__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi) __X,
       (__v8hi) __Y, __P,
       (__mmask8) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmp_epi16_mask (__mmask16 __U, __m256i __X, __m256i __Y,
       const int __P)
{
  return (__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi) __X,
        (__v16hi) __Y, __P,
        (__mmask16) __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmp_epi16_mask (__m256i __X, __m256i __Y, const int __P)
{
  return (__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi) __X,
        (__v16hi) __Y, __P,
        (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmp_epi8_mask (__mmask16 __U, __m128i __X, __m128i __Y,
   const int __P)
{
  return (__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi) __X,
        (__v16qi) __Y, __P,
        (__mmask16) __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmp_epi8_mask (__m128i __X, __m128i __Y, const int __P)
{
  return (__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi) __X,
        (__v16qi) __Y, __P,
        (__mmask16) -1);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmp_epi8_mask (__mmask32 __U, __m256i __X, __m256i __Y,
      const int __P)
{
  return (__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi) __X,
        (__v32qi) __Y, __P,
        (__mmask32) __U);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmp_epi8_mask (__m256i __X, __m256i __Y, const int __P)
{
  return (__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi) __X,
        (__v32qi) __Y, __P,
        (__mmask32) -1);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmp_epu16_mask (__mmask8 __U, __m128i __X, __m128i __Y,
    const int __P)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __X,
        (__v8hi) __Y, __P,
        (__mmask8) __U);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmp_epu16_mask (__m128i __X, __m128i __Y, const int __P)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __X,
        (__v8hi) __Y, __P,
        (__mmask8) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmp_epu16_mask (__mmask16 __U, __m256i __X, __m256i __Y,
       const int __P)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __X,
         (__v16hi) __Y, __P,
         (__mmask16) __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmp_epu16_mask (__m256i __X, __m256i __Y, const int __P)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __X,
         (__v16hi) __Y, __P,
         (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmp_epu8_mask (__mmask16 __U, __m128i __X, __m128i __Y,
   const int __P)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __X,
         (__v16qi) __Y, __P,
         (__mmask16) __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmp_epu8_mask (__m128i __X, __m128i __Y, const int __P)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __X,
         (__v16qi) __Y, __P,
         (__mmask16) -1);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmp_epu8_mask (__mmask32 __U, __m256i __X, __m256i __Y,
      const int __P)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __X,
         (__v32qi) __Y, __P,
         (__mmask32) __U);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmp_epu8_mask (__m256i __X, __m256i __Y, const int __P)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __X,
         (__v32qi) __Y, __P,
         (__mmask32) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_srli_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
   const int __imm)
{
  return (__m256i) __builtin_ia32_psrlwi256_mask ((__v16hi) __A, __imm,
        (__v16hi) __W,
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_srli_epi16 (__mmask16 __U, __m256i __A, const int __imm)
{
  return (__m256i) __builtin_ia32_psrlwi256_mask ((__v16hi) __A, __imm,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_srli_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
       const int __imm)
{
  return (__m128i) __builtin_ia32_psrlwi128_mask ((__v8hi) __A, __imm,
        (__v8hi) __W,
        (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_srli_epi16 (__mmask8 __U, __m128i __A, const int __imm)
{
  return (__m128i) __builtin_ia32_psrlwi128_mask ((__v8hi) __A, __imm,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_shufflehi_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
        const int __imm)
{
  return (__m256i) __builtin_ia32_pshufhw256_mask ((__v16hi) __A,
         __imm,
         (__v16hi) __W,
         (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_shufflehi_epi16 (__mmask16 __U, __m256i __A,
         const int __imm)
{
  return (__m256i) __builtin_ia32_pshufhw256_mask ((__v16hi) __A,
         __imm,
         (__v16hi)
         _mm256_avx512_setzero_si256 (),
         (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_shufflehi_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
     const int __imm)
{
  return (__m128i) __builtin_ia32_pshufhw128_mask ((__v8hi) __A, __imm,
         (__v8hi) __W,
         (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_shufflehi_epi16 (__mmask8 __U, __m128i __A, const int __imm)
{
  return (__m128i) __builtin_ia32_pshufhw128_mask ((__v8hi) __A, __imm,
         (__v8hi)
         _mm_avx512_setzero_si128 (),
         (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_shufflelo_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
        const int __imm)
{
  return (__m256i) __builtin_ia32_pshuflw256_mask ((__v16hi) __A,
         __imm,
         (__v16hi) __W,
         (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_shufflelo_epi16 (__mmask16 __U, __m256i __A,
         const int __imm)
{
  return (__m256i) __builtin_ia32_pshuflw256_mask ((__v16hi) __A,
         __imm,
         (__v16hi)
         _mm256_avx512_setzero_si256 (),
         (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_shufflelo_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
     const int __imm)
{
  return (__m128i) __builtin_ia32_pshuflw128_mask ((__v8hi) __A, __imm,
         (__v8hi) __W,
         (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_shufflelo_epi16 (__mmask8 __U, __m128i __A, const int __imm)
{
  return (__m128i) __builtin_ia32_pshuflw128_mask ((__v8hi) __A, __imm,
         (__v8hi)
         _mm_avx512_setzero_si128 (),
         (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_srai_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
   const unsigned int __imm)
{
  return (__m256i) __builtin_ia32_psrawi256_mask ((__v16hi) __A, __imm,
        (__v16hi) __W,
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_srai_epi16 (__mmask16 __U, __m256i __A, const unsigned int __imm)
{
  return (__m256i) __builtin_ia32_psrawi256_mask ((__v16hi) __A, __imm,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_srai_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
       const unsigned int __imm)
{
  return (__m128i) __builtin_ia32_psrawi128_mask ((__v8hi) __A, __imm,
        (__v8hi) __W,
        (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_srai_epi16 (__mmask8 __U, __m128i __A, const unsigned int __imm)
{
  return (__m128i) __builtin_ia32_psrawi128_mask ((__v8hi) __A, __imm,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_slli_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
   unsigned int __B)
{
  return (__m256i) __builtin_ia32_psllwi256_mask ((__v16hi) __A, __B,
        (__v16hi) __W,
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_slli_epi16 (__mmask16 __U, __m256i __A, unsigned int __B)
{
  return (__m256i) __builtin_ia32_psllwi256_mask ((__v16hi) __A, __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_slli_epi16 (__m128i __W, __mmask8 __U, __m128i __A, unsigned int __B)
{
  return (__m128i) __builtin_ia32_psllwi128_mask ((__v8hi) __A, __B,
        (__v8hi) __W,
        (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_slli_epi16 (__mmask8 __U, __m128i __A, unsigned int __B)
{
  return (__m128i) __builtin_ia32_psllwi128_mask ((__v8hi) __A, __B,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __U);
}
#else
#define _mm256_mask_alignr_epi8(W, U, X, Y, N) ((__m256i) __builtin_ia32_palignr256_mask ((__v4di)(__m256i)(X), (__v4di)(__m256i)(Y), (int)((N) * 8), (__v4di)(__m256i)(X), (__mmask32)(U)))
#define _mm256_mask_srli_epi16(W, U, A, B) ((__m256i) __builtin_ia32_psrlwi256_mask ((__v16hi)(__m256i)(A), (int)(B), (__v16hi)(__m256i)(W), (__mmask16)(U)))
#define _mm256_maskz_srli_epi16(U, A, B) ((__m256i) __builtin_ia32_psrlwi256_mask ((__v16hi)(__m256i)(A), (int)(B), (__v16hi)_mm256_avx512_setzero_si256 (), (__mmask16)(U)))
#define _mm_mask_srli_epi16(W, U, A, B) ((__m128i) __builtin_ia32_psrlwi128_mask ((__v8hi)(__m128i)(A), (int)(B), (__v8hi)(__m128i)(W), (__mmask8)(U)))
#define _mm_maskz_srli_epi16(U, A, B) ((__m128i) __builtin_ia32_psrlwi128_mask ((__v8hi)(__m128i)(A), (int)(B), (__v8hi)_mm_avx512_setzero_si128 (), (__mmask8)(U)))
#define _mm256_mask_srai_epi16(W, U, A, B) ((__m256i) __builtin_ia32_psrawi256_mask ((__v16hi)(__m256i)(A), (unsigned int)(B), (__v16hi)(__m256i)(W), (__mmask16)(U)))
#define _mm256_maskz_srai_epi16(U, A, B) ((__m256i) __builtin_ia32_psrawi256_mask ((__v16hi)(__m256i)(A), (unsigned int)(B), (__v16hi)_mm256_avx512_setzero_si256 (), (__mmask16)(U)))
#define _mm_mask_srai_epi16(W, U, A, B) ((__m128i) __builtin_ia32_psrawi128_mask ((__v8hi)(__m128i)(A), (unsigned int)(B), (__v8hi)(__m128i)(W), (__mmask8)(U)))
#define _mm_maskz_srai_epi16(U, A, B) ((__m128i) __builtin_ia32_psrawi128_mask ((__v8hi)(__m128i)(A), (unsigned int)(B), (__v8hi)_mm_avx512_setzero_si128(), (__mmask8)(U)))
#define _mm256_mask_shufflehi_epi16(W, U, A, B) ((__m256i) __builtin_ia32_pshufhw256_mask ((__v16hi)(__m256i)(A), (int)(B), (__v16hi)(__m256i)(W), (__mmask16)(U)))
#define _mm256_maskz_shufflehi_epi16(U, A, B) ((__m256i) __builtin_ia32_pshufhw256_mask ((__v16hi)(__m256i)(A), (int)(B), (__v16hi)(__m256i)_mm256_avx512_setzero_si256 (), (__mmask16)(U)))
#define _mm_mask_shufflehi_epi16(W, U, A, B) ((__m128i) __builtin_ia32_pshufhw128_mask ((__v8hi)(__m128i)(A), (int)(B), (__v8hi)(__m128i)(W), (__mmask8)(U)))
#define _mm_maskz_shufflehi_epi16(U, A, B) ((__m128i) __builtin_ia32_pshufhw128_mask ((__v8hi)(__m128i)(A), (int)(B), (__v8hi)(__m128i)_mm_avx512_setzero_si128 (), (__mmask8)(U)))
#define _mm256_mask_shufflelo_epi16(W, U, A, B) ((__m256i) __builtin_ia32_pshuflw256_mask ((__v16hi)(__m256i)(A), (int)(B), (__v16hi)(__m256i)(W), (__mmask16)(U)))
#define _mm256_maskz_shufflelo_epi16(U, A, B) ((__m256i) __builtin_ia32_pshuflw256_mask ((__v16hi)(__m256i)(A), (int)(B), (__v16hi)(__m256i)_mm256_avx512_setzero_si256 (), (__mmask16)(U)))
#define _mm_mask_shufflelo_epi16(W, U, A, B) ((__m128i) __builtin_ia32_pshuflw128_mask ((__v8hi)(__m128i)(A), (int)(B), (__v8hi)(__m128i)(W), (__mmask8)(U)))
#define _mm_maskz_shufflelo_epi16(U, A, B) ((__m128i) __builtin_ia32_pshuflw128_mask ((__v8hi)(__m128i)(A), (int)(B), (__v8hi)(__m128i)_mm_avx512_setzero_si128 (), (__mmask8)(U)))
#define _mm256_maskz_alignr_epi8(U, X, Y, N) ((__m256i) __builtin_ia32_palignr256_mask ((__v4di)(__m256i)(X), (__v4di)(__m256i)(Y), (int)((N) * 8), (__v4di)(__m256i)_mm256_avx512_setzero_si256 (), (__mmask32)(U)))
#define _mm_mask_alignr_epi8(W, U, X, Y, N) ((__m128i) __builtin_ia32_palignr128_mask ((__v2di)(__m128i)(X), (__v2di)(__m128i)(Y), (int)((N) * 8), (__v2di)(__m128i)(X), (__mmask16)(U)))
#define _mm_maskz_alignr_epi8(U, X, Y, N) ((__m128i) __builtin_ia32_palignr128_mask ((__v2di)(__m128i)(X), (__v2di)(__m128i)(Y), (int)((N) * 8), (__v2di)(__m128i)_mm_avx512_setzero_si128 (), (__mmask16)(U)))
#define _mm_mask_slli_epi16(W, U, X, C) ((__m128i)__builtin_ia32_psllwi128_mask ((__v8hi)(__m128i)(X), (unsigned int)(C), (__v8hi)(__m128i)(W), (__mmask8)(U)))
#define _mm_maskz_slli_epi16(U, X, C) ((__m128i)__builtin_ia32_psllwi128_mask ((__v8hi)(__m128i)(X), (unsigned int)(C), (__v8hi)(__m128i)_mm_avx512_setzero_si128 (), (__mmask8)(U)))
#define _mm256_dbsad_epu8(X, Y, C) ((__m256i) __builtin_ia32_dbpsadbw256_mask ((__v32qi)(__m256i) (X), (__v32qi)(__m256i) (Y), (int) (C), (__v16hi)(__m256i)_mm256_avx512_setzero_si256(), (__mmask16)-1))
#define _mm256_mask_slli_epi16(W, U, X, C) ((__m256i)__builtin_ia32_psllwi256_mask ((__v16hi)(__m256i)(X), (unsigned int)(C), (__v16hi)(__m256i)(W), (__mmask16)(U)))
#define _mm256_maskz_slli_epi16(U, X, C) ((__m256i)__builtin_ia32_psllwi256_mask ((__v16hi)(__m256i)(X), (unsigned int)(C), (__v16hi)(__m256i)_mm256_avx512_setzero_si256 (), (__mmask16)(U)))
#define _mm256_mask_dbsad_epu8(W, U, X, Y, C) ((__m256i) __builtin_ia32_dbpsadbw256_mask ((__v32qi)(__m256i) (X), (__v32qi)(__m256i) (Y), (int) (C), (__v16hi)(__m256i)(W), (__mmask16)(U)))
#define _mm256_maskz_dbsad_epu8(U, X, Y, C) ((__m256i) __builtin_ia32_dbpsadbw256_mask ((__v32qi)(__m256i) (X), (__v32qi)(__m256i) (Y), (int) (C), (__v16hi)(__m256i)_mm256_avx512_setzero_si256(), (__mmask16)(U)))
#define _mm_dbsad_epu8(X, Y, C) ((__m128i) __builtin_ia32_dbpsadbw128_mask ((__v16qi)(__m128i) (X), (__v16qi)(__m128i) (Y), (int) (C), (__v8hi)(__m128i)_mm_avx512_setzero_si128(), (__mmask8)-1))
#define _mm_mask_dbsad_epu8(W, U, X, Y, C) ((__m128i) __builtin_ia32_dbpsadbw128_mask ((__v16qi)(__m128i) (X), (__v16qi)(__m128i) (Y), (int) (C), (__v8hi)(__m128i)(W), (__mmask8)(U)))
#define _mm_maskz_dbsad_epu8(U, X, Y, C) ((__m128i) __builtin_ia32_dbpsadbw128_mask ((__v16qi)(__m128i) (X), (__v16qi)(__m128i) (Y), (int) (C), (__v8hi)(__m128i)_mm_avx512_setzero_si128(), (__mmask8)(U)))
#define _mm_cmp_epi16_mask(X, Y, P) ((__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi)(__m128i)(X), (__v8hi)(__m128i)(Y), (int)(P), (__mmask8)(-1)))
#define _mm_cmp_epi8_mask(X, Y, P) ((__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi)(__m128i)(X), (__v16qi)(__m128i)(Y), (int)(P), (__mmask16)(-1)))
#define _mm256_cmp_epi16_mask(X, Y, P) ((__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi)(__m256i)(X), (__v16hi)(__m256i)(Y), (int)(P), (__mmask16)(-1)))
#define _mm256_cmp_epi8_mask(X, Y, P) ((__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi)(__m256i)(X), (__v32qi)(__m256i)(Y), (int)(P), (__mmask32)(-1)))
#define _mm_cmp_epu16_mask(X, Y, P) ((__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi)(__m128i)(X), (__v8hi)(__m128i)(Y), (int)(P), (__mmask8)(-1)))
#define _mm_cmp_epu8_mask(X, Y, P) ((__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi)(__m128i)(X), (__v16qi)(__m128i)(Y), (int)(P), (__mmask16)(-1)))
#define _mm256_cmp_epu16_mask(X, Y, P) ((__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi)(__m256i)(X), (__v16hi)(__m256i)(Y), (int)(P), (__mmask16)(-1)))
#define _mm256_cmp_epu8_mask(X, Y, P) ((__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi)(__m256i)(X), (__v32qi)(__m256i)(Y), (int)(P), (__mmask32)-1))
#define _mm_mask_cmp_epi16_mask(M, X, Y, P) ((__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi)(__m128i)(X), (__v8hi)(__m128i)(Y), (int)(P), (__mmask8)(M)))
#define _mm_mask_cmp_epi8_mask(M, X, Y, P) ((__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi)(__m128i)(X), (__v16qi)(__m128i)(Y), (int)(P), (__mmask16)(M)))
#define _mm256_mask_cmp_epi16_mask(M, X, Y, P) ((__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi)(__m256i)(X), (__v16hi)(__m256i)(Y), (int)(P), (__mmask16)(M)))
#define _mm256_mask_cmp_epi8_mask(M, X, Y, P) ((__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi)(__m256i)(X), (__v32qi)(__m256i)(Y), (int)(P), (__mmask32)(M)))
#define _mm_mask_cmp_epu16_mask(M, X, Y, P) ((__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi)(__m128i)(X), (__v8hi)(__m128i)(Y), (int)(P), (__mmask8)(M)))
#define _mm_mask_cmp_epu8_mask(M, X, Y, P) ((__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi)(__m128i)(X), (__v16qi)(__m128i)(Y), (int)(P), (__mmask16)(M)))
#define _mm256_mask_cmp_epu16_mask(M, X, Y, P) ((__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi)(__m256i)(X), (__v16hi)(__m256i)(Y), (int)(P), (__mmask16)(M)))
#define _mm256_mask_cmp_epu8_mask(M, X, Y, P) ((__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi)(__m256i)(X), (__v32qi)(__m256i)(Y), (int)(P), (__mmask32)(M)))
#endif
extern __inline __mmask32
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpneq_epi8_mask (__m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi) __X,
        (__v32qi) __Y, 4,
        (__mmask32) -1);
}
extern __inline __mmask32
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmplt_epi8_mask (__m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi) __X,
        (__v32qi) __Y, 1,
        (__mmask32) -1);
}
extern __inline __mmask32
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpge_epi8_mask (__m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi) __X,
        (__v32qi) __Y, 5,
        (__mmask32) -1);
}
extern __inline __mmask32
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmple_epi8_mask (__m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi) __X,
        (__v32qi) __Y, 2,
        (__mmask32) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpneq_epi16_mask (__m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi) __X,
        (__v16hi) __Y, 4,
        (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmplt_epi16_mask (__m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi) __X,
        (__v16hi) __Y, 1,
        (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpge_epi16_mask (__m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi) __X,
        (__v16hi) __Y, 5,
        (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmple_epi16_mask (__m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi) __X,
        (__v16hi) __Y, 2,
        (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpneq_epu8_mask (__m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __X,
         (__v16qi) __Y, 4,
         (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_epu8_mask (__m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __X,
         (__v16qi) __Y, 1,
         (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpge_epu8_mask (__m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __X,
         (__v16qi) __Y, 5,
         (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmple_epu8_mask (__m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __X,
         (__v16qi) __Y, 2,
         (__mmask16) -1);
}
extern __inline __mmask8
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpneq_epu16_mask (__m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __X,
        (__v8hi) __Y, 4,
        (__mmask8) -1);
}
extern __inline __mmask8
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_epu16_mask (__m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __X,
        (__v8hi) __Y, 1,
        (__mmask8) -1);
}
extern __inline __mmask8
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpge_epu16_mask (__m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __X,
        (__v8hi) __Y, 5,
        (__mmask8) -1);
}
extern __inline __mmask8
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmple_epu16_mask (__m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __X,
        (__v8hi) __Y, 2,
        (__mmask8) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpneq_epi8_mask (__m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi) __X,
        (__v16qi) __Y, 4,
        (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_epi8_mask (__m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi) __X,
        (__v16qi) __Y, 1,
        (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpge_epi8_mask (__m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi) __X,
        (__v16qi) __Y, 5,
        (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmple_epi8_mask (__m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi) __X,
        (__v16qi) __Y, 2,
        (__mmask16) -1);
}
extern __inline __mmask8
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpneq_epi16_mask (__m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi) __X,
       (__v8hi) __Y, 4,
       (__mmask8) -1);
}
extern __inline __mmask8
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_epi16_mask (__m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi) __X,
       (__v8hi) __Y, 1,
       (__mmask8) -1);
}
extern __inline __mmask8
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpge_epi16_mask (__m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi) __X,
       (__v8hi) __Y, 5,
       (__mmask8) -1);
}
extern __inline __mmask8
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmple_epi16_mask (__m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi) __X,
       (__v8hi) __Y, 2,
       (__mmask8) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_mulhrs_epi16 (__m256i __W, __mmask16 __U, __m256i __X,
     __m256i __Y)
{
  return (__m256i) __builtin_ia32_pmulhrsw256_mask ((__v16hi) __X,
          (__v16hi) __Y,
          (__v16hi) __W,
          (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_mulhrs_epi16 (__mmask16 __U, __m256i __X, __m256i __Y)
{
  return (__m256i) __builtin_ia32_pmulhrsw256_mask ((__v16hi) __X,
          (__v16hi) __Y,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_mulhi_epu16 (__m256i __W, __mmask16 __U, __m256i __A,
    __m256i __B)
{
  return (__m256i) __builtin_ia32_pmulhuw256_mask ((__v16hi) __A,
         (__v16hi) __B,
         (__v16hi) __W,
         (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_mulhi_epu16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pmulhuw256_mask ((__v16hi) __A,
         (__v16hi) __B,
         (__v16hi)
         _mm256_avx512_setzero_si256 (),
         (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_mulhi_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
    __m256i __B)
{
  return (__m256i) __builtin_ia32_pmulhw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_mulhi_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pmulhw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_mulhi_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
        __m128i __B)
{
  return (__m128i) __builtin_ia32_pmulhw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi) __W,
        (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_mulhi_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pmulhw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_mulhi_epu16 (__m128i __W, __mmask8 __U, __m128i __A,
        __m128i __B)
{
  return (__m128i) __builtin_ia32_pmulhuw128_mask ((__v8hi) __A,
         (__v8hi) __B,
         (__v8hi) __W,
         (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_mulhi_epu16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pmulhuw128_mask ((__v8hi) __A,
         (__v8hi) __B,
         (__v8hi)
         _mm_avx512_setzero_si128 (),
         (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_mulhrs_epi16 (__m128i __W, __mmask8 __U, __m128i __X,
         __m128i __Y)
{
  return (__m128i) __builtin_ia32_pmulhrsw128_mask ((__v8hi) __X,
          (__v8hi) __Y,
          (__v8hi) __W,
          (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_mulhrs_epi16 (__mmask8 __U, __m128i __X, __m128i __Y)
{
  return (__m128i) __builtin_ia32_pmulhrsw128_mask ((__v8hi) __X,
          (__v8hi) __Y,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_mullo_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
    __m256i __B)
{
  return (__m256i) __builtin_ia32_pmullw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_mullo_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pmullw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_mullo_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
        __m128i __B)
{
  return (__m128i) __builtin_ia32_pmullw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi) __W,
        (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_mullo_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pmullw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtepi8_epi16 (__m256i __W, __mmask16 __U, __m128i __A)
{
  return (__m256i) __builtin_ia32_pmovsxbw256_mask ((__v16qi) __A,
          (__v16hi) __W,
          (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtepi8_epi16 (__mmask16 __U, __m128i __A)
{
  return (__m256i) __builtin_ia32_pmovsxbw256_mask ((__v16qi) __A,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtepi8_epi16 (__m128i __W, __mmask8 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_pmovsxbw128_mask ((__v16qi) __A,
          (__v8hi) __W,
          (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtepi8_epi16 (__mmask8 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_pmovsxbw128_mask ((__v16qi) __A,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cvtepu8_epi16 (__m256i __W, __mmask16 __U, __m128i __A)
{
  return (__m256i) __builtin_ia32_pmovzxbw256_mask ((__v16qi) __A,
          (__v16hi) __W,
          (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_cvtepu8_epi16 (__mmask16 __U, __m128i __A)
{
  return (__m256i) __builtin_ia32_pmovzxbw256_mask ((__v16qi) __A,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtepu8_epi16 (__m128i __W, __mmask8 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_pmovzxbw128_mask ((__v16qi) __A,
          (__v8hi) __W,
          (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtepu8_epi16 (__mmask8 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_pmovzxbw128_mask ((__v16qi) __A,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_avg_epu8 (__m256i __W, __mmask32 __U, __m256i __A,
        __m256i __B)
{
  return (__m256i) __builtin_ia32_pavgb256_mask ((__v32qi) __A,
       (__v32qi) __B,
       (__v32qi) __W,
       (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_avg_epu8 (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pavgb256_mask ((__v32qi) __A,
       (__v32qi) __B,
       (__v32qi)
       _mm256_avx512_setzero_si256 (),
       (__mmask32) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_avg_epu8 (__m128i __W, __mmask16 __U, __m128i __A,
     __m128i __B)
{
  return (__m128i) __builtin_ia32_pavgb128_mask ((__v16qi) __A,
       (__v16qi) __B,
       (__v16qi) __W,
       (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_avg_epu8 (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pavgb128_mask ((__v16qi) __A,
       (__v16qi) __B,
       (__v16qi)
       _mm_avx512_setzero_si128 (),
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_avg_epu16 (__m256i __W, __mmask16 __U, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_pavgw256_mask ((__v16hi) __A,
       (__v16hi) __B,
       (__v16hi) __W,
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_avg_epu16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pavgw256_mask ((__v16hi) __A,
       (__v16hi) __B,
       (__v16hi)
       _mm256_avx512_setzero_si256 (),
       (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_avg_epu16 (__m128i __W, __mmask8 __U, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_pavgw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi) __W,
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_avg_epu16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pavgw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_add_epi8 (__m256i __W, __mmask32 __U, __m256i __A,
        __m256i __B)
{
  return (__m256i) __builtin_ia32_paddb256_mask ((__v32qi) __A,
       (__v32qi) __B,
       (__v32qi) __W,
       (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_add_epi8 (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_paddb256_mask ((__v32qi) __A,
       (__v32qi) __B,
       (__v32qi)
       _mm256_avx512_setzero_si256 (),
       (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_add_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_paddw256_mask ((__v16hi) __A,
       (__v16hi) __B,
       (__v16hi) __W,
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_add_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_paddw256_mask ((__v16hi) __A,
       (__v16hi) __B,
       (__v16hi)
       _mm256_avx512_setzero_si256 (),
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_adds_epi8 (__m256i __W, __mmask32 __U, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_paddsb256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi) __W,
        (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_adds_epi8 (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_paddsb256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi)
        _mm256_avx512_setzero_si256 (),
        (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_adds_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
   __m256i __B)
{
  return (__m256i) __builtin_ia32_paddsw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_adds_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_paddsw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_adds_epu8 (__m256i __W, __mmask32 __U, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_paddusb256_mask ((__v32qi) __A,
         (__v32qi) __B,
         (__v32qi) __W,
         (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_adds_epu8 (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_paddusb256_mask ((__v32qi) __A,
         (__v32qi) __B,
         (__v32qi)
         _mm256_avx512_setzero_si256 (),
         (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_adds_epu16 (__m256i __W, __mmask16 __U, __m256i __A,
   __m256i __B)
{
  return (__m256i) __builtin_ia32_paddusw256_mask ((__v16hi) __A,
         (__v16hi) __B,
         (__v16hi) __W,
         (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_adds_epu16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_paddusw256_mask ((__v16hi) __A,
         (__v16hi) __B,
         (__v16hi)
         _mm256_avx512_setzero_si256 (),
         (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_sub_epi8 (__m256i __W, __mmask32 __U, __m256i __A,
        __m256i __B)
{
  return (__m256i) __builtin_ia32_psubb256_mask ((__v32qi) __A,
       (__v32qi) __B,
       (__v32qi) __W,
       (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_sub_epi8 (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psubb256_mask ((__v32qi) __A,
       (__v32qi) __B,
       (__v32qi)
       _mm256_avx512_setzero_si256 (),
       (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_sub_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_psubw256_mask ((__v16hi) __A,
       (__v16hi) __B,
       (__v16hi) __W,
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_sub_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psubw256_mask ((__v16hi) __A,
       (__v16hi) __B,
       (__v16hi)
       _mm256_avx512_setzero_si256 (),
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_subs_epi8 (__m256i __W, __mmask32 __U, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_psubsb256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi) __W,
        (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_subs_epi8 (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psubsb256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi)
        _mm256_avx512_setzero_si256 (),
        (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_subs_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
   __m256i __B)
{
  return (__m256i) __builtin_ia32_psubsw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_subs_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psubsw256_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_subs_epu8 (__m256i __W, __mmask32 __U, __m256i __A,
         __m256i __B)
{
  return (__m256i) __builtin_ia32_psubusb256_mask ((__v32qi) __A,
         (__v32qi) __B,
         (__v32qi) __W,
         (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_subs_epu8 (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psubusb256_mask ((__v32qi) __A,
         (__v32qi) __B,
         (__v32qi)
         _mm256_avx512_setzero_si256 (),
         (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_subs_epu16 (__m256i __W, __mmask16 __U, __m256i __A,
   __m256i __B)
{
  return (__m256i) __builtin_ia32_psubusw256_mask ((__v16hi) __A,
         (__v16hi) __B,
         (__v16hi) __W,
         (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_subs_epu16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psubusw256_mask ((__v16hi) __A,
         (__v16hi) __B,
         (__v16hi)
         _mm256_avx512_setzero_si256 (),
         (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_add_epi8 (__m128i __W, __mmask16 __U, __m128i __A,
     __m128i __B)
{
  return (__m128i) __builtin_ia32_paddb128_mask ((__v16qi) __A,
       (__v16qi) __B,
       (__v16qi) __W,
       (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_add_epi8 (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_paddb128_mask ((__v16qi) __A,
       (__v16qi) __B,
       (__v16qi)
       _mm_avx512_setzero_si128 (),
       (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_add_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_paddw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi) __W,
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_add_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_paddw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_unpackhi_epi8 (__m256i __W, __mmask32 __U, __m256i __A,
      __m256i __B)
{
  return (__m256i) __builtin_ia32_punpckhbw256_mask ((__v32qi) __A,
           (__v32qi) __B,
           (__v32qi) __W,
           (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_unpackhi_epi8 (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_punpckhbw256_mask ((__v32qi) __A,
           (__v32qi) __B,
           (__v32qi)
           _mm256_avx512_setzero_si256 (),
           (__mmask32) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_unpackhi_epi8 (__m128i __W, __mmask16 __U, __m128i __A,
   __m128i __B)
{
  return (__m128i) __builtin_ia32_punpckhbw128_mask ((__v16qi) __A,
           (__v16qi) __B,
           (__v16qi) __W,
           (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_unpackhi_epi8 (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_punpckhbw128_mask ((__v16qi) __A,
           (__v16qi) __B,
           (__v16qi)
           _mm_avx512_setzero_si128 (),
           (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_unpackhi_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
       __m256i __B)
{
  return (__m256i) __builtin_ia32_punpckhwd256_mask ((__v16hi) __A,
           (__v16hi) __B,
           (__v16hi) __W,
           (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_unpackhi_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_punpckhwd256_mask ((__v16hi) __A,
           (__v16hi) __B,
           (__v16hi)
           _mm256_avx512_setzero_si256 (),
           (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_unpackhi_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
    __m128i __B)
{
  return (__m128i) __builtin_ia32_punpckhwd128_mask ((__v8hi) __A,
           (__v8hi) __B,
           (__v8hi) __W,
           (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_unpackhi_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_punpckhwd128_mask ((__v8hi) __A,
           (__v8hi) __B,
           (__v8hi)
           _mm_avx512_setzero_si128 (),
           (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_unpacklo_epi8 (__m256i __W, __mmask32 __U, __m256i __A,
      __m256i __B)
{
  return (__m256i) __builtin_ia32_punpcklbw256_mask ((__v32qi) __A,
           (__v32qi) __B,
           (__v32qi) __W,
           (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_unpacklo_epi8 (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_punpcklbw256_mask ((__v32qi) __A,
           (__v32qi) __B,
           (__v32qi)
           _mm256_avx512_setzero_si256 (),
           (__mmask32) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_unpacklo_epi8 (__m128i __W, __mmask16 __U, __m128i __A,
   __m128i __B)
{
  return (__m128i) __builtin_ia32_punpcklbw128_mask ((__v16qi) __A,
           (__v16qi) __B,
           (__v16qi) __W,
           (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_unpacklo_epi8 (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_punpcklbw128_mask ((__v16qi) __A,
           (__v16qi) __B,
           (__v16qi)
           _mm_avx512_setzero_si128 (),
           (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_unpacklo_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
       __m256i __B)
{
  return (__m256i) __builtin_ia32_punpcklwd256_mask ((__v16hi) __A,
           (__v16hi) __B,
           (__v16hi) __W,
           (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_unpacklo_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_punpcklwd256_mask ((__v16hi) __A,
           (__v16hi) __B,
           (__v16hi)
           _mm256_avx512_setzero_si256 (),
           (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_unpacklo_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
    __m128i __B)
{
  return (__m128i) __builtin_ia32_punpcklwd128_mask ((__v8hi) __A,
           (__v8hi) __B,
           (__v8hi) __W,
           (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_unpacklo_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_punpcklwd128_mask ((__v8hi) __A,
           (__v8hi) __B,
           (__v8hi)
           _mm_avx512_setzero_si128 (),
           (__mmask8) __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_epi8_mask (__m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_pcmpeqb128_mask ((__v16qi) __A,
           (__v16qi) __B,
           (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_epu8_mask (__m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __A,
          (__v16qi) __B, 0,
          (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpeq_epu8_mask (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __A,
          (__v16qi) __B, 0,
          __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpeq_epi8_mask (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_pcmpeqb128_mask ((__v16qi) __A,
           (__v16qi) __B,
           __U);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpeq_epu8_mask (__m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __A,
          (__v32qi) __B, 0,
          (__mmask32) -1);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpeq_epi8_mask (__m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_pcmpeqb256_mask ((__v32qi) __A,
           (__v32qi) __B,
           (__mmask32) -1);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpeq_epu8_mask (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __A,
          (__v32qi) __B, 0,
          __U);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpeq_epi8_mask (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_pcmpeqb256_mask ((__v32qi) __A,
           (__v32qi) __B,
           __U);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_epu16_mask (__m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __A,
         (__v8hi) __B, 0,
         (__mmask8) -1);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_epi16_mask (__m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_pcmpeqw128_mask ((__v8hi) __A,
          (__v8hi) __B,
          (__mmask8) -1);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpeq_epu16_mask (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __A,
         (__v8hi) __B, 0, __U);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpeq_epi16_mask (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_pcmpeqw128_mask ((__v8hi) __A,
          (__v8hi) __B, __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpeq_epu16_mask (__m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __A,
          (__v16hi) __B, 0,
          (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpeq_epi16_mask (__m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_pcmpeqw256_mask ((__v16hi) __A,
           (__v16hi) __B,
           (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpeq_epu16_mask (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __A,
          (__v16hi) __B, 0,
          __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpeq_epi16_mask (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_pcmpeqw256_mask ((__v16hi) __A,
           (__v16hi) __B,
           __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_epu8_mask (__m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __A,
          (__v16qi) __B, 6,
          (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_epi8_mask (__m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_pcmpgtb128_mask ((__v16qi) __A,
           (__v16qi) __B,
           (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpgt_epu8_mask (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __A,
          (__v16qi) __B, 6,
          __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpgt_epi8_mask (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_pcmpgtb128_mask ((__v16qi) __A,
           (__v16qi) __B,
           __U);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpgt_epu8_mask (__m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __A,
          (__v32qi) __B, 6,
          (__mmask32) -1);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpgt_epi8_mask (__m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_pcmpgtb256_mask ((__v32qi) __A,
           (__v32qi) __B,
           (__mmask32) -1);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpgt_epu8_mask (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __A,
          (__v32qi) __B, 6,
          __U);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpgt_epi8_mask (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_pcmpgtb256_mask ((__v32qi) __A,
           (__v32qi) __B,
           __U);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_epu16_mask (__m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __A,
         (__v8hi) __B, 6,
         (__mmask8) -1);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_epi16_mask (__m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_pcmpgtw128_mask ((__v8hi) __A,
          (__v8hi) __B,
          (__mmask8) -1);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpgt_epu16_mask (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __A,
         (__v8hi) __B, 6, __U);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpgt_epi16_mask (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_pcmpgtw128_mask ((__v8hi) __A,
          (__v8hi) __B, __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpgt_epu16_mask (__m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __A,
          (__v16hi) __B, 6,
          (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpgt_epi16_mask (__m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_pcmpgtw256_mask ((__v16hi) __A,
           (__v16hi) __B,
           (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpgt_epu16_mask (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __A,
          (__v16hi) __B, 6,
          __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpgt_epi16_mask (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_pcmpgtw256_mask ((__v16hi) __A,
           (__v16hi) __B,
           __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_testn_epi8_mask (__m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_ptestnmb128 ((__v16qi) __A,
       (__v16qi) __B,
       (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_testn_epi8_mask (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__mmask16) __builtin_ia32_ptestnmb128 ((__v16qi) __A,
       (__v16qi) __B, __U);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_testn_epi8_mask (__m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_ptestnmb256 ((__v32qi) __A,
       (__v32qi) __B,
       (__mmask32) -1);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_testn_epi8_mask (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__mmask32) __builtin_ia32_ptestnmb256 ((__v32qi) __A,
       (__v32qi) __B, __U);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_testn_epi16_mask (__m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_ptestnmw128 ((__v8hi) __A,
      (__v8hi) __B,
      (__mmask8) -1);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_testn_epi16_mask (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__mmask8) __builtin_ia32_ptestnmw128 ((__v8hi) __A,
      (__v8hi) __B, __U);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_testn_epi16_mask (__m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_ptestnmw256 ((__v16hi) __A,
       (__v16hi) __B,
       (__mmask16) -1);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_testn_epi16_mask (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__mmask16) __builtin_ia32_ptestnmw256 ((__v16hi) __A,
       (__v16hi) __B, __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_shuffle_epi8 (__m256i __W, __mmask32 __U, __m256i __A,
     __m256i __B)
{
  return (__m256i) __builtin_ia32_pshufb256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi) __W,
        (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_shuffle_epi8 (__mmask32 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_pshufb256_mask ((__v32qi) __A,
        (__v32qi) __B,
        (__v32qi)
        _mm256_avx512_setzero_si256 (),
        (__mmask32) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_shuffle_epi8 (__m128i __W, __mmask16 __U, __m128i __A,
         __m128i __B)
{
  return (__m128i) __builtin_ia32_pshufb128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi) __W,
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_shuffle_epi8 (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_pshufb128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi)
        _mm_avx512_setzero_si128 (),
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_packs_epi16 (__mmask32 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_packsswb256_mask ((__v16hi) __A,
          (__v16hi) __B,
          (__v32qi)
          _mm256_avx512_setzero_si256 (),
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_packs_epi16 (__m256i __W, __mmask32 __M, __m256i __A,
    __m256i __B)
{
  return (__m256i) __builtin_ia32_packsswb256_mask ((__v16hi) __A,
          (__v16hi) __B,
          (__v32qi) __W,
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_packs_epi16 (__mmask16 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_packsswb128_mask ((__v8hi) __A,
          (__v8hi) __B,
          (__v16qi)
          _mm_avx512_setzero_si128 (),
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_packs_epi16 (__m128i __W, __mmask16 __M, __m128i __A,
        __m128i __B)
{
  return (__m128i) __builtin_ia32_packsswb128_mask ((__v8hi) __A,
          (__v8hi) __B,
          (__v16qi) __W,
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_packus_epi16 (__mmask32 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_packuswb256_mask ((__v16hi) __A,
          (__v16hi) __B,
          (__v32qi)
          _mm256_avx512_setzero_si256 (),
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_packus_epi16 (__m256i __W, __mmask32 __M, __m256i __A,
     __m256i __B)
{
  return (__m256i) __builtin_ia32_packuswb256_mask ((__v16hi) __A,
          (__v16hi) __B,
          (__v32qi) __W,
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_packus_epi16 (__mmask16 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_packuswb128_mask ((__v8hi) __A,
          (__v8hi) __B,
          (__v16qi)
          _mm_avx512_setzero_si128 (),
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_packus_epi16 (__m128i __W, __mmask16 __M, __m128i __A,
         __m128i __B)
{
  return (__m128i) __builtin_ia32_packuswb128_mask ((__v8hi) __A,
          (__v8hi) __B,
          (__v16qi) __W,
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_abs_epi8 (__m256i __W, __mmask32 __U, __m256i __A)
{
  return (__m256i) __builtin_ia32_pabsb256_mask ((__v32qi) __A,
       (__v32qi) __W,
       (__mmask32) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_abs_epi8 (__mmask32 __U, __m256i __A)
{
  return (__m256i) __builtin_ia32_pabsb256_mask ((__v32qi) __A,
       (__v32qi)
       _mm256_avx512_setzero_si256 (),
       (__mmask32) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_abs_epi8 (__m128i __W, __mmask16 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_pabsb128_mask ((__v16qi) __A,
       (__v16qi) __W,
       (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_abs_epi8 (__mmask16 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_pabsb128_mask ((__v16qi) __A,
       (__v16qi)
       _mm_avx512_setzero_si128 (),
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_abs_epi16 (__m256i __W, __mmask16 __U, __m256i __A)
{
  return (__m256i) __builtin_ia32_pabsw256_mask ((__v16hi) __A,
       (__v16hi) __W,
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_abs_epi16 (__mmask16 __U, __m256i __A)
{
  return (__m256i) __builtin_ia32_pabsw256_mask ((__v16hi) __A,
       (__v16hi)
       _mm256_avx512_setzero_si256 (),
       (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_abs_epi16 (__m128i __W, __mmask8 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_pabsw128_mask ((__v8hi) __A,
       (__v8hi) __W,
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_abs_epi16 (__mmask8 __U, __m128i __A)
{
  return (__m128i) __builtin_ia32_pabsw128_mask ((__v8hi) __A,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) __U);
}
extern __inline __mmask32
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpneq_epu8_mask (__m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __X,
         (__v32qi) __Y, 4,
         (__mmask32) -1);
}
extern __inline __mmask32
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmplt_epu8_mask (__m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __X,
         (__v32qi) __Y, 1,
         (__mmask32) -1);
}
extern __inline __mmask32
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpge_epu8_mask (__m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __X,
         (__v32qi) __Y, 5,
         (__mmask32) -1);
}
extern __inline __mmask32
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmple_epu8_mask (__m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __X,
         (__v32qi) __Y, 2,
         (__mmask32) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpneq_epu16_mask (__m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __X,
         (__v16hi) __Y, 4,
         (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmplt_epu16_mask (__m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __X,
         (__v16hi) __Y, 1,
         (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmpge_epu16_mask (__m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __X,
         (__v16hi) __Y, 5,
         (__mmask16) -1);
}
extern __inline __mmask16
  __attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_cmple_epu16_mask (__m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __X,
         (__v16hi) __Y, 2,
         (__mmask16) -1);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_storeu_epi16 (void *__P, __m256i __A)
{
  *(__v16hi_u *) __P = (__v16hi_u) __A;
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_storeu_epi16 (void *__P, __mmask16 __U, __m256i __A)
{
  __builtin_ia32_storedquhi256_mask ((short *) __P,
         (__v16hi) __A,
         (__mmask16) __U);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_storeu_epi16 (void *__P, __m128i __A)
{
  *(__v8hi_u *) __P = (__v8hi_u) __A;
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_storeu_epi16 (void *__P, __mmask8 __U, __m128i __A)
{
  __builtin_ia32_storedquhi128_mask ((short *) __P,
         (__v8hi) __A,
         (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_adds_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
       __m128i __B)
{
  return (__m128i) __builtin_ia32_paddsw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi) __W,
        (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_subs_epi8 (__m128i __W, __mmask16 __U, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_psubsb128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi) __W,
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_subs_epi8 (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psubsb128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi)
        _mm_avx512_setzero_si128 (),
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_subs_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
       __m128i __B)
{
  return (__m128i) __builtin_ia32_psubsw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi) __W,
        (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_subs_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psubsw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_subs_epu8 (__m128i __W, __mmask16 __U, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_psubusb128_mask ((__v16qi) __A,
         (__v16qi) __B,
         (__v16qi) __W,
         (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_subs_epu8 (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psubusb128_mask ((__v16qi) __A,
         (__v16qi) __B,
         (__v16qi)
         _mm_avx512_setzero_si128 (),
         (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_subs_epu16 (__m128i __W, __mmask8 __U, __m128i __A,
       __m128i __B)
{
  return (__m128i) __builtin_ia32_psubusw128_mask ((__v8hi) __A,
         (__v8hi) __B,
         (__v8hi) __W,
         (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_subs_epu16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psubusw128_mask ((__v8hi) __A,
         (__v8hi) __B,
         (__v8hi)
         _mm_avx512_setzero_si128 (),
         (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_srl_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
         __m128i __B)
{
  return (__m256i) __builtin_ia32_psrlw256_mask ((__v16hi) __A,
       (__v8hi) __B,
       (__v16hi) __W,
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_srl_epi16 (__mmask16 __U, __m256i __A, __m128i __B)
{
  return (__m256i) __builtin_ia32_psrlw256_mask ((__v16hi) __A,
       (__v8hi) __B,
       (__v16hi)
       _mm256_avx512_setzero_si256 (),
       (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_srl_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_psrlw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi) __W,
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_srl_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psrlw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_sra_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
         __m128i __B)
{
  return (__m256i) __builtin_ia32_psraw256_mask ((__v16hi) __A,
       (__v8hi) __B,
       (__v16hi) __W,
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_sra_epi16 (__mmask16 __U, __m256i __A, __m128i __B)
{
  return (__m256i) __builtin_ia32_psraw256_mask ((__v16hi) __A,
       (__v8hi) __B,
       (__v16hi)
       _mm256_avx512_setzero_si256 (),
       (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_sra_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_psraw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi) __W,
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_sra_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psraw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_adds_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_paddsw128_mask ((__v8hi) __A,
        (__v8hi) __B,
        (__v8hi)
        _mm_avx512_setzero_si128 (),
        (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_adds_epu8 (__m128i __W, __mmask16 __U, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_paddusb128_mask ((__v16qi) __A,
         (__v16qi) __B,
         (__v16qi) __W,
         (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_adds_epu8 (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_paddusb128_mask ((__v16qi) __A,
         (__v16qi) __B,
         (__v16qi)
         _mm_avx512_setzero_si128 (),
         (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_adds_epu16 (__m128i __W, __mmask8 __U, __m128i __A,
       __m128i __B)
{
  return (__m128i) __builtin_ia32_paddusw128_mask ((__v8hi) __A,
         (__v8hi) __B,
         (__v8hi) __W,
         (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_adds_epu16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_paddusw128_mask ((__v8hi) __A,
         (__v8hi) __B,
         (__v8hi)
         _mm_avx512_setzero_si128 (),
         (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_sub_epi8 (__m128i __W, __mmask16 __U, __m128i __A,
     __m128i __B)
{
  return (__m128i) __builtin_ia32_psubb128_mask ((__v16qi) __A,
       (__v16qi) __B,
       (__v16qi) __W,
       (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_sub_epi8 (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psubb128_mask ((__v16qi) __A,
       (__v16qi) __B,
       (__v16qi)
       _mm_avx512_setzero_si128 (),
       (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_sub_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_psubw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi) __W,
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_sub_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psubw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_adds_epi8 (__m128i __W, __mmask16 __U, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_paddsb128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi) __W,
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_adds_epi8 (__mmask16 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_paddsb128_mask ((__v16qi) __A,
        (__v16qi) __B,
        (__v16qi)
        _mm_avx512_setzero_si128 (),
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtepi16_epi8 (__m128i __A)
{
  return (__m128i) __builtin_ia32_pmovwb128_mask ((__v8hi) __A,
        (__v16qi)_mm_avx512_undefined_si128(),
        (__mmask8) -1);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtepi16_storeu_epi8 (void * __P, __mmask8 __M,__m128i __A)
{
  __builtin_ia32_pmovwb128mem_mask ((unsigned long long *) __P , (__v8hi) __A, __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cvtepi16_epi8 (__m128i __O, __mmask8 __M, __m128i __A)
{
  return (__m128i) __builtin_ia32_pmovwb128_mask ((__v8hi) __A,
        (__v16qi) __O, __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_cvtepi16_epi8 (__mmask8 __M, __m128i __A)
{
  return (__m128i) __builtin_ia32_pmovwb128_mask ((__v8hi) __A,
        (__v16qi)
        _mm_avx512_setzero_si128 (),
        __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_srav_epi16 (__m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psrav16hi_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_srav_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
   __m256i __B)
{
  return (__m256i) __builtin_ia32_psrav16hi_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_srav_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psrav16hi_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_srav_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psrav8hi_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_srav_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
       __m128i __B)
{
  return (__m128i) __builtin_ia32_psrav8hi_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi) __W,
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_srav_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psrav8hi_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_srlv_epi16 (__m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psrlv16hi_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_srlv_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
   __m256i __B)
{
  return (__m256i) __builtin_ia32_psrlv16hi_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_srlv_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psrlv16hi_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_srlv_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psrlv8hi_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_srlv_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
       __m128i __B)
{
  return (__m128i) __builtin_ia32_psrlv8hi_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi) __W,
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_srlv_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psrlv8hi_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_sllv_epi16 (__m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psllv16hi_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) -1);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_sllv_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
   __m256i __B)
{
  return (__m256i) __builtin_ia32_psllv16hi_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi) __W,
        (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_sllv_epi16 (__mmask16 __U, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_psllv16hi_mask ((__v16hi) __A,
        (__v16hi) __B,
        (__v16hi)
        _mm256_avx512_setzero_si256 (),
        (__mmask16) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_sllv_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psllv8hi_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) -1);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_sllv_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
       __m128i __B)
{
  return (__m128i) __builtin_ia32_psllv8hi_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi) __W,
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_sllv_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psllv8hi_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_sll_epi16 (__m128i __W, __mmask8 __U, __m128i __A,
      __m128i __B)
{
  return (__m128i) __builtin_ia32_psllw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi) __W,
       (__mmask8) __U);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_sll_epi16 (__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_psllw128_mask ((__v8hi) __A,
       (__v8hi) __B,
       (__v8hi)
       _mm_avx512_setzero_si128 (),
       (__mmask8) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_sll_epi16 (__m256i __W, __mmask16 __U, __m256i __A,
         __m128i __B)
{
  return (__m256i) __builtin_ia32_psllw256_mask ((__v16hi) __A,
       (__v8hi) __B,
       (__v16hi) __W,
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_sll_epi16 (__mmask16 __U, __m256i __A, __m128i __B)
{
  return (__m256i) __builtin_ia32_psllw256_mask ((__v16hi) __A,
       (__v8hi) __B,
       (__v16hi)
       _mm256_avx512_setzero_si256 (),
       (__mmask16) __U);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_packus_epi32 (__mmask16 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_packusdw256_mask ((__v8si) __A,
          (__v8si) __B,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_packus_epi32 (__m256i __W, __mmask16 __M, __m256i __A,
     __m256i __B)
{
  return (__m256i) __builtin_ia32_packusdw256_mask ((__v8si) __A,
          (__v8si) __B,
          (__v16hi) __W,
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_packus_epi32 (__mmask8 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_packusdw128_mask ((__v4si) __A,
          (__v4si) __B,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_packus_epi32 (__m128i __W, __mmask8 __M, __m128i __A,
         __m128i __B)
{
  return (__m128i) __builtin_ia32_packusdw128_mask ((__v4si) __A,
          (__v4si) __B,
          (__v8hi) __W, __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_maskz_packs_epi32 (__mmask16 __M, __m256i __A, __m256i __B)
{
  return (__m256i) __builtin_ia32_packssdw256_mask ((__v8si) __A,
          (__v8si) __B,
          (__v16hi)
          _mm256_avx512_setzero_si256 (),
          __M);
}
extern __inline __m256i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_packs_epi32 (__m256i __W, __mmask16 __M, __m256i __A,
    __m256i __B)
{
  return (__m256i) __builtin_ia32_packssdw256_mask ((__v8si) __A,
          (__v8si) __B,
          (__v16hi) __W,
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_packs_epi32 (__mmask8 __M, __m128i __A, __m128i __B)
{
  return (__m128i) __builtin_ia32_packssdw128_mask ((__v4si) __A,
          (__v4si) __B,
          (__v8hi)
          _mm_avx512_setzero_si128 (),
          __M);
}
extern __inline __m128i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_packs_epi32 (__m128i __W, __mmask8 __M, __m128i __A,
        __m128i __B)
{
  return (__m128i) __builtin_ia32_packssdw128_mask ((__v4si) __A,
          (__v4si) __B,
          (__v8hi) __W, __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpneq_epu8_mask (__mmask16 __M, __m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __X,
         (__v16qi) __Y, 4,
         (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmplt_epu8_mask (__mmask16 __M, __m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __X,
         (__v16qi) __Y, 1,
         (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpge_epu8_mask (__mmask16 __M, __m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __X,
         (__v16qi) __Y, 5,
         (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmple_epu8_mask (__mmask16 __M, __m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpb128_mask ((__v16qi) __X,
         (__v16qi) __Y, 2,
         (__mmask16) __M);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpneq_epu16_mask (__mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __X,
        (__v8hi) __Y, 4,
        (__mmask8) __M);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmplt_epu16_mask (__mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __X,
        (__v8hi) __Y, 1,
        (__mmask8) __M);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpge_epu16_mask (__mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __X,
        (__v8hi) __Y, 5,
        (__mmask8) __M);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmple_epu16_mask (__mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_ucmpw128_mask ((__v8hi) __X,
        (__v8hi) __Y, 2,
        (__mmask8) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpneq_epi8_mask (__mmask16 __M, __m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi) __X,
        (__v16qi) __Y, 4,
        (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmplt_epi8_mask (__mmask16 __M, __m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi) __X,
        (__v16qi) __Y, 1,
        (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpge_epi8_mask (__mmask16 __M, __m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi) __X,
        (__v16qi) __Y, 5,
        (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmple_epi8_mask (__mmask16 __M, __m128i __X, __m128i __Y)
{
  return (__mmask16) __builtin_ia32_cmpb128_mask ((__v16qi) __X,
        (__v16qi) __Y, 2,
        (__mmask16) __M);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpneq_epi16_mask (__mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi) __X,
       (__v8hi) __Y, 4,
       (__mmask8) __M);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmplt_epi16_mask (__mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi) __X,
       (__v8hi) __Y, 1,
       (__mmask8) __M);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmpge_epi16_mask (__mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi) __X,
       (__v8hi) __Y, 5,
       (__mmask8) __M);
}
extern __inline __mmask8
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_cmple_epi16_mask (__mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__mmask8) __builtin_ia32_cmpw128_mask ((__v8hi) __X,
       (__v8hi) __Y, 2,
       (__mmask8) __M);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpneq_epu8_mask (__mmask32 __M, __m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __X,
         (__v32qi) __Y, 4,
         (__mmask32) __M);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmplt_epu8_mask (__mmask32 __M, __m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __X,
         (__v32qi) __Y, 1,
         (__mmask32) __M);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpge_epu8_mask (__mmask32 __M, __m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __X,
         (__v32qi) __Y, 5,
         (__mmask32) __M);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmple_epu8_mask (__mmask32 __M, __m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_ucmpb256_mask ((__v32qi) __X,
         (__v32qi) __Y, 2,
         (__mmask32) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpneq_epu16_mask (__mmask16 __M, __m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __X,
         (__v16hi) __Y, 4,
         (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmplt_epu16_mask (__mmask16 __M, __m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __X,
         (__v16hi) __Y, 1,
         (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpge_epu16_mask (__mmask16 __M, __m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __X,
         (__v16hi) __Y, 5,
         (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmple_epu16_mask (__mmask16 __M, __m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_ucmpw256_mask ((__v16hi) __X,
         (__v16hi) __Y, 2,
         (__mmask16) __M);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpneq_epi8_mask (__mmask32 __M, __m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi) __X,
        (__v32qi) __Y, 4,
        (__mmask32) __M);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmplt_epi8_mask (__mmask32 __M, __m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi) __X,
        (__v32qi) __Y, 1,
        (__mmask32) __M);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpge_epi8_mask (__mmask32 __M, __m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi) __X,
        (__v32qi) __Y, 5,
        (__mmask32) __M);
}
extern __inline __mmask32
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmple_epi8_mask (__mmask32 __M, __m256i __X, __m256i __Y)
{
  return (__mmask32) __builtin_ia32_cmpb256_mask ((__v32qi) __X,
        (__v32qi) __Y, 2,
        (__mmask32) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpneq_epi16_mask (__mmask16 __M, __m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi) __X,
        (__v16hi) __Y, 4,
        (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmplt_epi16_mask (__mmask16 __M, __m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi) __X,
        (__v16hi) __Y, 1,
        (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmpge_epi16_mask (__mmask16 __M, __m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi) __X,
        (__v16hi) __Y, 5,
        (__mmask16) __M);
}
extern __inline __mmask16
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_cmple_epi16_mask (__mmask16 __M, __m256i __X, __m256i __Y)
{
  return (__mmask16) __builtin_ia32_cmpw256_mask ((__v16hi) __X,
        (__v16hi) __Y, 2,
        (__mmask16) __M);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_add_epi16 (__mmask8 __M, __m128i __W)
{
  __W = _mm_maskz_mov_epi16 (__M, __W);
  _MM_REDUCE_OPERATOR_BASIC_EPI16 (+);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_mul_epi16 (__mmask8 __M, __m128i __W)
{
  __W = _mm_mask_mov_epi16 (_mm_avx512_set1_epi16 (1), __M, __W);
  _MM_REDUCE_OPERATOR_BASIC_EPI16 (*);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_and_epi16 (__mmask8 __M, __m128i __W)
{
  __W = _mm_mask_mov_epi16 (_mm_avx512_set1_epi16 (-1), __M, __W);
  _MM_REDUCE_OPERATOR_BASIC_EPI16 (&);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_or_epi16 (__mmask8 __M, __m128i __W)
{
  __W = _mm_maskz_mov_epi16 (__M, __W);
  _MM_REDUCE_OPERATOR_BASIC_EPI16 (|);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_max_epi16 (__mmask16 __M, __m128i __V)
{
  __V = _mm_mask_mov_epi16 (_mm_avx512_set1_epi16 (-32767-1), __M, __V);
  _MM_REDUCE_OPERATOR_MAX_MIN_EP16 (avx512_max_epi16);
}
extern __inline unsigned short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_max_epu16 (__mmask16 __M, __m128i __V)
{
  __V = _mm_maskz_mov_epi16 (__M, __V);
  _MM_REDUCE_OPERATOR_MAX_MIN_EP16 (avx512_max_epu16);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_min_epi16 (__mmask16 __M, __m128i __V)
{
  __V = _mm_mask_mov_epi16 (_mm_avx512_set1_epi16 (32767), __M, __V);
  _MM_REDUCE_OPERATOR_MAX_MIN_EP16 (avx512_min_epi16);
}
extern __inline unsigned short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_min_epu16 (__mmask16 __M, __m128i __V)
{
  __V = _mm_mask_mov_epi16 (_mm_avx512_set1_epi16 (-1), __M, __V);
  _MM_REDUCE_OPERATOR_MAX_MIN_EP16 (avx512_min_epu16);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_add_epi16 (__mmask16 __M, __m256i __W)
{
  __W = _mm256_maskz_mov_epi16 (__M, __W);
  _MM256_AVX512_REDUCE_OPERATOR_BASIC_EPI16 (+);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_mul_epi16 (__mmask16 __M, __m256i __W)
{
  __W = _mm256_mask_mov_epi16 (_mm256_avx512_set1_epi16 (1), __M, __W);
  _MM256_AVX512_REDUCE_OPERATOR_BASIC_EPI16 (*);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_and_epi16 (__mmask16 __M, __m256i __W)
{
  __W = _mm256_mask_mov_epi16 (_mm256_avx512_set1_epi16 (-1), __M, __W);
  _MM256_AVX512_REDUCE_OPERATOR_BASIC_EPI16 (&);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_or_epi16 (__mmask16 __M, __m256i __W)
{
  __W = _mm256_maskz_mov_epi16 (__M, __W);
  _MM256_AVX512_REDUCE_OPERATOR_BASIC_EPI16 (|);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_max_epi16 (__mmask16 __M, __m256i __V)
{
  __V = _mm256_mask_mov_epi16 (_mm256_avx512_set1_epi16 (-32767-1), __M, __V);
  _MM256_AVX512_REDUCE_OPERATOR_MAX_MIN_EP16 (max_epi16);
}
extern __inline unsigned short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_max_epu16 (__mmask16 __M, __m256i __V)
{
  __V = _mm256_maskz_mov_epi16 (__M, __V);
  _MM256_AVX512_REDUCE_OPERATOR_MAX_MIN_EP16 (max_epu16);
}
extern __inline short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_min_epi16 (__mmask16 __M, __m256i __V)
{
  __V = _mm256_mask_mov_epi16 (_mm256_avx512_set1_epi16 (32767), __M, __V);
  _MM256_AVX512_REDUCE_OPERATOR_MAX_MIN_EP16 (min_epi16);
}
extern __inline unsigned short
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_min_epu16 (__mmask16 __M, __m256i __V)
{
  __V = _mm256_mask_mov_epi16 (_mm256_avx512_set1_epi16 (-1), __M, __V);
  _MM256_AVX512_REDUCE_OPERATOR_MAX_MIN_EP16 (min_epu16);
}
extern __inline char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_add_epi8 (__mmask16 __M, __m128i __W)
{
  __W = _mm_maskz_mov_epi8 (__M, __W);
  _MM_REDUCE_OPERATOR_BASIC_EPI8 (+);
}
extern __inline char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_mul_epi8 (__mmask16 __M, __m128i __W)
{
  __W = _mm_mask_mov_epi8 (_mm_avx512_set1_epi8 (1), __M, __W);
  _MM_REDUCE_OPERATOR_BASIC_EPI8 (*);
}
extern __inline char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_and_epi8 (__mmask16 __M, __m128i __W)
{
  __W = _mm_mask_mov_epi8 (_mm_avx512_set1_epi8 (-1), __M, __W);
  _MM_REDUCE_OPERATOR_BASIC_EPI8 (&);
}
extern __inline char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_or_epi8 (__mmask16 __M, __m128i __W)
{
  __W = _mm_maskz_mov_epi8 (__M, __W);
  _MM_REDUCE_OPERATOR_BASIC_EPI8 (|);
}
extern __inline signed char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_max_epi8 (__mmask16 __M, __m128i __V)
{
  __V = _mm_mask_mov_epi8 (_mm_avx512_set1_epi8 (-127-1), __M, __V);
  _MM_REDUCE_OPERATOR_MAX_MIN_EP8 (avx512_max_epi8);
}
extern __inline unsigned char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_max_epu8 (__mmask16 __M, __m128i __V)
{
  __V = _mm_maskz_mov_epi8 (__M, __V);
  _MM_REDUCE_OPERATOR_MAX_MIN_EP8 (avx512_max_epu8);
}
extern __inline signed char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_min_epi8 (__mmask16 __M, __m128i __V)
{
  __V = _mm_mask_mov_epi8 (_mm_avx512_set1_epi8 (127), __M, __V);
  _MM_REDUCE_OPERATOR_MAX_MIN_EP8 (avx512_min_epi8);
}
extern __inline unsigned char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_reduce_min_epu8 (__mmask16 __M, __m128i __V)
{
  __V = _mm_mask_mov_epi8 (_mm_avx512_set1_epi8 (-1), __M, __V);
  _MM_REDUCE_OPERATOR_MAX_MIN_EP8 (avx512_min_epu8);
}
extern __inline char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_add_epi8 (__mmask32 __M, __m256i __W)
{
  __W = _mm256_maskz_mov_epi8 (__M, __W);
  _MM256_AVX512_REDUCE_OPERATOR_BASIC_EPI8 (+);
}
extern __inline char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_mul_epi8 (__mmask32 __M, __m256i __W)
{
  __W = _mm256_mask_mov_epi8 (_mm256_avx512_set1_epi8 (1), __M, __W);
  _MM256_AVX512_REDUCE_OPERATOR_BASIC_EPI8 (*);
}
extern __inline char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_and_epi8 (__mmask32 __M, __m256i __W)
{
  __W = _mm256_mask_mov_epi8 (_mm256_avx512_set1_epi8 (-1), __M, __W);
  _MM256_AVX512_REDUCE_OPERATOR_BASIC_EPI8 (&);
}
extern __inline char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_or_epi8 (__mmask32 __M, __m256i __W)
{
  __W = _mm256_maskz_mov_epi8 (__M, __W);
  _MM256_AVX512_REDUCE_OPERATOR_BASIC_EPI8 (|);
}
extern __inline signed char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_max_epi8 (__mmask32 __M, __m256i __V)
{
  __V = _mm256_mask_mov_epi8 (_mm256_avx512_set1_epi8 (-127-1), __M, __V);
  _MM256_AVX512_REDUCE_OPERATOR_MAX_MIN_EP8 (max_epi8);
}
extern __inline unsigned char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_max_epu8 (__mmask32 __M, __m256i __V)
{
  __V = _mm256_maskz_mov_epi8 (__M, __V);
  _MM256_AVX512_REDUCE_OPERATOR_MAX_MIN_EP8 (max_epu8);
}
extern __inline signed char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_min_epi8 (__mmask32 __M, __m256i __V)
{
  __V = _mm256_mask_mov_epi8 (_mm256_avx512_set1_epi8 (127), __M, __V);
  _MM256_AVX512_REDUCE_OPERATOR_MAX_MIN_EP8 (min_epi8);
}
extern __inline unsigned char
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm256_mask_reduce_min_epu8 (__mmask32 __M, __m256i __V)
{
  __V = _mm256_mask_mov_epi8 (_mm256_avx512_set1_epi8 (-1), __M, __V);
  _MM256_AVX512_REDUCE_OPERATOR_MAX_MIN_EP8 (min_epu8);
}
#ifdef __DISABLE_AVX512VLBW__
#undef __DISABLE_AVX512VLBW__
#pragma GCC pop_options
#endif
#endif
#endif
