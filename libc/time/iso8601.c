/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"

/**
 * Converts timestamp to ISO-8601 formatted string.
 *
 * For example:
 *
 *     char *GetZuluTime(void) {
 *       char *p;
 *       struct tm tm;
 *       struct timespec ts;
 *       static _Thread_local int64_t last;
 *       static _Thread_local char str[21];
 *       clock_gettime(0, &ts);
 *       if (ts.tv_sec != last) {
 *         gmtime_r(&ts.tv_sec, &tm);
 *         last = ts.tv_sec;
 *         stpcpy(iso8601(str, &tm), "Z");
 *       }
 *       return str;
 *     }
 *
 * Will return timestamps that look like:
 *
 *     2020-01-01T13:14:15Z
 *
 * The generated timestamp is always exactly 19 characters long. It is
 * also always nul terminated too.
 *
 * This function defines no failure conditions. The results of passing
 * timestamp values outside of the appropriate intervals is undefined.
 *
 * @param p is buffer with at least 20 bytes
 * @param tm has valid gmtime_r() or localtime_r() output
 * @return pointer to nul terminator within `p`, cf. stpcpy()
 * @see iso8601us() for microsecond timestamps
 * @asyncsignalsafe
 */
char *iso8601(char p[hasatleast 20], struct tm *tm) {
  int x;
  x = tm->tm_year + 1900;
  unassert(0 <= x && x <= 9999);
  *p++ = '0' + x / 1000;
  *p++ = '0' + x / 100 % 10;
  *p++ = '0' + x / 10 % 10;
  *p++ = '0' + x % 10;
  *p++ = '-';
  x = tm->tm_mon + 1;
  unassert(1 <= x && x <= 12);
  *p++ = '0' + x / 10;
  *p++ = '0' + x % 10;
  *p++ = '-';
  x = tm->tm_mday;
  unassert(1 <= x && x <= 31);
  *p++ = '0' + x / 10;
  *p++ = '0' + x % 10;
  *p++ = 'T';
  x = tm->tm_hour;
  unassert(0 <= x && x <= 23);
  *p++ = '0' + x / 10;
  *p++ = '0' + x % 10;
  *p++ = ':';
  x = tm->tm_min;
  unassert(0 <= x && x <= 59);
  *p++ = '0' + x / 10;
  *p++ = '0' + x % 10;
  *p++ = ':';
  x = tm->tm_sec;
  unassert(0 <= x && x <= 60);
  *p++ = '0' + x / 10;
  *p++ = '0' + x % 10;
  *p = 0;
  return p;
}
