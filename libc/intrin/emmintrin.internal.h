#ifndef COSMOPOLITAN_LIBC_BITS_EMMINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_EMMINTRIN_H_
#include "libc/intrin/xmmintrin.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse2                                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef char __v16qi _Vector_size(16);
typedef unsigned char __v16qu _Vector_size(16);
typedef signed char __v16qs _Vector_size(16);

typedef short __v8hi _Vector_size(16);
typedef unsigned short __v8hu _Vector_size(16);

typedef double __v2df _Vector_size(16);
typedef double __m128d _Vector_size(16) forcealign(16);
typedef double __m128d_u _Vector_size(16) forcealign(1);

typedef long long __v2di _Vector_size(16);
typedef long long __m128i _Vector_size(16) forcealign(16);
typedef long long __m128i_u _Vector_size(16) forcealign(1);
typedef unsigned long long __v2du _Vector_size(16);

struct thatispacked mayalias __usi128ma {
  __m128i_u __v;
};

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse2 » memory ops                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_loadu_si128(M128IP) ((struct __usi128ma *)(M128IP))->__v
#define _mm_storeu_si128(M128IP, M128I) \
  (((struct __usi128ma *)(M128IP))->__v = (M128I))

#define _mm_set_epi8(I8_15, I8_14, I8_13, I8_12, I8_11, I8_10, I8_9, I8_8,  \
                     I8_7, I8_6, I8_5, I8_4, I8_3, I8_2, I8_1, I8_0)        \
  ((__m128i)(__v16qi){I8_0, I8_1, I8_2, I8_3, I8_4, I8_5, I8_6, I8_7, I8_8, \
                      I8_9, I8_10, I8_11, I8_12, I8_13, I8_14, I8_15})
#define _mm_set_epi16(I16_7, I16_6, I16_5, I16_4, I16_3, I16_2, I16_1, I16_0) \
  ((__m128i)(__v8hi){I16_0, I16_1, I16_2, I16_3, I16_4, I16_5, I16_6, I16_7})
#define _mm_set_epi32(I32_3, I32_2, I32_1, I32_0) \
  ((__m128i)(__v4si){I32_0, I32_1, I32_2, I32_3})
#define _mm_set_epi64x(I64_1, I64_0) ((__m128i)(__v2di){I64_0, I64_1})

#define _mm_setr_epi8(I8_15, I8_14, I8_13, I8_12, I8_11, I8_10, I8_9, I8_8, \
                      I8_7, I8_6, I8_5, I8_4, I8_3, I8_2, I8_1, I8_0)       \
  _mm_set_epi8(I8_0, I8_1, I8_2, I8_3, I8_4, I8_5, I8_6, I8_7, I8_8, I8_9,  \
               I8_10, I8_11, I8_12, I8_13, I8_14, I8_15)
#define _mm_setr_epi16(I16_7, I16_6, I16_5, I16_4, I16_3, I16_2, I16_1, I16_0) \
  _mm_set_epi16(I16_0, I16_1, I16_2, I16_3, I16_4, I16_5, I16_6, I16_7)
#define _mm_setr_epi32(I32_3, I32_2, I32_1, I32_0) \
  _mm_set_epi32(I32_0, I32_1, I32_2, I32_3)
#define _mm_setr_epi64x(I64_1, I64_0) _mm_set_epi64x(I64_0, I64_1)

#define _mm_set1_epi8(I8) \
  _mm_set_epi8(I8, I8, I8, I8, I8, I8, I8, I8, I8, I8, I8, I8, I8, I8, I8, I8)
#define _mm_set1_epi16(I16) \
  _mm_set_epi16(I16, I16, I16, I16, I16, I16, I16, I16)
#define _mm_set1_epi32(I32)  _mm_set_epi32(I32, I32, I32, I32)
#define _mm_set1_epi64x(I64) _mm_set_epi64x(I64, I64)

