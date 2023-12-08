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
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Duplicates file descriptor/handle.
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
 * @param flags may have O_CLOEXEC which is needed to preserve the
 *     close-on-execve() state after file descriptor duplication
 * @return newfd on success, or -1 w/ errno
 * @raise ENOTSUP if `oldfd` is a zip file descriptor
 * @raise EPERM if pledge() is in play without stdio
 * @raise EINVAL if `flags` has unsupported bits
 * @raise EINTR if a signal handler was called
 * @raise EBADF is `newfd` negative or too big
 * @raise EINVAL if `newfd` equals oldfd
 * @raise EBADF is `oldfd` isn't open
 * @see dup(), dup2()
 */
int dup3(int oldfd, int newfd, int flags) {
  int rc;
  // helps guarantee stderr log gets duplicated before user closes
  if (_weaken(kloghandle)) _weaken(kloghandle)();
  if (oldfd == newfd || (flags & ~O_CLOEXEC)) {
    rc = einval();  // NetBSD doesn't do this
  } else if (oldfd < 0 || newfd < 0) {
    rc = ebadf();
  } else if (!IsWindows()) {
    if (__isfdkind(oldfd, kFdZip) || __isfdkind(newfd, kFdZip)) {
      if (__vforked) {
        return enotsup();
      }
      rc = sys_dup3(oldfd, newfd, flags);
      if (rc != -1) {
        _weaken(__zipos_postdup)(oldfd, newfd);
      }
    } else {
      rc = sys_dup3(oldfd, newfd, flags);
    }
  } else {
    rc = sys_dup_nt(oldfd, newfd, flags, -1);
  }
  STRACE("dup3(%d, %d, %d) → %d% m", oldfd, newfd, flags, rc);
  return rc;
}
