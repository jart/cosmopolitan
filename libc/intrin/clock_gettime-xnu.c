/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/sysv/consts/clock.h"
#ifdef __x86_64__

int sys_clock_gettime_xnu(int clock, struct timespec *ts) {
  long ax, dx;
  if (clock == CLOCK_REALTIME) {
    // invoke the system call
    //
    //   int gettimeofday(struct timeval *tp,
    //                    struct timezone *tzp,
    //                    uint64_t *mach_absolute_time);
    //
    // as follows
    //
    //   ax, dx = gettimeofday(&ts, 0, 0);
    //
    // to support multiple calling conventions
    //
    //   1. new xnu returns *ts in memory via rdi
    //   2. old xnu returns *ts in rax:rdx regs
    //
    // we assume this system call always succeeds
    asm volatile("syscall"
                 : "=a"(ax), "=d"(dx)
                 : "0"(0x2000000 | 116), "D"(ts), "S"(0), "1"(0)
                 : "rcx", "r8", "r9", "r10", "r11", "memory");
    if (ax) {
      ts->tv_sec = ax;
      ts->tv_nsec = dx;
    }
    ts->tv_nsec *= 1000;
    return 0;
  } else if (clock == CLOCK_BOOTTIME ||   //
             clock == CLOCK_MONOTONIC ||  //
             clock == CLOCK_MONOTONIC_COARSE) {
    return sys_clock_gettime_mono(ts);
  } else {
    return -EINVAL;
  }
}

#endif /* __x86_64__ */
