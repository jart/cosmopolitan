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
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/errfuns.h"

/**
 * Reads data from file descriptor.
 *
 * This function changes the current file position. For documentation
 * on file position behaviors and gotchas, see the lseek() function.
 * This function may be used on socket file descriptors, including on
 * Windows.
 *
 * @param fd is something open()'d earlier
 * @param buf is copied into, cf. copy_file_range(), sendfile(), etc.
 * @param size in range [1..0x7ffff000] is reasonable
 * @return [1..size] bytes on success, 0 on EOF, or -1 w/ errno; with
 *     exception of size==0, in which case return zero means no error
 * @raise EBADF if `fd` is negative or not an open file descriptor
 * @raise EBADF if `fd` is open in `O_WRONLY` mode
 * @raise EFAULT if `size` is nonzero and `buf` points to bad memory
 * @raise EPERM if pledge() is in play without the stdio promise
 * @raise EIO if low-level i/o error happened
 * @raise EINTR if signal was delivered instead
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise ENOTCONN if `fd` is a socket and it isn't connected
 * @raise ECONNRESET if socket peer forcibly closed connection
 * @raise ETIMEDOUT if socket transmission timeout occurred
 * @raise EAGAIN if `O_NONBLOCK` is in play and read needs to block,
 *     or `SO_RCVTIMEO` is in play and the time interval elapsed
 * @raise ENOBUFS is specified by POSIX
 * @raise ENXIO is specified by POSIX
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable
 * @vforksafe
 */
ssize_t read(int fd, void *buf, size_t size) {
  ssize_t rc;
  BEGIN_CANCELATION_POINT;

  if (fd < 0) {
    rc = ebadf();
  } else if ((!buf && size) || (IsAsan() && !__asan_is_valid(buf, size))) {
    rc = efault();
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = _weaken(__zipos_read)(
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle,
        &(struct iovec){buf, size}, 1, -1);
  } else if (IsLinux() || IsXnu() || IsFreebsd() || IsOpenbsd() || IsNetbsd()) {
    rc = sys_read(fd, buf, size);
  } else if (fd >= g_fds.n) {
    rc = ebadf();
  } else if (IsMetal()) {
    rc = sys_readv_metal(fd, &(struct iovec){buf, size}, 1);
  } else if (IsWindows()) {
    rc = sys_readv_nt(fd, &(struct iovec){buf, size}, 1);
  } else {
    rc = enosys();
  }

  END_CANCELATION_POINT;
  DATATRACE("read(%d, [%#.*hhs%s], %'zu) → %'zd% m", fd,
            (int)MAX(0, MIN(40, rc)), buf, rc > 40 ? "..." : "", size, rc);
  return rc;
}
