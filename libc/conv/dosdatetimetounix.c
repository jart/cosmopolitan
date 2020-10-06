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
#include "libc/conv/conv.h"
#include "libc/macros.h"
#include "libc/time/time.h"

/**
 * Converts MS-DOS timestamp to UNIX.
 *
 * @note type signature supports dates greater than 2100
 * @see PKZIP, FAT
 */
int64_t DosDateTimeToUnix(unsigned date, unsigned time) {
  unsigned weekday, year, month, day, hour, minute, second, yday, leap;
  year = ((date & 0xfffffe00) >> 9) + 1980 - 1900;
  month = MAX(1, MIN(12, (date & 0x01e0) >> 5));
  day = (date & 0x001f) ? (date & 0x001f) - 1 : 0;
  hour = (time & 0x0000f800) >> 11;
  minute = (time & 0x000007e0) >> 5;
  second = (time & 0x0000001f) << 1;
  leap = year % 4 == 0 && (year % 100 || year % 400 == 0);
  yday = day + kMonthYearDay[leap][month - 1];
  return second + minute * 60 + hour * 3600 + yday * 86400 +
         (year - 70) * 31536000ull + ((year - 69) / 4) * 86400ull -
         ((year - 1) / 100) * 86400ull + ((year + 299) / 400) * 86400ull;
}
