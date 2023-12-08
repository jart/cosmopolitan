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
#include "libc/sock/sock.h"
#include "libc/sysv/errfuns.h"

/**
 * Writes data to file descriptor.
 *
 * This function changes the current file position. For documentation
 * on file position behaviors and gotchas, see the lseek() function.
 * This function may be used on socket file descriptors, including on
 * Windows.
 *
 * @param fd is open file descriptor
 * @param buf is copied from, cf. copy_file_range(), sendfile(), etc.
 * @return [1..size] bytes on success, or -1 w/ errno; noting zero is
 *     impossible unless size was passed as zero to do an error check
 * @raise EBADF if `fd` is negative or not an open file descriptor
 * @raise EBADF if `fd` wasn't opened with `O_WRONLY` or `O_RDWR`
 * @raise EPIPE if `fd` is a pipe whose other reader end was closed,
 *     after the `SIGPIPE` signal was delivered, blocked, or ignored
 * @raise EFBIG if `RLIMIT_FSIZE` soft limit was exceeded, after the
 *     `SIGXFSZ` signal was either delivered, blocked, or ignored
 * @raise EFAULT if `size` is nonzero and `buf` points to bad memory
 * @raise EPERM if pledge() is in play without the stdio promise
 * @raise ENOSPC if device containing `fd` is full
 * @raise EIO if low-level i/o error happened
 * @raise EINTR if signal was delivered instead
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EAGAIN if `O_NONBLOCK` is in play and write needs to block
 * @raise ENOBUFS if kernel lacked internal resources; which FreeBSD
 *     and XNU say could happen with sockets, and OpenBSD documents it
 *     as a general possibility; whereas other system don't specify it
 * @raise ENXIO is specified only by POSIX and XNU when a request is
 *     made of a nonexistent device or outside device capabilities
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable
 * @vforksafe
 */
ssize_t write(int fd, const void *buf, size_t size) {
  ssize_t rc;
  BEGIN_CANCELATION_POINT;

  if (fd < 0) {
    rc = ebadf();
  } else if (IsAsan() && !__asan_is_valid(buf, size)) {
    rc = efault();
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = ebadf();  // posix specifies this when not open()'d for writing
  } else if (IsLinux() || IsXnu() || IsFreebsd() || IsOpenbsd() || IsNetbsd()) {
    rc = sys_write(fd, buf, size);
  } else if (fd >= g_fds.n) {
    rc = ebadf();
  } else if (IsMetal()) {
    rc = sys_writev_metal(g_fds.p + fd, &(struct iovec){(void *)buf, size}, 1);
  } else if (IsWindows()) {
    rc = sys_writev_nt(fd, &(struct iovec){(void *)buf, size}, 1);
  } else {
    rc = enosys();
  }

  END_CANCELATION_POINT;
  DATATRACE("write(%d, %#.*hhs%s, %'zu) → %'zd% m", fd, MAX(0, MIN(40, rc)),
            buf, rc > 40 ? "..." : "", size, rc);
  return rc;
}
