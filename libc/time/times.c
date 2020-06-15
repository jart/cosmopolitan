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
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/tms.h"
#include "libc/conv/conv.h"
#include "libc/dce.h"
#include "libc/nt/accounting.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/filetime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/time/time.h"

/**
 * Returns accounting data for process on time-sharing system.
 */
long times(struct tms *out_times) {
  struct timeval tv;
  long tick = sysconf(_SC_CLK_TCK);
  if (!IsWindows()) {
    struct rusage ru;
    if (getrusage(RUSAGE_SELF, &ru) == -1) return -1;
    out_times->tms_utime = convertmicros(&ru.ru_utime, tick);
    out_times->tms_stime = convertmicros(&ru.ru_stime, tick);
    if (getrusage(RUSAGE_CHILDREN, &ru) == -1) return -1;
    out_times->tms_cutime = convertmicros(&ru.ru_utime, tick);
    out_times->tms_cstime = convertmicros(&ru.ru_stime, tick);
  } else {
    struct NtFileTime CreationFileTime;
    struct NtFileTime ExitFileTime;
    struct NtFileTime KernelFileTime;
    struct NtFileTime UserFileTime;
    if (!GetProcessTimes(GetCurrentProcess(), &CreationFileTime, &ExitFileTime,
                         &KernelFileTime, &UserFileTime)) {
      return winerr();
    }
    filetimetotimeval(&tv, UserFileTime);
    out_times->tms_utime = convertmicros(&tv, tick);
    filetimetotimeval(&tv, KernelFileTime);
    out_times->tms_stime = convertmicros(&tv, tick);
    out_times->tms_cutime = 0;
    out_times->tms_cstime = 0;
  }
  if (gettimeofday(&tv, NULL) == -1) return -1;
  return convertmicros(&tv, tick);
}
