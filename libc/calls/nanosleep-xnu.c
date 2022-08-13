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
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/clock.h"

int sys_nanosleep_xnu(const struct timespec *req, struct timespec *rem) {
  int rc;
  axdx_t axdx;
  struct timeval tv;
  struct timespec begin, end, elapsed;

  if (rem) {
    if (sys_clock_gettime_xnu(CLOCK_MONOTONIC, &begin) == -1) {
      return -1;
    }
  }

  tv = _timespec2timeval(*req);
  rc = sys_select(0, 0, 0, 0, &tv);

  if (rem) {
    if (!rc) {
      rem->tv_sec = 0;
      rem->tv_nsec = 0;
    } else if (rc == -1 && errno == EINTR) {
      sys_clock_gettime_xnu(CLOCK_MONOTONIC, &end);
      elapsed = _timespec_sub(end, begin);
      *rem = _timespec_sub(*req, elapsed);
      if (rem->tv_sec < 0) {
        rem->tv_sec = 0;
        rem->tv_nsec = 0;
      }
    }
  }

  return rc;
}
