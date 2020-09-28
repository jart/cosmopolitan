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
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/sysv/consts/clock.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/x/x.h"

STATIC_YOINK("stoa");
STATIC_YOINK("ntoa");

/**
 * @fileoverview Timestamps in One True Format w/o toil.
 */

static char *xiso8601$impl(struct timespec *opt_ts, int sswidth) {
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
  (asprintf)(&p, "%s.%0*ld%s", timebuf, sswidth, subsec, zonebuf);
  return p;
}

/**
 * Returns allocated string representation of nanosecond timestamp.
 */
char *xiso8601ts(struct timespec *opt_ts) {
  return xiso8601$impl(opt_ts, 9);
}

/**
 * Returns allocated string representation of microsecond timestamp.
 */
char *xiso8601tv(struct timeval *opt_tv) {
  return xiso8601$impl(
      opt_tv ? &(struct timespec){opt_tv->tv_sec, opt_tv->tv_usec} : NULL, 6);
}
