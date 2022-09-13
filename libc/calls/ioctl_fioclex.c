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
#include "libc/calls/ioctl.h"
#include "libc/intrin/strace.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Sets "close on exec" on file descriptor the fast way.
 *
 * @see ioctl(fd, FIOCLEX, 0) dispatches here
 */
int ioctl_fioclex(int fd, int req) {
  int rc;
  if (fd >= 0) {
    if (IsWindows() || (fd < g_fds.n && g_fds.p[fd].kind == kFdZip)) {
      if (__isfdopen(fd)) {
        if (req == FIOCLEX) {
          g_fds.p[fd].flags |= O_CLOEXEC;
        } else {
          g_fds.p[fd].flags &= ~O_CLOEXEC;
        }
        rc = 0;
      } else {
        rc = ebadf();
      }
    } else {
      rc = sys_ioctl(fd, req);
    }
  } else {
    rc = einval();
  }
  STRACE("%s(%d, %d) → %d% m", "ioctl_fioclex", fd, req, rc);
  return rc;
}
