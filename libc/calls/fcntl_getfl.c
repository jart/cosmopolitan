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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/pib.h"

#define o_nonblock 0004
#define o_append   0010
#define o_async    0100
#define o_direct   (IsFreebsd() ? 0x00010000 : IsNetbsd() ? 0x00080000 : 0)

static int __unxoflags(int flags) {
  if (flags == -1)
    return -1;
  if (IsLinux())
    return flags;
  int flags2 = flags & O_ACCMODE;
  if (flags & 4)
    flags2 |= O_NONBLOCK;
  if (flags & 8)
    flags2 |= O_APPEND;
  if (flags & 128)
    flags2 |= O_SYNC;
  if (flags & (IsXnu()       ? 0x00400000
               : IsFreebsd() ? 0x01000000
               : IsOpenbsd() ? 0x00000080
               : IsNetbsd()  ? 0x00010000
                             : 0))
    flags2 |= O_DSYNC;
  if (flags & (IsFreebsd() ? 0x00010000 : IsNetbsd() ? 0x00080000 : 0))
    flags2 |= O_DIRECT;
  if (flags & (IsXnu()       ? 0x01000000
               : IsFreebsd() ? 0x00100000
               : IsOpenbsd() ? 0x00010000
               : IsNetbsd()  ? 0x00400000
                             : 0))
    flags2 |= O_CLOEXEC;
  return flags2;
}

int __fcntl_getfl(int fd) {
  int rc;
  if (__isfdkind(fd, kFdZip)) {
    goto WindowsImpl;
  } else if (!IsWindows()) {
    rc = __sys_fcntl(fd, F_GETFL, 0);
    if (rc != -1 && !IsLinux())
      rc = __unxoflags(rc);
  } else if (__isfdopen(fd)) {
  WindowsImpl:
    rc = __get_pib()->fds.p[fd].flags &
         (O_ACCMODE | O_APPEND | O_DIRECT | O_NONBLOCK);
  } else {
    rc = ebadf();
  }
  STRACE("fcntl(%d, F_GETFL) → %s% m", fd, DescribeOpenFlags(rc));
  return rc;
}
