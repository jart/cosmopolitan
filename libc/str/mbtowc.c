/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2014 Rich Felker, et al.                                   │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/str/mb.internal.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

int mbtowc(wchar_t *restrict wc, const char *restrict src, size_t n) {
  unsigned c;
  const unsigned char *s = (const void *)src;
  wchar_t dummy;
  if (!s) return 0;
  if (!n) goto ilseq;
  if (!wc) wc = &dummy;
  if (*s < 0x80) return !!(*wc = *s);
  if (MB_CUR_MAX == 1) return (*wc = CODEUNIT(*s)), 1;
  if (*s - SA > SB - SA) goto ilseq;
  c = kMbBittab[*s++ - SA];
  /* Avoid excessive checks against n: If shifting the state n-1
   * times does not clear the high bit, then the value of n is
   * insufficient to read a character */
  if (n < 4 && ((c << (6 * n - 6)) & (1U << 31))) goto ilseq;
  if (OOB(c, *s)) goto ilseq;
  c = c << 6 | (*s++ - 0x80);
  if (!(c & (1U << 31))) {
    *wc = c;
    return 2;
  }
  if (*s - 0x80u >= 0x40) goto ilseq;
  c = c << 6 | (*s++ - 0x80);
  if (!(c & (1U << 31))) {
    *wc = c;
    return 3;
  }
  if (*s - 0x80u >= 0x40) goto ilseq;
  *wc = c << 6 | (*s++ - 0x80);
  return 4;
ilseq:
  errno = EILSEQ;
  return -1;
}
