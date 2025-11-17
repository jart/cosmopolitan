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
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rlimit.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/limits.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"

static int setrlimit_impl(int resource, const struct rlimit *rlim) {

  // validate api usage
  if (resource + 0u >= RLIM_NLIMITS)
    return einval();
  if (kisdangerous(rlim))
    return efault();
  if (rlim->rlim_cur > rlim->rlim_max)
    return einval();
  if (rlim->rlim_cur > ~__get_pib()->rlimit[resource].rlim_max)
    return eperm();

  // polyfill magic number differences
  int sysresource = resource;
  struct rlimit sysrlim = *rlim;
  if (IsBsd()) {

    // bsd uses u64 as rlim_t but RLIM_INFINITY omits sign bit
    if (sysrlim.rlim_cur > LONG_MAX)
      sysrlim.rlim_cur = LONG_MAX;
    if (sysrlim.rlim_max > LONG_MAX)
      sysrlim.rlim_max = LONG_MAX;

    // these magic numbers differ on bsd
    switch (sysresource) {
      case RLIMIT_NPROC:
      case RLIMIT_NOFILE:
        ++sysresource;
        break;
      case RLIMIT_MEMLOCK:
        sysresource = 6;
        break;
      case RLIMIT_AS:
        if (IsFreebsd() || IsNetbsd()) {
          sysresource = 10;
        } else if (IsOpenbsd()) {
          sysresource = RLIMIT_DATA;
        }
        break;
      default:
        break;
    }
  }

  // tell operating system about limit
  int rc;
  if (IsWindows() || IsMetal()) {
    rc = 0;
  } else if (IsXnu() && resource == RLIMIT_AS) {
    // Apple lazily defines RLIMIT_AS as RLIMIT_RSS in their header, but
    // the feature doesn't work the way we expect. For example if we set
    // rlim_cur to zero, then it'll raise EINVAL, which makes it useless
    // as a means of preventing further memory growth. Since we polyfill
    // this in userspace, let's just not even bother with the XNU kernel
    rc = 0;
  } else if (IsXnuSilicon()) {
    rc = _sysret(__syslib->__setrlimit(sysresource, &sysrlim));
  } else {
    rc = sys_setrlimit(sysresource, &sysrlim);
  }

  // store limit in memory if os didn't reject it
  if (!rc) {
    __get_pib()->rlimit[resource].rlim_cur = ~rlim->rlim_cur;
    __get_pib()->rlimit[resource].rlim_max = ~rlim->rlim_max;
  }

  return rc;
}

/**
 * Sets resource limit for current process.
 *
 * The following resources are recommended:
 *
 * - `RLIMIT_NOFILE` limits the number of open file descriptors and it
 *   works on all platforms.
 *
 * - `RLIMIT_AS` limits the size of the virtual address space. This is
 *   emulated on XNU and Windows and as such it won't propagate across
 *   execve() currently.
 *
 * - `RLIMIT_STACK` controls how much stack memory is available to the
 *   main thread. This setting is inherited across fork() and execve()
 *   Please note it's only safe for Cosmopolitan programs, to set this
 *   value to at least `PTHREAD_STACK_MIN * 2`. On Windows this cannot
 *   be used to extend the stack, which is currently hard-coded.
 *
 * - `RLIMIT_FSIZE` is the maximum permissible size for any given file
 *   which is specified in bytes. If an i/o operation like ftruncate()
 *   or write() would cause a file to exceed this size then the kernel
 *   will kill the process using `SIGXFSZ`. If this signal is ignored,
 *   then the i/o operation will raise `EFBIG`.
 *
 * - `RLIMIT_CPU` causes `SIGXCPU` to be sent to the process when the
 *   soft limit on CPU time is exceeded, and the process is destroyed
 *   when the hard limit is exceeded. It works everywhere except XNU.
 *
 * - `RLIMIT_CORE` limits the size of core dumps which should work on
 *   all platforms except Windows.
 *
 * - `RLIMIT_NPROC` limits the number of simultaneous processes and it
 *   should work on all platforms except Windows. Please be advised it
 *   limits the process, with respect to the activities of the user id
 *   as a whole.
 *
 * @param rlim specifies new resource limit
 * @return 0 on success or -1 w/ errno
 * @raise EINVAL if `resource` isn't supported
 * @raise EFAULT if `rlim` points to bad memory
 * @raise EINVAL if `rlim->rlim_cur` is above `rlim->rlim_max`
 * @raise EPERM if `rlim->rlim_cur` is above current `rlim_max`
 * @vforksafe
 */
int setrlimit(int resource, const struct rlimit *rlim) {
  int rc = setrlimit_impl(resource, rlim);
  STRACE("setrlimit(%s, %s) → %d% m", DescribeRlimitName(resource),
         DescribeRlimit(0, rlim), rc);
  return rc;
}

__weak_reference(setrlimit, setrlimit64);
