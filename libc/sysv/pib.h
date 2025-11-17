#ifndef COSMOPOLITAN_LIBC_SYSV_PIB_H_
#define COSMOPOLITAN_LIBC_SYSV_PIB_H_
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigset.h"
#include "libc/intrin/fds.h"
#include "libc/limits.h"
#include "libc/proc/proc.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/runtime/runtime.h"
COSMOPOLITAN_C_START_

struct CursorRefs {
  size_t n, c;
  struct Cursor **p;
};

struct CosmoPib {
  int pid;  // must come first (see ape.S)
  int umask;
  int ppid_cosmo;  // use sys_getppid_nt()
  int ppid_win32;  // use sys_getppid_nt_win32()
  int dwCreationFlags;
  int dwPriorityClass;
  intptr_t hStopEvent;
  intptr_t hStopChurn;
  struct CursorRefs refedcursors;
  struct Proc *proc;
  atomic_ulong *sigpending;
  struct Fds fds;
  unsigned sighandrvas[NSIG];
  unsigned sighandflags[NSIG];
  sigset_t sighandmask[NSIG];
  char cwd[PATH_MAX];
  struct rlimit rlimit[RLIM_NLIMITS];  // bit reflected
  jmp_buf vforkret;
};

struct CosmoPib *__get_pib(void) libcesque pureconst;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_PIB_H_ */
