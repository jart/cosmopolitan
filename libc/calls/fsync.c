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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/errfuns.h"

/**
 * Blocks until kernel flushes buffers for fd to disk.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EROFS if `fd` is on a read-only filesystem e.g. /zip
 * @raise EINVAL if `fd` is a special file w/o synchronization
 * @raise ENOSPC if disk space was exhausted
 * @raise EBADF if `fd` isn't an open file
 * @raise EINTR if signal was delivered
 * @raise EIO if an i/o error happened
 * @see __nosync to secretly disable
 * @see fdatasync()
 * @cancelationpoint
 * @asyncsignalsafe
 */
int fsync(int fd) {
  int rc;
  bool fake = __nosync == 0x5453455454534146;
  BEGIN_CANCELATION_POINT;
  if (__isfdkind(fd, kFdZip)) {
    rc = erofs();
  } else if (!IsWindows()) {
    if (!fake) {
      rc = sys_fsync(fd);
    } else {
      rc = sys_fsync_fake(fd);
    }
  } else {
    rc = sys_fdatasync_nt(fd, fake);
  }
  END_CANCELATION_POINT;
  STRACE("fsync%s(%d) → %d% m", fake ? "_fake" : "", fd, rc);
  return rc;
}
