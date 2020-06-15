/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/conv/conv.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/errors.h"
#include "libc/nt/nt/time.h"
#include "libc/nt/synchronization.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Sleeps for a particular amount of time.
 */
int nanosleep(const struct timespec *req, struct timespec *rem) {
  long res, millis, hectonanos;
  if (!IsWindows()) {
    if (!IsXnu()) {
      return nanosleep$sysv(req, rem);
    } else {
      return select$sysv(0, 0, 0, 0, /* lool */
                         &(struct timeval){req->tv_sec, req->tv_nsec / 1000});
    }
  } else {
    if (req->tv_sec && req->tv_nsec) {
      hectonanos = MAX(1, req->tv_sec * 10000000L + req->tv_nsec / 100L);
    } else {
      hectonanos = 1;
    }
    if (NtError(NtDelayExecution(true, &hectonanos))) {
      millis = hectonanos / 10000;
      res = SleepEx(millis, true);
      if (res == kNtWaitIoCompletion) return eintr();
    }
    return 0;
  }
}
