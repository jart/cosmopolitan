/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
