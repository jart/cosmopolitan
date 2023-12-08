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
#include "libc/calls/sched-sysv.internal.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"

/**
 * Gets scheduler policy for `pid`.
 *
 * @param pid is the id of the process whose scheduling policy should be
 *     queried. Setting `pid` to zero means the same thing as getpid().
 *     This applies to all threads associated with the process. Linux is
 *     special; the kernel treats this as a thread id (noting that
 *     `getpid() == gettid()` is always the case on Linux for the main
 *     thread) and will only take effect for the specified tid.
 *     Therefore this function is POSIX-compliant iif `!__threaded`.
 * @return scheduler policy, or -1 w/ errno
 * @error ESRCH if `pid` not found
 * @error EPERM if not permitted
 * @error EINVAL if `pid` is negative on Linux
 */
int sched_getscheduler(int pid) {
  int rc;
  if (IsNetbsd()) {
    struct sched_param p;
    rc = sys_sched_getscheduler_netbsd(pid, &p);
  } else {
    rc = sys_sched_getscheduler(pid);
  }
  STRACE("sched_getscheduler(%d) → %s% m", pid, DescribeSchedPolicy(rc));
  return rc;
}
