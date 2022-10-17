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
#include "libc/calls/internal.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/strace.internal.h"
#include "libc/mem/alloca.h"
#include "libc/nt/comms.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

#define kNtPurgeTxabort 1
#define kNtPurgeRxabort 2

static const char *DescribeFlow(char buf[12], int action) {
  if (action == TCOOFF) return "TCOOFF";
  if (action == TCOON) return "TCOON";
  if (action == TCIOFF) return "TCIOFF";
  if (action == TCION) return "TCION";
  FormatInt32(buf, action);
  return buf;
}

static int sys_tcflow_bsd(int fd, int action) {
  int rc;
  uint8_t c;
  struct termios t;
  if (action == TCOOFF) return sys_ioctl(fd, TIOCSTOP, 0);
  if (action == TCOON) return sys_ioctl(fd, TIOCSTART, 0);
  if (action != TCIOFF && action != TCION) return einval();
  if (sys_ioctl(fd, TCGETS, &t) == -1) return -1;
  c = t.c_cc[action == TCIOFF ? VSTOP : VSTART];
  if (c == 255) return 0;  // code is disabled
  if (sys_write(fd, &c, 1) == -1) return -1;
  return 0;
}

static dontinline textwindows int sys_tcflow_nt(int fd, int action) {
  bool32 ok;
  int64_t h;
  if (!__isfdopen(fd)) return ebadf();
  h = g_fds.p[fd].handle;
  if (action == TCOOFF) {
    ok = PurgeComm(h, kNtPurgeTxabort);
  } else if (action == TCIOFF) {
    ok = PurgeComm(h, kNtPurgeRxabort);
  } else if (action == TCOON || action == TCION) {
    ok = ClearCommBreak(h);
  } else {
    return einval();
  }
  return ok ? 0 : __winerr();
}

/**
 * Changes flow of teletypewriter data.
 *
 * @param fd is file descriptor of tty
 * @param action may be one of:
 *     - `TCOOFF` to suspend output
 *     - `TCOON` to resume output
 *     - `TCIOFF` to transmit a `STOP` character
 *     - `TCION` to transmit a `START` character
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `action` is invalid
 * @raise ENOSYS on Windows and Bare Metal
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise ENOTTY if `fd` is open but not a teletypewriter
 * @raise EIO if process group of writer is orphoned, calling thread is
 *     not blocking `SIGTTOU`, and process isn't ignoring `SIGTTOU`
 * @asyncsignalsafe
 */
int tcflow(int fd, int action) {
  int rc;
  if (IsMetal()) {
    rc = enosys();
  } else if (IsBsd()) {
    rc = sys_ioctl(fd, TCXONC, action);
  } else if (!IsWindows()) {
    rc = sys_ioctl(fd, TCXONC, action);
  } else {
    rc = sys_tcflow_nt(fd, action);
  }
  STRACE("tcflow(%d, %s) → %d% m", fd, DescribeFlow(alloca(12), action), rc);
  return rc;
}
