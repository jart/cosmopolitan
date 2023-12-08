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
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/pollfd.internal.h"
#include "libc/stdckdint.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

/**
 * Waits for something to happen on multiple file descriptors at once.
 *
 * This function is the same as saying:
 *
 *     sigset_t old;
 *     sigprocmask(SIG_SETMASK, sigmask, &old);
 *     poll(fds, nfds, timeout);
 *     sigprocmask(SIG_SETMASK, old, 0);
 *
 * Except it happens atomically when the kernel supports doing that. On
 * kernel such as XNU and NetBSD which don't, this wrapper will fall
 * back to using the example above. Consider using pselect() which is
 * atomic on all supported platforms.
 *
 * The Linux Kernel modifies the timeout parameter. This wrapper gives
 * it a local variable due to POSIX requiring that `timeout` be const.
 * If you need that information from the Linux Kernel use sys_ppoll().
 *
 * @param timeout if null will block indefinitely
 * @param sigmask may be null in which case no mask change happens
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @cancelationpoint
 * @asyncsignalsafe
 * @norestart
 */
int ppoll(struct pollfd *fds, size_t nfds, const struct timespec *timeout,
          const sigset_t *sigmask) {
  size_t n;
  int e, rc;
  sigset_t oldmask;
  struct timespec ts, *tsp;
  BEGIN_CANCELATION_POINT;

  if (IsAsan() &&
      (ckd_mul(&n, nfds, sizeof(struct pollfd)) || !__asan_is_valid(fds, n) ||
       (timeout && !__asan_is_valid(timeout, sizeof(timeout))) ||
       (sigmask && !__asan_is_valid(sigmask, sizeof(sigmask))))) {
    rc = efault();
  } else if (!IsWindows()) {
    e = errno;
    if (timeout) {
      ts = *timeout;
      tsp = &ts;
    } else {
      tsp = 0;
    }
    rc = sys_ppoll(fds, nfds, tsp, sigmask, 8);
    if (rc == -1 && errno == ENOSYS) {
      int ms;
      errno = e;
      if (!timeout || ckd_add(&ms, timeout->tv_sec,
                              (timeout->tv_nsec + 999999) / 1000000)) {
        ms = -1;
      }
      if (sigmask) sys_sigprocmask(SIG_SETMASK, sigmask, &oldmask);
      rc = poll(fds, nfds, ms);
      if (sigmask) sys_sigprocmask(SIG_SETMASK, &oldmask, 0);
    }
  } else {
    uint32_t ms;
    if (!timeout ||
        ckd_add(&ms, timeout->tv_sec, (timeout->tv_nsec + 999999) / 1000000)) {
      ms = -1u;
    }
    rc = sys_poll_nt(fds, nfds, &ms, sigmask);
  }

  END_CANCELATION_POINT;
  STRACE("ppoll(%s, %'zu, %s, %s) → %d% lm", DescribePollFds(rc, fds, nfds),
         nfds, DescribeTimespec(0, timeout), DescribeSigset(0, sigmask), rc);
  return rc;
}
