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
#include "libc/calls/internal.h"
#include "libc/nt/files.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

textwindows int fcntl$nt(int fd, int cmd, unsigned arg) {
  uint32_t flags;
  if (!__isfdkind(fd, kFdFile)) return ebadf();
  switch (cmd) {
    case F_GETFL:
      return g_fds.p[fd].flags;
    case F_SETFL:
      return (g_fds.p[fd].flags = arg);
    case F_GETFD:
      if (g_fds.p[fd].flags & O_CLOEXEC) {
        return FD_CLOEXEC;
      } else {
        return 0;
      }
    case F_SETFD:
      if (arg & O_CLOEXEC) {
        g_fds.p[fd].flags |= O_CLOEXEC;
        return FD_CLOEXEC;
      } else {
        g_fds.p[fd].flags &= ~O_CLOEXEC;
        return 0;
      }
    default:
      return 0; /* TODO(jart): Implement me. */
  }
}
