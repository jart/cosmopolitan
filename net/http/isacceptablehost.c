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

#define DNS_NAME_MAX 253

extern const char kHostChars[256];  // -_0-9A-Za-z

/**
 * Returns true if host seems legit.
 *
 * This function may be called after ParseUrl() or ParseHost() has
 * already handled things like percent encoding. There's currently
 * no support for IPv6 and IPv7.
 *
 * Here's examples of permitted inputs:
 *
 * - ""
 * - 1.2.3.4
 * - 1.2.3.4.arpa
 * - localservice
 * - hello.example
 * - _hello.example
 * - -hello.example
 * - hi-there.example
 *
 * Here's some examples of forbidden inputs:
 *
 * - ::1
 * - 1.2.3
 * - 1.2.3.4.5
 * - .hi.example
 * - hi..example
 *
 * @param n if -1 implies strlen
 */
bool IsAcceptableHost(const char *s, size_t n) {
  size_t i;
  int c, b, j;
  if (n == -1) n = s ? strlen(s) : 0;
  if (!n) return true;
  if (n > DNS_NAME_MAX) {
    return false;
  }
  for (b = j = i = 0; i < n; ++i) {
    c = s[i] & 255;
    if (isdigit(c)) {
      b *= 10;
      b += c - '0';
    } else if (c == '.') {
      if (!i || s[i - 1] == '.') {
        return false;
      }
      b = 0;
      ++j;
    } else {
      for (;;) {
        if (!kHostChars[c] && (c != '.' || (!i || s[i - 1] == '.'))) {
          return false;
        }
        if (++i < n) {
          c = s[i] & 255;
        } else {
          return true;
        }
      }
    }
  }
  if (j != 3) {
    return false;
  }
  if (i && s[i - 1] == '.') {
    return false;
  }
  return true;
}
