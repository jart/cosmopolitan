#ifndef COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#include "libc/stdio/stdio.h"
#include "libc/thread/thread.h"

#define PUSHBACK 12

COSMOPOLITAN_C_START_

struct FILE {
  uint8_t bufmode; /* _IOFBF, etc. (ignored if fd=-1) */
  char noclose;    /* for fake dup() todo delete! */
  char dynamic;    /* did malloc() create this object? */
  uint32_t iomode; /* O_RDONLY, etc. (ignored if fd=-1) */
  int32_t state;   /* 0=OK, -1=EOF, >0=errno */
  int fd;          /* ≥0=fd, -1=closed|buffer */
  uint32_t beg;
  uint32_t end;
  char *buf;
  uint32_t size;
  uint32_t nofree;
  int pid;
  char *getln;
  pthread_mutex_t lock;
  struct FILE *next;
  char mem[BUFSIZ];
};

extern uint64_t g_rando;

int __fflush_impl(FILE *);
int __fflush_register(FILE *);
void __fflush_unregister(FILE *);
bool __stdio_isok(FILE *);
FILE *__stdio_alloc(void);
void __stdio_free(FILE *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_ */
