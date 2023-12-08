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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/strace.internal.h"
#include "libc/mem/alloca.h"
#include "libc/nt/comms.h"
#include "libc/nt/console.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

#define TCFLSH    0x0000540b
#define TIOCFLUSH 0x80047410

#define kNtPurgeTxclear 4
#define kNtPurgeRxclear 8

static const char *DescribeFlush(char buf[12], int action) {
  if (action == TCIFLUSH) return "TCIFLUSH";
  if (action == TCOFLUSH) return "TCOFLUSH";
  if (action == TCIOFLUSH) return "TCIOFLUSH";
  FormatInt32(buf, action);
  return buf;
}

static dontinline textwindows int sys_tcflush_nt(int fd, int queue) {
  if (!sys_isatty(fd)) {
    return -1;  // ebadf, enotty
  }
  if (queue == TCOFLUSH) {
    return 0;  // windows console output is never buffered
  }
  return FlushConsoleInputBytes();
}

/**
 * Discards queued data on teletypewriter.
 *
 * @param queue may be one of:
 *     - `TCIFLUSH` flushes data received but not read
 *     - `TCOFLUSH` flushes data written but not transmitted
 *     - `TCIOFLUSH` does both `TCOFLUSH` and `TCIFLUSH`
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `action` is invalid
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise ENOTTY if `fd` is open but not a teletypewriter
 * @raise EIO if process group of writer is orphoned, calling thread is
 *     not blocking `SIGTTOU`, and process isn't ignoring `SIGTTOU`
 * @raise ENOSYS on bare metal
 * @asyncsignalsafe
 */
int tcflush(int fd, int queue) {
  int rc;
  if (queue != TCIFLUSH && queue != TCOFLUSH && queue != TCIOFLUSH) {
    rc = einval();
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotty();
  } else if (IsLinux()) {
    rc = sys_ioctl(fd, TCFLSH, queue);
  } else if (IsBsd()) {
    rc = sys_ioctl(fd, TIOCFLUSH, &queue);
  } else if (IsWindows()) {
    rc = sys_tcflush_nt(fd, queue);
  } else {
    rc = enosys();
  }
  STRACE("tcflush(%d, %s) → %d% m", fd, DescribeFlush(alloca(12), queue), rc);
  return rc;
}
