/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/accounting.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/processmemorycounters.h"
#include "libc/proc/proc.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

static textwindows struct timespec GetNextDeadline(struct timespec deadline) {
  if (timespec_iszero(deadline)) deadline = timespec_real();
  struct timespec delay = timespec_frommillis(__SIG_PROC_INTERVAL_MS);
  return timespec_add(deadline, delay);
}

static textwindows int ReapZombie(struct Proc *pr, int *wstatus,
                                  struct rusage *opt_out_rusage) {
  if (wstatus) {
    *wstatus = pr->wstatus;
  }
  if (opt_out_rusage) {
    *opt_out_rusage = pr->ru;
  }
  if (!pr->waiters) {
    dll_remove(&__proc.zombies, &pr->elem);
    dll_make_first(&__proc.free, &pr->elem);
  }
  return pr->pid;
}

static textwindows int CheckZombies(int pid, int *wstatus,
                                    struct rusage *opt_out_rusage) {
  struct Dll *e;
  for (e = dll_first(__proc.zombies); e; e = dll_next(__proc.zombies, e)) {
    struct Proc *pr = PROC_CONTAINER(e);
    if (pid == -1 && pr->waiters) {
      continue;  // this zombie has been claimed
    }
    if (pid == -1 || pid == pr->pid) {
      return ReapZombie(pr, wstatus, opt_out_rusage);
    }
  }
  return 0;
}

static textwindows void UnwindWaiterCount(void *arg) {
  int *waiters = arg;
  --*waiters;
}

static textwindows int WaitForProcess(int pid, int *wstatus, int options,
                                      struct rusage *rusage,
                                      uint64_t waitmask) {
  uint64_t m;
  int rc, *wv;
  nsync_cv *cv;
  struct Dll *e;
  struct Proc *pr;
  struct timespec deadline = timespec_zero;

  // check list of processes that've already exited
  if ((rc = CheckZombies(pid, wstatus, rusage))) {
    return rc;
  }

  // find the mark
  pr = 0;
  if (pid == -1) {
    if (dll_is_empty(__proc.list)) {
      return echild();
    }
    cv = &__proc.onexit;
    wv = &__proc.waiters;
  } else {
    for (e = dll_first(__proc.list); e; e = dll_next(__proc.list, e)) {
      if (pid == PROC_CONTAINER(e)->pid) {
        pr = PROC_CONTAINER(e);
      }
    }
    if (pr) {
      unassert(!pr->iszombie);
      cv = &pr->onexit;
      wv = &pr->waiters;
    } else {
      return echild();
    }
  }

  // wait for status change
  if (options & WNOHANG) return 0;
WaitMore:
  deadline = GetNextDeadline(deadline);
SpuriousWakeup:
  ++*wv;
  pthread_cleanup_push(UnwindWaiterCount, wv);
  m = __sig_beginwait(waitmask);
  if ((rc = _check_signal(true)) != -1) {
    rc = nsync_cv_wait_with_deadline(cv, &__proc.lock, deadline, 0);
  }
  __sig_finishwait(m);
  pthread_cleanup_pop(true);
  if (rc == -1) return -1;
  if (rc == ETIMEDOUT) goto WaitMore;
  if (rc == ECANCELED) return ecanceled();
  if (pr && pr->iszombie) return ReapZombie(pr, wstatus, rusage);
  unassert(!rc);  // i have to follow my dreams however crazy they seem
  if (!pr && (rc = CheckZombies(pid, wstatus, rusage))) return rc;
  goto SpuriousWakeup;
}

textwindows int sys_wait4_nt(int pid, int *opt_out_wstatus, int options,
                             struct rusage *opt_out_rusage) {
  int rc;
  uint64_t m;
  // no support for WCONTINUED and WUNTRACED yet
  if (options & ~WNOHANG) return einval();
  // XXX: NT doesn't really have process groups. For instance the
  //      CreateProcess() flag for starting a process group actually
  //      just does an "ignore ctrl-c" internally.
  if (pid == 0) pid = -1;
  if (pid < -1) pid = -pid;
  m = __sig_block();
  __proc_lock();
  pthread_cleanup_push((void *)__proc_unlock, 0);
  rc = WaitForProcess(pid, opt_out_wstatus, options, opt_out_rusage,
                      m | 1ull << (SIGCHLD - 1));
  pthread_cleanup_pop(true);
  __sig_unblock(m);
  return rc;
}

#endif /* __x86_64__ */
