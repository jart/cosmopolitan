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
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/accounting.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/enum/th32cs.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/processentry32.h"
#include "libc/nt/struct/processmemorycounters.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/lcg.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"

#ifdef __x86_64__

static textwindows void AddProcessStats(int64_t h, struct rusage *ru) {
  struct NtProcessMemoryCountersEx memcount = {
      .cb = sizeof(struct NtProcessMemoryCountersEx)};
  if (GetProcessMemoryInfo(h, &memcount, sizeof(memcount))) {
    ru->ru_maxrss = MAX(ru->ru_maxrss, memcount.PeakWorkingSetSize / 1024);
    ru->ru_majflt += memcount.PageFaultCount;
  } else {
    STRACE("%s failed %u", "GetProcessMemoryInfo", GetLastError());
  }
  struct NtFileTime createfiletime, exitfiletime;
  struct NtFileTime kernelfiletime, userfiletime;
  if (GetProcessTimes(h, &createfiletime, &exitfiletime, &kernelfiletime,
                      &userfiletime)) {
    ru->ru_utime = timeval_add(
        ru->ru_utime, WindowsDurationToTimeVal(ReadFileTime(userfiletime)));
    ru->ru_stime = timeval_add(
        ru->ru_stime, WindowsDurationToTimeVal(ReadFileTime(kernelfiletime)));
  } else {
    STRACE("%s failed %u", "GetProcessTimes", GetLastError());
  }
}

static textwindows int sys_wait4_nt_impl(int *pid, int *opt_out_wstatus,
                                         int options,
                                         struct rusage *opt_out_rusage) {
  int64_t handle;
  int rc, pids[64];
  int64_t handles[64];
  uint32_t dwExitCode;
  uint32_t i, j, count;
  if (*pid != -1 && *pid != 0) {
    if (*pid < 0) {
      // XXX: this is sloppy
      *pid = -*pid;
    }
    if (!__isfdkind(*pid, kFdProcess)) {
      // XXX: this is sloppy (see fork-nt.c)
      if (!__isfdopen(*pid) &&
          (handle = OpenProcess(kNtSynchronize | kNtProcessQueryInformation,
                                true, *pid))) {
        if ((*pid = __reservefd_unlocked(-1)) != -1) {
          g_fds.p[*pid].kind = kFdProcess;
          g_fds.p[*pid].handle = handle;
          g_fds.p[*pid].flags = O_CLOEXEC;
        } else {
          CloseHandle(handle);
          return echild();
        }
      } else {
        return echild();
      }
    }
    handles[0] = g_fds.p[*pid].handle;
    pids[0] = *pid;
    count = 1;
  } else {
    count = __sample_pids(pids, handles, false);
    if (!count) {
      return echild();
    }
  }
  dwExitCode = kNtStillActive;
  if (options & WNOHANG) {
    i = WaitForMultipleObjects(count, handles, false, 0);
    if (i == kNtWaitTimeout) {
      return 0;
    }
  } else {
    i = WaitForMultipleObjects(count, handles, false,
                               __SIG_POLLING_INTERVAL_MS);
    if (i == kNtWaitTimeout) {
      return -2;
    }
  }
  if (i == kNtWaitFailed) {
    STRACE("%s failed %u", "WaitForMultipleObjects", GetLastError());
    return __winerr();
  }
  if (!GetExitCodeProcess(handles[i], &dwExitCode)) {
    STRACE("%s failed %u", "GetExitCodeProcess", GetLastError());
    return __winerr();
  }
  if (dwExitCode == kNtStillActive) {
    return -2;
  }
  if (dwExitCode == 0xc9af3d51u) {
    dwExitCode = kNtStillActive;
  }
  if (opt_out_wstatus) {
    *opt_out_wstatus = dwExitCode;
  }
  if (opt_out_rusage) {
    bzero(opt_out_rusage, sizeof(*opt_out_rusage));
    AddProcessStats(handles[i], opt_out_rusage);
  }
  CloseHandle(handles[i]);
  __releasefd(pids[i]);
  return pids[i];
}

textwindows int sys_wait4_nt(int pid, int *opt_out_wstatus, int options,
                             struct rusage *opt_out_rusage) {
  int rc;
  sigset_t oldmask, mask = {0};
  sigaddset(&mask, SIGCHLD);
  __sig_mask(SIG_BLOCK, &mask, &oldmask);
  do {
    rc = _check_interrupts(kSigOpRestartable | kSigOpNochld, 0);
    if (rc == -1) break;
    __fds_lock();
    rc = sys_wait4_nt_impl(&pid, opt_out_wstatus, options, opt_out_rusage);
    __fds_unlock();
  } while (rc == -2);
  __sig_mask(SIG_SETMASK, &oldmask, 0);
  return rc;
}

#endif /* __x86_64__ */
