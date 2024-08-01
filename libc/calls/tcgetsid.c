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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/intrin/strace.h"
#include "libc/sysv/errfuns.h"

#define TIOCGSID (IsLinux() ? 0x5429 : 0x40047463)

/**
 * Returns session id controlling terminal.
 */
int tcgetsid(int fd) {
  int rc, sid;
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotty();
  } else {
    rc = sys_ioctl(fd, TIOCGSID, &sid);
  }
  STRACE("tcgetsid(%d) → %d% m", fd, rc);
  return rc != -1 ? sid : -1;
}
