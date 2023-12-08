/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Syncs filesystem associated with file descriptor.
 *
 * Since Linux 5.8, syncfs() will report an error if an inode failed to
 * be written during the time since syncfs() was last called.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise EIO if some kind of data/metadata write error happened
 * @raise ENOSPC if disk space was exhausted during sync
 * @raise EDQUOT (or ENOSPC) on some kinds of NFS errors
 * @raise EBADF if `fd` isn't a valid file descriptor
 * @raise EROFS if `fd` is a zip file
 * @raise ENOSYS on non-Linux
 */
int syncfs(int fd) {
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    return erofs();
  } else {
    return sys_syncfs(fd);
  }
}
