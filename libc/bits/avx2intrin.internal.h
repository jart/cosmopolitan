#ifndef COSMOPOLITAN_LIBC_BITS_AVX2INTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_AVX2INTRIN_H_
#include "libc/bits/avxintrin.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define _mm256_min_epi16(M256_0, M256_1) \
  ((__m256i)__builtin_ia32_minps((__v16hi)(M256_0), (__v16hi)(M256_1)))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » avx2 » simd ops                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm256_add_ps(M256_0, M256_1) \
  ((__m256)((__v8sf)(M256_0) + (__v8sf)(M256_1)))
#define _mm256_sub_ps(M256_0, M256_1) \
  ((__m256)((__v8sf)(M256_0) - (__v8sf)(M256_1)))
#define _mm256_mul_ps(M256_0, M256_1) \
  ((__m256)((__v8sf)(M256_0) * (__v8sf)(M256_1)))
#define _mm256_div_ps(M256_0, M256_1) \
  ((__m256)((__v8sf)(M256_0) / (__v8sf)(M256_1)))
#define _mm256_and_ps(M256_0, M256_1) \
  ((__m256)((__v8su)(M256_0) & (__v8su)(M256_1)))
#define _mm256_or_ps(M256_0, M256_1) \
  ((__m256)((__v8su)(M256_0) | (__v8su)(M256_1)))
#define _mm256_xor_ps(M256_0, M256_1) /* XORPD [u32 simd xor] */ \
  ((__m256)((__v8su)(M256_0) ^ (__v8su)(M256_1)))
#define _mm256_andnot_ps(M256_0, M256_1) /* ANDNPS [u32 simd nand] */ \
  ((__m256)(~(__v8su)(M256_0) & (__v8su)(M256_1)))
#define _mm256_rcp_ps(M256)   __builtin_ia32_rcpps256((__v8sf)(M256))
#define _mm256_sqrt_ps(M256)  __builtin_ia32_sqrtps256((__v8sf)(M256))
#define _mm256_rsqrt_ps(M256) __builtin_ia32_rsqrtps256((__v8sf)(M256))
#define _mm256_round_ps(M256, IMM) \
  ((__m256)__builtin_ia32_roundps256((__v8sf)(__m256)(M256), IMM))

#define _mm256_add_epi32(M256I_0, M256I_1) \
  ((__m256i)((__v8su)(M256I_0) + (__v8su)(M256I_1)))
#define _mm256_cmpgt_epi32(M256I_0, M256I_1) \
  ((__m256i)((__v8si)(M256I_0) > (__v8si)(M256I_1)))
#define _mm256_min_epi32(M256I_0, M256I_1) \
  ((__m256i)__builtin_ia32_pminsd256((__v8si)(M256I_0), (__v8si)(M256I_1)))
#define _mm256_min_epu32(M256I_0, M256I_1) \
  ((__m256i)__builtin_ia32_pminud256((__v8si)(M256I_0), (__v8si)(M256I_1)))
#define _mm256_max_epi32(M256I_0, M256I_1) \
  ((__m256i)__builtin_ia32_pmaxsd256((__v8si)(M256I_0), (__v8si)(M256I_1)))
#define _mm256_max_epu32(M256I_0, M256I_1) \
  ((__m256i)__builtin_ia32_pmaxud256((__v8si)(M256I_0), (__v8si)(M256I_1)))
#define _mm256_blendv_epi8(M256I_0, M256I_1, M256I_2)                          \
  ((__m256i)__builtin_ia32_pblendvb256((__v32qi)(M256I_0), (__v32qi)(M256I_1), \
                                       (__v32qi)(M256I_2)))

#define _mm256_min_ps(M256_0, M256_1)                        \
  ((__m256)__builtin_ia32_minps256((__v8sf)(__m256)(M256_0), \
                                   (__v8sf)(__m256)(M256_1)))
#define _mm256_max_ps(M256_0, M256_1)                        \
  ((__m256)__builtin_ia32_maxps256((__v8sf)(__m256)(M256_0), \
                                   (__v8sf)(__m256)(M256_1)))
