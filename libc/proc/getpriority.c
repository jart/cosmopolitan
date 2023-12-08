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
#include "libc/errno.h"
#include "libc/intrin/asmflag.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns nice value of thing.
 *
 * Since -1 might be a valid return value for this API, it's necessary
 * to clear `errno` beforehand and see if it changed, in order to truly
 * determine if an error happened.
 *
 * On Windows, there's only six priority classes. We define them as -16
 * (realtime), -10 (high), -5 (above), 0 (normal), 5 (below), 15 (idle)
 * which are the only values that'll roundtrip getpriority/setpriority.
 *
 * @param which can be one of:
 *     - `PRIO_PROCESS` is supported universally
 *     - `PRIO_PGRP` is supported on unix
 *     - `PRIO_USER` is supported on unix
 * @param who is the pid, pgid, or uid (0 means current)
 * @return value ∈ [-NZERO,NZERO) or -1 w/ errno
 * @raise EINVAL if `which` was invalid or unsupported
 * @raise EPERM if access to process was denied
 * @raise ESRCH if no such process existed
 * @see setpriority()
 */
int getpriority(int which, unsigned who) {
  int rc;
#ifdef __x86_64__
  char cf;
  if (IsLinux()) {
    asm volatile("syscall"
                 : "=a"(rc)
                 : "0"(140), "D"(which), "S"(who)
                 : "rcx", "r11", "memory");
    if (rc >= 0) {
      rc = NZERO - rc;
    } else {
      errno = -rc;
      rc = -1;
    }
  } else if (IsBsd()) {
    asm volatile(CFLAG_ASM("syscall")
                 : CFLAG_CONSTRAINT(cf), "=a"(rc)
                 : "1"((IsXnu() ? 0x2000000 : 0) | 100), "D"(which), "S"(who)
                 : "rcx", "rdx", "r8", "r9", "r10", "r11", "memory");
    if (cf) {
      errno = rc;
      rc = -1;
    }
  } else if (IsWindows()) {
    rc = sys_getpriority_nt(which, who);
  } else {
    rc = enosys();
  }
#else
  rc = sys_getpriority(which, who);
  if (rc != -1) {
    rc = NZERO - rc;
  }
#endif
  STRACE("getpriority(%s, %u) → %d% m", DescribeWhichPrio(which), who, rc);
  return rc;
}
