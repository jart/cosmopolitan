/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/mach.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/systemtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"

/**
 * Returns nanosecond time.
 *
 * This is a high-precision timer that supports multiple definitions of
 * time. Among the more popular is CLOCK_MONOTONIC. This function has a
 * zero syscall implementation of that on modern x86.
 *
 * @param clockid can be CLOCK_REALTIME, CLOCK_MONOTONIC, etc. noting
 *     that on Linux CLOCK_MONOTONIC is redefined to use the monotonic
 *     clock that's actually monotonic lool
 * @param out_ts is where the nanoseconds are stored if non-NULL
 * @return 0 on success or -1 w/ errno on error
 * @error ENOSYS if clockid isn't available; in which case this function
 *     guarantees an ordinary timestamp is still stored to out_ts; and
 *     errno isn't restored to its original value, to detect prec. loss
 * @see strftime(), gettimeofday()
 * @asyncsignalsafe
 */
int clock_gettime(int clockid, struct timespec *out_ts) {
  /* TODO(jart): Just ignore O/S for MONOTONIC and measure RDTSC on start */
  if (!IsWindows()) {
    if (!IsXnu()) {
      if (out_ts) {
        out_ts->tv_sec = 0;
        out_ts->tv_nsec = 0;
      }
      return clock_gettime$sysv(clockid, out_ts);
    } else {
      int rc;
      static_assert(sizeof(struct timeval) == sizeof(struct timespec));
      if (out_ts) {
        out_ts->tv_sec = 0;
        out_ts->tv_nsec = 0;
      }
      rc = gettimeofday$sysv((struct timeval *)out_ts, NULL);
      if (out_ts) {
        out_ts->tv_nsec *= 1000;
      }
      return rc;
    }
  } else {
    struct NtFileTime ft;
    GetSystemTimeAsFileTime(&ft);
    *out_ts = FileTimeToTimeSpec(ft);
    return 0;
  }
}
