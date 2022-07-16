#ifndef COSMOPOLITAN_LIBC_FMT_NF32_H_
#define COSMOPOLITAN_LIBC_FMT_NF32_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const char kNfcimal[256];

uint32_t DecodeNf32(const char *, char **);
char *EncodeNf32(char[hasatleast 12], uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_NF32_H_ */
