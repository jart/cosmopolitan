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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "net/http/escape.h"

/**
 * Escapes HTML entities.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param z if non-NULL receives output length
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char *EscapeHtml(const char *p, size_t n, size_t *z) {
  int c;
  size_t i;
  char *q, *r;
  if (z) *z = 0;
  if (n == -1) n = p ? strlen(p) : 0;
  if ((q = r = malloc(n * 6 + 1))) {
    for (i = 0; i < n; ++i) {
      switch ((c = p[i])) {
        case '&':
          q[0] = '&';
          q[1] = 'a';
          q[2] = 'm';
          q[3] = 'p';
          q[4] = ';';
          q += 5;
          break;
        case '<':
          q[0] = '&';
          q[1] = 'l';
          q[2] = 't';
          q[3] = ';';
          q += 4;
          break;
        case '>':
          q[0] = '&';
          q[1] = 'g';
          q[2] = 't';
          q[3] = ';';
          q += 4;
          break;
        case '"':
          q[0] = '&';
          q[1] = 'q';
          q[2] = 'u';
          q[3] = 'o';
          q[4] = 't';
          q[5] = ';';
          q += 6;
          break;
        case '\'':
          q[0] = '&';
          q[1] = '#';
          q[2] = '3';
          q[3] = '9';
          q[4] = ';';
          q += 5;
          break;
        default:
          *q++ = c;
          break;
      }
    }
    if (z) *z = q - r;
    *q++ = '\0';
    if ((q = realloc(r, q - r))) r = q;
  }
  return r;
}
