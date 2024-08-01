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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

int __utimens(int fd, const char *path, const struct timespec ts[2],
              int flags) {
  int rc;
  struct ZiposUri zipname;
  if (IsMetal()) {
    rc = enosys();
  } else if ((flags & ~AT_SYMLINK_NOFOLLOW)) {
    rc = einval();  // unsupported flag
  } else if (__isfdkind(fd, kFdZip) ||
             (path && (_weaken(__zipos_parseuri) &&
                       _weaken(__zipos_parseuri)(path, &zipname) != -1))) {
    rc = erofs();
  } else if (IsXnu() || (IsLinux() && !__is_linux_2_6_23())) {
    rc = sys_utimensat_old(fd, path, ts, flags);
  } else if (IsLinux() || IsFreebsd() || IsOpenbsd() || IsNetbsd()) {
    rc = sys_utimensat(fd, path, ts, flags);
  } else if (IsWindows()) {
    rc = sys_utimensat_nt(fd, path, ts, flags);
  } else if (IsMetal()) {
    rc = enosys();
  } else {
    rc = enosys();
  }
  return rc;
}
