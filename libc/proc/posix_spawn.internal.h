#ifndef COSMOPOLITAN_LIBC_STDIO_SPAWNA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_SPAWNA_INTERNAL_H_
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigset.h"
#include "libc/proc/posix_spawn.h"

#define _POSIX_SPAWN_CLOSE  1
#define _POSIX_SPAWN_DUP2   2
#define _POSIX_SPAWN_OPEN   3
#define _POSIX_SPAWN_CHDIR  4
#define _POSIX_SPAWN_FCHDIR 5

COSMOPOLITAN_C_START_

struct _posix_spawna {
  short flags;
  bool schedparam_isset;
  bool schedpolicy_isset;
  int pgroup;
  int schedpolicy;
  int rlimset;
  struct sched_param schedparam;
  sigset_t sigmask;
  sigset_t sigdefault;
  struct rlimit rlim[16];
};

struct _posix_faction {
  struct _posix_faction *next;
  int action;
  int fildes;
  int oflag;
  union {
    int newfildes;
    unsigned mode;
  };
  char *path;
};

int __posix_spawn_add_file_action(posix_spawn_file_actions_t *,
                                  struct _posix_faction);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_SPAWNA_INTERNAL_H_ */
