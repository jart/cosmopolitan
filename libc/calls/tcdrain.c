/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/files.h"
#include "libc/sysv/errfuns.h"

static textwindows int sys_tcdrain_nt(int fd) {
  if (!__isfdopen(fd)) return ebadf();
  if (_check_interrupts(false, g_fds.p)) return -1;
  if (!FlushFileBuffers(g_fds.p[fd].handle)) return __winerr();
  return 0;
}

/**
 * Waits until all written output is transmitted.
 *
 * @param fd is file descriptor of tty
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise ENOTTY if `fd` is open but not a teletypewriter
 * @raise EIO if process group of writer is orphoned, calling thread is
 *     not blocking `SIGTTOU`, and process isn't ignoring `SIGTTOU`
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if signal was delivered
 * @raise ENOSYS on bare metal
 * @cancellationpoint
 * @asyncsignalsafe
 */
int tcdrain(int fd) {
  int rc;
  BEGIN_CANCELLATION_POINT;
  if (IsMetal()) {
    rc = enosys();
  } else if (!IsWindows()) {
    rc = sys_ioctl_cp(fd, TCSBRK, (void *)(intptr_t)1);
  } else {
    rc = sys_tcdrain_nt(fd);
  }
  END_CANCELLATION_POINT;
  STRACE("tcdrain(%d) → %d% m", fd, rc);
  return rc;
}
