/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/rusage.h"
#include "libc/conv/conv.h"
#include "libc/nt/accounting.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"

textwindows int wait4$nt(int pid, int *opt_out_wstatus, int options,
                         struct rusage *opt_out_rusage) {
  uint32_t dwExitCode;
  struct NtFileTime createfiletime, exitfiletime, kernelfiletime, userfiletime;
  if (!isfdkind(pid, kFdProcess)) return esrch();
  for (;;) {
    dwExitCode = kNtStillActive;
    if (!(options & WNOHANG)) {
      WaitForSingleObject(g_fds.p[pid].handle, 0xffffffff);
    }
    if (GetExitCodeProcess(g_fds.p[pid].handle, &dwExitCode)) {
      if (dwExitCode != kNtStillActive) {
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
        return pid;
      } else if (options & WNOHANG) {
        return pid;
      } else {
        continue;
      }
    } else {
      return winerr();
    }
  }
}
