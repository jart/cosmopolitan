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
#include "libc/calls/bo.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/timespec.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
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
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"

#ifdef __x86_64__

static textwindows void GetProcessStats(int64_t h, struct rusage *ru) {
  bzero(ru, sizeof(*ru));
  struct NtProcessMemoryCountersEx memcount = {sizeof(memcount)};
  unassert(GetProcessMemoryInfo(h, &memcount, sizeof(memcount)));
  ru->ru_maxrss = memcount.PeakWorkingSetSize / 1024;
  ru->ru_majflt = memcount.PageFaultCount;
  struct NtFileTime createtime, exittime;
  struct NtFileTime kerneltime, usertime;
  unassert(GetProcessTimes(h, &createtime, &exittime, &kerneltime, &usertime));
  ru->ru_utime = WindowsDurationToTimeVal(ReadFileTime(usertime));
  ru->ru_stime = WindowsDurationToTimeVal(ReadFileTime(kerneltime));
  struct NtIoCounters iocount;
  unassert(GetProcessIoCounters(h, &iocount));
  ru->ru_inblock = iocount.ReadOperationCount;
  ru->ru_oublock = iocount.WriteOperationCount;
}

static textwindows struct timespec GetNextDeadline(struct timespec deadline) {
  if (__tls_enabled && __get_tls()->tib_sigmask == -1) return timespec_max;
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
    GetProcessStats(pr->handle, opt_out_rusage);
  }
  if (!pr->waiters) {
    CloseHandle(pr->handle);
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

static textwindows int WaitForProcess(int pid, int *wstatus, int options,
                                      struct rusage *opt_out_rusage) {
  int rc, *wv;
  nsync_cv *cv;
  struct Dll *e;
  struct Proc *pr;
  struct timespec deadline = timespec_zero;

  // check list of processes that've already exited
  if ((rc = CheckZombies(pid, wstatus, opt_out_rusage))) {
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
CheckForInterrupt:
  if (_check_interrupts(kSigOpRestartable) == -1) return -1;
  deadline = GetNextDeadline(deadline);
SpuriousWakeup:
  BEGIN_BLOCKING_OPERATION;
  ++*wv;
  rc = nsync_cv_wait_with_deadline(cv, &__proc.lock, deadline, 0);
  --*wv;
  END_BLOCKING_OPERATION;
  if (rc == ECANCELED) return ecanceled();
  if (pr && pr->iszombie) return ReapZombie(pr, wstatus, opt_out_rusage);
  if (rc == ETIMEDOUT) goto CheckForInterrupt;
  unassert(!rc);
  if (!pr && (rc = CheckZombies(pid, wstatus, opt_out_rusage))) return rc;
  goto SpuriousWakeup;
}

textwindows int sys_wait4_nt(int pid, int *opt_out_wstatus, int options,
                             struct rusage *opt_out_rusage) {
  int rc;
  if (options & ~WNOHANG) {
    return einval();  // no support for WCONTINUED and WUNTRACED yet
  }
  // XXX: NT doesn't really have process groups. For instance the
  //      CreateProcess() flag for starting a process group actually
  //      just does an "ignore ctrl-c" internally.
  if (pid == 0) pid = -1;
  if (pid < -1) pid = -pid;
  __proc_lock();
  rc = WaitForProcess(pid, opt_out_wstatus, options, opt_out_rusage);
  __proc_unlock();
  return rc;
}

#endif /* __x86_64__ */
