/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/files.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

#define TCSBRK    0x5409      // linux
#define TIOCDRAIN 0x2000745e  // xnu, freebsd, openbsd, netbsd

static dontinline textwindows int sys_tcdrain_nt(int fd) {
  if (!sys_isatty(fd)) return -1;  // ebadf, enotty
  // Tried FlushFileBuffers but it made Emacs hang when run in cmd.exe
  // "Console output is not buffered." -Quoth MSDN on FlushFileBuffers
  return 0;
}

/**
 * Waits until all written output is transmitted.
 *
 * @param fd is file descriptor of tty
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise ENOTTY if `fd` is open but not a teletypewriter
 * @raise EIO if process group of writer is orphaned, calling thread is
 *     not blocking `SIGTTOU`, and process isn't ignoring `SIGTTOU`
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @raise ENOSYS on bare metal
 * @cancelationpoint
 * @asyncsignalsafe
 */
int tcdrain(int fd) {
  int rc;
  BEGIN_CANCELATION_POINT;
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotty();
  } else if (IsLinux()) {
    rc = sys_ioctl_cp(fd, TCSBRK, (uintptr_t)1);
  } else if (IsBsd()) {
    rc = sys_ioctl_cp(fd, TIOCDRAIN, 0);
  } else if (IsWindows()) {
    rc = sys_tcdrain_nt(fd);
  } else {
    rc = enosys();
  }
  END_CANCELATION_POINT;
  STRACE("tcdrain(%d) → %d% m", fd, rc);
  return rc;
}
