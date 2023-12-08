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
#include "libc/calls/createfileflags.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static int __zipos_dupfd(int fd, int cmd, int start) {
  int rc;
  if (start < 0) return einval();
  if (IsWindows()) {
    return sys_dup_nt(fd, -1, (cmd == F_DUPFD_CLOEXEC ? _O_CLOEXEC : 0),
                      start);
  }
  rc = sys_fcntl(fd, cmd, start, __sys_fcntl);
  if (rc != -1) {
    __zipos_postdup(fd, rc);
  }
  return rc;
}

int __zipos_fcntl(int fd, int cmd, uintptr_t arg) {
  if (cmd == F_GETFD) {
    if (g_fds.p[fd].flags & O_CLOEXEC) {
      return FD_CLOEXEC;
    } else {
      return 0;
    }
  } else if (cmd == F_SETFD) {
    if (arg & FD_CLOEXEC) {
      g_fds.p[fd].flags |= O_CLOEXEC;
      return FD_CLOEXEC;
    } else {
      g_fds.p[fd].flags &= ~O_CLOEXEC;
      return 0;
    }
  } else if (cmd == F_DUPFD || cmd == F_DUPFD_CLOEXEC) {
    return __zipos_dupfd(fd, cmd, arg);
  } else {
    return einval();
  }
}
