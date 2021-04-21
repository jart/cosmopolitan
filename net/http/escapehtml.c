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
#include "libc/x/x.h"
#include "net/http/escape.h"

/**
 * Escapes HTML entities.
 *
 * @param size if -1 implies strlen
 */
struct EscapeResult EscapeHtml(const char *data, size_t size) {
  int c;
  char *p;
  size_t i;
  struct EscapeResult r;
  if (size == -1) size = data ? strlen(data) : 0;
  p = r.data = xmalloc(size * 6 + 1);
  for (i = 0; i < size; ++i) {
    switch ((c = data[i])) {
      case '&':
        p[0] = '&';
        p[1] = 'a';
        p[2] = 'm';
        p[3] = 'p';
        p[4] = ';';
        p += 5;
        break;
      case '<':
        p[0] = '&';
        p[1] = 'l';
        p[2] = 't';
        p[3] = ';';
        p += 4;
        break;
      case '>':
        p[0] = '&';
        p[1] = 'g';
        p[2] = 't';
        p[3] = ';';
        p += 4;
        break;
      case '"':
        p[0] = '&';
        p[1] = 'q';
        p[2] = 'u';
        p[3] = 'o';
        p[4] = 't';
        p[5] = ';';
        p += 6;
        break;
      case '\'':
        p[0] = '&';
        p[1] = '#';
        p[2] = '3';
        p[3] = '9';
        p[4] = ';';
        p += 5;
        break;
      default:
        *p++ = c;
        break;
    }
  }
  r.size = p - r.data;
  r.data = xrealloc(r.data, r.size + 1);
  r.data[r.size] = '\0';
  return r;
}
