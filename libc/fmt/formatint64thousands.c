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
#include "libc/fmt/itoa.h"

/**
 * Converts unsigned 64-bit integer to string w/ commas.
 *
 * @param p needs at least 27 bytes
 * @return pointer to nul byte
 */
dontinline char *FormatUint64Thousands(char p[static 27], uint64_t x) {
  size_t i;
  char m[26];
  i = 0;
  do {
    m[i++] = x % 10 + '0';
    x = x / 10;
  } while (x);
  for (;;) {
    *p++ = m[--i];
    if (!i) break;
    if (!(i % 3)) *p++ = ',';
  }
  *p = '\0';
  return p;
}

/**
 * Converts 64-bit integer to string w/ commas.
 *
 * @param p needs at least 27 bytes
 * @return pointer to nul byte
 */
char *FormatInt64Thousands(char p[static 27], int64_t x) {
  if (x < 0) *p++ = '-', x = -(uint64_t)x;
  return FormatUint64Thousands(p, x);
}
