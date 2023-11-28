#ifndef COSMOPOLITAN_LIBC_LCG_H_
#define COSMOPOLITAN_LIBC_LCG_H_
#ifdef _COSMO_SOURCE

forceinline uint64_t KnuthLinearCongruentialGenerator(uint64_t prev[1]) {
  /* Knuth, D.E., "The Art of Computer Programming," Vol 2,
     Seminumerical Algorithms, Third Edition, Addison-Wesley, 1998,
     p. 106 (line 26) & p. 108 */
  prev[0] = prev[0] * 6364136223846793005 + 1442695040888963407;
  return prev[0]; /* be sure to shift! */
}

#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_LCG_H_ */