#define _mm_cvtsi128_si32(M128I) ((__v4si)(M128I))[0]
#define _mm_cvtsi32_si128(I32)   ((__m128i)(__v4si){(I32), 0, 0, 0})
#define _mm_setzero_si128()      ((__m128i)(__v2di){0LL, 0LL})
#define _mm_castsi128_ps(M128I)  ((__m128)(M128I))
#define _mm_castps_si128(M128)   ((__m128i)(M128))
#define _mm_load_si128(M128I)    (*(M128I))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse2 » simd ops                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_and_si128(M128I_0, M128I_1) \
  ((__m128i)((__v2du)(M128I_0) & (__v2du)(M128I_1)))
#define _mm_or_si128(M128I_0, M128I_1) \
  ((__m128i)((__v2du)(M128I_0) | (__v2du)(M128I_1)))
#define _mm_xor_si128(M128I_0, M128I_1) \
  ((__m128i)((__v2du)(M128I_0) ^ (__v2du)(M128I_1)))
#define _mm_andnot_si128(M128I_0, M128I_1) \
  ((__m128i)(~(__v2du)(M128I_0) & (__v2du)(M128I_1)))

#define _mm_add_pd(M128D_0, M128D_1) \
  (__m128d)((__v2df)(M128D_0) + (__v2df)(M128D_1))
#define _mm_sub_pd(M128D_0, M128D_1) \
  (__m128d)((__v2df)(M128D_0) - (__v2df)(M128D_1))
#define _mm_mul_pd(M128D_0, M128D_1) \
  (__m128d)((__v2df)(M128D_0) * (__v2df)(M128D_1))
#define _mm_div_pd(M128D_0, M128D_1) \
  (__m128d)((__v2df)(M128D_0) / (__v2df)(M128D_1))
#define _mm_and_pd(M128D_0, M128D_1) \
  (__m128d)((__v2df)(M128D_0) & (__v2df)(M128D_1))
#define _mm_or_pd(M128D_0, M128D_1) \
  (__m128d)((__v2df)(M128D_0) | (__v2df)(M128D_1))
#define _mm_xor_pd(M128D_0, M128D_1) \
  (__m128d)((__v2df)(M128D_0) ^ (__v2df)(M128D_1))
#define _mm_andnot_pd(M128D_0, M128D_1) \
  (__m128d)(~(__v2df)(M128D_0) & (__v2df)(M128D_1))
#define _mm_sqrt_pd(M128D) __builtin_ia32_sqrtpd((__v2df)(M128D))

#define _mm_min_pd(M128D_0, M128D_1) \
  __builtin_ia32_minpd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_max_pd(M128D_0, M128D_1) \
  __builtin_ia32_maxpd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpeq_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmpeqpd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpneq_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmpneqpd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmplt_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmpltpd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpnlt_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmpnltpd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmple_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmplepd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpnle_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmpnlepd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpgt_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmpltpd((__v2df)(M128D_1), (__v2df)(M128D_0))
#define _mm_cmpngt_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmpnltpd((__v2df)(M128D_1), (__v2df)(M128D_0))
#define _mm_cmpge_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmplepd((__v2df)(M128D_1), (__v2df)(M128D_0))
#define _mm_cmpnge_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmpnlepd((__v2df)(M128D_1), (__v2df)(M128D_0))
#define _mm_cmpord_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmpordpd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpunord_pd(M128D_0, M128D_1) \
  __builtin_ia32_cmpunordpd((__v2df)(M128D_0), (__v2df)(M128D_1))

#define _mm_sad_epu8(M128I_0, M128I_1) \
  __builtin_ia32_psadbw128((__v16qi)(M128I_0), (__v16qi)(M128I_1))

#define _mm_subs_epi8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psubsb128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))
#define _mm_subs_epu8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psubusw128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))
#define _mm_subs_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psubsw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))
#define _mm_subs_epu16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psubusw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_add_epi32(M128I_0, M128I_1) \
  ((__m128i)((__v4su)(M128I_0) + (__v4su)(M128I_1)))
