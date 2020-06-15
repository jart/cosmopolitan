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
#include "libc/dce.h"
#include "libc/nt/accounting.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/thread.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/sysv/errfuns.h"

static textwindows noinline int getrusage$nt(int who, struct rusage *usage) {
  struct NtFileTime CreationFileTime;
  struct NtFileTime ExitFileTime;
  struct NtFileTime KernelFileTime;
  struct NtFileTime UserFileTime;
  memset(usage, 0, sizeof(*usage));
  if ((who == RUSAGE_SELF ? GetProcessTimes : GetThreadTimes)(
          (who == RUSAGE_SELF ? GetCurrentProcess : GetCurrentThread)(),
          &CreationFileTime, &ExitFileTime, &KernelFileTime, &UserFileTime)) {
    filetimetotimeval(&usage->ru_utime, UserFileTime);
    filetimetotimeval(&usage->ru_stime, KernelFileTime);
    return 0;
  } else {
    return winerr();
  }
}

/**
 * Returns resource usage statistics.
 *
 * @param who can be RUSAGE_{SELF,CHILDREN,THREAD}
 * @return 0 on success, or -1 w/ errno
 */
int getrusage(int who, struct rusage *usage) {
  if (who == 99) return enosys(); /* @see libc/sysv/consts.sh */
  if (!usage) return efault();
  if (!IsWindows()) {
    return getrusage$sysv(who, usage);
  } else {
    return getrusage$nt(who, usage);
  }
}
