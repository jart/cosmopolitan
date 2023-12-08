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
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Writes data from multiple buffers.
 *
 * This is the same thing as write() except it has multiple buffers.
 * This yields a performance boost in situations where it'd be expensive
 * to stitch data together using memcpy() or issuing multiple syscalls.
 * This wrapper is implemented so that writev() calls where iovlen<2 may
 * be passed to the kernel as write() instead. This yields a 100 cycle
 * performance boost in the case of a single small iovec.
 *
 * Please note that it's not an error for a short write to happen. This
 * can happen in the kernel if EINTR happens after some of the write has
 * been committed. It can also happen if we need to polyfill this system
 * call using write().
 *
 * @return number of bytes actually handed off, or -1 w/ errno
 * @cancelationpoint
 * @restartable
 */
ssize_t writev(int fd, const struct iovec *iov, int iovlen) {
  ssize_t rc;
  BEGIN_CANCELATION_POINT;

  if (fd < 0) {
    rc = ebadf();
  } else if (iovlen < 0) {
    rc = einval();
  } else if (IsAsan() && !__asan_is_valid_iov(iov, iovlen)) {
    rc = efault();
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = ebadf();  // posix specifies this when not open()'d for writing
  } else if (IsLinux() || IsXnu() || IsFreebsd() || IsOpenbsd() || IsNetbsd()) {
    if (iovlen == 1) {
      rc = sys_write(fd, iov[0].iov_base, iov[0].iov_len);
    } else {
      rc = sys_writev(fd, iov, iovlen);
    }
  } else if (fd >= g_fds.n) {
    rc = ebadf();
  } else if (IsMetal()) {
    rc = sys_writev_metal(g_fds.p + fd, iov, iovlen);
  } else if (IsWindows()) {
    rc = sys_writev_nt(fd, iov, iovlen);
  } else {
    rc = enosys();
  }

  END_CANCELATION_POINT;
  STRACE("writev(%d, %s, %d) → %'ld% m", fd,
         DescribeIovec(rc != -1 ? rc : -2, iov, iovlen), iovlen, rc);
  return rc;
}
