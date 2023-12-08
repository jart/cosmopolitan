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
#include "libc/assert.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/time/time.h"

int sys_utimensat(int dirfd, const char *path, const struct timespec ts[2],
                  int flags) {
  int rc, olderr;
  struct timeval tv[2];
  unassert(!IsWindows() && !IsXnu());
  if (!path && (IsFreebsd() || IsNetbsd() || IsOpenbsd())) {
    rc = sys_futimens(dirfd, ts);
  } else {
    olderr = errno;
    rc = __sys_utimensat(dirfd, path, ts, flags);
    // TODO(jart): How does RHEL5 do futimes()?
    if (rc == -1 && errno == ENOSYS && path) {
      errno = olderr;
      if (ts) {
        tv[0] = timespec_totimeval(ts[0]);
        tv[1] = timespec_totimeval(ts[1]);
        rc = sys_utimes(path, tv);
      } else {
        rc = sys_utimes(path, NULL);
      }
    }
  }
  return rc;
}
