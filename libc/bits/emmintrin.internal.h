#ifndef COSMOPOLITAN_LIBC_BITS_EMMINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_EMMINTRIN_H_
#include "libc/bits/progn.internal.h"
#include "libc/bits/xmmintrin.internal.h"
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

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse2 » scalar ops                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_sqrt_sd(M128D_0, M128D_1)                          \
  ({                                                           \
    __m128d M128d2 = __builtin_ia32_sqrtsd((__v2df)(M128D_1)); \
    (__m128d){M128d2[0], (M128D_0)[1]};                        \
  })

#define _mm_add_sd(M128D_0, M128D_1) \
  PROGN((M128D_0)[0] += (M128D_1)[0], (M128D_0))
#define _mm_sub_sd(M128D_0, M128D_1) \
  PROGN((M128D_0)[0] -= (M128D_1)[0], (M128D_0))
#define _mm_mul_sd(M128D_0, M128D_1) \
  PROGN((M128D_0)[0] *= (M128D_1)[0], (M128D_0))
#define _mm_div_sd(M128D_0, M128D_1) \
  PROGN((M128D_0)[0] /= (M128D_1)[0], (M128D_0))

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

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse2 » miscellaneous                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_pause() asm("rep nop")

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_EMMINTRIN_H_ */
