#ifndef COSMOPOLITAN_LIBC_ZIPOS_ZIPOS_H_
#define COSMOPOLITAN_LIBC_ZIPOS_ZIPOS_H_
COSMOPOLITAN_C_START_

#define ZIPOS_PATH_MAX 1024

#define ZIPOS_SYNTHETIC_DIRECTORY 0

struct stat;
struct iovec;
struct Zipos;

struct ZiposUri {
  uint32_t len;
  char path[ZIPOS_PATH_MAX];
};

struct ZiposHandle {
  struct ZiposHandle *next;
  struct Zipos *zipos;
  size_t size;
  size_t mapsize;
  size_t cfile;
  _Atomic(size_t) refs;
  _Atomic(size_t) pos;
  uint8_t *mem;
  uint8_t data[];
};

struct Zipos {
  long pagesz;
  uint8_t *map;
  uint8_t *cdir;
  uint64_t dev;
  size_t *index;
  size_t records;
  struct ZiposHandle *freelist;
};

int __zipos_close(int);
void __zipos_drop(struct ZiposHandle *);
struct ZiposHandle *__zipos_keep(struct ZiposHandle *);
struct Zipos *__zipos_get(void) pureconst;
size_t __zipos_normpath(char *, const char *, size_t);
ssize_t __zipos_find(struct Zipos *, struct ZiposUri *);
ssize_t __zipos_scan(struct Zipos *, struct ZiposUri *);
ssize_t __zipos_parseuri(const char *, struct ZiposUri *);
uint64_t __zipos_inode(struct Zipos *, int64_t, const void *, size_t);
int __zipos_open(struct ZiposUri *, int);
void __zipos_postdup(int, int);
int __zipos_access(struct ZiposUri *, int);
int __zipos_stat(struct ZiposUri *, struct stat *);
int __zipos_fstat(struct ZiposHandle *, struct stat *);
int __zipos_stat_impl(struct Zipos *, size_t, struct stat *);
ssize_t __zipos_read(struct ZiposHandle *, const struct iovec *, size_t,
                     ssize_t);
int64_t __zipos_seek(struct ZiposHandle *, int64_t, unsigned);
int __zipos_fcntl(int, int, uintptr_t);
int __zipos_notat(int, const char *);
void *__zipos_mmap(void *, uint64_t, int32_t, int32_t, struct ZiposHandle *,
                   int64_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_ZIPOS_ZIPOS_H_ */
