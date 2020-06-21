#ifndef COSMOPOLITAN_LIBC_STDIO_UNLOCKED_H_
#define COSMOPOLITAN_LIBC_STDIO_UNLOCKED_H_
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int getc_unlocked(FILE *) paramsnonnull();
int getchar_unlocked(void);
int putc_unlocked(int, FILE *) paramsnonnull();
int putchar_unlocked(int);
void clearerr_unlocked(FILE *);
int feof_unlocked(FILE *);
int ferror_unlocked(FILE *);
int fileno_unlocked(FILE *);
int fflush_unlocked(FILE *);
int fgetc_unlocked(FILE *);
int fputc_unlocked(int, FILE *);
size_t fread_unlocked(void *, size_t, size_t, FILE *);
size_t fwrite_unlocked(const void *, size_t, size_t, FILE *);
char *fgets_unlocked(char *, int, FILE *);
int fputs_unlocked(const char *, FILE *);
wint_t getwc_unlocked(FILE *);
wint_t getwchar_unlocked(void);
wint_t fgetwc_unlocked(FILE *);
wint_t fputwc_unlocked(wchar_t, FILE *);
wint_t putwc_unlocked(wchar_t, FILE *);
wint_t putwchar_unlocked(wchar_t);
wchar_t *fgetws_unlocked(wchar_t *, int, FILE *);
int fputws_unlocked(const wchar_t *, FILE *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_UNLOCKED_H_ */
