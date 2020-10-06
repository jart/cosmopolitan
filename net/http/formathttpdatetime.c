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
#include "libc/str/str.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "net/http/http.h"

/**
 * Formats HTTP timestamp, e.g.
 *
 *   Sun, 04 Oct 2020 19:50:10 GMT
 *
 * @param tm must be zulu see gmtime_r() and nowl()
 * @see ParseHttpDateTime()
 */
char *FormatHttpDateTime(char p[hasatleast 30], struct tm *tm) {
  unsigned i;
  p = mempcpy(p, kWeekdayNameShort[tm->tm_wday], 3);
  *p++ = ',';
  *p++ = ' ';
  i = tm->tm_mday;
  *p++ = '0' + i / 10;
  *p++ = '0' + i % 10;
  *p++ = ' ';
  p = mempcpy(p, kMonthNameShort[tm->tm_mon], 3);
  *p++ = ' ';
  i = tm->tm_year + 1900;
  *p++ = '0' + i / 1000;
  *p++ = '0' + i / 100 % 10;
  *p++ = '0' + i / 10 % 10;
  *p++ = '0' + i % 10;
  *p++ = ' ';
  i = tm->tm_hour;
  *p++ = '0' + i / 10;
  *p++ = '0' + i % 10;
  *p++ = ':';
  i = tm->tm_min;
  *p++ = '0' + i / 10;
  *p++ = '0' + i % 10;
  *p++ = ':';
  i = tm->tm_sec;
  *p++ = '0' + i / 10;
  *p++ = '0' + i % 10;
  *p++ = ' ';
  *p++ = 'G';
  *p++ = 'M';
  *p++ = 'T';
  *p = '\0';
  return p;
}
