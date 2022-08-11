#ifndef COSMOPOLITAN_LIBC_RAND_XORSHIFT_H_
#define COSMOPOLITAN_LIBC_RAND_XORSHIFT_H_

#define kMarsagliaXorshift64Seed 88172645463325252
#define kMarsagliaXorshift32Seed 2463534242

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

uint32_t MarsagliaXorshift32(uint32_t[hasatleast 1]);
uint64_t MarsagliaXorshift64(uint64_t[hasatleast 1]);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RAND_XORSHIFT_H_ */
