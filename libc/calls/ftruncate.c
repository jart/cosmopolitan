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
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Changes size of open file.
 *
 * If the file size is increased, the extended area shall appear as if
 * it were zero-filled. If your file size is decreased, the extra data
 * shall be lost.
 *
 * This function never changes the file position. This is true even if
 * ftruncate() causes the position to become beyond the end of file in
 * which case, the rules described in the lseek() documentation apply.
 *
 * Some operating systems implement an optimization, where `length` is
 * treated as a logical size and the requested physical space won't be
 * allocated until non-zero values get written into it. Our tests show
 * this happens on Linux (usually with 4096 byte granularity), FreeBSD
 * (which favors 512-byte granularity), and MacOS (prefers 4096 bytes)
 * however Windows, OpenBSD, and NetBSD always reserve physical space.
 * This may be inspected using fstat() and consulting stat::st_blocks.
 *
 * @param fd must be open for writing
 * @param length may be greater than current current file size
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `length` is negative
 * @raise EINTR if signal was delivered instead
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EIO if a low-level i/o error happened
 * @raise EFBIG or EINVAL if `length` is too huge
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise EINVAL if `fd` is a non-file, e.g. pipe, socket
 * @raise EINVAL if `fd` wasn't opened in a writeable mode
 * @raise EROFS if `fd` is on a read-only filesystem (e.g. zipos)
 * @raise ENOSYS on bare metal
 * @cancelationpoint
 * @asyncsignalsafe
 */
int ftruncate(int fd, int64_t length) {
  int rc;
  BEGIN_CANCELATION_POINT;

  if (fd < 0) {
    rc = ebadf();
  } else if (__isfdkind(fd, kFdZip)) {
    rc = erofs();
  } else if (IsMetal()) {
    rc = enosys();
  } else if (!IsWindows()) {
    rc = sys_ftruncate(fd, length, length);
    if (IsNetbsd() && rc == -1 && errno == ENOSPC) {
      errno = EFBIG;  // POSIX doesn't specify ENOSPC for ftruncate()
    }
  } else if (__isfdopen(fd)) {
    rc = sys_ftruncate_nt(g_fds.p[fd].handle, length);
  } else {
    rc = ebadf();
  }

  END_CANCELATION_POINT;
  STRACE("ftruncate(%d, %'ld) → %d% m", fd, length, rc);
  return rc;
}

__weak_reference(ftruncate, ftruncate64);
