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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Sets current directory based on file descriptor.
 *
 * This does *not* update the `PWD` environment variable.
 *
 * @raise EACCES if search permission was denied on directory
 * @raise ENOTDIR if `dirfd` doesn't refer to a directory
 * @raise EBADF if `dirfd` isn't a valid file descriptor
 * @raise ENOTSUP if `dirfd` refers to `/zip/...` file
 * @see open(path, O_DIRECTORY)
 * @asyncsignalsafe
 */
int fchdir(int dirfd) {
  int rc;
  if (__isfdkind(dirfd, kFdZip)) {
    rc = enotsup();
  } else if (!IsWindows()) {
    rc = sys_fchdir(dirfd);
  } else {
    rc = sys_fchdir_nt(dirfd);
  }
  STRACE("fchdir(%d) → %d% m", dirfd, rc);
  return rc;
}
