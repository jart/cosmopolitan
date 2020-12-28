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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/rusage.h"
#include "libc/fmt/conv.h"
#include "libc/macros.h"
#include "libc/nt/accounting.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"

textwindows int wait4$nt(int pid, int *opt_out_wstatus, int options,
                         struct rusage *opt_out_rusage) {
  int pids[64];
  int64_t handles[64];
  uint32_t dwExitCode;
  uint32_t i, count, timeout;
  struct NtFileTime createfiletime, exitfiletime, kernelfiletime, userfiletime;
  if (pid != -1) {
    if (!__isfdkind(pid, kFdProcess)) {
      return echild();
    }
    handles[0] = g_fds.p[pid].handle;
    pids[0] = pid;
    count = 1;
  } else {
    for (count = 0, i = g_fds.n; i--;) {
      if (g_fds.p[i].kind == kFdProcess) {
        pids[count] = i;
        handles[count] = g_fds.p[i].handle;
        if (++count == ARRAYLEN(handles)) break;
      }
    }
    if (!count) {
      return echild();
    }
  }
  for (;;) {
    dwExitCode = kNtStillActive;
    if (options & WNOHANG) {
      i = WaitForMultipleObjects(count, handles, false, 0);
      if (i == kNtWaitTimeout) return 0;
    } else {
      i = WaitForMultipleObjects(count, handles, false, -1);
    }
    if (i == kNtWaitFailed) return __winerr();
    if (!GetExitCodeProcess(handles[i], &dwExitCode)) return __winerr();
    if (dwExitCode == kNtStillActive) continue;
    if (opt_out_wstatus) { /* @see WEXITSTATUS() */
      *opt_out_wstatus = (dwExitCode & 0xff) << 8;
    }
    if (opt_out_rusage) {
      memset(opt_out_rusage, 0, sizeof(*opt_out_rusage));
      GetProcessTimes(GetCurrentProcess(), &createfiletime, &exitfiletime,
                      &kernelfiletime, &userfiletime);
      FileTimeToTimeVal(&opt_out_rusage->ru_utime, userfiletime);
      FileTimeToTimeVal(&opt_out_rusage->ru_stime, kernelfiletime);
    }
    CloseHandle(g_fds.p[pids[i]].handle);
    g_fds.p[pids[i]].kind = kFdEmpty;
    return pids[i];
  }
}
