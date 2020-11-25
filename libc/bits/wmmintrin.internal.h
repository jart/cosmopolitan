#ifndef COSMOPOLITAN_LIBC_BITS_WMMINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_WMMINTRIN_H_
#include "libc/bits/emmintrin.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define _mm_clmulepi64_si128(X, Y, IMM)                       \
  ((__m128i)__builtin_ia32_pclmulqdq128((__v2di)(__m128i)(X), \
                                        (__v2di)(__m128i)(Y), (char)(IMM)))

#define _mm_aesenc_si128(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_aesenc128((__v2di)(M128I_0), (__v2di)(M128I_1)))
#define _mm_aesenclast_si128(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_aesenclast128((__v2di)(M128I_0), (__v2di)(M128I_1)))

#define _mm_aesdec_si128(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_aesdec128((__v2di)(M128I_0), (__v2di)(M128I_1)))
#define _mm_aesdeclast_si128(M128I_0, M128I_1) \
  ((__m128i)__builtin_ia32_aesdeclast128((__v2di)(M128I_0), (__v2di)(M128I_1)))

#define _mm_aesimc_si128(M128I) \
  ((__m128i)__builtin_ia32_aesimc128((__v2di)(M128I)))
#define _mm_aesimclast_si128(M128I) \
  ((__m128i)__builtin_ia32_aesimclast128((__v2di)(M128I)))

#define _mm_aeskeygenassist_si128(X, Y) \
  ((__m128i)__builtin_ia32_aeskeygenassist128((__v2di)(__m128i)(X), (int)(Y)))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_WMMINTRIN_H_ */
