#ifndef COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#include "libc/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/thread/thread.h"

#define PUSHBACK 12

#define FILE_CONTAINER(e) DLL_CONTAINER(struct FILE, elem, e)

COSMOPOLITAN_C_START_

struct FILE {
  char bufmode;  /* _IOFBF, _IOLBF, or _IONBF */
  char freethis; /* fclose() should free(this) */
  char freebuf;  /* fclose() should free(this->buf) */
  char forking;  /* used by fork() implementation */
  int oflags;    /* O_RDONLY, etc. */
  int state;     /* 0=OK, -1=EOF, >0=errno */
  int fd;        /* ≥0=fd, -1=closed|buffer */
  int pid;
  atomic_int refs;
  size_t size;
  size_t beg;
  size_t end;
  char *buf;
  char **memstream_bufp;
  pthread_mutex_t lock;
  struct Dll elem;
  char *getln;
  size_t *memstream_sizep;
};

struct Stdio {
  pthread_mutex_t lock; /* Subordinate to FILE::lock */
  struct Dll *files;
};

extern struct Stdio __stdio;

void __stdio_lock(void);
void __stdio_unlock(void);
void __stdio_ref(FILE *);
void __stdio_unref(FILE *);
void __stdio_unref_unlocked(FILE *);
bool __stdio_isok(FILE *);
FILE *__stdio_alloc(void);

/* use these in stdio wrapper functions that are cancelation points */
#define FLOCKFILE(f)                                     \
  struct _pthread_cleanup_buffer cb;                     \
  if (__isthreaded >= 2) {                               \
    flockfile(f);                                        \
    __pthread_cleanup_push(&cb, (void *)funlockfile, f); \
  }
#define FUNLOCKFILE(f)   \
  if (__isthreaded >= 2) \
  __pthread_cleanup_pop(&cb, 1)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_ */
