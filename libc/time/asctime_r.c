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
#include "libc/fmt/fmt.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"

STATIC_YOINK("ntoa");
STATIC_YOINK("stoa");

static unsigned clip(unsigned index, unsigned count) {
  return index < count ? index : 0;
}

char *asctime_r(const struct tm *date, char *buf /*[64]*/) {
  (snprintf)(buf, 64, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
             kWeekdayNameShort[clip(date->tm_wday, 7)],
             kMonthNameShort[clip(date->tm_mon, 12)], date->tm_mday,
             date->tm_hour, date->tm_min, date->tm_sec, 1900 + date->tm_year);
  return buf;
}
