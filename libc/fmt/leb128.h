#ifndef COSMOPOLITAN_LIBC_FMT_LEB128_H_
#define COSMOPOLITAN_LIBC_FMT_LEB128_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#ifndef __STRICT_ANSI__
int sleb128(const void *, size_t, int128_t);
int unsleb128(const void *, size_t, int128_t *);
#endif /* ANSI */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_LEB128_H_ */
