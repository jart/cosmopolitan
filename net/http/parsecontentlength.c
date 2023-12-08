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
#include "libc/str/str.h"
#include "net/http/http.h"

#define MAXIMUM (1024L * 1024L * 1024L * 1024L)

/**
 * Parses Content-Length header.
 *
 * @param size is byte length and -1 implies strlen
 * @return -1 on invalid or overflow, otherwise >=0 value
 */
int64_t ParseContentLength(const char *s, size_t n) {
  size_t i;
  int64_t r;
  if (n == -1) n = s ? strlen(s) : 0;
  if (!n) return -1;
  for (r = i = 0; i < n; ++i) {
    if (s[i] == ',' && i > 0) break;
    if (!isdigit(s[i])) return -1;
    r *= 10;
    r += s[i] - '0';
    if (r >= MAXIMUM) return -1;
  }
  return r;
}
