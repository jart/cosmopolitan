#ifndef COSMOPOLITAN_LIBC_STDIO_STDIO_EXT_H_
#define COSMOPOLITAN_LIBC_STDIO_STDIO_EXT_H_
#include "libc/stdio/stdio.h"

#define FSETLOCKING_QUERY    0
#define FSETLOCKING_INTERNAL 1
#define FSETLOCKING_BYCALLER 2

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

size_t __fbufsize(FILE *);
size_t __fpending(FILE *);
int __flbf(FILE *);
int __freadable(FILE *);
int __fwritable(FILE *);
int __freading(FILE *);
int __fwriting(FILE *);
int __fsetlocking(FILE *, int);
void _flushlbf(void);
void __fpurge(FILE *);
void __fseterr(FILE *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_STDIO_EXT_H_ */
