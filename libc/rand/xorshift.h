#ifndef COSMOPOLITAN_LIBC_RAND_XORSHIFT_H_
#define COSMOPOLITAN_LIBC_RAND_XORSHIFT_H_

#define kMarsagliaXorshift64Seed 88172645463325252
#define kMarsagliaXorshift32Seed 2463534242

#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline uint64_t MarsagliaXorshift64(uint64_t state[hasatleast 1]) {
  uint64_t x = state[0];
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  state[0] = x;
  return x;
}

forceinline uint32_t MarsagliaXorshift32(uint32_t state[hasatleast 1]) {
  uint32_t x = state[0];
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  state[0] = x;
  return x;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RAND_XORSHIFT_H_ */
