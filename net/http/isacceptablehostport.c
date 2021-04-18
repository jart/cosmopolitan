/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
 * Returns true if HOST[:PORT] seems legit.
 *
 * This parser is permissive and imposes the subset of restrictions
 * that'll make things easier for the caller. For example, only one
 * colon is allowed to appear, which makes memchr() so much easier.
 *
 * Here's examples of permitted inputs:
 *
 * - 1.2.3.4
 * - 1.2.3.4.arpa
 * - 1.2.3.4:8080
 * - localservice
 * - hello.example
 * - _hello.example
 * - -hello.example
 * - hi-there.example
 * - hello.example:443
 *
 * Here's some examples of forbidden inputs:
 *
 * - :443
 * - 1.2.3
 * - 1.2.3.4.5
 * - [::1]:8080
 * - .hi.example
 * - hi..example
 * - hi.example::80
 * - hi.example:-80
 * - hi.example:65536
 *
 * @param n if -1 implies strlen
 */
bool IsAcceptableHostPort(const char *s, size_t n) {
  size_t i;
  bool isip;
  int c, t, p, b, j;
  if (n == -1) n = s ? strlen(s) : 0;
  if (!n) return false;
  for (isip = true, b = j = p = t = i = 0; i < n; ++i) {
    c = s[i] & 255;
    if (!t) {
      if (c == ':') {
        if (!i || s[i - 1] == '.') {
          return false;
        } else {
          t = 1;
        }
      } else if (c == '.' && (!i || s[i - 1] == '.')) {
        return false;
      } else if (!(isalnum(c) || c == '-' || c == '_' || c == '.')) {
        return false;
      }
      if (isip) {
        if (isdigit(c)) {
          b *= 10;
          b += c - '0';
          if (b > 255) {
            return false;
          }
        } else if (c == '.') {
          b = 0;
          ++j;
        } else {
          isip = false;
        }
      }
    } else {
      if (c == ':') {
        return false;
      } else if ('0' <= c && c <= '9') {
        p *= 10;
        p += c - '0';
        if (p > 65535) {
          return false;
        }
      } else {
        return false;
      }
    }
  }
  if (isip && j != 3) return false;
  if (!t && s[i - 1] == '.') return false;
  return true;
}
