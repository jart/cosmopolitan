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
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/clock.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/x/x.h"

// TODO(jart): DELETE

static char *xiso8601_impl(struct timespec *opt_ts, int sswidth) {
  char *p;
  struct tm tm;
  struct timespec ts;
  int64_t sec, subsec;
  char timebuf[64], zonebuf[8];
  if (opt_ts) {
    sec = opt_ts->tv_sec;
    subsec = opt_ts->tv_nsec;
  } else {
    errno = 0;
    clock_gettime(CLOCK_REALTIME, &ts);
    sec = ts.tv_sec;
    subsec = ts.tv_nsec;
    sswidth = 9;
    if (errno == ENOSYS) {
      subsec /= 1000;
      sswidth = 6;
    }
  }
  if (IsWindows() && sswidth == 9) {
    subsec /= 100;
    sswidth = 7; /* windows nt uses hectonanoseconds */
  }
  localtime_r(&sec, &tm);
  strftime(timebuf, sizeof(timebuf), "%Y-%m-%dT%H:%M:%S", &tm);
  strftime(zonebuf, sizeof(zonebuf), "%z", &tm);
  asprintf(&p, "%s.%0*ld%s", timebuf, sswidth, subsec, zonebuf);
  return p;
}

/**
 * Returns allocated string representation of nanosecond timestamp.
 */
char *xiso8601ts(struct timespec *opt_ts) {
  return xiso8601_impl(opt_ts, 9);
}

/**
 * Returns allocated string representation of microsecond timestamp.
 */
char *xiso8601tv(struct timeval *opt_tv) {
  return xiso8601_impl(
      opt_tv ? &(struct timespec){opt_tv->tv_sec, opt_tv->tv_usec} : NULL, 6);
}
