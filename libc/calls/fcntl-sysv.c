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
#include "libc/calls/struct/flock.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

int sys_fcntl(int fd, int cmd, uintptr_t arg, int impl(int, int, ...)) {
  int e, rc;
  bool islock;
  if ((islock = cmd == F_GETLK ||  //
                cmd == F_SETLK ||  //
                cmd == F_SETLKW)) {
    if ((!IsAsan() && !arg) ||
        (IsAsan() &&
         !__asan_is_valid((struct flock *)arg, sizeof(struct flock)))) {
      return efault();
    }
    cosmo2flock(arg);
  }
  e = errno;
  rc = impl(fd, cmd, arg);
  if (islock) {
    flock2cosmo(arg);
  } else if (rc == -1 && cmd == F_DUPFD_CLOEXEC && errno == EINVAL) {
    errno = e;
    rc = __fixupnewfd(impl(fd, F_DUPFD, arg), O_CLOEXEC);
  }
  return rc;
}
