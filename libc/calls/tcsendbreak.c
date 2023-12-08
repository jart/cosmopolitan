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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/comms.h"
#include "libc/sysv/errfuns.h"

#define TCSBRK   0x5409      // linux
#define TIOCSBRK 0x2000747b  // bsd
#define TIOCCBRK 0x2000747a  // bsd

static int sys_tcsendbreak_bsd(int fd) {
  if (sys_ioctl(fd, TIOCSBRK, 0) == -1) return -1;
  usleep(400000);
  if (sys_ioctl(fd, TIOCCBRK, 0) == -1) return -1;
  return 0;
}

static textwindows int sys_tcsendbreak_nt(int fd) {
  if (!__isfdopen(fd)) return ebadf();
  if (!TransmitCommChar(g_fds.p[fd].handle, '\0')) return __winerr();
  return 0;
}

/**
 * Sends break.
 *
 * @param fd is file descriptor of tty
 * @param duration of 0 sends a break for 0.25-0.5 seconds, and other
 *     durations are treated the same by this implementation
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise ENOTTY if `fd` is open but not a teletypewriter
 * @raise EIO if process group of writer is orphoned, calling thread is
 *     not blocking `SIGTTOU`, and process isn't ignoring `SIGTTOU`
 * @raise ENOSYS on bare metal
 * @asyncsignalsafe
 */
int tcsendbreak(int fd, int duration) {
  int rc;
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotty();
  } else if (IsLinux()) {
    rc = sys_ioctl(fd, TCSBRK, 0);
  } else if (IsBsd()) {
    rc = sys_tcsendbreak_bsd(fd);
  } else if (IsWindows()) {
    rc = sys_tcsendbreak_nt(fd);
  } else {
    rc = enosys();
  }
  STRACE("tcsendbreak(%d, %u) → %d% m", fd, duration, rc);
  return rc;
}
