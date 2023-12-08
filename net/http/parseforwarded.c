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
 * Parses X-Forwarded-For.
 *
 * This header is used by reverse proxies. For example:
 *
 *     X-Forwarded-For: 203.0.110.2, 203.0.113.42:31337
 *
 * The port is optional and will be set to zero if absent.
 *
 * @param s is input data
 * @param n if -1 implies strlen
 * @param ip receives last/right ip on success if not NULL
 * @param port receives port on success if not NULL
 * @return 0 on success or -1 on failure
 * @see RFC7239's poorly designed Forwarded header
 */
int ParseForwarded(const char *s, size_t n, uint32_t *ip, uint16_t *port) {
  int c, t;
  size_t i;
  char *r;
  uint32_t x;
  if (n == -1) n = s ? strlen(s) : 0;
  if (n) {
    t = x = i = 0;
    if ((r = memrchr(s, ',', n))) {
      i = r - s;
      if ((s[++i] & 255) == ' ') ++i;  // skip optional space
    }
    do {
      c = s[i++] & 255;
      if (isdigit(c)) {
        t *= 10;
        t += c - '0';
        if (t > 255) {
          return -1;
        }
      } else if (c == '.') {
        x <<= 8;
        x |= t;
        t = 0;
      } else if (c == ':') {
        break;
      } else {
        return -1;
      }
    } while (i < n);
    x <<= 8;
    x |= t;
    t = 0;
    if (c == ':') {
      while (i < n) {
        c = s[i++] & 255;
        if (isdigit(c)) {
          t *= 10;
          t += c - '0';
          if (t > 65535) {
            return -1;
          }
        } else {
          return -1;
        }
      }
    }
    if (ip) *ip = x;
    if (port) *port = t;
    return 0;
  } else {
    return -1;
  }
}
