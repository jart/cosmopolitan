#ifndef COSMOPOLITAN_LIBC_FMT_ITOA_H_
#define COSMOPOLITAN_LIBC_FMT_ITOA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § integer conversion                                        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  FASTEST + TINY

  - uint64toarray_radix10(0x31337, a)         l: 68 (20ns) m: 112 (33ns)
  - int64toarray_radix10(0x31337, a)          l: 69 (20ns) m: 134 (39ns)

  FAST + AWESOME

  - snprintf(a, sizeof(a), "%d", 0x31337)     l: 199 (58ns) m: 421 (123ns)
  - uint128toarray_radix10(0x31337, a)        l: 93 (27ns) m: 141 (41ns)
  - int128toarray_radix10(0x31337, a)         l: 96 (28ns) m: 173 (51ns) */

size_t int64toarray_radix10(int64_t, char[hasatleast 21]);
size_t uint64toarray_radix10(uint64_t, char[hasatleast 21]);
size_t uint64toarray_radix16(uint64_t, char[hasatleast 17]);
size_t uint64toarray_fixed16(uint64_t, char[hasatleast 17], uint8_t);
size_t uint64toarray_radix8(uint64_t, char[hasatleast 24]);

#ifndef __STRICT_ANSI__
size_t int128toarray_radix10(int128_t, char *);
size_t uint128toarray_radix10(uint128_t, char *);
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_ITOA_H_ */
