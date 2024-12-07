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
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/timer.h"

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
 *     - `CLOCK_REALTIME`
 *     - `CLOCK_BOOTTIME`
 *     - `CLOCK_MONOTONIC`
 *     - `CLOCK_REALTIME_COARSE` but is likely to sleep negative time
 *     - `CLOCK_MONTONIC_COARSE` but is likely to sleep negative time
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
  if (IsMetal())
    return ENOSYS;
  if (IsLinux() && clock == CLOCK_REALTIME_COARSE)
    clock = CLOCK_REALTIME;
  if (IsLinux() && clock == CLOCK_MONOTONIC_COARSE)
    clock = CLOCK_MONOTONIC;
  if (clock == 127 ||              //
      (flags & ~TIMER_ABSTIME) ||  //
      req->tv_sec < 0 ||           //
      !(0 <= req->tv_nsec && req->tv_nsec <= 999999999))
    return EINVAL;
  int rc;
  errno_t err, old = errno;
  rc = sys_clock_nanosleep(clock, flags, req, rem);
  err = !rc ? 0 : errno;
  errno = old;
  return err;
}
