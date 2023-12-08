/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

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
 * The following flags are available:
 *
 * - `CLOSE_RANGE_UNSHARE` (Linux-only)
 * - `CLOSE_RANGE_CLOEXEC` (Linux-only)
 *
 * This is only supported on Linux 5.9+ and FreeBSD 13+. Consider using
 * closefrom() which will work on OpenBSD too.
 *
 * @return 0 on success, or -1 w/ errno
 * @error EINVAL if flags are bad or first is greater than last
 * @error EMFILE if a weird race condition happens on Linux
 * @error ENOSYS if not Linux 5.9+ or FreeBSD 13+
 * @error ENOMEM on Linux maybe
 * @see closefrom()
 */
int close_range(unsigned int first, unsigned int last, unsigned int flags) {
  int rc;
  if (IsLinux() || IsFreebsd()) {
    rc = sys_close_range(first, last, flags);
  } else {
    rc = enosys();
  }
  STRACE("close_range(%d, %d, %#x) → %d% m", first, last, flags, rc);
  return rc;
}
