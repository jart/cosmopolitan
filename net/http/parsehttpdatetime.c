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
#include "libc/bits/bits.h"
#include "libc/time/time.h"
#include "net/http/http.h"

static unsigned ParseMonth(const char *p) {
  int i;
  for (i = 0; i < 12; ++i) {
    if ((READ32LE(p) & 0x00ffffff) == READ32LE(kMonthNameShort[i])) {
      return i + 1;
    }
  }
  return 1;
}

/**
 * Parses HTTP timestamp, e.g.
 *
 *   Sun, 04 Oct 2020 19:50:10 GMT
 *
 * @return seconds from unix epoch
 * @see FormatHttpDateTime()
 */
int64_t ParseHttpDateTime(const char *p, size_t n) {
  unsigned weekday, year, month, day, hour, minute, second, yday, leap;
  if (n != 29) return 0;
  day = (p[5] - '0') * 10 + (p[6] - '0') - 1;
  month = ParseMonth(p + 8);
  year = (p[12] - '0') * 1000 + (p[13] - '0') * 100 + (p[14] - '0') * 10 +
         (p[15] - '0') - 1900;
  hour = (p[17] - '0') * 10 + (p[18] - '0');
  minute = (p[20] - '0') * 10 + (p[21] - '0');
  second = (p[23] - '0') * 10 + (p[24] - '0');
  leap = year % 4 == 0 && (year % 100 || year % 400 == 0);
  yday = day + kMonthYearDay[leap][month - 1];
  return second + minute * 60 + hour * 3600 + yday * 86400 +
         (year - 70) * 31536000ull + ((year - 69) / 4) * 86400ull -
         ((year - 1) / 100) * 86400ull + ((year + 299) / 400) * 86400ull;
}
