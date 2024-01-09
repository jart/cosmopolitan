#ifndef COSMOPOLITAN_LIBC_STDIO_STDIO_EXT_H_
#define COSMOPOLITAN_LIBC_STDIO_STDIO_EXT_H_
#include "libc/stdio/stdio.h"

#define FSETLOCKING_QUERY    0
#define FSETLOCKING_INTERNAL 1
#define FSETLOCKING_BYCALLER 2

COSMOPOLITAN_C_START_

size_t __fbufsize(FILE *) libcesque;
size_t __fpending(FILE *) libcesque;
int __flbf(FILE *) libcesque;
int __freadable(FILE *) libcesque;
int __fwritable(FILE *) libcesque;
int __freading(FILE *) libcesque;
int __fwriting(FILE *) libcesque;
int __fsetlocking(FILE *, int) libcesque;
void _flushlbf(void) libcesque;
void __fpurge(FILE *) libcesque;
void __fseterr(FILE *) libcesque;
const char *__freadptr(FILE *, size_t *) libcesque;
size_t __freadahead(FILE *) libcesque;
void __freadptrinc(FILE *, size_t) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_STDIO_EXT_H_ */
