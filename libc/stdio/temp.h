#ifndef COSMOPOLITAN_LIBC_STDIO_TEMP_H_
#define COSMOPOLITAN_LIBC_STDIO_TEMP_H_
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

dontdiscard FILE *tmpfile(void);
dontdiscard int mkstemp(char *);
dontdiscard int mkostemp(char *, unsigned);
dontdiscard int mkstemps(char *, int);
dontdiscard int mkostemps(char *, int, unsigned);
dontdiscard int mkostempsm(char *, int, unsigned, int);
compatfn char *mktemp(char *);
char *tmpnam(char *);

int mkostempsmi(char *, int, unsigned, uint64_t *, int,
                int (*)(const char *, int, ...)) hidden dontdiscard;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_TEMP_H_ */
