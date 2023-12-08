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
#include "libc/fmt/conv.h"
#include "libc/macros.internal.h"
#include "libc/time/time.h"

/**
 * Converts MS-DOS timestamp to UNIX.
 *
 * @note type signature supports dates greater than 2100
 * @see PKZIP, FAT
 */
int64_t DosDateTimeToUnix(uint32_t date, uint32_t time) {
  unsigned year, month, day, hour, minute, second, yday, leap;
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
