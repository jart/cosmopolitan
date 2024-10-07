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
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.h"
#include "libc/limits.h"
#include "libc/runtime/stack.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/pollfd.internal.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

static int ppoll_impl(struct pollfd *fds, size_t nfds,
                      const struct timespec *timeout, const sigset_t *sigmask) {
  int e, fdcount;
  sigset_t oldmask;
  struct timespec ts, *tsp;

  // validate timeout
  if (timeout && timeout->tv_nsec >= 1000000000ull)
    return einval();

  // The OpenBSD poll() man pages claims it'll ignore POLLERR, POLLHUP,
  // and POLLNVAL in pollfd::events except it doesn't actually do this.
  size_t bytes = 0;
  struct pollfd *fds2 = 0;
  if (IsOpenbsd()) {
    if (ckd_mul(&bytes, nfds, sizeof(struct pollfd)))
      return einval();
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
#pragma GCC diagnostic ignored "-Wanalyzer-out-of-bounds"
    fds2 = alloca(bytes);
#pragma GCC pop_options
    CheckLargeStackAllocation(fds2, bytes);
    memcpy(fds2, fds, bytes);
    for (size_t i = 0; i < nfds; ++i)
      fds2[i].events &= ~(POLLERR | POLLHUP | POLLNVAL);
    struct pollfd *swap = fds;
    fds = fds2;
    fds2 = swap;
  }

  if (!IsWindows()) {
    e = errno;
    if (timeout) {
      ts = *timeout;
      tsp = &ts;
    } else {
      tsp = 0;
    }
    fdcount = sys_ppoll(fds, nfds, tsp, sigmask, 8);
    if (fdcount == -1 && errno == ENOSYS) {
      int64_t ms;
      errno = e;
      if (timeout) {
        ms = timespec_tomillis(*timeout);
        if (ms > INT_MAX)
          ms = -1;
      } else {
        ms = -1;
      }
      if (sigmask)
        sys_sigprocmask(SIG_SETMASK, sigmask, &oldmask);
      fdcount = sys_poll(fds, nfds, ms);
      if (sigmask)
        sys_sigprocmask(SIG_SETMASK, &oldmask, 0);
    }
  } else {
    fdcount = sys_poll_nt(fds, nfds, timeout, sigmask);
  }

  if (IsOpenbsd() && fdcount != -1) {
    struct pollfd *swap = fds;
    fds = fds2;
    fds2 = swap;
    memcpy(fds, fds2, bytes);
  }

  // One of the use cases for poll() is checking if a large number of
  // file descriptors exist. However on XNU if none of the meaningful
  // event flags are specified (e.g. POLLIN, POLLOUT) then it doesn't
  // perform the POLLNVAL check that's implied on all other platforms
  if (IsXnu() && fdcount != -1) {
    for (size_t i = 0; i < nfds; ++i) {
      if (fds[i].fd >= 0 &&   //
          !fds[i].revents &&  //
          !(fds[i].events & (POLLIN | POLLOUT | POLLPRI))) {
        int err = errno;
        if (fcntl(fds[i].fd, F_GETFL) == -1) {
          errno = err;
          fds[i].revents = POLLNVAL;
          ++fdcount;
        }
      }
    }
  }

  return fdcount;
}

/**
 * Checks status on multiple file descriptors at once.
 *
 * This function is the same as saying:
 *
 *     sigset_t old;
 *     sigprocmask(SIG_SETMASK, sigmask, &old);
 *     poll(fds, nfds, timeout);
 *     sigprocmask(SIG_SETMASK, old, 0);
 *
 * Except it happens atomically when the kernel supports doing that. On
 * kernels such as XNU and NetBSD which don't, this wrapper will fall
 * back to using the example above. If you need ironclad assurances of
 * signal mask atomicity, then consider using pselect() which Cosmo Libc
 * guarantees to be atomic on all supported platforms.
 *
 * Servers that need to handle an unbounded number of client connections
 * should just create a separate thread for each client. poll(), ppoll()
 * and select() aren't scalable i/o solutions on any platform.
 *
 * On Windows it's only possible to poll 64 file descriptors at a time;
 * it's a limitation imposed by WSAPoll(). Cosmopolitan Libc's ppoll()
 * polyfill can go higher in some cases; for example, It's possible to
 * poll 64 sockets and 64 pipes/terminals at the same time. Furthermore,
 * elements whose fd field is set to a negative number are ignored and
 * will not count against this limit.
 *
 * One of the use cases for poll() is to quickly check if a number of
 * file descriptors are valid. The canonical way to do this is to set
 * events to 0 which prevents blocking and causes only the invalid,
 * hangup, and error statuses to be checked.
 *
 * On XNU, the POLLHUP and POLLERR statuses aren't checked unless either
 * POLLIN, POLLOUT, or POLLPRI are specified in the events field. Cosmo
 * will however polyfill the checking of POLLNVAL on XNU with the events
 * doesn't specify any of the above i/o events.
 *
 * When XNU and BSD OSes report POLLHUP, they will always set POLLIN too
 * when POLLIN is requested, even in cases when there isn't unread data.
 *
 * @param fds[𝑖].fd should be a socket, input pipe, or conosle input
 *     and if it's a negative number then the entry is ignored, plus
 *     revents will be set to zero
 * @param fds[𝑖].events flags can have POLLIN, POLLOUT, POLLPRI,
 *     POLLRDNORM, POLLWRNORM, POLLRDBAND, POLLWRBAND as well as
 *     POLLERR, POLLHUP, and POLLNVAL although the latter are
 *     always implied (assuming fd≥0) so they're ignored here
 * @param timeout_ms if 0 means don't wait and negative waits forever
 * @return number of `fds` whose revents field has been set to a nonzero
 *     number, 0 if the timeout elapsed without events, or -1 w/ errno
 * @return fds[𝑖].revents is always zero initializaed and then will
 *     be populated with POLL{IN,OUT,PRI,HUP,ERR,NVAL} if something
 *     was determined about the file descriptor
 * @param timeout if null will block indefinitely
 * @param sigmask may be null in which case no mask change happens
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINVAL if `nfds` exceeded `RLIMIT_NOFILE`
 * @raise ENOMEM on failure to allocate memory
 * @raise EINVAL if `*timeout` is invalid
 * @raise EINTR if signal was delivered
 * @cancelationpoint
 * @asyncsignalsafe
 * @norestart
 */
int ppoll(struct pollfd *fds, size_t nfds, const struct timespec *timeout,
          const sigset_t *sigmask) {
  int fdcount;
  BEGIN_CANCELATION_POINT;
  fdcount = ppoll_impl(fds, nfds, timeout, sigmask);
  END_CANCELATION_POINT;
  STRACE("ppoll(%s, %'zu, %s, %s) → %d% lm",
         DescribePollFds(fdcount, fds, nfds), nfds,
         DescribeTimespec(0, timeout), DescribeSigset(0, sigmask), fdcount);
  return fdcount;
}
