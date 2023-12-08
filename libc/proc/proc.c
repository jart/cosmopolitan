/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/leaky.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/nt/accounting.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/events.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/iocounters.h"
#include "libc/nt/struct/processmemorycounters.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/proc/proc.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

/**
 * @fileoverview Windows Subprocess Management.
 */

struct Procs __proc;

static textwindows void __proc_stats(int64_t h, struct rusage *ru) {
  bzero(ru, sizeof(*ru));
  struct NtProcessMemoryCountersEx memcount = {sizeof(memcount)};
  GetProcessMemoryInfo(h, &memcount, sizeof(memcount));
  ru->ru_maxrss = memcount.PeakWorkingSetSize / 1024;
  ru->ru_majflt = memcount.PageFaultCount;
  struct NtFileTime createtime, exittime;
  struct NtFileTime kerneltime, usertime;
  GetProcessTimes(h, &createtime, &exittime, &kerneltime, &usertime);
  ru->ru_utime = WindowsDurationToTimeVal(ReadFileTime(usertime));
  ru->ru_stime = WindowsDurationToTimeVal(ReadFileTime(kerneltime));
  struct NtIoCounters iocount;
  GetProcessIoCounters(h, &iocount);
  ru->ru_inblock = iocount.ReadOperationCount;
  ru->ru_oublock = iocount.WriteOperationCount;
}

// performs accounting on exited process
// multiple threads can wait on a process
// it's important that only one calls this
textwindows int __proc_harvest(struct Proc *pr, bool iswait4) {
  int sic = 0;
  uint32_t status;
  struct rusage ru;
  GetExitCodeProcess(pr->handle, &status);
  if (status == kNtStillActive) return 0;
  __proc_stats(pr->handle, &ru);
  rusage_add(&pr->ru, &ru);
  rusage_add(&__proc.ruchlds, &ru);
  if ((status & 0xFF000000u) == 0x23000000u) {
    // handle child execve()
    CloseHandle(pr->handle);
    pr->handle = status & 0x00FFFFFF;
  } else {
    // handle child _Exit()
    if (status == 0xc9af3d51u) {
      status = kNtStillActive;
    }
    pr->wstatus = status;
    if (!iswait4 && !pr->waiters && !__proc.waiters &&
        (__sighandrvas[SIGCHLD] == (uintptr_t)SIG_IGN ||
         (__sighandflags[SIGCHLD] & SA_NOCLDWAIT))) {
      // perform automatic zombie reaping
      dll_remove(&__proc.list, &pr->elem);
      dll_make_first(&__proc.free, &pr->elem);
      CloseHandle(pr->handle);
    } else {
      // transitions process to zombie state
      // wait4 is responsible for reaping it
      pr->status = PROC_ZOMBIE;
      dll_remove(&__proc.list, &pr->elem);
      dll_make_first(&__proc.zombies, &pr->elem);
      SetEvent(__proc.haszombies);
      if (!pr->waiters && !__proc.waiters) {
        if (WIFSIGNALED(status)) {
          sic = CLD_KILLED;
        } else {
          sic = CLD_EXITED;
        }
      }
    }
  }
  return sic;
}

