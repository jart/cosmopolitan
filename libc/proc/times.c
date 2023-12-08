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
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/tms.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/nt/accounting.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/time/time.h"

static dontinline long ConvertMicros(struct timeval tv) {
  return tv.tv_sec * CLK_TCK + tv.tv_usec / (1000000 / CLK_TCK);
}

static dontinline long times2(struct tms *out_times, struct rusage *ru) {
  struct timeval tv;
  struct NtFileTime CreationTime, ExitTime, KernelTime, UserTime;
  if (!IsWindows()) {
    if (getrusage(RUSAGE_SELF, ru) == -1) return -1;
    out_times->tms_utime = ConvertMicros(ru->ru_utime);
    out_times->tms_stime = ConvertMicros(ru->ru_stime);
    if (getrusage(RUSAGE_CHILDREN, ru) == -1) return -1;
    out_times->tms_cutime = ConvertMicros(ru->ru_utime);
    out_times->tms_cstime = ConvertMicros(ru->ru_stime);
  } else {
    if (!GetProcessTimes(GetCurrentProcess(), &CreationTime, &ExitTime,
                         &KernelTime, &UserTime)) {
      return __winerr();
    }
    out_times->tms_utime = ReadFileTime(UserTime);
    out_times->tms_stime = ReadFileTime(KernelTime);
    out_times->tms_cutime = 0;
    out_times->tms_cstime = 0;
  }
  if (gettimeofday(&tv, NULL) == -1) return -1;
  return ConvertMicros(tv);
}

/**
 * Returns accounting data for process on time-sharing system.
 */
long times(struct tms *out_times) {
  struct rusage ru;
  return times2(out_times, &ru);
}
