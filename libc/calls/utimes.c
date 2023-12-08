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
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

/**
 * Changes last accessed/modified timestamps on file.
 *
 * @param tv is access/modified timestamps, or NULL which means now
 * @return 0 on success, or -1 w/ errno
 * @note truncates to second precision on rhel5
 * @see utimensat() for modern version
 * @asyncsignalsafe
 */
int utimes(const char *path, const struct timeval tv[2]) {
  int rc;
  struct timespec ts[2];
  if (!path) {
    rc = efault();
  } else if (tv) {
    ts[0] = timeval_totimespec(tv[0]);
    ts[1] = timeval_totimespec(tv[1]);
    rc = __utimens(AT_FDCWD, path, ts, 0);
  } else {
    rc = __utimens(AT_FDCWD, path, 0, 0);
  }
  STRACE("utimes(%#s, {%s, %s}) → %d% m", path, DescribeTimeval(0, tv),
         DescribeTimeval(0, tv ? tv + 1 : 0), rc);
  return rc;
}
