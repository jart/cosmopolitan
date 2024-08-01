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
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/stdckdint.h"
#include "libc/sysv/errfuns.h"

/**
 * Gets list of supplementary group IDs
 *
 * @param size - maximum number of items that can be stored in list
 * @param list - buffer to store output gid_t
 * @return -1 w/ EFAULT
 */
int getgroups(int size, uint32_t list[]) {
  int rc;
  if (IsLinux() || IsNetbsd() || IsOpenbsd() || IsFreebsd() || IsXnu()) {
    rc = sys_getgroups(size, list);
  } else {
    rc = enosys();
  }
  STRACE("getgroups(%d, %s) → %d% m", size, DescribeGidList(rc, rc, list), rc);
  return rc;
}
