#ifndef COSMOPOLITAN_LIBC_PROC_H_
#define COSMOPOLITAN_LIBC_PROC_H_
#include "libc/atomic.h"
#include "libc/calls/struct/rusage.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/mu.h"
COSMOPOLITAN_C_START_

#define PROC_ALIVE  0
#define PROC_ZOMBIE 1
#define PROC_UNDEAD 2

#define PROC_CONTAINER(e) DLL_CONTAINER(struct Proc, elem, e)

struct Proc {
  int pid;
  int status;
  int waiters;
  bool wasforked;
  uint32_t wstatus;
  int64_t handle;
  struct Dll elem;
  struct rusage ru;
};

struct Procs {
  int waiters;
  atomic_uint once;
  nsync_mu lock;
  intptr_t thread;
  intptr_t onbirth;
  intptr_t haszombies;
  struct Dll *list;
  struct Dll *free;
  struct Dll *undead;
  struct Dll *zombies;
  struct Proc pool[8];
  unsigned allocated;
  struct rusage ruchlds;
};

extern struct Procs __proc;

void __proc_wipe(void) libcesque;
void __proc_lock(void) libcesque;
void __proc_unlock(void) libcesque;
int64_t __proc_handle(int) libcesque;
int64_t __proc_search(int) libcesque;
struct Proc *__proc_new(void) libcesque;
void __proc_add(struct Proc *) libcesque;
void __proc_free(struct Proc *) libcesque;
int __proc_harvest(struct Proc *, bool) libcesque;
int sys_wait4_nt(int, int *, int, struct rusage *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_PROC_H_ */
