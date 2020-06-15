#ifndef COSMOPOLITAN_LIBC_STDIO_UNLOCKED_H_
#define COSMOPOLITAN_LIBC_STDIO_UNLOCKED_H_
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int getc_unlocked(FILE *f) paramsnonnull();
int getchar_unlocked(void);
int putc_unlocked(int c, FILE *f) paramsnonnull();
int putchar_unlocked(int c);
void clearerr_unlocked(FILE *f);
int feof_unlocked(FILE *f);
int ferror_unlocked(FILE *f);
int fileno_unlocked(FILE *f);
int fflush_unlocked(FILE *f);
int fgetc_unlocked(FILE *f);
int fputc_unlocked(int c, FILE *f);
size_t fread_unlocked(void *ptr, size_t size, size_t n, FILE *f);
size_t fwrite_unlocked(const void *ptr, size_t size, size_t n, FILE *f);
char *fgets_unlocked(char *s, int n, FILE *f);
int fputs_unlocked(const char *s, FILE *f);
wint_t getwc_unlocked(FILE *f);
wint_t getwchar_unlocked(void);
wint_t fgetwc_unlocked(FILE *f);
wint_t fputwc_unlocked(wchar_t wc, FILE *f);
wint_t putwc_unlocked(wchar_t wc, FILE *f);
wint_t putwchar_unlocked(wchar_t wc);
wchar_t *fgetws_unlocked(wchar_t *ws, int n, FILE *f);
int fputws_unlocked(const wchar_t *ws, FILE *f);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_UNLOCKED_H_ */
