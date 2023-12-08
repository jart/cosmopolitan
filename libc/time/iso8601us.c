/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
 *     char *GetTimestamp(void) {
 *       struct timespec ts;
 *       static _Thread_local struct tm tm;
 *       static _Thread_local int64_t last;
 *       static _Thread_local char str[27];
 *       clock_gettime(0, &ts);
 *       if (ts.tv_sec != last) {
 *         localtime_r(&ts.tv_sec, &tm);
 *         last = ts.tv_sec;
 *       }
 *       iso8601us(str, &tm, ts.tv_nsec);
 *       return str;
 *     }
 *
 * Will return timestamps that look like:
 *
 *     2020-01-01T13:14:15.123456
 *
 * The generated timestamp is always exactly 26 characters long. It is
 * also always nul terminated too.
 *
 * This function defines no failure conditions. The results of passing
 * timestamp, or nanosecond values outside their appropriate intervals
 * is undefined.
 *
 * This goes 19x faster than strftime().
 *
 *     iso8601             l:        21c         7ns
 *     iso8601us           l:        39c        13ns
 *     strftime            l:       779c       252ns
 *
 * @param p is buffer with at least 20 bytes
 * @param tm has valid gmtime_r() or localtime_r() output
 * @param ns is nanosecond value associated with timestamp
 * @return pointer to nul terminator within `p`, cf. stpcpy()
 * @see iso8601() if microsecond resolution isn't desirable
 * @asyncsignalsafe
 */
char *iso8601us(char p[hasatleast 27], struct tm *tm, long ns) {
  p = iso8601(p, tm);
  unassert(0 <= ns && ns < 1000000000);
  *p++ = '.';
  *p++ = '0' + ns / 100000000;
  *p++ = '0' + ns / 10000000 % 10;
  *p++ = '0' + ns / 1000000 % 10;
  *p++ = '0' + ns / 100000 % 10;
  *p++ = '0' + ns / 10000 % 10;
  *p++ = '0' + ns / 1000 % 10;
  *p = 0;
  return p;
}
