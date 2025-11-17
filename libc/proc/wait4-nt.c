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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/events.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/proc/proc.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"
#if SupportsWindows()

__msabi extern typeof(WaitForMultipleObjects)
    *const __imp_WaitForMultipleObjects;

// turns some win32 wait statuses into unix style ones
textwindows static int __proc_wstatus(uint32_t dwExitCode) {
  switch (dwExitCode) {
    case kNtStatusControlCExit:
      return SIGINT;
    case kNtStatusStackOverflow:
    case kNtStatusAccessViolation:
    case kNtStatusGuardPageViolation:
      return SIGSEGV;
    case kNtStatusInPageError:
      return SIGBUS;
    case kNtStatusIllegalInstruction:
    case kNtStatusPrivilegedInstruction:
      return SIGILL;
    case kNtStatusBreakpoint:
      return SIGTRAP;
    case kNtStatusIntegerOverflow:
    case kNtStatusFloatDivideByZero:
    case kNtStatusFloatOverflow:
    case kNtStatusFloatUnderflow:
    case kNtStatusFloatInexactResult:
    case kNtStatusFloatDenormalOperand:
    case kNtStatusFloatInvalidOperation:
    case kNtStatusFloatStackCheck:
    case kNtStatusIntegerDivideBYZero:
      return SIGFPE;
    case kNtStatusDllNotFound:
    case kNtStatusDllInitFailed:
    case kNtStatusOrdinalNotFound:
    case kNtStatusEntrypointNotFound:
      return SIGSYS;
    case kNtStatusAssertionFailure:
      return SIGABRT;
    default:
      return dwExitCode;
  }
}

textwindows static int __proc_unstop(struct Proc *pr, int *wstatus,
                                     struct rusage *opt_out_rusage) {
  if (wstatus)
    *wstatus = __proc_wstatus(pr->dwExitCode);
  if (opt_out_rusage)
    *opt_out_rusage = (struct rusage){0};
  dll_remove(&__proc.stopped, &pr->stopelem);
  if (dll_is_empty(__proc.stopped))
    ResetEvent(__proc.hasstopped);
  ResetEvent(pr->hStopEvent);
  pr->status = PROC_ALIVE;
  return pr->pid;
}

textwindows static int __proc_reap_zombie(struct Proc *pr, int *wstatus,
                                          struct rusage *opt_out_rusage) {
  if (wstatus)
    *wstatus = __proc_wstatus(pr->dwExitCode);
  if (opt_out_rusage)
    *opt_out_rusage = pr->ru;
  dll_remove(&__proc.zombies, &pr->elem);
  pr->status = PROC_UNDEAD;
  dll_make_first(&__proc.undead, &pr->elem);
  SetEvent(__proc.hLifeChurn);
  return pr->pid;
}

textwindows static int __proc_check(int pid, int *wstatus, int options,
                                    struct rusage *opt_out_rusage) {
  struct Dll *e;
  if (options & WUNTRACED) {
    for (e = dll_first(__proc.stopped); e; e = dll_next(__proc.stopped, e)) {
      struct Proc *pr = STOP_PROC_CONTAINER(e);
      if (pid == -1 || pid == pr->pid)
        return __proc_unstop(pr, wstatus, opt_out_rusage);
    }
  }
  for (e = dll_first(__proc.zombies); e; e = dll_next(__proc.zombies, e)) {
    struct Proc *pr = PROC_CONTAINER(e);
    if (pid == -1 || pid == pr->pid) {
      int rc = __proc_reap_zombie(pr, wstatus, opt_out_rusage);
      if (!(__get_pib()->sighandrvas[SIGCHLD - 1] == (uintptr_t)SIG_IGN ||
            (__get_pib()->sighandflags[SIGCHLD - 1] & SA_NOCLDWAIT)))
        return rc;
    }
  }
  return 0;
}

