#ifndef COSMOPOLITAN_LIBC_STDIO_APPEND_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_APPEND_INTERNAL_H_
#define APPEND_COOKIE 21578
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct appendz {
  size_t i; /* data size */
  size_t n; /* allocation size */
};

int appendf(char **, const char *, ...);
int vappendf(char **, const char *, va_list);
int appends(char **, const char *);
int appendd(char **, const void *, size_t);
struct appendz appendz(char *);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define appendf(BUF, FMT, ...) (appendf)(BUF, PFLINK(FMT), ##__VA_ARGS__)
#define vappendf(BUF, FMT, VA) (vappendf)(BUF, PFLINK(FMT), VA)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_APPEND_INTERNAL_H_ */
