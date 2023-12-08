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
#include "libc/assert.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/timer.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

int sys_clock_nanosleep_xnu(int clock, int flags, const struct timespec *req,
                            struct timespec *rem) {
#ifdef __x86_64__
  if (flags & TIMER_ABSTIME) {
    struct timespec now;
    sys_clock_gettime_xnu(clock, &now);
    if (timespec_cmp(*req, now) > 0) {
      struct timeval rel = timespec_totimeval(timespec_sub(*req, now));
      return sys_select(0, 0, 0, 0, &rel);
    } else {
      return 0;
    }
  } else {
    int rc;
    struct timespec beg;
    if (rem) sys_clock_gettime_xnu(CLOCK_REALTIME, &beg);
    struct timeval rel = timespec_totimeval(*req);  // rounds up
    rc = sys_select(0, 0, 0, 0, &rel);
    if (rc == -1 && rem && errno == EINTR) {
      struct timespec end;
      sys_clock_gettime_xnu(CLOCK_REALTIME, &end);
      *rem = timespec_subz(*req, timespec_sub(end, beg));
    }
    return rc;
  }
#else
  long res;
  struct timespec abs, now, rel;
  if (_weaken(pthread_testcancel_np) &&  //
      _weaken(pthread_testcancel_np)()) {
    return ecanceled();
  }
  if (flags & TIMER_ABSTIME) {
    abs = *req;
    if (!(res = __syslib->__clock_gettime(clock, &now))) {
      if (timespec_cmp(abs, now) > 0) {
        rel = timespec_sub(abs, now);
        res = __syslib->__nanosleep(&rel, 0);
      }
    }
  } else {
    res = __syslib->__nanosleep(req, rem);
  }
  if (res == -EINTR &&                    //
      (_weaken(pthread_testcancel_np) &&  //
       _weaken(pthread_testcancel_np)())) {
    return ecanceled();
  }
  return _sysret(res);
#endif
}