textwindows static int __proc_wait(int pid, int *wstatus, int options,
                                   struct rusage *rusage, sigset_t waitmask) {
  for (;;) {

    // check for signals and cancelation
    int sig, handler_was_called;
    if (_check_cancel() == -1)
      return -1;
    if (_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))) {
      handler_was_called = _weaken(__sig_relay)(sig, SI_KERNEL, waitmask);
      if (_check_cancel() == -1)
        return -1;  // ECANCELED because SIGTHR was just handled
      if (handler_was_called & SIG_HANDLED_NO_RESTART)
        return eintr();  // a non-SA_RESTART handler was called
    }

    // check for zombie to harvest
    __proc_lock();
  CheckForZombies:
    int rc = __proc_check(pid, wstatus, options, rusage);

    // if there's no zombies left
    // check if there's any living processes
    if (!rc && dll_is_empty(__proc.list)) {
      __proc_unlock();
      return echild();
    }

    // otherwise return zombie or zero
    if (rc || (options & WNOHANG)) {
      __proc_unlock();
      return rc;
    }

    // get appropriate wait object
    // register ourself as waiting
    int handcount = 2;
    intptr_t hands[3];
    struct Proc *pr = 0;
    if (pid == -1) {
      // wait for any status change
      hands[0] = __proc.haszombies;
      ++__proc.waiters;
      if (options & WUNTRACED) {
        ++__proc.stopwaiters;
        hands[handcount++] = __proc.hasstopped;
      }
    } else {
      // wait on specific child
      for (struct Dll *e = dll_first(__proc.list); e;
           e = dll_next(__proc.list, e)) {
        pr = PROC_CONTAINER(e);
        if (pid == pr->pid)
          break;
      }
      if (pr) {
        // by making the waiter count non-zero, the proc daemon stops
        // being obligated to monitor this process. this means we may
        // need to assume responsibility later on for zombifying this
        ++pr->waiters;
        hands[0] = pr->hProcess;
        if (options & WUNTRACED) {
          ++pr->stopwaiters;
          hands[handcount++] = pr->hStopEvent;
        }
      } else {
        __proc_unlock();
        return echild();
      }
    }
    __proc_unlock();

    // perform blocking operation
    uint32_t wi;
    if ((hands[1] = __interruptible_start(waitmask))) {
      wi = __imp_WaitForMultipleObjects(handcount, hands, 0, -1u);
      __interruptible_end();
    } else {
      wi = -1u;
    }

    // log warning if handle unexpectedly closed
    if (wi & kNtWaitAbandoned) {
      wi &= ~kNtWaitAbandoned;
      STRACE("wait4 abandoned %u", wi);
    }

    // lock again
    __proc_lock();

    // clear our waiter status
    if (pr) {
      --pr->waiters;
      if (options & WUNTRACED)
        --pr->stopwaiters;
    } else {
      --__proc.waiters;
      if (options & WUNTRACED)
        --__proc.stopwaiters;
    }

    // handle weird case that can happen with auto zombie reaping
    if (wi == 0) {
      if (dll_is_empty(__proc.zombies))
        ResetEvent(__proc.haszombies);
      if (pr && pr->status == PROC_UNDEAD) {
        __proc_unlock();
        return echild();
      }
    }

    // check for wait() style wakeup
    if (!pr && (wi == 0 || wi == 2))
      goto CheckForZombies;

    // check if killed or win32 error
    if (wi == 1 || wi == -1u) {
      __proc_unlock();
      if (wi == 1) {
        // __sig_wake() woke our semaphore
        continue;
      } else {
        // neither posix or win32 define i/o error conditions for
        // generic wait. failure should only be due to api misuse
        return einval();
      }
    }

    // handle process exit notification
    if (pr->status == PROC_ALIVE || pr->status == PROC_STOPPED)
      __proc_harvest(pr, true);
    switch (pr->status) {
      case PROC_ALIVE:
        // exit caused by execve() reparenting
        __proc_unlock();
        break;
      case PROC_ZOMBIE:
        // exit happened and we're the first to know
        rc = __proc_reap_zombie(pr, wstatus, rusage);
        __proc_unlock();
        return rc;
      case PROC_UNDEAD:
        // this could happen because (1) exit happened but another
        // thread waited first, or (2) SIG_IGN SIGCHLD or SA_NOCLDSTOP
        // is in play and __proc_harvest() refiled the process as undead
        __proc_unlock();
        return echild();
      case PROC_STOPPED:
        rc = __proc_unstop(pr, wstatus, rusage);
        __proc_unlock();
        if (options & WUNTRACED)
          return rc;
        break;
      default:
        __builtin_unreachable();
    }
  }
}

textwindows int sys_wait4_nt(int pid, int *opt_out_wstatus, int options,
                             struct rusage *opt_out_rusage) {
  // no support for WCONTINUED yet
  if (options & ~(WNOHANG | WUNTRACED))
    return einval();
  // XXX: NT doesn't really have process groups. For instance the
  //      CreateProcess() flag for starting a process group actually
  //      just does an "ignore ctrl-c" internally.
  if (pid == 0)
    pid = -1;
  if (pid < -1)
    pid = -pid;
  sigset_t m = __sig_block();
  int rc = __proc_wait(pid, opt_out_wstatus, options, opt_out_rusage,
                       m | 1ull << (SIGCHLD - 1));
  __sig_unblock(m);
  return rc;
}

#endif /* __x86_64__ */
