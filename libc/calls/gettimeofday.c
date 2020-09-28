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
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/time/struct/timezone.h"
#include "libc/time/time.h"

/**
 * Returns system wall time in microseconds.
 *
 * @param tv points to timeval that receives result if non-NULL
 * @param tz receives UTC timezone if non-NULL
 * @return always zero
 * @see	clock_gettime() for nanosecond precision
 * @see	strftime() for string formatting
 */
int gettimeofday(struct timeval *tv, struct timezone *tz) {
  if (!IsWindows()) {
    return gettimeofday$sysv(tv, tz);
  } else {
    return gettimeofday$nt(tv, tz);
  }
}
