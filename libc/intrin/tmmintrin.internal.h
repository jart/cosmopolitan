#ifndef COSMOPOLITAN_LIBC_BITS_TMMINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_TMMINTRIN_H_
#include "libc/intrin/emmintrin.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » ssse3                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_hadd_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_phaddw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_hadd_epi32(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_phaddd128((__v4si)(M128I_0), (__v4si)(M128I_1)))

#define _mm_hadds_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_phaddsw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_hsub_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_phsubw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_hsub_epi32(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_phsubd128((__v4si)(M128I_0), (__v4si)(M128I_1)))

#define _mm_hsubs_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_phsubsw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_maddubs_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pmaddubsw128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_mulhrs_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pmulhrsw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_shuffle_epi8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pshufb128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_sign_epi8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psignb128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_sign_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psignw128((__v8hi)(M128I_0), (__v8hi)(M128I_1)))

#define _mm_sign_epi32(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_psignd128((__v4si)(M128I_0), (__v4si)(M128I_1)))

#define _mm_abs_epi8(M128I_0) \
  ((__m128i)__builtin_ia32_pabsb128((__v16qi)(M128I_0)))

#define _mm_abs_epi16(M128I_0) \
  ((__m128i)__builtin_ia32_pabsw128((__v8hi)(M128I_0)))

#define _mm_abs_epi32(M128I_0) \
  ((__m128i)__builtin_ia32_pabsd128((__v4si)(M128I_0)))

#define _mm_alignr_epi8(M128I_0, M128I_1, N)                      \
  ((__m128i)__builtin_ia32_palignr128((__v2di)(__m128i)(M128I_0), \
                                      (__v2di)(__m128i)(M128I_1), (int)(N)*8))

#define _mm_abs_pi8(M64_0) ((__m64)__builtin_ia32_pabsb((__v8qi)(M64_0)))

#define _mm_abs_pi16(M64_0) ((__m64)__builtin_ia32_pabsw((__v4hi)(M64_0)))

#define _mm_abs_pi32(M64_0) ((__m64)__builtin_ia32_pabsd((__v2si)(M64_0)))

#define _mm_hadd_pi16(M64_0, M64_1) \
  ((__m64)__builtin_ia32_phaddw((__v4hi)(M64_0), (__v4hi)(M64_1)))

#define _mm_hadd_pi32(M64_0, M64_1) \
  ((__m64)__builtin_ia32_phaddd((__v2si)(M64_0), (__v2si)(M64_1)))

#define _mm_hadds_pi16(M64_0, M64_1) \
  ((__m64)__builtin_ia32_phaddsw((__v4hi)(M64_0), (__v4hi)(M64_1)))

#define _mm_hsub_pi16(M64_0, M64_1) \
  ((__m64)__builtin_ia32_phsubw((__v4hi)(M64_0), (__v4hi)(M64_1)))

#define _mm_hsub_pi32(M64_0, M64_1) \
  ((__m64)__builtin_ia32_phsubd((__v2si)(M64_0), (__v2si)(M64_1)))

#define _mm_hsubs_pi16(M64_0, M64_1) \
  ((__m64)__builtin_ia32_phsubsw((__v4hi)(M64_0), (__v4hi)(M64_1)))

#define _mm_sign_pi8(M64_0, M64_1) \
  ((__m64)__builtin_ia32_psignb((__v8qi)(M64_0), (__v8qi)(M64_1)))

#define _mm_sign_pi16(M64_0, M64_1) \
  ((__m64)__builtin_ia32_psignw((__v4hi)(M64_0), (__v4hi)(M64_1)))

#define _mm_sign_pi32(M64_0, M64_1) \
  ((__m64)__builtin_ia32_psignd((__v2si)(M64_0), (__v2si)(M64_1)))

#define _mm_shuffle_pi8(M64_0, M64_1) \
  ((__m64)__builtin_ia32_pshufb((__v8qi)(M64_0), (__v8qi)(M64_1)))

#define _mm_mulhrs_pi16(M64_0, M64_1) \
  ((__m64)__builtin_ia32_pmulhrsw((__v4hi)(M64_0), (__v4hi)(M64_1)))

#define _mm_maddubs_pi16(M64_0, M64_1) \
  ((__m64)__builtin_ia32_pmaddubsw((__v8qi)(M64_0), (__v8qi)(M64_1)))

#define _mm_alignr_pi8(M64_0, M64_1, N)                  \
  ((__m64)__builtin_ia32_palignr((__v1di)(__m64)(M64_0), \
                                 (__v1di)(__m64)(M64_1), (int)(N)*8))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_TMMINTRIN_H_ */
