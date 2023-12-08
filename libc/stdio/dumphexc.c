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
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.h"
#include "libc/stdio/hex.internal.h"
#include "libc/str/str.h"

/**
 * Turns data into "\x00..." string literal.
 */
char *DumpHexc(const char *p, size_t n, size_t *z) {
  long o;
  int i, m;
  char A[128], *q, *s = 0;
  if (n == -1) n = p ? strlen(p) : 0;
  appendw(&s, '"' | '\\' << 8 | '\n' << 16);
  for (o = 0; (m = MIN(16, n)); p += m, n -= m) {
    q = A;
    for (i = 0; i < m; ++i) {
      *q++ = '\\';
      *q++ = 'x';
      *q++ = "0123456789abcdef"[(p[i] & 0xF0) >> 4];
      *q++ = "0123456789abcdef"[(p[i] & 0x0F) >> 0];
    }
    if (o) appendw(&s, '\\' | '\n' << 8);
    appendd(&s, A, q - A);
    o += m;
  }
  if (appendw(&s, '"') != -1) {
    if (z) *z = appendz(s).i;
    return s;
  } else {
    free(s);
    return 0;
  }
}
