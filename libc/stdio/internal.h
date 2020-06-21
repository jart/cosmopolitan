#ifndef COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern FILE g_stdio[3];
extern unsigned char g_stdinbuf[BUFSIZ];
extern unsigned char g_stdoutbuf[BUFSIZ];
extern unsigned char g_stderrbuf[BUFSIZ];

int fflushregister(FILE *) hidden;
void fflushunregister(FILE *) hidden;

int freadbuf(FILE *) hidden;
int fwritebuf(FILE *) hidden;
int fsreadbuf(FILE *) hidden;
int fswritebuf(FILE *) hidden;
long fseteof(FILE *) hidden;
long fseterrno(FILE *) hidden;
long fseterr(FILE *, int) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_ */