#define _mm_sub_epi32(M128I_0, M128I_1) \
  ((__m128i)((__v4su)(M128I_0) - (__v4su)(M128I_1)))
#define _mm_madd_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pmaddwd128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))
#define _mm_shuffle_epi32(V, IMM) \
  ((__m128i)__builtin_ia32_pshufd((__v4si)(__m128i)(V), (int)(IMM)))

#define _mm_slli_epi32(M128I, COUNT) \
  ((__m128i)__builtin_ia32_pslldi128((__v4si)(M128I), (COUNT)))

#define _mm_slli_si128(M128I, IMM) \
  ((__m128i)__builtin_ia32_pslldqi128((__v2di)(__m128i)(M128I), (int)(IMM)*8))
#define _mm_srli_si128(M128I, IMM) \
  ((__m128i)__builtin_ia32_psrldqi128((__v2di)(__m128i)(M128I), (int)(IMM)*8))

#define _mm_cmpeq_epi8(a, b) ((__m128i)((__v16qi)(a) == (__v16qi)(b)))
#define _mm_movemask_epi8(a) __builtin_ia32_pmovmskb128((__v16qi)(a))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse2 » scalar ops                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_sqrt_sd(M128D_0, M128D_1)                          \
  ({                                                           \
    __m128d M128d2 = __builtin_ia32_sqrtsd((__v2df)(M128D_1)); \
    (__m128d){M128d2[0], (M128D_0)[1]};                        \
  })

#define _mm_add_sd(M128D_0, M128D_1) \
  ({                                 \
    (M128D_0)[0] += (M128D_1)[0];    \
    (M128D_0);                       \
  })
#define _mm_sub_sd(M128D_0, M128D_1) \
  ({                                 \
    (M128D_0)[0] -= (M128D_1)[0];    \
    (M128D_0);                       \
  })
#define _mm_mul_sd(M128D_0, M128D_1) \
  ({                                 \
    (M128D_0)[0] *= (M128D_1)[0];    \
    (M128D_0);                       \
  })
#define _mm_div_sd(M128D_0, M128D_1) \
  ({                                 \
    (M128D_0)[0] /= (M128D_1)[0];    \
    (M128D_0);                       \
  })

#define _mm_min_sd(M128D_0, M128D_1) \
  __builtin_ia32_minsd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_max_sd(M128D_0, M128D_1) \
  __builtin_ia32_maxsd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpeq_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmpeqsd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpneq_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmpneqsd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmplt_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmpltsd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpnlt_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmpnltsd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmple_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmplesd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpnle_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmpnlesd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpgt_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmpltsd((__v2df)(M128D_1), (__v2df)(M128D_0))
#define _mm_cmpngt_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmpnltsd((__v2df)(M128D_1), (__v2df)(M128D_0))
#define _mm_cmpge_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmplesd((__v2df)(M128D_1), (__v2df)(M128D_0))
#define _mm_cmpnge_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmpnlesd((__v2df)(M128D_1), (__v2df)(M128D_0))
#define _mm_cmpord_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmpordsd((__v2df)(M128D_0), (__v2df)(M128D_1))
#define _mm_cmpunord_sd(M128D_0, M128D_1) \
  __builtin_ia32_cmpunordsd((__v2df)(M128D_0), (__v2df)(M128D_1))

