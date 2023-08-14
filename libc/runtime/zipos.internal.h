#ifndef COSMOPOLITAN_LIBC_ZIPOS_ZIPOS_H_
#define COSMOPOLITAN_LIBC_ZIPOS_ZIPOS_H_
#include "libc/intrin/nopl.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct stat;
struct iovec;

struct ZiposUri {
  const char *path;
  size_t len;
};

struct ZiposHandle {
  struct ZiposHandle *next;
  pthread_mutex_t lock;
  size_t size;    /* byte length of `mem` */
  size_t mapsize; /* total size of this struct */
  size_t pos;     /* read/write byte offset state */
  uint32_t cfile; /* central directory entry rva */
  uint8_t *mem;   /* points to inflated data or uncompressed image */
  uint8_t data[]; /* uncompressed file memory */
};

struct Zipos {
  uint8_t *map;
  uint8_t *cdir;
  struct ZiposHandle *freelist;
};

void __zipos_lock(void);
void __zipos_unlock(void);
int __zipos_close(int);
struct Zipos *__zipos_get(void) pureconst;
void __zipos_free(struct Zipos *, struct ZiposHandle *);
ssize_t __zipos_parseuri(const char *, struct ZiposUri *);
ssize_t __zipos_find(struct Zipos *, const struct ZiposUri *);
int __zipos_open(const struct ZiposUri *, unsigned, int);
int __zipos_access(const struct ZiposUri *, int);
int __zipos_stat(const struct ZiposUri *, struct stat *);
int __zipos_fstat(const struct ZiposHandle *, struct stat *);
int __zipos_stat_impl(struct Zipos *, size_t, struct stat *);
ssize_t __zipos_read(struct ZiposHandle *, const struct iovec *, size_t,
                     ssize_t);
ssize_t __zipos_write(struct ZiposHandle *, const struct iovec *, size_t,
                      ssize_t);
int64_t __zipos_lseek(struct ZiposHandle *, int64_t, unsigned);
int __zipos_fcntl(int, int, uintptr_t);
int __zipos_notat(int, const char *);
void *__zipos_mmap(void *, uint64_t, int32_t, int32_t, struct ZiposHandle *,
                   int64_t) dontasan;

#ifdef _NOPL0
#define __zipos_lock()   _NOPL0("__threadcalls", __zipos_lock)
#define __zipos_unlock() _NOPL0("__threadcalls", __zipos_unlock)
#else
#define __zipos_lock()   (__threaded ? __zipos_lock() : 0)
#define __zipos_unlock() (__threaded ? __zipos_unlock() : 0)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ZIPOS_ZIPOS_H_ */
