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
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/struct/sysinfo.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

#define CTL_VM     2
#define VM_LOADAVG 2

struct loadavg {
  uint32_t ldavg[3];
  int64_t fscale;
};

/**
 * Returns system load average.
 *
 * @param a should be array of 3 doubles
 * @param n should be 3
 * @return number of items placed in `a` or -1 w/ errno
 * @raise ENOSYS on metal
 */
int getloadavg(double *a, int n) {
  // cat /proc/loadavg
  int i, rc;
  if (n > 3) n = 3;
  if (!n) {
    rc = 0;
  } else if (n < 0) {
    rc = einval();
  } else if (IsWindows()) {
    return sys_getloadavg_nt(a, n);
  } else if (IsLinux()) {
    struct sysinfo si;
    if ((rc = sys_sysinfo(&si)) != -1) {
      for (i = 0; i < n; i++) {
        a[i] = 1. / 65536 * si.loads[i];
      }
      rc = n;
    }
  } else if (IsFreebsd() || IsNetbsd() || IsOpenbsd() || IsXnu()) {
    size_t size;
    struct loadavg loadinfo;
    int mib[2] = {CTL_VM, VM_LOADAVG};
    size = sizeof(loadinfo);
    if ((rc = sys_sysctl(mib, 2, &loadinfo, &size, 0, 0)) != -1) {
      for (i = 0; i < n; i++) {
        a[i] = (double)loadinfo.ldavg[i] / loadinfo.fscale;
      }
      rc = n;
    }
  } else {
    rc = enosys();
  }
  STRACE("getloadavg(%p, %d) → %d% m", a, n, rc);
  return rc;
}
