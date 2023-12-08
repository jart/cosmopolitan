/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/events.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/proc/proc.internal.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

static textwindows int __proc_reap(struct Proc *pr, int *wstatus,
                                   struct rusage *opt_out_rusage) {
  if (wstatus) {
    *wstatus = pr->wstatus;
  }
  if (opt_out_rusage) {
    *opt_out_rusage = pr->ru;
  }
  dll_remove(&__proc.zombies, &pr->elem);
  if (dll_is_empty(__proc.zombies)) {
    ResetEvent(__proc.haszombies);
  }
  if (pr->waiters) {
    pr->status = PROC_UNDEAD;
    dll_make_first(&__proc.undead, &pr->elem);
  } else {
    dll_make_first(&__proc.free, &pr->elem);
    CloseHandle(pr->handle);
  }
  return pr->pid;
}

static textwindows int __proc_check(int pid, int *wstatus,
                                    struct rusage *opt_out_rusage) {
  struct Dll *e;
  for (e = dll_first(__proc.zombies); e; e = dll_next(__proc.zombies, e)) {
    struct Proc *pr = PROC_CONTAINER(e);
    if (pid == -1 || pid == pr->pid) {
      return __proc_reap(pr, wstatus, opt_out_rusage);
    }
  }
  return 0;
}

static textwindows int __proc_wait(int pid, int *wstatus, int options,
                                   struct rusage *rusage, sigset_t waitmask) {
  for (;;) {

    // check for signals and cancelation
    int sig, handler_was_called;
    if (_check_cancel() == -1) {
      return -1;
    }
    if (_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))) {
      handler_was_called = _weaken(__sig_relay)(sig, SI_KERNEL, waitmask);
      if (_check_cancel() == -1) {
        return -1;  // ECANCELED because SIGTHR was just handled
      }
      if (handler_was_called & SIG_HANDLED_NO_RESTART) {
        return eintr();  // a non-SA_RESTART handler was called
      }
    }

    // check for zombie to harvest
    __proc_lock();
  CheckForZombies:
    int rc = __proc_check(pid, wstatus, rusage);
    if (rc || (options & WNOHANG)) {
      __proc_unlock();
      return rc;
    }

    // there's no zombies left
    // check if there's any living processes
    if (dll_is_empty(__proc.list)) {
      __proc_unlock();
      return echild();
    }

    // get appropriate wait object
    // register ourself as waiting
    struct Proc *pr = 0;
    uintptr_t hWaitObject;
    if (pid == -1) {
      // wait for any status change
      hWaitObject = __proc.haszombies;
      ++__proc.waiters;
    } else {
      // wait on specific child
      for (struct Dll *e = dll_first(__proc.list); e;
           e = dll_next(__proc.list, e)) {
        pr = PROC_CONTAINER(e);
        if (pid == pr->pid) break;
      }
      if (pr) {
        // by making the waiter count non-zero, the proc daemon stops
        // being obligated to monitor this process. this means we may
        // need to assume responsibility later on for zombifying this
        ++pr->waiters;
        hWaitObject = pr->handle;
      } else {
        __proc_unlock();
        return echild();
      }
    }
    __proc_unlock();

    // perform blocking operation
    uint32_t wi;
    uintptr_t sem;
    struct PosixThread *pt = _pthread_self();
    pt->pt_blkmask = waitmask;
    pt->pt_semaphore = sem = CreateSemaphore(0, 0, 1, 0);
    atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_SEM,
                          memory_order_release);
    wi = WaitForMultipleObjects(2, (intptr_t[2]){hWaitObject, sem}, 0, -1u);
    atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
    CloseHandle(sem);

    // log warning if handle unexpectedly closed
    if (wi & kNtWaitAbandoned) {
      wi &= ~kNtWaitAbandoned;
      STRACE("wait4 abandoned %u", wi);
    }

    // check for wait() style wakeup
    __proc_lock();
    if (!wi && !pr) {
      --__proc.waiters;
      goto CheckForZombies;
    }

    // check if killed or win32 error
    if (wi) {
      if (pr) {
        if (!--pr->waiters && pr->status == PROC_UNDEAD) {
          __proc_free(pr);
        }
      } else {
        --__proc.waiters;
      }
      __proc_unlock();
      if (wi == 1) {
        // __sig_cancel() woke our semaphore
        continue;
      } else {
        // neither posix or win32 define i/o error conditions for
        // generic wait. failure should only be due to api misuse
        return einval();
      }
    }

    // handle process exit notification
    --pr->waiters;
    if (pr->status == PROC_ALIVE) {
      __proc_harvest(pr, true);
    }
    switch (pr->status) {
      case PROC_ALIVE:
        // exit caused by execve() reparenting
        __proc_unlock();
        if (!pr->waiters) {
          // avoid deadlock that could theoretically happen
          SetEvent(__proc.onbirth);
        }
        break;
      case PROC_ZOMBIE:
        // exit happened and we're the first to know
        rc = __proc_reap(pr, wstatus, rusage);
        __proc_unlock();
        return rc;
      case PROC_UNDEAD:
        // exit happened but another thread waited first
        if (!pr->waiters) {
          __proc_free(pr);
        }
        __proc_unlock();
        return echild();
      default:
        __builtin_unreachable();
    }
  }
}

textwindows int sys_wait4_nt(int pid, int *opt_out_wstatus, int options,
                             struct rusage *opt_out_rusage) {
  // no support for WCONTINUED and WUNTRACED yet
  if (options & ~WNOHANG) return einval();
  // XXX: NT doesn't really have process groups. For instance the
  //      CreateProcess() flag for starting a process group actually
  //      just does an "ignore ctrl-c" internally.
  if (pid == 0) pid = -1;
  if (pid < -1) pid = -pid;
  sigset_t m = __sig_block();
  int rc = __proc_wait(pid, opt_out_wstatus, options, opt_out_rusage,
                       m | 1ull << (SIGCHLD - 1));
  __sig_unblock(m);
  return rc;
}

#endif /* __x86_64__ */
