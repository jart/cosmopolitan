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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"

/**
 * Sets nice value of thing.
 *
 * On Windows, there's only six priority classes. We define them as -16
 * (realtime), -10 (high), -5 (above), 0 (normal), 5 (below), 15 (idle)
 * which are the only values that'll roundtrip getpriority/setpriority.
 *
 * @param which can be one of:
 *     - `PRIO_PROCESS` is supported universally
 *     - `PRIO_PGRP` is supported on unix
 *     - `PRIO_USER` is supported on unix
 * @param who is the pid, pgid, or uid, 0 meaning current
 * @param value ∈ [-NZERO,NZERO) which is clamped automatically
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `which` was invalid or unsupported
 * @error EACCES if `value` lower that `RLIMIT_NICE`
 * @error EACCES on Linux without `CAP_SYS_NICE`
 * @raise EPERM if access to process was denied
 * @raise ESRCH if the process didn't exist
 * @see getpriority()
 */
int setpriority(int which, unsigned who, int value) {
  int rc;
  if (!IsWindows()) {
    rc = sys_setpriority(which, who, value);
  } else {
    rc = sys_setpriority_nt(which, who, value);
  }
  STRACE("setpriority(%s, %u, %d) → %d% m", DescribeWhichPrio(which), who,
         value, rc);
  return rc;
}
