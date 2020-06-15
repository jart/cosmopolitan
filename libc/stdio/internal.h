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

int freadbuf(FILE *f) hidden;
int fwritebuf(FILE *f) hidden;
int fsreadbuf(FILE *f) hidden;
int fswritebuf(FILE *f) hidden;
long fseteof(FILE *f) hidden;
long fseterrno(FILE *f) hidden;
long fseterr(FILE *f, int err) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_ */
