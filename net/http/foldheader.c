/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "net/http/http.h"

/**
 * Collapses repeating headers onto a single line.
 */
char *FoldHeader(struct HttpMessage *msg, const char *b, int h, size_t *z) {
  char *p, *p2;
  size_t i, n, m;
  struct HttpHeader *x;
  n = msg->headers[h].b - msg->headers[h].a;
  if ((p = malloc(n))) {
    memcpy(p, b + msg->headers[h].a, n);
    for (i = 0; i < msg->xheaders.n; ++i) {
      x = msg->xheaders.p + i;
      if (GetHttpHeader(b + x->k.a, x->k.b - x->k.a) == h) {
        m = x->v.b - x->v.a;
        if ((p2 = realloc(p, n + 2 + m))) {
          memcpy(mempcpy((p = p2) + n, ", ", 2), b + x->v.a, m);
          n += 2 + m;
        } else {
          free(p);
          return 0;
        }
      }
    }
    *z = n;
  }
  return p;
}
