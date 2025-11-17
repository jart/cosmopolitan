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
#include "libc/proc/proc.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.h"
#include "libc/nt/accounting.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/events.h"
#include "libc/nt/memory.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/iocounters.h"
#include "libc/nt/struct/processmemorycounters.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/pib.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#if SupportsWindows()

/**
 * @fileoverview Windows Subprocess Management.
 */

#define STACK_SIZE 65536

__msabi extern typeof(WaitForMultipleObjects)
    *const __imp_WaitForMultipleObjects;

struct Procs __proc;
alignas(64) static pthread_mutex_t __proc_lock_obj = PTHREAD_MUTEX_INITIALIZER;

textwindows static void __proc_stats(int64_t h, struct rusage *ru) {
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

textwindows static int __proc_checkstop(struct Proc *pr) {
  int sic = 0;
  if (pr->status == PROC_ALIVE) {
    if (!WaitForSingleObject(pr->hStopEvent, 0)) {
      atomic_ulong *pSigs;
      if ((pSigs = __sig_map_process(pr->pid, kNtOpenExisting))) {
        sigset_t sigs = atomic_load(pSigs);
        int stopsig;
        if (sigs & (1ull << (SIGSTOP - 1))) {
          stopsig = SIGSTOP;
        } else if (sigs & (1ull << (SIGTSTP - 1))) {
          stopsig = SIGTSTP;
        } else if (sigs & (1ull << (SIGTTIN - 1))) {
          stopsig = SIGTTIN;
        } else if (sigs & (1ull << (SIGTTOU - 1))) {
          stopsig = SIGTTOU;
        } else {
          stopsig = 0;
        }
        UnmapViewOfFile(pSigs);
        if (stopsig) {
          pr->status = PROC_STOPPED;
          pr->dwExitCode = stopsig << 8 | 0x7f;
          dll_make_first(&__proc.stopped, &pr->stopelem);
          SetEvent(__proc.hasstopped);
          if (!pr->stopwaiters && !__proc.stopwaiters &&
              !(__get_pib()->sighandflags[SIGCHLD - 1] & SA_NOCLDSTOP))
            sic = CLD_STOPPED;
        }
      }
    }
  }
  return sic;
}

// performs accounting on exited process
// multiple threads can wait on a process
// it's important that only one calls this
textwindows int __proc_harvest(struct Proc *pr, bool iswait4) {
  int sic = 0;
  uint32_t dwExitCode;
  struct rusage ru;
  if (pr->isvfork) {
    if (pr->hProcess2) {
      // vfork() process called execve() and wants to replace its event
      // handle with the process handle it obtained from CreateProcess()
      CloseHandle(pr->hProcess);
      pr->hProcess = pr->hProcess2;
      pr->hProcess2 = 0;
      pr->isvfork = false;
      return 0;
    } else {
      // vfork() process called _Exit() or was terminated by some signal
      dwExitCode = pr->dwExitCode;
    }
  } else {
    GetExitCodeProcess(pr->hProcess, &dwExitCode);
  }
  if (dwExitCode == kNtStillActive)
    return __proc_checkstop(pr);
  __proc_stats(pr->hProcess, &ru);
  if (!pr->isvfork) {
    rusage_add(&pr->ru, &ru);
    rusage_add(&__proc.ruchlds, &ru);
  }
  if ((dwExitCode & 0xFF000000u) == 0x23000000u) {
    // handle child execve()
    CloseHandle(pr->hProcess);
    pr->hProcess = dwExitCode & 0x00FFFFFF;
  } else {
    // handle child _Exit()
    if (dwExitCode == 0xc9af3d51u)
      dwExitCode = kNtStillActive;
    pr->dwExitCode = dwExitCode;
    if (pr->status == PROC_STOPPED) {
      dll_remove(&__proc.stopped, &pr->stopelem);
      if (dll_is_empty(__proc.stopped))
        ResetEvent(__proc.hasstopped);
    }
    if (__get_pib()->sighandrvas[SIGCHLD - 1] == (uintptr_t)SIG_IGN ||
        (__get_pib()->sighandflags[SIGCHLD - 1] & SA_NOCLDWAIT)) {
      // perform automatic zombie reaping
      STRACE("automatically reaping zombie");
      pr->status = PROC_UNDEAD;
      dll_remove(&__proc.list, &pr->elem);
      dll_make_first(&__proc.undead, &pr->elem);
      SetEvent(__proc.haszombies);
    } else {
      // transitions process to zombie state
      // wait4 is responsible for reaping it
      pr->status = PROC_ZOMBIE;
      dll_remove(&__proc.list, &pr->elem);
      dll_make_first(&__proc.zombies, &pr->elem);
      SetEvent(__proc.haszombies);
      if (!pr->waiters && !__proc.waiters) {
        if (WIFSIGNALED(dwExitCode)) {
          sic = CLD_KILLED;
        } else {
          sic = CLD_EXITED;
        }
      }
    }
  }
  return sic;
}

textwindows dontinstrument static uint32_t __proc_worker(void *arg) {
  struct CosmoTib tls;
  char *sp = __builtin_frame_address(0);
  __bootstrap_tls(&tls, sp);
  STRACE("__proc_worker() started");
  __maps_lock();
  __maps_track(
      (char *)(((uintptr_t)sp + __pagesize - 1) & -__pagesize) - STACK_SIZE,
      STACK_SIZE, PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANONYMOUS | MAP_NOFORK);
  __maps_unlock();
  for (;;) {
    __proc_lock();

    // free undead processes
    struct Dll *e, *e2;
    for (e = dll_first(__proc.undead); e; e = e2) {
      struct Proc *pr = PROC_CONTAINER(e);
      e2 = dll_next(__proc.undead, e);
      // once a process has become live, this worker thread is the only
      // thing that can transition it from the undead to free state and
      // then close its handles, so long as wait4() isn't waiting on it
      if (!pr->waiters) {
        dll_remove(&__proc.undead, &pr->elem);
        dll_make_first(&__proc.free, &pr->elem);
        CloseHandle(pr->hProcess);
        CloseHandle(pr->hStopEvent);
        if (pr->hProcess2)
          CloseHandle(pr->hProcess2);
      }
    }

    // assemble a group of processes to wait on. if more than 62
    // children exist, then we'll use a small timeout and select
    // processes with a shifting window via a double linked list
    uint32_t n = 0;
    int64_t handles[64];
    struct Dll *plucked = 0;
    struct Proc *objects[64];
    for (e = dll_first(__proc.list); e && n < 62; e = e2) {
      struct Proc *pr = PROC_CONTAINER(e);
      e2 = dll_next(__proc.list, e);
      // cycle process to end of list
      dll_remove(&__proc.list, e);
      dll_make_last(&plucked, e);
      // don't bother waiting if it's already awaited
      if (!pr->waiters) {
        handles[n] = pr->hProcess;
        objects[n] = pr;
        ++n;
      }
    }
    dll_make_last(&__proc.list, plucked);
    __proc_unlock();

    // wait for something to happen
    handles[n++] = __proc.hStopChurn;
    handles[n++] = __proc.hLifeChurn;
    uint32_t millis = n == 64 ? POLL_INTERVAL_MS : -1u;
    uint32_t i = __imp_WaitForMultipleObjects(n, handles, false, millis);
    if (i == -1u) {
      // TODO(jart): why can this flake with kNtErrorInvalidHandle? even
      //             if we defer vfork() CloseHandle above it'll happen!
      //             the good news is it seems ephemeral when it happens
      STRACE("proc worker wait failed %d", GetLastError());
      continue;
    }

    // not sure the correct thing to do here
    if (i & kNtWaitAbandoned) {
      i &= ~kNtWaitAbandoned;
      STRACE("proc %u handle %ld abandoned", i, handles[i]);
    }

    // check if we need to busy loop due to >64 processes
    if (i == kNtWaitTimeout)
      continue;

    // check if fork or vfork woke us by adding a new living process, or
    // wait4() woke us, because there is new undead processes to harvest
    if (handles[i] == __proc.hLifeChurn)
      continue;

    // lock process manager again
    int sic = 0;
    __proc_lock();

    if (handles[i] == __proc.hStopChurn) {
      // churn can also happen if a child process stopped itself because
      // this check is expensive, we only do it on process manager churn
      for (e = dll_first(__proc.list); e; e = dll_next(__proc.list, e))
        sic |= __proc_checkstop(PROC_CONTAINER(e));
    } else {
      // otherwise it must mean a process died
      switch (objects[i]->status) {
        case PROC_ALIVE:
        case PROC_STOPPED:
          sic = __proc_harvest(objects[i], false);
          break;
        case PROC_ZOMBIE:
        case PROC_UNDEAD:
          // these could happen if wait4() started waiting on a process,
          // after we did and then won the race to transition the state.
          // we can ignore this since it's no longer in the __proc.list.
          break;
        default:
          __builtin_unreachable();
      }
    }

    // unlock process manager
    __proc_unlock();

    // generate process-wide signal if no one's waiting
    if (sic)
      __sig_generate(SIGCHLD, sic);
  }
  return 0;
}

/**
 * Lazy initializes process tracker data structures and worker.
 */
textwindows static void __proc_setup(void) {
  __proc.hStopChurn = CreateEvent(&kNtIsInheritable, 0, 0, 0);  // auto reset
  __proc.hLifeChurn = CreateEvent(0, 0, 0, 0);                  // auto reset
  __proc.haszombies = CreateEvent(0, 1, 0, 0);                  // manual reset
  __proc.hasstopped = CreateEvent(0, 1, 0, 0);                  // manual reset
  __proc.thread = CreateThread(0, STACK_SIZE, __proc_worker, 0,
                               kNtStackSizeParamIsAReservation, 0);
}

/**
 * Locks process tracker.
 */
textwindows void __proc_lock(void) {
  cosmo_once(&__proc.once, __proc_setup);
  pthread_mutex_lock(&__proc_lock_obj);
}

/**
 * Unlocks process tracker.
 */
textwindows void __proc_unlock(void) {
  pthread_mutex_unlock(&__proc_lock_obj);
}

/**
 * Returns handle of proc worker thread.
 */
textwindows intptr_t __proc_worker_handle(void) {
  cosmo_once(&__proc.once, __proc_setup);
  return __proc.thread;
}

/**
 * Resets process tracker from forked child.
 */
textwindows void __proc_wipe_and_reset(void) {
  // TODO(jart): Should we preserve this state in forked children?
  pthread_mutex_wipe_np(&__proc_lock_obj);
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
  if ((e = dll_first(__proc.free))) {
    proc = PROC_CONTAINER(e);
    dll_remove(&__proc.free, &proc->elem);
  }
  if (!proc && !(proc = HeapAlloc(GetProcessHeap(), 0, sizeof(struct Proc))))
    return 0;
  bzero(proc, sizeof(*proc));
  dll_init(&proc->elem);
  dll_init(&proc->stopelem);
  return proc;
}

/**
 * Adds process to active list.
 *
 * The handle and pid must be filled in before calling this.
 */
textwindows void __proc_add(struct Proc *proc) {
  dll_make_first(&__proc.list, &proc->elem);
  SetEvent(__proc.hLifeChurn);
}

// returns owned handle of direct child process
// this is intended for the __proc_handle() implementation
textwindows int64_t __proc_search(int pid) {
  struct Dll *e;
  int64_t handle = 0;
  BLOCK_SIGNALS;
  __proc_lock();
  // TODO(jart): we should increment a reference count when returning
  for (e = dll_first(__proc.list); e; e = dll_next(__proc.list, e)) {
    if (pid == PROC_CONTAINER(e)->pid) {
      handle = PROC_CONTAINER(e)->hProcess;
      break;
    }
  }
  __proc_unlock();
  ALLOW_SIGNALS;
  return handle;
}

#endif /* __x86_64__ */
