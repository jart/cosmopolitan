/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nt/errors.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/clockstonanos.internal.h"

textwindows int sys_nanosleep_nt(const struct timespec *req,
                                 struct timespec *rem) {
  bool alertable;
  uint32_t slice;
  uint64_t begin;
  int64_t ms, toto, nanos;
  struct timespec elapsed;

  // check req is legal timespec
  if (!(0 <= req->tv_nsec && req->tv_nsec < 1000000000)) {
    return einval();
  }

  // save beginning timestamp
  if (!__time_critical && rem) {
    begin = rdtsc();
  } else {
    begin = 0;  // to prevent uninitialized warning
  }

  // convert timespec to milliseconds
  if (__builtin_mul_overflow(req->tv_sec, 1000, &ms) ||
      __builtin_add_overflow(ms, (req->tv_nsec + 999999) / 1000000, &ms)) {
    ms = INT64_MAX;
  }

  for (toto = ms;;) {

    // check if signal was delivered
    if (!__time_critical && _check_interrupts(false, g_fds.p)) {
      if (rem) {
        nanos = ClocksToNanos(rdtsc(), begin);
        elapsed.tv_sec = nanos / 1000000000;
        elapsed.tv_nsec = nanos % 1000000000;
        *rem = _timespec_sub(*req, elapsed);
        if (rem->tv_sec < 0) {
          rem->tv_sec = 0;
          rem->tv_nsec = 0;
        }
      }
      return eintr();
    }

    // configure the sleep
    slice = MIN(__SIG_POLLING_INTERVAL_MS, ms);
    if (__time_critical) {
      alertable = false;
    } else {
      alertable = true;
      POLLTRACE("... sleeping %'ldms of %'ld", toto - ms, toto);
    }

    // perform the sleep
    if (SleepEx(slice, alertable) == kNtWaitIoCompletion) {
      POLLTRACE("IOCP EINTR");  // in case we ever figure it out
      continue;
    }

    // check if full duration has elapsed
    if ((ms -= slice) <= 0) {
      if (rem) {
        rem->tv_sec = 0;
        rem->tv_nsec = 0;
      }
      return 0;
    }
  }
}
