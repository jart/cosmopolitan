/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/nt/accounting.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"

#define _CLOCK_REALTIME           0
#define _CLOCK_MONOTONIC          1
#define _CLOCK_REALTIME_COARSE    2
#define _CLOCK_BOOTTIME           3
#define _CLOCK_PROCESS_CPUTIME_ID 4
#define _CLOCK_THREAD_CPUTIME_ID  5

static struct {
  uint64_t base;
  uint64_t freq;
} g_winclock;

textwindows int sys_clock_gettime_nt(int clock, struct timespec *ts) {
  uint64_t t;
  struct NtFileTime ft, ftExit, ftUser, ftKernel, ftCreation;
  switch (clock) {
    case _CLOCK_REALTIME:
      if (ts) {
        GetSystemTimePreciseAsFileTime(&ft);
        *ts = FileTimeToTimeSpec(ft);
      }
      return 0;
    case _CLOCK_REALTIME_COARSE:
      if (ts) {
        GetSystemTimeAsFileTime(&ft);
        *ts = FileTimeToTimeSpec(ft);
      }
      return 0;
    case _CLOCK_MONOTONIC:
      if (ts) {
        QueryPerformanceCounter(&t);
        t = ((t - g_winclock.base) * 1000000000) / g_winclock.freq;
        *ts = timespec_fromnanos(t);
      }
      return 0;
    case _CLOCK_BOOTTIME:
      if (ts) {
        *ts = timespec_frommillis(GetTickCount64());
      }
      return 0;
    case _CLOCK_PROCESS_CPUTIME_ID:
      if (ts) {
        GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel,
                        &ftUser);
        *ts = WindowsDurationToTimeSpec(ReadFileTime(ftUser) +
                                        ReadFileTime(ftKernel));
      }
      return 0;
    case _CLOCK_THREAD_CPUTIME_ID:
      if (ts) {
        GetThreadTimes(GetCurrentThread(), &ftCreation, &ftExit, &ftKernel,
                       &ftUser);
        *ts = WindowsDurationToTimeSpec(ReadFileTime(ftUser) +
                                        ReadFileTime(ftKernel));
      }
      return 0;
    default:
      return -EINVAL;
  }
}

static textstartup void winclock_init() {
  if (IsWindows()) {
    QueryPerformanceCounter(&g_winclock.base);
    QueryPerformanceFrequency(&g_winclock.freq);
  }
}

const void *const winclock_ctor[] initarray = {
    winclock_init,
};