static textwindows dontinstrument uint32_t __proc_worker(void *arg) {
  struct CosmoTib tls;
  __bootstrap_tls(&tls, __builtin_frame_address(0));
  for (;;) {

    // assemble a group of processes to wait on. if more than 64
    // children exist, then we'll use a small timeout and select
    // processes with a shifting window via a double linked list
    // if fewer than 64 processes exist then we'll also wait for
    // process birth notifications, and wait on them immediately
    int64_t handles[64];
    struct Proc *objects[64];
    uint32_t millis, i, n = 0;
    struct Dll *e, *e2, *samples = 0;
    __proc_lock();
    for (e = dll_first(__proc.list); e && n < 64; e = e2) {
      struct Proc *pr = PROC_CONTAINER(e);
      e2 = dll_next(__proc.list, e);
      // cycle process to end of list
      dll_remove(&__proc.list, e);
      dll_make_last(&samples, e);
      // don't bother waiting if it's already awaited
      if (!pr->waiters) {
        handles[n] = pr->handle;
        objects[n] = pr;
        ++pr->waiters;
        ++n;
      }
    }
    dll_make_last(&__proc.list, samples);
    __proc_unlock();

    // wait for something to happen
    if (n == 64) {
      millis = 5;
    } else {
      millis = -1u;
      handles[n++] = __proc.onbirth;
    }
    i = WaitForMultipleObjects(n, handles, false, millis);
    if (i == -1u) {
      STRACE("proc wait panic %d", GetLastError());
      _Exit(157);
    }
    if (i & kNtWaitAbandoned) {
      i &= ~kNtWaitAbandoned;
      STRACE("proc %u handle %ld abandoned", i, handles[i]);
    }
    __proc_lock();

    // release our waiter status
    for (int j = 0; j < n; ++j) {
      if (handles[j] == __proc.onbirth) continue;
      if (j == i) continue;
      if (!--objects[j]->waiters && objects[j]->status == PROC_UNDEAD) {
        __proc_free(objects[j]);
      }
    }

    // check if we need to churn due to >64 processes
    if (i == kNtWaitTimeout) {
      __proc_unlock();
      continue;
    }

    // churn on new process birth
    if (handles[i] == __proc.onbirth) {
      __proc_unlock();
      continue;
    }

    // handle process status change
    int sic = 0;
    --objects[i]->waiters;
    switch (objects[i]->status) {
      case PROC_ALIVE:
        sic = __proc_harvest(objects[i], false);
        break;
      case PROC_ZOMBIE:
        break;
      case PROC_UNDEAD:
        if (!objects[i]->waiters) {
          __proc_free(objects[i]);
        }
        break;
      default:
        __builtin_unreachable();
    }

    __proc_unlock();

    // don't raise SIGCHLD if
    // 1. wait4() is being used
    // 2. SIGCHLD has SIG_IGN handler
    // 3. SIGCHLD has SA_NOCLDWAIT flag
    if (sic) {
      __sig_generate(SIGCHLD, sic);
    }
  }
  return 0;
}

/**
 * Lazy initializes process tracker data structures and worker.
 */
static textwindows void __proc_setup(void) {
  __proc.onbirth = CreateEvent(0, 0, 0, 0);     // auto reset
  __proc.haszombies = CreateEvent(0, 1, 0, 0);  // manual reset
  __proc.thread = CreateThread(0, 65536, __proc_worker, 0,
                               kNtStackSizeParamIsAReservation, 0);
}

/**
 * Locks process tracker.
 */
textwindows void __proc_lock(void) {
  cosmo_once(&__proc.once, __proc_setup);
  nsync_mu_lock(&__proc.lock);
}

/**
 * Unlocks process tracker.
 */
textwindows void __proc_unlock(void) {
  nsync_mu_unlock(&__proc.lock);
}

/**
 * Resets process tracker from forked child.
 */
textwindows void __proc_wipe(void) {
  bzero(&__proc, sizeof(__proc));
}

/**
 * Allocates object for new process.
 *
 * The returned memory is not tracked by any list. It must be filled in
 * with system process information and then added back to the system by
 * calling __proc_add(). If process creation fails, then it needs to be
 * added back to the __proc.free list by caller.
 */
textwindows struct Proc *__proc_new(void) {
  struct Dll *e;
  struct Proc *proc = 0;
  // fork() + wait() don't depend on malloc() so neither shall we
  if (__proc.allocated < ARRAYLEN(__proc.pool)) {
    proc = __proc.pool + __proc.allocated++;
  } else {
    if ((e = dll_first(__proc.free))) {
      proc = PROC_CONTAINER(e);
      dll_remove(&__proc.free, &proc->elem);
    }
    if (!proc) {
      if (_weaken(malloc)) {
        proc = _weaken(malloc)(sizeof(struct Proc));
      } else {
        enomem();
        return 0;
      }
    }
  }
  if (proc) {
    bzero(proc, sizeof(*proc));
    dll_init(&proc->elem);
  }
  return proc;
}

IGNORE_LEAKS(__proc_new)

/**
 * Adds process to active list.
 *
 * The handle and pid must be filled in before calling this.
 */
textwindows void __proc_add(struct Proc *proc) {
  dll_make_first(&__proc.list, &proc->elem);
  SetEvent(__proc.onbirth);
}

textwindows void __proc_free(struct Proc *pr) {
  dll_remove(&__proc.undead, &pr->elem);
  dll_make_first(&__proc.free, &pr->elem);
  CloseHandle(pr->handle);
}

// returns owned handle of direct child process
// this is intended for the __proc_handle() implementation
textwindows int64_t __proc_search(int pid) {
  struct Dll *e;
  int64_t handle = 0;
  BLOCK_SIGNALS;
  __proc_lock();
  for (e = dll_first(__proc.list); e; e = dll_next(__proc.list, e)) {
    if (pid == PROC_CONTAINER(e)->pid) {
      handle = PROC_CONTAINER(e)->handle;
      break;
    }
  }
  __proc_unlock();
  ALLOW_SIGNALS;
  return handle;
}

#endif /* __x86_64__ */
