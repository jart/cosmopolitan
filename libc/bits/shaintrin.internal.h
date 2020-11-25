#ifndef COSMOPOLITAN_LIBC_BITS_SHAINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_SHAINTRIN_H_
#include "libc/bits/emmintrin.internal.h"
#include "libc/bits/xmmintrin.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define _mm_sha1rnds4_epu32(M128I_0, M128I_1, MEM)     \
  __builtin_ia32_sha1rnds4((__v4si)(__m128i)(M128I_0), \
                           (__v4si)(__m128i)(M128I_1), (MEM))

#define _mm_sha1nexte_epu32(M128I_0, M128I_1)                    \
  ((__m128i)__builtin_ia32_sha1nexte((__v4si)(__m128i)(M128I_0), \
                                     (__v4si)(__m128i)(M128I_1)))

#define _mm_sha1msg1_epu32(M128I_0, M128I_1)                    \
  ((__m128i)__builtin_ia32_sha1msg1((__v4si)(__m128i)(M128I_0), \
                                    (__v4si)(__m128i)(M128I_1)))

#define _mm_sha1msg2_epu32(M128I_0, M128I_1)                    \
  ((__m128i)__builtin_ia32_sha1msg2((__v4si)(__m128i)(M128I_0), \
                                    (__v4si)(__m128i)(M128I_1)))

#define _mm_sha256rnds2_epu32(M128I_0, M128I_1, M128I_2)           \
  ((__m128i)__builtin_ia32_sha256rnds2((__v4si)(__m128i)(M128I_0), \
                                       (__v4si)(__m128i)(M128I_1), \
                                       (__v4si)(__m128i)(M128I_2)))

#define _mm_sha256msg1_epu32(M128I_0, M128I_1)                    \
  ((__m128i)__builtin_ia32_sha256msg1((__v4si)(__m128i)(M128I_0), \
                                      (__v4si)(__m128i)(M128I_1)))

#define _mm_sha256msg2_epu32(M128I_0, M128I_1)                    \
  ((__m128i)__builtin_ia32_sha256msg2((__v4si)(__m128i)(M128I_0), \
                                      (__v4si)(__m128i)(M128I_1)))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_SHAINTRIN_H_ */
