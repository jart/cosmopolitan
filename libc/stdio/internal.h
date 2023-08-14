#ifndef COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_
#include "libc/stdio/stdio.h"
#include "libc/thread/thread.h"

#define PUSHBACK 12

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct FILE {
  uint8_t bufmode;      /* 0x00 _IOFBF, etc. (ignored if fd=-1) */
  bool noclose;         /* 0x01 for fake dup() todo delete! */
  uint32_t iomode;      /* 0x04 O_RDONLY, etc. (ignored if fd=-1) */
  int32_t state;        /* 0x08 0=OK, -1=EOF, >0=errno */
  int fd;               /* 0x0c ≥0=fd, -1=closed|buffer */
  uint32_t beg;         /* 0x10 */
  uint32_t end;         /* 0x14 */
  char *buf;            /* 0x18 */
  uint32_t size;        /* 0x20 */
  uint32_t nofree;      /* 0x24 */
  int pid;              /* 0x28 */
  char *getln;          /* 0x30 */
  pthread_mutex_t lock; /* 0x38 */
  struct FILE *next;    /* 0x48 */
  char mem[BUFSIZ];     /* 0x50 */
};

extern uint64_t g_rando;

int __fflush_impl(FILE *);
int __fflush_register(FILE *);
void __fflush_unregister(FILE *);
bool __stdio_isok(FILE *);
FILE *__stdio_alloc(void);
void __stdio_free(FILE *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_INTERNAL_H_ */
