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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/nt/errors.h"
#include "libc/nt/nt/time.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/errfuns.h"

textwindows noinstrument int sys_nanosleep_nt(const struct timespec *req,
                                              struct timespec *rem) {
  int rc;
  int64_t sec, nsec;
  uint64_t ms, slice;
  if (__builtin_mul_overflow(req->tv_sec, 1000, &ms) ||
      __builtin_add_overflow(ms, req->tv_nsec / 1000000, &ms)) {
    ms = -1;
  }
  if (!ms && (req->tv_sec || req->tv_nsec)) {
    ms = 1;
  }
  rc = 0;
  do {
    if (!__time_critical && _check_interrupts(false, g_fds.p)) {
      rc = eintr();
      break;
    }
    if (ms > __SIG_POLLING_INTERVAL_MS) {
      slice = __SIG_POLLING_INTERVAL_MS;
    } else {
      slice = ms;
    }
    if (SleepEx(slice, true) == kNtWaitIoCompletion) {
      STRACE("IOCP TRIGGERED EINTR");
      rc = eintr();
      break;
    }
    ms -= slice;
  } while (ms);
  if (rem) {
    sec = ms / 1000;
    nsec = ms % 1000 * 1000000000;
    rem->tv_nsec -= nsec;
    if (rem->tv_nsec < 0) {
      --rem->tv_sec;
      rem->tv_nsec = 1000000000 - rem->tv_nsec;
    }
    rem->tv_sec -= sec;
    if (rem->tv_sec < 0) {
      rem->tv_sec = 0;
      rem->tv_nsec = 0;
    }
  }
  return rc;
}
