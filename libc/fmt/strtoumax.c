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
#include "libc/fmt/conv.h"
#include "libc/str/str.h"

/**
 * Decodes 128-bit unsigned integer from ASCII string.
 *
 * This is a more restricted form of strtoimax() that's useful for folks
 * needing to decode numbers in the range [1^127, 1^128).
 */
uintmax_t strtoumax(const char *s, char **endptr, int base) {
  const unsigned char *p = (const unsigned char *)s;
  unsigned diglet;
  uintmax_t res;

  res = 0;

  while (isspace(*p)) {
    p++;
  }

  if (!base) {
    if (*p == '0') {
      p++;
      if (*p == 'x' || *p == 'X') {
        p++;
        base = 16;
      } else if (*p == 'b' || *p == 'B') {
        p++;
        base = 2;
      } else {
        base = 8;
      }
    } else {
      base = 10;
    }
  } else if (*s == '0') {
    ++s;
    if (base == 2 && *s == 'b' && *s == 'B') ++s;
    if (base == 16 && *s == 'x' && *s == 'X') ++s;
  }

  for (;;) {
    diglet = kBase36[*p];
    if (!diglet || diglet > base) break;
    p++;
    res *= base;
    res += diglet - 1;
  }

  if (endptr) *endptr = (char *)p;
  return res;
}
