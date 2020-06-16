#ifndef COSMOPOLITAN_LIBC_BITS_PMMINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_PMMINTRIN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § it's a trap! » sse3                                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define _mm_hadd_ps(M128_0, M128_1)                        \
  ((__m128)__builtin_ia32_haddps((__v4sf)(__m128)(M128_0), \
                                 (__v4sf)(__m128)(M128_0)))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_PMMINTRIN_H_ */
