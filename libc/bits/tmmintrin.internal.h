#ifndef COSMOPOLITAN_LIBC_BITS_TMMINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_TMMINTRIN_H_
#include "libc/bits/emmintrin.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » ssse3                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_maddubs_epi16(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pmaddubsw128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#define _mm_shuffle_epi8(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_pshufb128((__v16qi)(M128I_0), (__v16qi)(M128I_1)))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_TMMINTRIN_H_ */
