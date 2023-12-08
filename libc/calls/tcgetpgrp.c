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

#define TIOCGPGRP_linux 0x0000540f
#define TIOCGPGRP_bsd   0x40047477

/**
 * Returns which process group controls terminal.
 *
 * @return process group id on success, or -1 w/ errno
 * @raise ENOTTY if `fd` is isn't controlling teletypewriter
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise ENOSYS on Windows and Bare Metal
 * @asyncsignalsafe
 */
int tcgetpgrp(int fd) {
  int rc, pgrp;
  if (IsLinux()) {
    rc = sys_ioctl(fd, TIOCGPGRP_linux, &pgrp);
  } else if (IsBsd()) {
    rc = sys_ioctl(fd, TIOCGPGRP_bsd, &pgrp);
  } else if (sys_isatty(fd)) {
    pgrp = rc = getpid();
  } else {
    rc = -1;  // ebadf, enotty
  }
  STRACE("tcgetpgrp(%d) → %d% m", fd, rc == -1 ? rc : pgrp);
  return rc == -1 ? rc : pgrp;
}
