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
#include "libc/serialize.h"
#include "libc/str/str.h"
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
 * @param n if -1 implies strlen
 * @see FormatHttpDateTime()
 */
int64_t ParseHttpDateTime(const char *p, size_t n) {
  unsigned year, month, day, hour, minute, second, yday, leap;
  if (n == -1) n = p ? strlen(p) : 0;
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
