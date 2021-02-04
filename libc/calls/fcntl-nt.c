/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/files.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_fcntl_nt(int fd, int cmd, unsigned arg) {
  uint32_t flags;
  if (__isfdkind(fd, kFdFile) || __isfdkind(fd, kFdSocket)) {
    switch (cmd) {
      case F_GETFL:
        return g_fds.p[fd].flags & (O_ACCMODE | O_APPEND | O_ASYNC | O_DIRECT |
                                    O_NOATIME | O_NONBLOCK);
      case F_SETFL:
        /*
         * - O_APPEND doesn't appear to be tunable at cursory glance
         * - O_NONBLOCK might require we start doing all i/o in threads
         * - O_DSYNC / O_RSYNC / O_SYNC maybe if we fsync() everything
         */
        return einval();
      case F_GETFD:
        if (g_fds.p[fd].flags & O_CLOEXEC) {
          return FD_CLOEXEC;
        } else {
          return 0;
        }
      case F_SETFD:
        if (arg & FD_CLOEXEC) {
          g_fds.p[fd].flags |= O_CLOEXEC;
          return FD_CLOEXEC;
        } else {
          g_fds.p[fd].flags &= ~O_CLOEXEC;
          return 0;
        }
      default:
        return einval();
    }
  } else {
    return ebadf();
  }
}
