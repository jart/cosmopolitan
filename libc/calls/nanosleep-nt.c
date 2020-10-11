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
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/errors.h"
#include "libc/nt/nt/time.h"
#include "libc/nt/synchronization.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

textwindows int nanosleep$nt(const struct timespec *req, struct timespec *rem) {
  int64_t millis, hectonanos, relasleep;
  if (rem) memcpy(rem, req, sizeof(*rem));
  hectonanos = req->tv_sec * 10000000ull + div100int64(req->tv_nsec);
  hectonanos = MAX(1, hectonanos);
  relasleep = -hectonanos;
  if (NtError(NtDelayExecution(true, &relasleep))) {
    millis = div10000int64(hectonanos);
    millis = MAX(1, millis);
    if (SleepEx(millis, true) == kNtWaitIoCompletion) {
      return eintr();
    }
  }
  if (rem) memset(rem, 0, sizeof(*rem));
  return 0;
}
