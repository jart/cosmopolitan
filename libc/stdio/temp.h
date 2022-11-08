#ifndef COSMOPOLITAN_LIBC_STDIO_TEMP_H_
#define COSMOPOLITAN_LIBC_STDIO_TEMP_H_
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

FILE *tmpfile(void);
char *mkdtemp(char *);
char *mktemp(char *);
char *tmpnam(char *);
int mkostemp(char *, unsigned);
int mkostemps(char *, int, unsigned);
int mkostempsm(char *, int, unsigned, int);
int mkstemp(char *);
int mkstemps(char *, int);

int mkostempsmi(char *, int, unsigned, uint64_t *, int,
                int (*)(const char *, int, ...)) _Hide dontdiscard;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_TEMP_H_ */
