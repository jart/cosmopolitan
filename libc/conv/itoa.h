#ifndef COSMOPOLITAN_LIBC_CONV_ITOA_H_
#define COSMOPOLITAN_LIBC_CONV_ITOA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*

  FASTEST + TINY

  - uint64toarray_radix10(0x31337, a)         l: 68 (20ns) m: 112 (33ns)
  - int64toarray_radix10(0x31337, a)          l: 69 (20ns) m: 134 (39ns)

  FAST + AWESOME

  - snprintf(a, sizeof(a), "%d", 0x31337)     l: 199 (58ns) m: 421 (123ns)
  - uint128toarray_radix10(0x31337, a)        l: 93 (27ns) m: 141 (41ns)
  - int128toarray_radix10(0x31337, a)         l: 96 (28ns) m: 173 (51ns)

  SLOWEST + GENERAL

  - int64toarray(0x31337, a, 10)              l: 218 (64ns) m: 262 (77ns)
  - uint64toarray(0x31337, a, 10)             l: 565 (166ns) m: 260 (76ns)

*/

size_t int128toarray_radix10(int128_t, char *);
size_t uint128toarray_radix10(uint128_t, char *);
size_t int64toarray_radix10(int64_t, char *);
size_t uint64toarray_radix10(uint64_t, char *);
size_t int64toarray(int64_t, char *, int);
size_t uint64toarray(uint64_t, char *, int);
size_t uint64toarray_radix16(uint64_t, char[hasatleast 17]);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CONV_ITOA_H_ */
