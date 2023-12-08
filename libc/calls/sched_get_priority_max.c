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
#include "libc/calls/sched-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/sched.h"
#include "libc/sysv/errfuns.h"

static int sys_sched_get_priority_max_netbsd(int policy) {
  if (policy == SCHED_OTHER) {
    return -1;
  } else if (policy == SCHED_RR || policy == SCHED_FIFO) {
    return 63;  // NetBSD Libc needs 19 system calls to compute this!
  } else {
    return einval();
  }
}

/**
 * Returns maximum `sched_param::sched_priority` for `policy`.
 *
 * @return priority, or -1 w/ errno
 * @raise ENOSYS on XNU, Windows, OpenBSD
 * @raise EINVAL if `policy` is invalid
 */
int sched_get_priority_max(int policy) {
  int rc;
  if (IsNetbsd()) {
    rc = sys_sched_get_priority_max_netbsd(policy);
  } else {
    rc = sys_sched_get_priority_max(policy);
  }
  STRACE("sched_get_priority_max(%s) → %d% m", DescribeSchedPolicy(policy), rc);
  return rc;
}
