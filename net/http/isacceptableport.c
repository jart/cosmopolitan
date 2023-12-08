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
#include "libc/str/str.h"
#include "net/http/http.h"

/**
 * Returns true if port seems legit.
 *
 * Here's examples of permitted inputs:
 *
 * - ""
 * - 0
 * - 65535
 *
 * Here's some examples of forbidden inputs:
 *
 * - -1
 * - 65536
 * - https
 *
 * @param n if -1 implies strlen
 */
bool IsAcceptablePort(const char *s, size_t n) {
  int p, c;
  size_t i;
  if (n == -1) n = s ? strlen(s) : 0;
  for (p = i = 0; i < n; ++i) {
    c = s[i] & 255;
    if ('0' <= c && c <= '9') {
      p *= 10;
      p += c - '0';
      if (p > 65535) {
        return false;
      }
    } else {
      return false;
    }
  }
  return true;
}
