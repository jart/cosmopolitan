/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/strace.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/sysv/errfuns.h"

/**
 * Closes extra file descriptors, e.g.
 *
 *     // close all non-stdio file descriptors
 *     if (closefrom(3) == -1) {
 *       for (int i = 3; i < 256; ++i) {
 *         close(i);
 *       }
 *     }
 *
 * @return 0 on success, or -1 w/ errno
 * @error ENOSYS if not Linux 5.9+ / FreeBSD / OpenBSD
 * @error EBADF on OpenBSD if `first` is greater than highest fd
 * @error EINVAL if flags are bad or first is greater than last
 * @error EMFILE if a weird race condition happens on Linux
 * @error EINTR possibly on OpenBSD
 * @error ENOMEM on Linux maybe
 * @note supported on Linux 5.9+, FreeBSD 8+, and OpenBSD
 */
int closefrom(int first) {
  int rc, err;
  if (first >= 0) {
    err = errno;
    if ((rc = sys_close_range(first, -1, 0)) == -1) {
      if (errno == ENOSYS) {
        errno = err;
        rc = sys_closefrom(first);
      }
    }
  } else {
    rc = ebadf();
  }
  STRACE("closefrom(%d) → %d% m", first, rc);
  return rc;
}
