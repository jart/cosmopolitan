#ifndef COSMOPOLITAN_LIBC_STDIO_TEMP_H_
#define COSMOPOLITAN_LIBC_STDIO_TEMP_H_
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

nodiscard FILE *tmpfile(void);
nodiscard int mkstemp(char *);
nodiscard int mkostemp(char *, unsigned);
nodiscard int mkstemps(char *, int);
nodiscard int mkostemps(char *, int, unsigned);
nodiscard int mkostempsm(char *, int, unsigned, int);
compatfn char *mktemp(char *);

int mkostempsmi(char *, int, unsigned, uint64_t *, int,
                int (*)(const char *, int, ...)) hidden nodiscard;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_TEMP_H_ */
