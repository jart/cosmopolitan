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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/timer.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

static int sys_clock_nanosleep(int clock, int flags,  //
                               const struct timespec *req,
                               struct timespec *rem) {
  int rc;
  BEGIN_CANCELATION_POINT;
  if (IsLinux() || IsFreebsd() || IsNetbsd()) {
    rc = __sys_clock_nanosleep(clock, flags, req, rem);
  } else if (IsXnu()) {
    rc = sys_clock_nanosleep_xnu(clock, flags, req, rem);
  } else if (IsOpenbsd()) {
    rc = sys_clock_nanosleep_openbsd(clock, flags, req, rem);
  } else if (IsWindows()) {
    rc = sys_clock_nanosleep_nt(clock, flags, req, rem);
  } else {
    rc = enosys();
  }
  if (rc > 0) {
    errno = rc;
    rc = -1;
  }
  // system call support might not detect cancelation on bsds
  if (rc == -1 && errno == EINTR &&      //
      _weaken(pthread_testcancel_np) &&  //
      _weaken(pthread_testcancel_np)()) {
    rc = ecanceled();
  }
  END_CANCELATION_POINT;
  STRACE("sys_clock_nanosleep(%s, %s, %s, [%s]) → %d% m",
         DescribeClockName(clock), DescribeSleepFlags(flags),
         DescribeTimespec(0, req), DescribeTimespec(rc, rem), rc);
  return rc;
}

static int cosmo_clock_nanosleep(int clock, int flags,
                                 const struct timespec *req,
                                 struct timespec *rem) {

  // pick clocks
  int time_clock;
  int sleep_clock;
  if (clock == CLOCK_REALTIME ||  //
      clock == CLOCK_REALTIME_PRECISE) {
    time_clock = clock;
    sleep_clock = CLOCK_REALTIME;
  } else if (clock == CLOCK_MONOTONIC ||  //
             clock == CLOCK_MONOTONIC_PRECISE) {
    time_clock = clock;
    sleep_clock = CLOCK_MONOTONIC;
  } else if (clock == CLOCK_REALTIME_COARSE ||  //
             clock == CLOCK_REALTIME_FAST) {
    return sys_clock_nanosleep(CLOCK_REALTIME, flags, req, rem);
  } else if (clock == CLOCK_MONOTONIC_COARSE ||  //
             clock == CLOCK_MONOTONIC_FAST) {
    return sys_clock_nanosleep(CLOCK_MONOTONIC, flags, req, rem);
  } else {
    return sys_clock_nanosleep(clock, flags, req, rem);
  }

  // sleep bulk of time in kernel
  struct timespec start, deadline, remain, waitfor, now;
  struct timespec quantum = timespec_fromnanos(1000000000 / CLK_TCK);
  unassert(!clock_gettime(time_clock, &start));
  deadline = flags & TIMER_ABSTIME ? *req : timespec_add(start, *req);
  if (timespec_cmp(start, deadline) >= 0) return 0;
  remain = timespec_sub(deadline, start);
  if (timespec_cmp(remain, quantum) > 0) {
    waitfor = timespec_sub(remain, quantum);
    if (sys_clock_nanosleep(sleep_clock, 0, &waitfor, rem) == -1) {
      if (!flags && rem && errno == EINTR) {
        *rem = timespec_add(*rem, quantum);
      }
      return -1;
    }
  }

  // spin through final scheduling quantum
  int rc = 0;
  ftrace_enabled(-1);
  do {
    if (_check_cancel()) {
      rc = -1;
      break;
    }
    unassert(!clock_gettime(time_clock, &now));
  } while (timespec_cmp(now, deadline) < 0);
  ftrace_enabled(+1);
  return rc;
}

/**
 * Sleeps for particular amount of time.
 *
 * Here's how you could sleep for one second:
 *
 *     clock_nanosleep(0, 0, &(struct timespec){1}, 0);
 *
 * Your sleep will be interrupted automatically if you do something like
 * press ctrl-c during the wait. That's an `EINTR` error and it lets you
 * immediately react to status changes. This is always the case, even if
 * you're using `SA_RESTART` since this is a `@norestart` system call.
 *
 *     void OnCtrlC(int sig) {} // EINTR only happens after delivery
 *     signal(SIGINT, OnCtrlC); // do delivery rather than kill proc
 *     printf("save me from sleeping forever by pressing ctrl-c\n");
 *     clock_nanosleep(0, 0, &(struct timespec){INT_MAX}, 0);
 *     printf("you're my hero\n");
 *
 * If you want to perform an uninterruptible sleep without having to use
 * sigprocmask() to block all signals then this function provides a good
 * solution to that problem. For example:
 *
 *     struct timespec rel, now, abs;
 *     clock_gettime(CLOCK_REALTIME, &now);
 *     rel = timespec_frommillis(100);
 *     abs = timespec_add(now, rel);
 *     while (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &abs, 0));
 *
 * will accurately spin on `EINTR` errors. That way you're not impeding
 * signal delivery and you're not loosing precision on the wait timeout.
 * This function has first-class support on Linux, FreeBSD, and NetBSD;
 * on OpenBSD it's good; on XNU it's bad; and on Windows it's ugly.
 *
 * @param clock may be
 *     - `CLOCK_REALTIME` to have nanosecond-accurate wall time sleeps
 *     - `CLOCK_REALTIME_COARSE` to not spin through scheduler quantum
 *     - `CLOCK_MONOTONIC` to base the sleep off the monotinic clock
 *     - `CLOCK_MONOTONIC_COARSE` to once again not do userspace spin
 * @param flags can be 0 for relative and `TIMER_ABSTIME` for absolute
 * @param req can be a relative or absolute time, depending on `flags`
 * @param rem shall be updated with the remainder of unslept time when
 *     (1) it's non-null; (2) `flags` is 0; and (3) -1 w/ `EINTR` is
 *     returned; if this function returns 0 then `rem` is undefined;
 *     if flags is `TIMER_ABSTIME` then `rem` is ignored
 * @return 0 on success, or errno on error
 * @raise EINTR when a signal got delivered while we were waiting
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise ENOTSUP if `clock` is known but we can't use it here
 * @raise EFAULT if `req` or null or bad memory was passed
 * @raise EINVAL if `clock` is unknown to current platform
 * @raise EINVAL if `flags` has an unrecognized value
 * @raise EINVAL if `req->tv_nsec ∉ [0,1000000000)`
 * @raise ENOSYS on bare metal
 * @cancelationpoint
 * @returnserrno
 * @norestart
 */
errno_t clock_nanosleep(int clock, int flags,        //
                        const struct timespec *req,  //
                        struct timespec *rem) {
  if (IsMetal()) {
    return ENOSYS;
  }
  if (clock == 127 ||              //
      (flags & ~TIMER_ABSTIME) ||  //
      req->tv_sec < 0 ||           //
      !(0 <= req->tv_nsec && req->tv_nsec <= 999999999)) {
    return EINVAL;
  }
  errno_t old = errno;
  int rc = cosmo_clock_nanosleep(clock, flags, req, rem);
  errno_t err = !rc ? 0 : errno;
  errno = old;
  return err;
}
