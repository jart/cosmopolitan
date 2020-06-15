#ifndef COSMOPOLITAN_LIBC_CALLS_UTIME_H_
#define COSMOPOLITAN_LIBC_CALLS_UTIME_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct utimbuf {
  int64_t actime;  /* access time */
  int64_t modtime; /* modified time */
};

int utime(const char *path, const struct utimbuf *times) paramsnonnull((1));

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_UTIME_H_ */
