#ifndef COSMOPOLITAN_LIBC_BITS_SMMINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_SMMINTRIN_H_

/**
 * @fileoverview SSE4 intrinsics.
 */

#define _MM_FROUND_CEIL           (_MM_FROUND_TO_POS_INF | _MM_FROUND_RAISE_EXC)
#define _MM_FROUND_CUR_DIRECTION  4
#define _MM_FROUND_FLOOR          (_MM_FROUND_TO_NEG_INF | _MM_FROUND_RAISE_EXC)
#define _MM_FROUND_NEARBYINT      (_MM_FROUND_CUR_DIRECTION | _MM_FROUND_NO_EXC)
#define _MM_FROUND_NINT           (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_RAISE_EXC)
#define _MM_FROUND_NO_EXC         8
#define _MM_FROUND_RAISE_EXC      0
#define _MM_FROUND_RINT           (_MM_FROUND_CUR_DIRECTION | _MM_FROUND_RAISE_EXC)
#define _MM_FROUND_TO_NEAREST_INT 0
#define _MM_FROUND_TO_NEG_INF     1
#define _MM_FROUND_TO_POS_INF     2
#define _MM_FROUND_TO_ZERO        3
#define _MM_FROUND_TRUNC          (_MM_FROUND_TO_ZERO | _MM_FROUND_RAISE_EXC)

#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define _mm_extract_epi32(M128I, I32) \
  ((int)__builtin_ia32_vec_ext_v4si((__v4si)(__m128i)(M128I), (int)(I32)))

#define _mm_minpos_epu16(M128I) \
  ((int)__builtin_ia32_phminposuw128((__v4si)(__m128i)(M128I), (int)(I32)))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_SMMINTRIN_H_ */
