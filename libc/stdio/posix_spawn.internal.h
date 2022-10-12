#ifndef COSMOPOLITAN_LIBC_STDIO_SPAWNA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_SPAWNA_INTERNAL_H_
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigset.h"

#define _POSIX_SPAWN_CLOSE 1
#define _POSIX_SPAWN_DUP2  2
#define _POSIX_SPAWN_OPEN  3

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct _posix_spawna {
  char flags;
  bool schedparam_isset;
  bool schedpolicy_isset;
  bool sigmask_isset;
  int pgroup;
  int schedpolicy;
  struct sched_param schedparam;
  sigset_t sigmask;
  sigset_t sigdefault;
};

struct _posix_faction {
  struct _posix_faction *next;
  int action;
  union {
    int fildes;
    int oflag;
  };
  union {
    int newfildes;
    unsigned mode;
  };
  const char *path;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_SPAWNA_INTERNAL_H_ */
