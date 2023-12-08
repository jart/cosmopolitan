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
#include "libc/calls/internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/console.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

/**
 * Sets foreground process group id.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `pgrp` is invalid
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise EPERM if `pgrp` didn't match process in our group
 * @raise ENOTTY if `fd` is isn't controlling teletypewriter
 * @raise EIO if process group of writer is orphoned, calling thread is
 *     not blocking `SIGTTOU`, and process isn't ignoring `SIGTTOU`
 * @asyncsignalsafe
 */
int tcsetpgrp(int fd, int pgrp) {
  int rc;
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotty();
  } else if (IsWindows() || IsMetal()) {
    if (sys_isatty(fd)) {
      rc = 0;
    } else {
      rc = -1;  // ebadf, enotty
    }
  } else {
    rc = sys_ioctl(fd, TIOCSPGRP, &pgrp);
  }
  STRACE("tcsetpgrp(%d, %d) → %d% m", fd, pgrp, rc);
  return rc;
}
