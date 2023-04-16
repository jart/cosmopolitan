#ifndef COSMOPOLITAN_LIBC_BITS_PMMINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_PMMINTRIN_H_
#include "libc/intrin/emmintrin.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse3                                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_addsub_ps(M128_0, M128_1) \
  ((__m128)__builtin_ia32_addsubps((__v4sf)(M128_0), (__v4sf)(M128_1)))

#define _mm_hadd_ps(M128_0, M128_1) \
  ((__m128)__builtin_ia32_haddps((__v4sf)(M128_0), (__v4sf)(M128_1)))

#define _mm_hsub_ps(M128_0, M128_1) \
  ((__m128)__builtin_ia32_hsubps((__v4sf)(M128_0), (__v4sf)(M128_1)))

#define _mm_movehdup_ps(M128_0) \
  ((__m128)__builtin_ia32_movshdup((__v4sf)(M128_0)))

#define _mm_moveldup_ps(M128_0) \
  ((__m128)__builtin_ia32_movsldup((__v4sf)(M128_0)))

#define _mm_addsub_pd(M128D_0, M128D_1) \
  ((__m128d)__builtin_ia32_addsubpd((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_hadd_pd(M128D_0, M128D_1) \
  ((__m128d)__builtin_ia32_haddpd((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_hsub_pd(M128D_0, M128D_1) \
  ((__m128d)__builtin_ia32_hsubpd((__v2df)(M128D_0), (__v2df)(M128D_1)))

#define _mm_movedup_pd(M128D_0) (_mm_shuffle_pd((M128D_0), (M128D_0), 0))

#define _mm_loaddup_pd(CONSTDBL_PTR) (_mm_load1_pd((CONSTDBL_PTR)))

#define _mm_lddqu_si128(M128I_PTR0) \
  ((__m128i)__builtin_ia32_lddqu((char const *)(M128I_PTR0)))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_PMMINTRIN_H_ */
