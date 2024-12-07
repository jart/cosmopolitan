/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/enum/threadpriority.h"
#include "libc/nt/ntdll.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/nt/windows.h"

#define MAXIMUM    1e8
#define ITERATIONS 10

const char *MyDescribeClockName(int clock) {
  if (clock == CLOCK_REALTIME)
    return "CLOCK_REALTIME";
  if (clock == CLOCK_MONOTONIC)
    return "CLOCK_MONOTONIC";
  if (clock == CLOCK_BOOTTIME)
    return "CLOCK_BOOTTIME";
  if (clock == CLOCK_REALTIME_COARSE)
    return "CLOCK_REALTIME_COARSE";
  if (clock == CLOCK_MONOTONIC_COARSE)
    return "CLOCK_MONOTONIC_COARSE";
  __builtin_trap();
}

void TestSleepRelative(int clock) {
  printf("\n");
  printf("testing: clock_nanosleep(%s) with relative timeout\n",
         MyDescribeClockName(clock));
  for (long nanos = 1; nanos < (long)MAXIMUM; nanos *= 4) {
    struct timespec t1, t2, wf;
    wf = timespec_fromnanos(nanos);
    if (clock_gettime(clock, &t1))
      return;
    for (int i = 0; i < ITERATIONS; ++i) {
      unassert(!clock_nanosleep(clock, 0, &wf, 0));
    }
    clock_gettime(clock, &t2);
    long took = timespec_tonanos(timespec_sub(t2, t1)) / ITERATIONS;
    printf("%,12ld ns sleep took %,12ld ns delta %,12ld ns\n", nanos, took,
           took - nanos);
  }
}

int main(int argc, char *argv[]) {
  TestSleepRelative(CLOCK_REALTIME);
  TestSleepRelative(CLOCK_REALTIME_COARSE);
  TestSleepRelative(CLOCK_MONOTONIC);
  TestSleepRelative(CLOCK_BOOTTIME);
  TestSleepRelative(CLOCK_MONOTONIC_COARSE);
}
