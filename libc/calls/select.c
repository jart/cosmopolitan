/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/sock/select.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/itimerval.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/select.h"
#include "libc/sock/select.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Does what poll() does except with bitset API.
 *
 * This system call is supported on all platforms. However, on Windows,
 * this is polyfilled to translate into poll(). So it's recommended that
 * poll() be used instead.
 *
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @cancelationpoint
 * @asyncsignalsafe
 * @norestart
 */
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout) {

  int rc;
  fd_set old_readfds;
  fd_set *old_readfds_ptr = 0;
  fd_set old_writefds;
  fd_set *old_writefds_ptr = 0;
  fd_set old_exceptfds;
  fd_set *old_exceptfds_ptr = 0;
  struct timeval old_timeout;
  struct timeval *old_timeout_ptr = 0;

  POLLTRACE("select(%d, %p, %p, %p, %s) → ...", nfds, readfds, writefds,
            exceptfds, DescribeTimeval(0, timeout));

  BEGIN_CANCELATION_POINT;
  if (nfds < 0) {
    rc = einval();
  } else if (IsAsan() &&
             ((readfds && !__asan_is_valid(readfds, FD_SIZE(nfds))) ||
              (writefds && !__asan_is_valid(writefds, FD_SIZE(nfds))) ||
              (exceptfds && !__asan_is_valid(exceptfds, FD_SIZE(nfds))) ||
              (timeout && !__asan_is_valid(timeout, sizeof(*timeout))))) {
    rc = efault();
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
    if (timeout) {
      old_timeout = *timeout;
      old_timeout_ptr = &old_timeout;
    }
    if (!IsWindows()) {
#ifdef __aarch64__
      struct timespec ts, *tsp;
      if (timeout) {
        ts = timeval_totimespec(*timeout);
        tsp = &ts;
      } else {
        tsp = 0;
      }
      rc = sys_pselect(nfds, readfds, writefds, exceptfds, tsp, 0);
      if (timeout) {
        *timeout = timespec_totimeval(ts);
      }
#else
      rc = sys_select(nfds, readfds, writefds, exceptfds, timeout);
#endif
    } else {
      rc = sys_select_nt(nfds, readfds, writefds, exceptfds, timeout, 0);
    }
  }
  END_CANCELATION_POINT;

  STRACE("select(%d, %s → [%s], %s → [%s], %s → [%s], %s → [%s]) → %d% m", nfds,
         DescribeFdSet(rc, nfds, old_readfds_ptr),
         DescribeFdSet(rc, nfds, readfds),
         DescribeFdSet(rc, nfds, old_writefds_ptr),
         DescribeFdSet(rc, nfds, writefds),
         DescribeFdSet(rc, nfds, old_exceptfds_ptr),
         DescribeFdSet(rc, nfds, exceptfds),    //
         DescribeTimeval(rc, old_timeout_ptr),  //
         DescribeTimeval(rc, timeout), rc);
  return rc;
}
