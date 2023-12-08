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
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Duplicates file descriptor, granting it specific number.
 *
 * Unlike dup3(), the dup2() function permits oldfd and newfd to be the
 * same, in which case the only thing this function does is test if
 * oldfd is open.
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
 * @param oldfd isn't closed afterwards
 * @param newfd if already assigned, is silently closed beforehand;
 *     unless it's equal to oldfd, in which case dup2() is a no-op
 * @return new file descriptor, or -1 w/ errno
 * @raise EPERM if pledge() is in play without stdio
 * @raise EMFILE if `RLIMIT_NOFILE` has been reached
 * @raise ENOTSUP if `oldfd` is on zip file system
 * @raise EINTR if a signal handler was called
 * @raise EBADF is `newfd` negative or too big
 * @raise EBADF is `oldfd` isn't open
 * @asyncsignalsafe
 * @vforksafe
 */
int dup2(int oldfd, int newfd) {
  int rc;
  // helps guarantee stderr log gets duplicated before user closes
  if (_weaken(kloghandle)) _weaken(kloghandle)();
#ifdef __aarch64__
  if (oldfd == newfd) {
    // linux aarch64 defines dup3() but not dup2(), which wasn't such a
    // great decision, since the two syscalls don't behave the same way
    if (!(rc = read(oldfd, 0, 0))) rc = oldfd;
  } else
#endif
  if (!IsWindows()) {
    if (__isfdkind(oldfd, kFdZip) || __isfdkind(newfd, kFdZip)) {
      if (__vforked) {
        return enotsup();
      }
      rc = sys_dup2(oldfd, newfd, 0);
      if (rc != -1) {
        _weaken(__zipos_postdup)(oldfd, newfd);
      }
    } else {
      rc = sys_dup2(oldfd, newfd, 0);
    }
  } else if (newfd < 0) {
    rc = ebadf();
  } else if (oldfd == newfd) {
    if (__isfdopen(oldfd)) {
      rc = newfd;
    } else {
      rc = ebadf();
    }
  } else {
    rc = sys_dup_nt(oldfd, newfd, 0, -1);
  }
  STRACE("dup2(%d, %d) → %d% m", oldfd, newfd, rc);
  return rc;
}
