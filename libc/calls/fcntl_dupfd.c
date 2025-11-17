/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/errfuns.h"

int __fcntl_dupfd(int fd, ...) {
  int rc;
  int arg;
  va_list va;
  va_start(va, fd);
  arg = va_arg(va, int);
  va_end(va);
  if (!IsWindows()) {
    rc = __sys_fcntl(fd, F_DUPFD, arg);
  } else if (arg < 0) {
    rc = einval();
  } else if (__isfdopen(fd)) {
    rc = sys_dup_nt(fd, -1, 0, arg);
  } else {
    rc = ebadf();
  }
  if (rc != -1 && _weaken(__zipos_postdup))
    _weaken(__zipos_postdup)(fd, rc);
  STRACE("fcntl(%d, F_DUPFD, %d) → %d% m", fd, arg, rc);
  return rc;
}
