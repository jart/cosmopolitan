#ifndef COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern FILE g_stdio[3];
extern unsigned char g_stdinbuf[BUFSIZ];
extern unsigned char g_stdoutbuf[BUFSIZ];
extern unsigned char g_stderrbuf[BUFSIZ];

int _fflushregister(FILE *) hidden;
void _fflushunregister(FILE *) hidden;

int __freadbuf(FILE *) hidden;
int __fwritebuf(FILE *) hidden;
long __fseteof(FILE *) hidden;
long __fseterrno(FILE *) hidden;
long __fseterr(FILE *, int) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_ */
