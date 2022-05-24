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
#include "libc/calls/asan.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Sleeps for a particular amount of time.
 * @norestart
 */
noinstrument int nanosleep(const struct timespec *req, struct timespec *rem) {
  int rc;
  char buf[2][45];
  if (!req || (IsAsan() && (!__asan_is_valid_timespec(req) ||
                            (rem && !__asan_is_valid_timespec(rem))))) {
    rc = efault();
  } else if (req->tv_sec < 0 ||
             !(0 <= req->tv_nsec && req->tv_nsec <= 999999999)) {
    rc = einval();
  } else if (!IsWindows() && !IsMetal() && !IsXnu()) {
    rc = sys_nanosleep(req, rem);
  } else if (IsXnu()) {
    rc = sys_nanosleep_xnu(req, rem);
  } else if (IsMetal()) {
    rc = enosys(); /* TODO: Sleep on Metal */
  } else {
    rc = sys_nanosleep_nt(req, rem);
  }
  if (!__time_critical) {
    POLLTRACE("nanosleep(%s, [%s]) → %d% m",
              DescribeTimespec(buf[0], sizeof(buf[0]), rc, req),
              DescribeTimespec(buf[1], sizeof(buf[1]), rc, rem), rc);
  }
  return rc;
}
