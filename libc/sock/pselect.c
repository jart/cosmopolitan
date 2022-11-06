/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/select.h"
#include "libc/sysv/errfuns.h"

/**
 * Does what poll() does except with bitset API.
 *
 * This function is the same as saying:
 *
 *     sigset_t old;
 *     sigprocmask(SIG_SETMASK, sigmask, &old);
 *     select(nfds, readfds, writefds, exceptfds, timeout);
 *     sigprocmask(SIG_SETMASK, old, 0);
 *
 * Except it happens atomically.
 *
 * The Linux Kernel modifies the timeout parameter. This wrapper gives
 * it a local variable due to POSIX requiring that `timeout` be const.
 * If you need that information from the Linux Kernel use sys_pselect.
 *
 * This system call is supported on all platforms. It's like select()
 * except that it atomically changes the sigprocmask() during the op.
 *
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @cancellationpoint
 * @asyncsignalsafe
 * @threadsafe
 * @norestart
 */
int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
            const struct timespec *timeout, const sigset_t *sigmask) {
  int rc;
  sigset_t oldmask;
  struct timeval tv, *tvp;
  struct timespec ts, *tsp;
  struct {
    const sigset_t *s;
    size_t n;
  } ss;
  BEGIN_CANCELLATION_POINT;

  if (nfds < 0) {
    rc = einval();
  } else if (IsAsan() &&
             ((readfds && !__asan_is_valid(readfds, FD_SIZE(nfds))) ||
              (writefds && !__asan_is_valid(writefds, FD_SIZE(nfds))) ||
              (exceptfds && !__asan_is_valid(exceptfds, FD_SIZE(nfds))) ||
              (timeout && !__asan_is_valid(timeout, sizeof(*timeout))) ||
              (sigmask && !__asan_is_valid(sigmask, sizeof(*sigmask))))) {
    rc = efault();
  } else if (IsLinux()) {
    if (timeout) {
      ts = *timeout;
      tsp = &ts;
    } else {
      tsp = 0;
    }
    ss.s = sigmask;
    ss.n = 8;
    rc = sys_pselect(nfds, readfds, writefds, exceptfds, tsp, &ss);
  } else if (!IsWindows()) {
    rc = sys_pselect(nfds, readfds, writefds, exceptfds, timeout, sigmask);
  } else {
    if (timeout) {
      tv.tv_sec = timeout->tv_sec;
      tv.tv_usec = timeout->tv_nsec / 1000;
      tvp = &tv;
    } else {
      tvp = 0;
    }
    rc = sys_select_nt(nfds, readfds, writefds, exceptfds, tvp, sigmask);
  }

  END_CANCELLATION_POINT;
  POLLTRACE("pselect(%d, %p, %p, %p, %s, %s) → %d% m", nfds, readfds, writefds,
            exceptfds, DescribeTimeval(0, timeout), DescribeSigset(0, sigmask),
            rc);
  return rc;
}
