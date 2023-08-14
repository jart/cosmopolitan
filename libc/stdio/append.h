#ifndef COSMOPOLITAN_LIBC_STDIO_APPEND_H_
#define COSMOPOLITAN_LIBC_STDIO_APPEND_H_
#ifdef _COSMO_SOURCE

#define APPEND_COOKIE 21578

#define appendz   __appendz
#define appendr   __appendr
#define appendd   __appendd
#define appendw   __appendw
#define appends   __appends
#define appendf   __appendf
#define vappendf  __vappendf
#define kappendf  __kappendf
#define kvappendf __kvappendf

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

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_STDIO_APPEND_H_ */
