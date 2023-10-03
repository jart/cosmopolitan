#ifndef COSMOPOLITAN_ZIPOS_H_
#define COSMOPOLITAN_ZIPOS_H_
#include "libc/thread/thread.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define ZIPOS_PATH_MAX 1024

#define ZIPOS_SYNTHETIC_DIRECTORY -2

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
  int64_t handle;   /* original upstream open()'d handle */
  uint64_t size;    /* accessible bytes stored at data[] */
  uint64_t mapsize; /* the full byte size of this struct */
  uint64_t pos;     /* the file position, relative start */
  int cfile;        /* byte offset into zipos->cdir ents */
  uint8_t data[];   /* original file content or dir name */
};

struct Zipos {
  _Atomic(unsigned) once;
  int cnt;       /* element count, accessible via `index` */
  int cdirsize;  /* number of bytes accessible via `cdir` */
  uint64_t dev;  /* remembers st_dev, of zipos image file */
  int *index;    /* points to cdirsize+cnt*4 mmap'd bytes */
  uint8_t *cdir; /* points after index, in the above mmap */
  const char *progpath;
  struct ZiposHandle *freelist;
  char *mapend;
  size_t maptotal;
  pthread_mutex_t lock;
};

int __zipos_close(int);
void __zipos_lock(void);
void __zipos_unlock(void);
void __zipos_free(struct ZiposHandle *);
struct Zipos *__zipos_get(void) pureconst;
size_t __zipos_normpath(char *, const char *, size_t);
int __zipos_find(struct Zipos *, struct ZiposUri *);
int __zipos_scan(struct Zipos *, struct ZiposUri *);
ssize_t __zipos_parseuri(const char *, struct ZiposUri *);
uint64_t __zipos_inode(struct Zipos *, int64_t, const void *, size_t);
int __zipos_open(struct ZiposUri *, int);
int __zipos_access(struct ZiposUri *, int);
int __zipos_stat(struct ZiposUri *, struct stat *);
int __zipos_fstat(struct ZiposHandle *, struct stat *);
int __zipos_stat_impl(struct Zipos *, int, struct stat *);
ssize_t __zipos_read(struct ZiposHandle *, const struct iovec *, size_t,
                     ssize_t);
int64_t __zipos_seek(struct ZiposHandle *, int64_t, unsigned);
int __zipos_fcntl(int, int, uintptr_t);
int __zipos_notat(int, const char *);
void *__zipos_mmap(void *, uint64_t, int32_t, int32_t, struct ZiposHandle *,
                   int64_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_ZIPOS_H_ */
