#ifndef COSMOPOLITAN_LIBC_STR_STRWIDTH_H_
#define COSMOPOLITAN_LIBC_STR_STRWIDTH_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int strwidth(const char *, size_t) strlenesque;
int strnwidth(const char *, size_t, size_t) strlenesque;
int strwidth16(const char16_t *, size_t) strlenesque;
int strnwidth16(const char16_t *, size_t, size_t) strlenesque;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_STRWIDTH_H_ */
