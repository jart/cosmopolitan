/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/select.h"

/**
 * Does what poll() does except with bitset API.
 *
 * This system call is supported on all platforms. However, on Windows,
 * this is polyfilled to translate into poll(). So it's recommended that
 * poll() be used instead.
 */
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout) {
  int rc;
  POLLTRACE("select(%d, %p, %p, %p, %s) → ...", nfds, readfds, writefds,
            exceptfds, DescribeTimeval(0, timeout));
  if (!IsWindows()) {
    rc = sys_select(nfds, readfds, writefds, exceptfds, timeout);
  } else {
    rc = sys_select_nt(nfds, readfds, writefds, exceptfds, timeout);
  }
  POLLTRACE("select(%d, %p, %p, %p, [%s]) → %d% m", nfds, readfds, writefds,
            exceptfds, DescribeTimeval(rc, timeout), rc);
  return rc;
}
