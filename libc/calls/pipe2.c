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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates file-less file descriptors for interprocess communication.
 *
 * This function offers atomic operation on all supported platforms
 * except for XNU and RHEL5 where it's polyfilled.
 *
 * @params flags may contain `O_CLOEXEC`, `O_NONBLOCK`, or the non-POSIX
 *     packet mode flag `O_DIRECT`, which is `EINVAL` on MacOS / OpenBSD
 * @raise EINVAL if flags has invalid or unsupported bits
 * @raise EFAULT if `pipefd` doesn't point to valid memory
 * @raise EMFILE if process `RLIMIT_NOFILE` has been reached
 * @raise ENFILE if system-wide file limit has been reached
 * @param pipefd is used to return (reader, writer) file descriptors
 * @param flags can have O_CLOEXEC or O_DIRECT or O_NONBLOCK
 * @return 0 on success, or -1 w/ errno and pipefd isn't modified
 */
int pipe2(int pipefd[hasatleast 2], int flags) {
  int rc;
  if (flags & ~(O_CLOEXEC | O_NONBLOCK | (O_DIRECT != -1u ? O_DIRECT : 0))) {
    return einval();
  } else if (!pipefd ||
             (IsAsan() && !__asan_is_valid(pipefd, sizeof(int) * 2))) {
    rc = efault();
  } else if (!IsWindows()) {
    rc = sys_pipe2(pipefd, flags);
  } else {
    rc = sys_pipe_nt(pipefd, flags);
  }
  if (!rc) {
    STRACE("pipe2([{%d, %d}], %#o) → %d% m", pipefd[0], pipefd[1], flags, rc);
  } else {
    STRACE("pipe2(%p, %#o) → %d% m", pipefd, flags, rc);
  }
  return rc;
}
