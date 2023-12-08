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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates file-less file descriptors for interprocess communication.
 *
 * @param fd is (reader, writer)
 * @return 0 on success or -1 w/ errno
 * @raise EFAULT if pipefd is NULL or an invalid address
 * @raise EMFILE if RLIMIT_NOFILE is exceedde
 * @asyncsignalsafe
 * @see pipe2()
 */
int pipe(int pipefd[hasatleast 2]) {
  int rc;
  if (!pipefd || (IsAsan() && !__asan_is_valid(pipefd, sizeof(int) * 2))) {
    // needed for windows which is polyfilled
    // needed for xnu and netbsd which don't take an argument
    rc = efault();
  } else if (!IsWindows()) {
    rc = sys_pipe(pipefd);
  } else {
    rc = sys_pipe_nt(pipefd, 0);
  }
  if (!rc) {
    STRACE("pipe([{%d, %d}]) → %d% m", pipefd[0], pipefd[1], rc);
  } else {
    STRACE("pipe(%p) → %d% m", pipefd, rc);
  }
  return rc;
}
