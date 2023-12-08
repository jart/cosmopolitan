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
#include "net/https/https.h"

void FormatSslTime(char p[restrict hasatleast 16], struct tm *tm) {
  p[0x0] = '0' + (tm->tm_year + 1900) / 1000;
  p[0x1] = '0' + (tm->tm_year + 1900) / 100 % 10;
  p[0x2] = '0' + (tm->tm_year + 1900) / 10 % 10;
  p[0x3] = '0' + (tm->tm_year + 1900) % 10;
  p[0x4] = '0' + (tm->tm_mon + 1) / 10;
  p[0x5] = '0' + (tm->tm_mon + 1) % 10;
  p[0x6] = '0' + tm->tm_mday / 10;
  p[0x7] = '0' + tm->tm_mday % 10;
  p[0x8] = '0' + tm->tm_hour / 10;
  p[0x9] = '0' + tm->tm_hour % 10;
  p[0xa] = '0' + tm->tm_min / 10;
  p[0xb] = '0' + tm->tm_min % 10;
  p[0xc] = '0' + tm->tm_sec / 10;
  p[0xd] = '0' + tm->tm_sec % 10;
  p[0xe] = 0;
}
