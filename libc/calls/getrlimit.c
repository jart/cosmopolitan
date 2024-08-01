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
#include "libc/calls/struct/rlimit.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/errfuns.h"

/**
 * Gets resource limit for current process.
 *
 * @param resource can be RLIMIT_{CPU,FSIZE,DATA,STACK,CORE,RSS,etc.}
 * @param rlim receives result, modified only on success
 * @return 0 on success or -1 w/ errno
 * @see libc/sysv/consts.sh
 */
int getrlimit(int resource, struct rlimit *rlim) {
  int rc;
  if (resource == 127) {
    rc = einval();
  } else if (!rlim) {
    rc = efault();
  } else if (IsXnuSilicon()) {
    rc = _sysret(__syslib->__getrlimit(resource, rlim));
  } else if (!IsWindows()) {
    rc = sys_getrlimit(resource, rlim);
  } else if (resource == RLIMIT_STACK) {
    rlim->rlim_cur = GetStaticStackSize();
    rlim->rlim_max = GetStaticStackSize();
    rc = 0;
  } else if (resource == RLIMIT_AS) {
    rlim->rlim_cur = __virtualmax;
    rlim->rlim_max = __virtualmax;
    rc = 0;
  } else {
    rc = einval();
  }
  STRACE("getrlimit(%s, [%s]) → %d% m", DescribeRlimitName(resource),
         DescribeRlimit(rc, rlim), rc);
  return rc;
}

__weak_reference(getrlimit, getrlimit64);
