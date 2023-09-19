#ifndef COSMOPOLITAN_LIBC_PROC_H_
#define COSMOPOLITAN_LIBC_PROC_H_
#include "libc/atomic.h"
#include "libc/calls/struct/rusage.h"
#include "libc/intrin/dll.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define PROC_CONTAINER(e) DLL_CONTAINER(struct Proc, elem, e)

struct Proc {
  int pid;
  int waiters;
  bool iszombie;
  bool wasforked;
  uint32_t wstatus;
  int64_t handle;
  struct Dll elem;
  nsync_cv onexit;
};

struct Procs {
  int waiters;
  atomic_uint once;
  nsync_mu lock;
  nsync_cv onexit;
  intptr_t thread;
  intptr_t onstart;
  struct Dll *list;
  struct Dll *free;
  struct Dll *zombies;
  struct Proc pool[8];
  unsigned allocated;
  struct CosmoTib tls;
};

extern struct Procs __proc;

void __proc_wipe(void);
void __proc_lock(void);
void __proc_unlock(void);
struct Proc *__proc_new(void);
void __proc_add(struct Proc *);
void __proc_free(struct Proc *);
int sys_wait4_nt(int, int *, int, struct rusage *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_PROC_H_ */
