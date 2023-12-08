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
#include "libc/calls/groups.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/stdckdint.h"
#include "libc/sysv/errfuns.h"

/**
 * Sets list of supplementary group IDs.
 *
 * On recent versions of Linux only, it's possible to say:
 *
 *     setgroups(0, NULL);
 *
 * Which will cause subsequent calls to `EPERM`.
 *
 * @param size number of items in list
 * @param list input set of gid_t to set
 * @return -1 w/ EFAULT
 */
int setgroups(size_t size, const uint32_t list[]) {
  int rc;
  size_t n;
  if (IsAsan() &&
      (ckd_mul(&n, size, sizeof(list[0])) || !__asan_is_valid(list, n))) {
    rc = efault();
  } else if (IsLinux() || IsNetbsd() || IsOpenbsd() || IsFreebsd() || IsXnu()) {
    rc = sys_setgroups(size, list);
  } else {
    rc = enosys();
  }
  STRACE("setgroups(%u, %s) → %d% m", size, DescribeGidList(rc, size, list),
         rc);
  return rc;
}
