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
#include "libc/calls/struct/itimerval.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/sock/internal.h"
#include "libc/sock/select.h"
#include "libc/sock/select.internal.h"
#include "libc/sysv/consts/nrlinux.h"
#include "libc/sysv/errfuns.h"

/**
 * Checks status on multiple file descriptors at once.
 *
 * This function is the same as saying:
 *
 *     sigset_t old;
 *     sigprocmask(SIG_SETMASK, sigmask, &old);
 *     select(nfds, readfds, writefds, exceptfds, timeout);
 *     sigprocmask(SIG_SETMASK, old, 0);
 *
 * Except it happens atomically. Unlike ppoll() Cosmo guarantees this is
 * atomic on all supported platforms.
 *
 * @param nfds is the number of the highest file descriptor set in these
 *     bitsets by the caller, plus one; this value can't be greater than
 *     `FD_SETSIZE` which Cosmopolitan currently defines as 1024 because
 *     `fd_set` has a static size
 * @param readfds may be used to be notified when you can call read() on
 *     a file descriptor without it blocking; this includes when data is
 *     is available to be read as well as eof and error conditions
 * @param writefds may be used to be notified when write() may be called
 *     on a file descriptor without it blocking
 * @param exceptfds may be used to be notified of exceptional conditions
 *     such as out-of-band data on a socket; it is equivalent to POLLPRI
 *     in the revents of poll()
 * @param timeout if null will block indefinitely
 * @param sigmask may be null in which case no mask change happens
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @cancelationpoint
 * @asyncsignalsafe
 * @norestart
 */
int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
            const struct timespec *timeout, const sigset_t *sigmask) {
  int rc;
  struct timespec ts, *tsp;
  struct {
    const sigset_t *s;
    size_t n;
  } ss;

  fd_set old_readfds;
  fd_set *old_readfds_ptr = 0;
  fd_set old_writefds;
  fd_set *old_writefds_ptr = 0;
  fd_set old_exceptfds;
  fd_set *old_exceptfds_ptr = 0;

  BEGIN_CANCELATION_POINT;
  if (nfds < 0 || nfds > FD_SETSIZE) {
    rc = einval();
  } else {
    if (readfds) {
      old_readfds = *readfds;
      old_readfds_ptr = &old_readfds;
    }
    if (writefds) {
      old_writefds = *writefds;
      old_writefds_ptr = &old_writefds;
    }
    if (exceptfds) {
      old_exceptfds = *exceptfds;
      old_exceptfds_ptr = &old_exceptfds;
    }
    if (IsLinux()) {
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
      rc = sys_pselect(nfds, readfds, writefds, exceptfds,
                       (struct timespec *)timeout, sigmask);
    } else {
      rc = sys_select_nt(nfds, readfds, writefds, exceptfds, timeout, sigmask);
    }
  }
  END_CANCELATION_POINT;

  STRACE("pselect(%d, %s → [%s], %s → [%s], %s → [%s], %s, %s) → %d% m", nfds,
         DescribeFdSet(0, nfds, old_readfds_ptr),
         DescribeFdSet(rc, nfds, readfds),
         DescribeFdSet(0, nfds, old_writefds_ptr),
         DescribeFdSet(rc, nfds, writefds),
         DescribeFdSet(0, nfds, old_exceptfds_ptr),
         DescribeFdSet(rc, nfds, exceptfds),  //
         DescribeTimespec(0, timeout),        //
         DescribeSigset(0, sigmask), rc);
  return rc;
}
