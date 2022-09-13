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

/**
 * Closes inclusive range of file descriptors, e.g.
 *
 *     // close all non-stdio file descriptors
 *     if (close_range(3, -1, 0) == -1) {
 *       for (int i = 3; i < 256; ++i) {
 *         close(i);
 *       }
 *     }
 *
 * This is supported on Linux 5.9+, FreeBSD, and OpenBSD. On FreeBSD,
 * `flags` must be zero. On OpenBSD, we call closefrom(int) so `last`
 * should be `-1` in order to get OpenBSD support, otherwise `ENOSYS`
 * will be returned. We also polyfill closefrom on FreeBSD since it's
 * available on older kernels.
 *
 * On Linux, the following flags are supported:
 *
 * - CLOSE_RANGE_UNSHARE
 * - CLOSE_RANGE_CLOEXEC
 *
 * @return 0 on success, or -1 w/ errno
 * @error ENOSYS if not Linux 5.9+ / FreeBSD / OpenBSD
 * @error EBADF on OpenBSD if `first` is greater than highest fd
 * @error EINVAL if flags are bad or first is greater than last
 * @error EMFILE if a weird race condition happens on Linux
 * @error EINTR possibly on OpenBSD
 * @error ENOMEM on Linux maybe
 */
int close_range(unsigned int first, unsigned int last, unsigned int flags) {
  int rc, err;
  err = errno;
  if ((rc = sys_close_range(first, last, flags)) == -1) {
    if (errno == ENOSYS && first <= INT_MAX && last == UINT_MAX && !flags) {
      errno = err;
      rc = sys_closefrom(first);
    }
  }
  STRACE("close_range(%d, %d, %#x) → %d% m", first, last, flags, rc);
  return rc;
}
