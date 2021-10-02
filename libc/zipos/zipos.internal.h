#ifndef COSMOPOLITAN_LIBC_ZIPOS_ZIPOS_H_
#define COSMOPOLITAN_LIBC_ZIPOS_ZIPOS_H_
#include "libc/calls/calls.h"
#include "libc/calls/sysdebug.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#if DEBUGSYS
#define ZTRACE(FMT, ...) __printf("ZIP: " FMT "\n", ##__VA_ARGS__)
#else
#define ZTRACE(FMT, ...) (void)0
#endif

struct stat;
struct iovec;

struct Zipos {
  uint8_t *map;
  uint8_t *cdir;
};

struct ZiposUri {
  const char *path;
  size_t len;
};

struct ZiposHandle {
  uint8_t *mem;   /* uncompressed file memory */
  size_t size;    /* byte length of file memory */
  size_t pos;     /* read/write byte offset state */
  uint32_t cfile; /* central directory entry rva */
  int64_t handle;
  uint8_t *freeme;
};

int __zipos_close(int) hidden;
struct Zipos *__zipos_get(void) pureconst hidden;
ssize_t __zipos_parseuri(const char *, struct ZiposUri *) hidden;
ssize_t __zipos_find(struct Zipos *, const struct ZiposUri *);
int __zipos_open(const struct ZiposUri *, unsigned, int) hidden;
int __zipos_stat(const struct ZiposUri *, struct stat *) hidden;
int __zipos_fstat(const struct ZiposHandle *, struct stat *) hidden;
int __zipos_stat_impl(struct Zipos *, size_t, struct stat *) hidden;
ssize_t __zipos_read(struct ZiposHandle *, const struct iovec *, size_t,
                     ssize_t) hidden;
ssize_t __zipos_write(struct ZiposHandle *, const struct iovec *, size_t,
                      ssize_t) hidden;
int64_t __zipos_lseek(struct ZiposHandle *, int64_t, unsigned) hidden;
int __zipos_fcntl(int, int, uintptr_t) hidden;
int __zipos_notat(int, const char *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ZIPOS_ZIPOS_H_ */
