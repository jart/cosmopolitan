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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/statfs-meta.internal.h"
#include "libc/calls/struct/statfs.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns information about filesystem.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @raise ENOTSUP if /zip path
 * @cancelationpoint
 */
int statfs(const char *path, struct statfs *sf) {
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  union statfs_meta m;
  CheckLargeStackAllocation(&m, sizeof(m));
#pragma GCC pop_options
  int rc;
  struct ZiposUri zipname;
  BEGIN_CANCELATION_POINT;

  CheckLargeStackAllocation(&m, sizeof(m));
  if (_weaken(__zipos_parseuri) &&
      _weaken(__zipos_parseuri)(path, &zipname) != -1) {
    rc = enotsup();
  } else if (!IsWindows()) {
    if ((rc = sys_statfs(path, &m)) != -1) {
      statfs2cosmo(sf, &m);
    }
  } else {
    rc = sys_statfs_nt(path, sf);
  }

  END_CANCELATION_POINT;
  STRACE("statfs(%#s, [%s]) → %d% m", path, DescribeStatfs(rc, sf));
  return rc;
}

__weak_reference(statfs, statfs64);
