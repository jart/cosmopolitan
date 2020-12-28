/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/internal.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/dce.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"

/**
 * Schedules delivery of one-shot or intermittent interrupt signal, e.g.
 *
 * Raise SIGALRM every 1.5s:
 *
 *     CHECK_NE(-1, sigaction(SIGALRM,
 *                            &(struct sigaction){.sa_sigaction = missingno},
 *                            NULL));
 *     CHECK_NE(-1, setitimer(ITIMER_REAL,
 *                            &(const struct itimerval){{1, 500000},
 *                                                      {1, 500000}},
 *                            NULL));
 *
 * Set single-shot 50ms timer callback to interrupt laggy connect():
 *
 *     CHECK_NE(-1, sigaction(SIGALRM,
 *                            &(struct sigaction){.sa_sigaction = missingno,
 *                                                .sa_flags = SA_RESETHAND},
 *                            NULL));
 *     CHECK_NE(-1, setitimer(ITIMER_REAL,
 *                            &(const struct itimerval){{0, 0}, {0, 50000}},
 *                            NULL));
 *     if (connect(...) == -1 && errno == EINTR) { ... }
 *
 * Disarm timer:
 *
 *     CHECK_NE(-1, setitimer(ITIMER_REAL, &(const struct itimerval){0}, NULL));
 *
 * Be sure to check for EINTR on your i/o calls, for best low latency.
 *
 * @param which can be ITIMER_REAL, ITIMER_VIRTUAL, etc.
 * @param newvalue specifies the interval ({0,0} means one-shot) and
 *     duration ({0,0} means disarm) in microseconds ∈ [0,999999] and
 *     if this parameter is NULL, we'll polyfill getitimer() behavior
 * @param out_opt_old may receive remainder of previous op (if any)
 * @return 0 on success or -1 w/ errno
 */
int setitimer(int which, const struct itimerval *newvalue,
              struct itimerval *out_opt_oldvalue) {
  if (!IsWindows()) {
    if (newvalue) {
      return setitimer$sysv(which, newvalue, out_opt_oldvalue);
    } else {
      return getitimer$sysv(which, out_opt_oldvalue);
    }
  } else {
    return setitimer$nt(which, newvalue, out_opt_oldvalue);
  }
}
