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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rlimit.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/errfuns.h"

/**
 * Sets resource limit for current process.
 *
 * The following resources are recommended:
 *
 * - `RLIMIT_STACK` controls how much stack memory is available to the
 *   main thread. This setting is inherited across fork() and execve()
 *   Please note it's only safe for Cosmopolitan programs, to set this
 *   value to at least `PTHREAD_STACK_MIN * 2`. On Windows this cannot
 *   be used to extend the stack, which is currently hard-coded.
 *
 * - `RLIMIT_AS` limits the size of the virtual address space. This will
 *   work on all platforms except WSL. It is emulated on XNU and Windows
 *   which means it won't propagate across execve() currently.
 *
 * - `RLIMIT_CPU` causes `SIGXCPU` to be sent to the process when the
 *   soft limit on CPU time is exceeded, and the process is destroyed
 *   when the hard limit is exceeded. It works everywhere but Windows
 *   where it should be possible to poll getrusage() with setitimer()
 *
 * - `RLIMIT_FSIZE` causes `SIGXFSZ` to sent to the process when the
 *   soft limit on file size is exceeded and the process is destroyed
 *   when the hard limit is exceeded. It works everywhere but Windows
 *   and it also causes `EFBIG` to be returned by i/o functions after
 *   the `SIGXFSZ` signal is delivered or ignored
 *
 * - `RLIMIT_NPROC` limits the number of simultaneous processes and it
 *   should work on all platforms except Windows. Please be advised it
 *   limits the process, with respect to the activities of the user id
 *   as a whole.
 *
 * - `RLIMIT_NOFILE` limits the number of open file descriptors and it
 *   should work on all platforms except Windows (TODO)
 *
 * The rlimit magnums differ for each platform but occupy the interval
 * zero through `RLIM_NLIMITS`. Usually they're set to `RLIM_INFINITY`
 * which is `-1` on Linux/Windows, and `LONG_MAX` on BSDs. In any case
 * they're both very large numbers under the Cosmopolitan unsigned ABI
 * because struct rlimit uses uint64_t. The special magnum 127 is used
 * for constant values that aren't supported by the host platform.
 *
 * @param rlim specifies new resource limit
 * @return 0 on success or -1 w/ errno
 * @see libc/sysv/consts.sh
 * @vforksafe
 */
int setrlimit(int resource, const struct rlimit *rlim) {
  int rc;
  if (resource == 127) {
    rc = einval();
  } else if (!rlim || (IsAsan() && !__asan_is_valid(rlim, sizeof(*rlim)))) {
    rc = efault();
  } else if (IsXnuSilicon()) {
    rc = _sysret(__syslib->__setrlimit(resource, rlim));
  } else if (!IsWindows()) {
    rc = sys_setrlimit(resource, rlim);
    if (IsXnu() && !rc && resource == RLIMIT_AS) {
      // TODO(jart): What's up with XNU and NetBSD?
      __virtualmax = rlim->rlim_cur;
    }
  } else if (resource == RLIMIT_STACK) {
    rc = enotsup();
  } else if (resource == RLIMIT_AS) {
    __virtualmax = rlim->rlim_cur;
    rc = 0;
  } else {
    rc = einval();
  }
  STRACE("setrlimit(%s, %s) → %d% m", DescribeRlimitName(resource),
         DescribeRlimit(0, rlim), rc);
  return rc;
}

__weak_reference(setrlimit, setrlimit64);
