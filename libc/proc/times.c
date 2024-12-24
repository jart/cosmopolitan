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
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/tms.h"
#include "libc/runtime/clktck.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/rusage.h"

static long MicrosToTicks(struct timeval tv) {
  return tv.tv_sec * CLK_TCK + tv.tv_usec / (1000000 / CLK_TCK);
}

static long NanosToTicks(struct timespec ts) {
  return ts.tv_sec * CLK_TCK + ts.tv_nsec / (1000000000 / CLK_TCK);
}

/**
 * Returns accounting data for process on time-sharing system.
 * @return number of `CLK_TCK` from `CLOCK_BOOTTIME` epoch
 */
long times(struct tms *out_times) {
  struct timespec bt;
  struct rusage rus, ruc;
  if (getrusage(RUSAGE_SELF, &rus))
    return -1;
  if (getrusage(RUSAGE_CHILDREN, &ruc))
    return -1;
  if (clock_gettime(CLOCK_BOOTTIME, &bt))
    return -1;
  out_times->tms_utime = MicrosToTicks(rus.ru_utime);
  out_times->tms_stime = MicrosToTicks(rus.ru_stime);
  out_times->tms_cutime = MicrosToTicks(ruc.ru_utime);
  out_times->tms_cstime = MicrosToTicks(ruc.ru_stime);
  return NanosToTicks(bt);
}
