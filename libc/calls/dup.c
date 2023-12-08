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
#include "libc/intrin/weaken.h"
#include "libc/sysv/errfuns.h"
#include "libc/runtime/zipos.internal.h"

/**
 * Duplicates file descriptor.
 *
 * The `O_CLOEXEC` flag shall be cleared from the resulting file
 * descriptor; see dup3() to preserve it.
 *
 * One use case for duplicating file descriptors is to be able to
 * reassign an open()'d file or pipe() to the stdio of an executed
 * subprocess. On Windows, in order for this to work, the subprocess
 * needs to be a Cosmopolitan program that has socket() linked.
 *
 * Only small programs should duplicate sockets. That's because this
 * implementation uses DuplicateHandle() on Windows, which Microsoft
 * says might cause its resources to leak internally. Thus it likely
 * isn't a good idea to design a server that does it a lot and lives
 * a long time, without contributing a patch to this implementation.
 *
 * @param fd remains open afterwards
 * @return some arbitrary new number for fd
 * @raise EPERM if pledge() is in play without stdio
 * @raise ENOTSUP if `fd` is a zip file descriptor
 * @raise EBADF if `fd` is negative or not open
 * @asyncsignalsafe
 * @vforksafe
 */
int dup(int fd) {
  int rc;
  if (!IsWindows()) {
    rc = sys_dup(fd);
    if (rc != -1 && __isfdkind(fd, kFdZip)) {
      _weaken(__zipos_postdup)(fd, rc);
    }
  } else {
    rc = sys_dup_nt(fd, -1, 0, -1);
  }
  STRACE("%s(%d) → %d% m", "dup", fd, rc);
  return rc;
}
