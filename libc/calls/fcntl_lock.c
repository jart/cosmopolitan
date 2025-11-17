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
#include "libc/calls/cp.internal.h"
#include "libc/calls/flocks.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/flock.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/strace.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"

int __fcntl_lock(int fd, int cmd, ...) {
  int rc;
  va_list va;
  uintptr_t arg;
  va_start(va, cmd);
  arg = va_arg(va, uintptr_t);
  va_end(va);
  if (__isfdkind(fd, kFdZip)) {
    rc = einval();
  } else if (!IsWindows()) {
    cosmo2flock(arg);
    if (cmd == F_SETLKW) {
      BEGIN_CANCELATION_POINT;
      rc = __sys_fcntl_cp(fd, cmd, arg);
      END_CANCELATION_POINT;
    } else {
      rc = __sys_fcntl(fd, cmd, arg);
    }
    flock2cosmo(arg);
  } else if (__isfdopen(fd)) {
    BLOCK_SIGNALS;
    struct Fd *f = __get_pib()->fds.p + fd;
    if (f->cursor) {
      rc = __flocks_fcntl(f, fd, cmd, arg, _SigMask);
    } else {
      rc = ebadf();
    }
    ALLOW_SIGNALS;
  } else {
    rc = ebadf();
  }
  STRACE("fcntl(%d, %s, %s) → %d% m", fd, DescribeFcntlCmd(cmd),
         DescribeFlock(cmd, (struct flock *)arg), rc);
  return rc;
}
