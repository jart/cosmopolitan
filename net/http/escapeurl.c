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
#include "libc/x/x.h"
#include "net/http/escape.h"

/**
 * Escapes URL component using generic table.
 *
 * This function is agnostic to the underlying charset.
 * Always using UTF-8 is a good idea.
 *
 * @see EscapeUrlParam
 * @see EscapeUrlFragment
 * @see EscapeUrlPathSegment
 */
struct EscapeResult EscapeUrl(const char *data, size_t size,
                              const char xlat[hasatleast 256]) {
  int c;
  char *p;
  size_t i;
  struct EscapeResult r;
  p = r.data = xmalloc(size * 6 + 1);
  for (i = 0; i < size; ++i) {
    switch (xlat[(c = data[i] & 0xff)]) {
      case 0:
        *p++ = c;
        break;
      case 1:
        *p++ = '+';
        break;
      case 2:
        p[0] = '%';
        p[1] = "0123456789ABCDEF"[(c & 0xF0) >> 4];
        p[2] = "0123456789ABCDEF"[(c & 0x0F) >> 0];
        p += 3;
        break;
      default:
        unreachable;
    }
  }
  r.size = p - r.data;
  r.data = xrealloc(r.data, r.size + 1);
  r.data[r.size] = '\0';
  return r;
}
