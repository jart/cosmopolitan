#ifndef COSMOPOLITAN_LIBC_STDIO_APPEND_H_
#define COSMOPOLITAN_LIBC_STDIO_APPEND_H_
#include "libc/fmt/pflink.h"

#define APPEND_COOKIE 21578

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct appendz {
  size_t i;
  size_t n;
};

struct appendz appendz(char *);
ssize_t appendr(char **, size_t);
ssize_t appendd(char **, const void *, size_t);
ssize_t appendw(char **, uint64_t);
ssize_t appends(char **, const char *);
ssize_t appendf(char **, const char *, ...);
ssize_t vappendf(char **, const char *, va_list);
ssize_t kappendf(char **, const char *, ...);
ssize_t kvappendf(char **, const char *, va_list);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define appendf(BUF, FMT, ...) appendf(BUF, PFLINK(FMT), ##__VA_ARGS__)
#define vappendf(BUF, FMT, VA) vappendf(BUF, PFLINK(FMT), VA)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_APPEND_H_ */
