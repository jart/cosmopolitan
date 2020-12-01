#ifndef COSMOPOLITAN_LIBC_LCG_H_
#define COSMOPOLITAN_LIBC_LCG_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline uint64_t KnuthLinearCongruentialGenerator(uint64_t prev[1]) {
  /* Knuth, D.E., "The Art of Computer Programming," Vol 2,
     Seminumerical Algorithms, Third Edition, Addison-Wesley, 1998,
     p. 106 (line 26) & p. 108 */
  prev[0] = prev[0] * 6364136223846793005 + 1442695040888963407;
  return prev[0];
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LCG_H_ */
