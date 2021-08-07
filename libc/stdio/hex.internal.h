#ifndef COSMOPOLITAN_LIBC_STDIO_HEX_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_HEX_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *DumpHex(const char *, size_t, size_t *);
char *DumpHexc(const char *, size_t, size_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_HEX_INTERNAL_H_ */