#define _mm_SSE2(op, A, B)                  \
  ({                                        \
    __m128i R = A;                          \
    asm(#op " %1, %0" : "+x"(R) : "xm"(B)); \
    R;                                      \
  })
#define _mm_mul_epu32(A, B)      _mm_SSE2(pmuludq, A, B)
#define _mm_add_epi64(A, B)      _mm_SSE2(paddq, A, B)
#define _mm_srli_epi64(A, B)     _mm_SSE2(psrlq, A, B)
#define _mm_slli_epi64(A, B)     _mm_SSE2(psllq, A, B)
#define _mm_unpacklo_epi64(A, B) _mm_SSE2(punpcklqdq, A, B)
#define _mm_unpackhi_epi64(A, B) _mm_SSE2(punpckhqdq, A, B)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse2 » miscellaneous                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_pause() asm("rep nop")

#define _mm_set_sd(DBL_0) ((__m128d){(double)(DBL_0), 0.0})

#define _mm_set1_pd(DBL_0) ((__m128d){(double)(DBL_0), (double)(DBL_0)})

#define _mm_set_pd1(DBL_0) (_mm_set1_pd((double)(DBL_0)))

#define _mm_set_pd(DBL_0, DBL_1) ((__m128d){(double)(DBL_1), (double)(DBL_0)})

#define _mm_setr_pd(DBL_0, DBL_1) ((__m128d){(double)(DBL_0), (double)(DBL_1)})

#define _mm_undefined_pd() \
  ({                       \
    __m128d __Y = __Y;     \
    return __Y;            \
  })

#define _mm_setzero_pd() ((__m128d){0.0, 0.0})

#define _mm_move_sd(M128D_0, M128D_1) \
  ((__m128d)__builtin_shuffle((__v2df)__A, (__v2df)__B, (__v2di){2, 1}))

#define _mm_load_pd(DBL_CONSTPTR_0) \
  (*(__m128d *)(double const *)(DBL_CONSTPTR_0))

#define _mm_loadu_pd(DBL_CONSTPTR_0) \
  (*(__m128d_u *)(double const *)(DBL_CONSTPTR_0))

#define _mm_load1_pd(DBL_CONSTPTR_0) \
  (_mm_set1_pd(*(double const *)(DBL_CONSTPTR_0)))

#define _mm_load_sd(DBL_CONSTPTR_0) \
  (_mm_set_sd(*(double const *)(DBL_CONSTPTR_0)))

#define _mm_load_pd1(DBL_CONSTPTR_0) \
  (_mm_load1_pd((double const *)(DBL_CONSTPTR_0)))

#define _mm_loadr_pd(DBL_CONSTPTR_0)                               \
  ({                                                               \
    __m128d __tmp = _mm_load_pd((double const *)(DBL_CONSTPTR_0)); \
    __builtin_ia32_shufpd(__tmp, __tmp, 1);                        \
  })

#define _mm_store_pd(DBLPTR_0, M128D_0) \
  (*(__m128d *)(double *)(DBLPTR_0) = (M128D_0))

#define _mm_storeu_pd(DBLPTR_0, M128D_0) \
  (*(__m128d_u *)(double *)(DBLPTR_0) = (M128D_0))

#define _mm_store_sd(DBLPTR_0, M128D_0) \
  (*(double *)(DBLPTR_0) = ((__v2df)(M128D_0))[0])

#define _mm_cvtsd_f64(M128D_0) (((__v2df)(M128D_0))[0])

#define _mm_storel_pd(DBLPTR_0, M128D_0) \
  (_mm_store_sd((double *)(DBLPTR_0), (M128D_0)))

#define _mm_storeh_pd(DBLPTR_0, M128D_0) \
  (*(double *)(DBLPTR_0) = ((__v2df)(M128D_0))[1])

#define _mm_store1_pd(DBLPTR_0, M128D_0) \
  (_mm_store_pd((double *)(DBLPTR_0),    \
                __builtin_ia32_shufpd((M128D_0), (M128D_0), 0)))

#define _mm_store_pd1(DBLPTR_0, M128D_0) \
  (_mm_store1_pd((double *)(DBLPTR_0), (M128D_0)))

#define _mm_storer_pd(DBLPTR_0, M128D_0) \
  (_mm_store_pd((double *)(DBLPTR_0),    \
                __builtin_ia32_shufpd((M128D_0), (M128D_0), 1)))

#define _mm_comieq_sd(M128D_0, M128D_1) \
  (__builtin_ia32_comisdeq((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_comilt_sd(M128D_0, M128D_1) \
  (__builtin_ia32_comisdlt((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_comile_sd(M128D_0, M128D_1) \
  (__builtin_ia32_comisdle((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_comigt_sd(M128D_0, M128D_1) \
  (__builtin_ia32_comisdgt((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_comige_sd(M128D_0, M128D_1) \
  (__builtin_ia32_comisdge((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_comineq_sd(M128D_0, M128D_1) \
  (__builtin_ia32_comisdneq((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_ucomieq_sd(M128D_0, M128D_1) \
  (__builtin_ia32_ucomisdeq((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_ucomilt_sd(M128D_0, M128D_1) \
  (__builtin_ia32_ucomisdlt((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_ucomile_sd(M128D_0, M128D_1) \
  (__builtin_ia32_ucomisdle((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_ucomigt_sd(M128D_0, M128D_1) \
  (__builtin_ia32_ucomisdgt((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_ucomige_sd(M128D_0, M128D_1) \
  (__builtin_ia32_ucomisdge((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_ucomineq_sd(M128D_0, M128D_1) \
  (__builtin_ia32_ucomisdneq((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_cvtepi32_pd(M128I_0) ((__m128d)__builtin_ia32_cvtdq2pd((__v4si)__A))

#define _mm_cvtpd_epi32(M128D_0) \
  ((__m128i)__builtin_ia32_cvtpd2dq((__v2df)(M128D_0)))

#define _mm_cvtpd_pi32(M128D_0) \
  ((__m64)__builtin_ia32_cvtpd2pi((__v2df)(M128D_0)))

#define _mm_cvtpd_ps(M128D_0) \
  ((__m128)__builtin_ia32_cvtpd2ps((__v2df)(M128D_0)))

#define _mm_cvttpd_epi32(M128D_0) \
  ((__m128i)__builtin_ia32_cvttpd2dq((__v2df)(M128D_0)))

#define _mm_cvttpd_pi32(M128D_0) \
  ((__m64)__builtin_ia32_cvttpd2pi((__v2df)(M128D_0)))

#define _mm_cvtpi32_pd(M64_0) \
  ((__m128d)__builtin_ia32_cvtpi2pd((__v2si)(M64_0)))

#define _mm_cvtps_pd(M128_0) \
  ((__m128d)__builtin_ia32_cvtps2pd((__v4sf)(M128_0)))

#define _mm_cvtsd_si32(M128D_0) (__builtin_ia32_cvtsd2si((__v2df)(M128D_0)))

#define _mm_cvtsd_si64(M128D_0) (__builtin_ia32_cvtsd2si64((__v2df)(M128D_0)))

#define _mm_cvtsd_si64x(M128D_0) (__builtin_ia32_cvtsd2si64((__v2df)(M128D_0)))

#define _mm_cvttsd_si32(M128D_0) (__builtin_ia32_cvttsd2si((__v2df)(M128D_0)))

#define _mm_cvttsd_si64(M128D_0) (__builtin_ia32_cvttsd2si64((__v2df)(M128D_0)))

#define _mm_cvttsd_si64x(M128D_0) \
  (__builtin_ia32_cvttsd2si64((__v2df)(M128D_0)))

#define _mm_cvtsd_ss(M128_0, M128D_1) \
  ((__m128)__builtin_ia32_cvtsd2ss((__v4sf)(M128_0), (__v2df)(M128D_1)))

#define _mm_cvtsi32_sd(M128D_0, INT_1) \
  ((__m128d)__builtin_ia32_cvtsi2sd((__v2df)(M128D_0), (INT_1)))

#define _mm_cvtsi64_sd(M128D_0, LL_1) \
  ((__m128d)__builtin_ia32_cvtsi642sd((__v2df)(M128D_0), (LL_1)))

#define _mm_cvtsi64x_sd(M128D_0, LL_1) \
  ((__m128d)__builtin_ia32_cvtsi642sd((__v2df)(M128D_0), (LL_1)))

#define _mm_cvtss_sd(M128D_0, M128_1) \
  ((__m128d)__builtin_ia32_cvtss2sd((__v2df)(M128D_0), (__v4sf)(M128_1)))

#define _mm_shuffle_pd(M128D_0, M128D_1, MSK) \
  ((__m128d)__builtin_ia32_shufpd((__v2df)(M128D_0), (__v2df)(M128D_1), (MSK)))

#define _mm_unpackhi_pd(M128D_0, M128D_1) \
  ((__m128d)__builtin_ia32_unpckhpd((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_unpacklo_pd(M128D_0, M128D_1) \
  ((__m128d)__builtin_ia32_unpcklpd((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_loadh_pd(M128D_0, DBL_CONSTPTR_1)         \
  ((__m128d)__builtin_ia32_loadhpd((__v2df)(M128D_0), \
                                   (double const *)(DBL_CONSTPTR_1)))

#define _mm_loadl_pd(M128D_0, DBL_CONSTPTR_1)         \
  ((__m128d)__builtin_ia32_loadlpd((__v2df)(M128D_0), \
                                   (double const *)(DBL_CONSTPTR_1)))

#define _mm_movemask_pd(M128D_0) (__builtin_ia32_movmskpd((__v2df)(M128D_0)))

#define _mm_stream_pd(DBLPTR_0, M128D_1) \
  (__builtin_ia32_movntpd((double *)(DBLPTR_0), (__v2df)(M128D_1)))

#define _mm_castpd_ps(M128D_0) ((__m128)(M128D_0))

#define _mm_castpd_si128(M128D_0) ((__m128i)(M128D_0))

#define _mm_castps_pd(M128_0) ((__m128d)(M128_0))

#define _mm_cvtsi128_si64(M128I_0) (((__v2di)(M128I_0))[0])

#define _mm_cvtsi128_si64x(M128I_0) (((__v2di)(M128I_0))[0])

#define _mm_set_epi64(LL_1, LL_0) (_mm_set_epi64x((LL_1), (LL_0)))

#define _mm_set1_epi64(M64_0) (_mm_set_epi64((M64_0), (M64_0)))

#define _mm_setr_epi64(M64_0, M64_1) (_mm_set_epi64((M64_1), (M64_0)))

#define _mm_loadl_epi64(PTR) \
  (_mm_set_epi64((__m64)0LL, *(__m64_u *)(__m128i_u const *)(PTR)))

#define _mm_loadu_si64(PTR) (_mm_loadl_epi64((__m128i_u *)(PTR)))

#define _mm_store_si128(M128I_PTR_0, M128I_1) (*(M128I_PTR_0) = (M128I_1))

#define _mm_storel_epi64(PTR, M128I_1) \
  (*(__m64_u *)(__m128i_u *)(PTR) = (__m64)((__v2di)(M128_I))[0])

#define _mm_storeu_si64(PTR, M128I_1) \
  (_mm_storel_epi64((__m128i_u *)(PTR), M128I_1))

#define _mm_movepi64_pi64(M128I_1) ((__m64)((__v2di)(M128I_1))[0])

#define _mm_movpi64_epi64(M64_0) (_mm_set_epi64((__m64)0LL, (M64_0)))

#define _mm_move_epi64(M128I_0) \
  ((__m128i)__builtin_ia32_movq128((__v2di)(M128I_0)))

#define _mm_undefined_si128() ({ __m128i __Y = __Y; })

#define _mm_cvtepi32_ps(M128I_0) \
  ((__m128)__builtin_ia32_cvtdq2ps((__v4si)(M128I_0)))

#define _mm_cvtps_epi32(M128_0) \
  ((__m128i)__builtin_ia32_cvtps2dq((__v4sf)(M128_0)))

#define _mm_cvttps_epi32(M128_0) \
  ((__m128i)__builtin_ia32_cvttps2dq((__v4sf)(M128_0)))

#define _mm_packs_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_packsswb128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_packs_epi32(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_packssdw128((__v4si)(M128I_0), (__v4si)(M128I_1)))

#define _mm_packus_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_packuswb128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_unpackhi_epi8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_punpckhbw128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_unpackhi_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_punpckhwd128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_unpackhi_epi32(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_punpckhdq128((__v4si)(M128I_0), (__v4si)(M128I_1)))

#define _mm_unpacklo_epi8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_punpcklbw128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_unpacklo_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_punpcklwd128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_unpacklo_epi32(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_punpckldq128((__v4si)(M128I_0), (__v4si)(M128I_1)))

#define _mm_add_epi8(M128I_0, M128I_1) \
  ((__m128i)((__v16qu)(M128I_0) + (__v16qu)(M128I_1)))

#define _mm_add_epi16(M128I_0, M128I_1) \
  ((__m128i)((__v8hu)(M128I_0) + (__v8hu)(M128I_1)))

#define _mm_adds_epi8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_paddsb128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_adds_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_paddsw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_adds_epu8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_paddusb128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_adds_epu16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_paddusw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_sub_epi8(M128I_0, M128I_1) \
  ((__m128i)((__v16qu)(M128I_0) - (__v16qu)(M128I_1)))

#define _mm_sub_epi16(M128I_0, M128I_1) \
  ((__m128i)((__v8hu)(M128I_0) - (__v8hu)(M128I_1)))

#define _mm_sub_epi64(M128I_0, M128I_1) \
  ((__m128i)((__v2du)(M128I_0) - (__v2du)(M128I_1)))

#define _mm_mulhi_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pmulhw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_mullo_epi16(M128I_0, M128I_1) \
  ((__m128i)((__v8hu)(M128I_0) * (__v8hu)(M128I_1)))

#define _mm_mul_su32(M64_0, M64_1) \
  ((__m64)__builtin_ia32_pmuludq((__v2si)(M64_0), (__v2si)(M64_1)))

#define _mm_slli_epi16(M128I_0, INT_1) \
  ((__m128i)__builtin_ia32_psllwi128((__v8hi)(M128I_0), (INT_1)))

#define _mm_srai_epi16(M128I_0, INT_1) \
  ((__m128i)__builtin_ia32_psrawi128((__v8hi)(M128I_0), (INT_1)))

#define _mm_srai_epi32(M128I_0, INT_1) \
  ((__m128i)__builtin_ia32_psradi128((__v4si)(M128I_0), (INT_1)))

#define _mm_bsrli_si128(M128I_0, N) \
  ((__m128i)__builtin_ia32_psrldqi128((M128I_0), (N)*8))

#define _mm_bslli_si128(M128I_0, N) \
  ((__m128i)__builtin_ia32_pslldqi128((M128I_0), (N)*8))

#define _mm_srli_epi16(M128I_0, INT_1) \
  ((__m128i)__builtin_ia32_psrlwi128((__v8hi)(M128I_0), (INT_1)))

#define _mm_srli_epi32(M128I_0, INT_1) \
  ((__m128i)__builtin_ia32_psrldi128((__v4si)(M128I_0), (INT_1)))

#define _mm_sll_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psllw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_sll_epi32(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pslld128((__v4si)(M128I_0), (__v4si)(M128I_1)))

#define _mm_sll_epi64(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psllq128((__v2di)(M128I_0), (__v2di)(M128I_1)))

#define _mm_sra_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psraw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_sra_epi32(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psrad128((__v4si)(M128I_0), (__v4si)(M128I_1)))

#define _mm_srl_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psrlw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_srl_epi32(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psrld128((__v4si)(M128I_0), (__v4si)(M128I_1)))

#define _mm_srl_epi64(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psrlq128((__v2di)(M128I_0), (__v2di)(M128I_1)))

#define _mm_cmpeq_epi16(M128I_0, M128I_1) \
  ((__m128i)((__v8hi)(M128I_0) == (__v8hi)(M128I_1)))

#define _mm_cmpeq_epi32(M128I_0, M128I_1) \
  ((__m128i)((__v4si)(M128I_0) == (__v4si)(M128I_1)))

#define _mm_cmplt_epi8(M128I_0, M128I_1) \
  ((__m128i)((__v16qs)(M128I_0) < (__v16qs)(M128I_1)))

#define _mm_cmplt_epi16(M128I_0, M128I_1) \
  ((__m128i)((__v8hi)(M128I_0) < (__v8hi)(M128I_1)))

#define _mm_cmplt_epi32(M128I_0, M128I_1) \
  ((__m128i)((__v4si)(M128I_0) < (__v4si)(M128I_1)))

#define _mm_cmpgt_epi8(M128I_0, M128I_1) \
  ((__m128i)((__v16qs)(M128I_0) > (__v16qs)(M128I_1)))

#define _mm_cmpgt_epi16(M128I_0, M128I_1) \
  ((__m128i)((__v8hi)(M128I_0) > (__v8hi)(M128I_1)))

#define _mm_cmpgt_epi32(M128I_0, M128I_1) \
  ((__m128i)((__v4si)(M128I_0) > (__v4si)(M128I_1)))

#define _mm_extract_epi16(M128I_CONST_0, INT_1)                         \
  ((unsigned short)__builtin_ia32_vec_ext_v8hi((__v8hi)(M128I_CONST_0), \
                                               (INT_1)))

#define _mm_insert_epi16(M128I_CONST_0, INT_1, INT_2) \
  ((__m128i)__builtin_ia32_vec_set_v8hi((__v8hi)(M128I_CONST_0), INT_1, INT_2))

#define _mm_max_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pmaxsw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_max_epu8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pmaxub128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_min_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pminsw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_min_epu8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pminub128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_mulhi_epu16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pmulhuw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_shufflehi_epi16(M128I_0, MSK) \
  ((__m128i)__builtin_ia32_pshufhw((__v8hi)(M128I_0), (MSK)))

#define _mm_shufflelo_epi16(M128I_0, MSK) \
  ((__m128i)__builtin_ia32_pshuflw((__v8hi)(M128I_0), (MSK)))

#define _mm_maskmoveu_si128(M128I_0, M128I_1, CHAR_PTR_2) \
  (__builtin_ia32_maskmovdqu((__v16qi)(M128I_0), (__v16qi)(M128I_1), (char*)(CHAR_PTR_2))

#define _mm_avg_epu8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pavgb128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_avg_epu16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pavgw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_stream_si32(INT_PTR_0, INT_1) \
  (__builtin_ia32_movnti((int *)(INT_PTR_0), (INT_1)))

#define _mm_stream_si64(LL_PTR_0, LL_1) \
  (__builtin_ia32_movnti64((LL_PTR_0), (LL_1)))

#define _mm_stream_si128(M128I_PTR_0, M128I_1) \
  (__builtin_ia32_movntdq((__v2di *)(M128I_PTR_0), (__v2di)(M128I_1)))

#define _mm_clflush(PTR) (__builtin_ia32_clflush((void const *)(PTR)))

#define _mm_lfence() (__builtin_ia32_lfence())

#define _mm_mfence() (__builtin_ia32_mfence())

#define _mm_cvtsi64_si128(LL_0) (_mm_set_epi64x(0, (LL_0)))

#define _mm_cvtsi64x_si128(LL_0) (_mm_set_epi64x(0, (LL_0)))

#define _mm_castsi128_pd(M128I_0) ((__m128d)(M128I_0))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_EMMINTRIN_H_ */
