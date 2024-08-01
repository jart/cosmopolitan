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
#include "libc/calls/calls.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/itimerval.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/sysv/errfuns.h"

/**
 * Retrieves last setitimer() value, correcting for remaining time.
 *
 * @param which can be ITIMER_REAL, ITIMER_VIRTUAL, etc.
 * @return 0 on success or -1 w/ errno
 */
int getitimer(int which, struct itimerval *curvalue) {
  int rc;
  if (!IsWindows()) {
    rc = sys_getitimer(which, curvalue);
  } else if (!curvalue) {
    rc = efault();
  } else {
    rc = sys_setitimer_nt(which, 0, curvalue);
  }
  STRACE("getitimer(%s, [%s]) → %d% m", DescribeItimer(which),
         DescribeItimerval(rc, curvalue), rc);
  return rc;
}
