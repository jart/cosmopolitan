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
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/limits.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"

static int getrlimit_impl(int resource, struct rlimit *rlim) {

  // validate api usage
  if (resource + 0u >= RLIM_NLIMITS)
    return einval();
  if (kisdangerous(rlim))
    return efault();

  // if it's not unix or not working then we're done
  if (IsWindows() || IsMetal() || (IsXnu() && resource == RLIMIT_AS)) {
    rlim->rlim_cur = ~__get_pib()->rlimit[resource].rlim_cur;
    rlim->rlim_max = ~__get_pib()->rlimit[resource].rlim_max;

    // avoid closefrom polyfill spinning its wheels forever
    if ((IsWindows() || IsMetal()) && resource == RLIMIT_NOFILE) {
      unsigned long maxfd = __get_pib()->fds.n;
      if (rlim->rlim_cur > maxfd)
        rlim->rlim_cur = maxfd;
    }

    return 0;
  }

  // these magic numbers differ on bsd
  int sysresource = resource;
  if (IsBsd()) {
    switch (sysresource) {
      case RLIMIT_NPROC:
      case RLIMIT_NOFILE:
        ++sysresource;
        break;
      case RLIMIT_MEMLOCK:
        sysresource = 6;
        break;
      case RLIMIT_AS:
        if (IsXnu()) {
          sysresource = RLIMIT_RSS;
        } else if (IsFreebsd() || IsNetbsd()) {
          sysresource = 10;
        } else if (IsOpenbsd()) {
          sysresource = RLIMIT_DATA;
        }
        break;
      default:
        break;
    }
  }

  // get limit from operating system
  int rc;
  if (IsXnuSilicon()) {
    rc = _sysret(__syslib->__getrlimit(sysresource, rlim));
  } else {
    rc = sys_getrlimit(sysresource, rlim);
  }

  // don't modify memory upon failure
  // failure here actually shouldn't be possible
  if (rc == -1)
    return -1;

  // extend bsd rlim_infinity to linux version
  if (IsBsd()) {
    if (rlim->rlim_cur >= LONG_MAX)
      rlim->rlim_cur = RLIM_INFINITY;
    if (rlim->rlim_max >= LONG_MAX)
      rlim->rlim_max = RLIM_INFINITY;
  }

  return 0;
}

/**
 * Gets resource limit for current process.
 *
 * See setrlimit() for documentation on the different limits supported.
 *
 * @param resource can be RLIMIT_{CPU,FSIZE,DATA,STACK,CORE,RSS,etc.}
 * @param rlim receives result, modified only on success
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `resource` isn't supported
 * @raise EFAULT if `rlim` points to bad memory
 */
int getrlimit(int resource, struct rlimit *rlim) {
  int rc = getrlimit_impl(resource, rlim);
  STRACE("getrlimit(%s, [%s]) → %d% m", DescribeRlimitName(resource),
         DescribeRlimit(rc, rlim), rc);
  return rc;
}

__weak_reference(getrlimit, getrlimit64);