#define _mm256_cmpneq_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpneqps((__v8sf)(__m256)(M256_0), \
                                   (__v8sf)(__m256)(M256_1)))
#define _mm256_cmplt_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpltps((__v8sf)(__m256)(M256_0), \
                                  (__v8sf)(__m256)(M256_1)))
#define _mm256_cmpnlt_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpnltps((__v8sf)(__m256)(M256_0), \
                                   (__v8sf)(__m256)(M256_1)))
#define _mm256_cmple_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpleps((__v8sf)(__m256)(M256_0), \
                                  (__v8sf)(__m256)(M256_1)))
#define _mm256_cmpnle_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpnleps((__v8sf)(__m256)(M256_0), \
                                   (__v8sf)(__m256)(M256_1)))
#define _mm256_cmpgt_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpltps((__v8sf)(__m256)(M256_1), \
                                  (__v8sf)(__m256)(M256_0)))
#define _mm256_cmpngt_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpnltps((__v8sf)(__m256)(M256_1), \
                                   (__v8sf)(__m256)(M256_0)))
#define _mm256_cmpge_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpleps((__v8sf)(__m256)(M256_1), \
                                  (__v8sf)(__m256)(M256_0)))
#define _mm256_cmpnge_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpnleps((__v8sf)(__m256)(M256_1), \
                                   (__v8sf)(__m256)(M256_0)))
#define _mm256_cmpord_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpordps((__v8sf)(__m256)(M256_0), \
                                   (__v8sf)(__m256)(M256_1)))
#define _mm256_cmpunord_ps(M256_0, M256_1)                     \
  ((__m256)__builtin_ia32_cmpunordps((__v8sf)(__m256)(M256_0), \
                                     (__v8sf)(__m256)(M256_1)))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § avx2 » memory ops                                         ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

struct thatispacked PackedMayaliasIntyYmm {
  __m256i Ymm;
} mayalias;

#define _mm256_set_ps(FLT_0, FLT_1, FLT_2, FLT_3, FLT_4, FLT_5, FLT_6, FLT_7) \
  ((__m256)(__v8sf){(float)(FLT_0), (float)(FLT_1), (float)(FLT_2),           \
                    (float)(FLT_3), (float)(FLT_4), (float)(FLT_5),           \
                    (float)(FLT_6), (float)(FLT_7)})
#define _mm256_set1_ps(FLT_0) \
  _mm256_set_ps(FLT_0, FLT_0, FLT_0, FLT_0, FLT_0, FLT_0, FLT_0, FLT_0)
#define _mm256_setr_ps(FLT_0, FLT_1, FLT_2, FLT_3, FLT_4, FLT_5, FLT_6, FLT_7) \
  _mm256_set_ps(FLT_7, FLT_6, FLT_5, FLT_4, FLT_3, FLT_2, FLT_1, FLT_0)

#define _mm256_set_epi32(INT_0, INT_1, INT_2, INT_3, INT_4, INT_5, INT_6,    \
                         INT_7)                                              \
  ((__m256i)(__v8si){(int)(INT_0), (int)(INT_1), (int)(INT_2), (int)(INT_3), \
                     (int)(INT_4), (int)(INT_5), (int)(INT_6), (int)(INT_7)})
#define _mm256_set1_epi32(INT_0) \
  _mm256_set_epi32(INT_0, INT_0, INT_0, INT_0, INT_0, INT_0, INT_0, INT_0)
#define _mm256_setr_epi32(INT_0, INT_1, INT_2, INT_3, INT_4, INT_5, INT_6, \
                          INT_7)                                           \
  _mm256_set_epi32(INT_7, INT_6, INT_5, INT_4, INT_3, INT_2, INT_1, INT_0)

#define _mm256_loadu_si256(M256IP_0)            \
  ({                                            \
    const __m256i *Ymm = (M256IP_0);            \
    ((struct PackedMayaliasIntyYmm *)Ymm)->Ymm; \
  })

#define _mm256_storeu_si256(M256IP_0, M256I_1)            \
  ({                                                      \
    __m256i *Ymm = (M256IP_0);                            \
    ((struct PackedMayaliasIntyYmm *)Ymm)->Ymm = M256I_1; \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_AVX2INTRIN_H_ */
