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
#include "libc/macros.internal.h"
#include "libc/str/mb.internal.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

size_t mbrtowc(wchar_t *wc, const char *src, size_t n, mbstate_t *st) {
  static unsigned internal_state;
  long wut;
  unsigned c;
  const unsigned char *s = (const void *)src;
  const unsigned N = n;
  wchar_t dummy;
  if (!st) st = (void *)&internal_state;
  c = *(unsigned *)st;
  if (!s) {
    if (c) goto ilseq;
    return 0;
  } else if (!wc) {
    wc = &dummy;
  }
  if (!n) return -2;
  if (!c) {
    if (*s < 0x80) return !!(*wc = *s);
    if (MB_CUR_MAX == 1) return (*wc = CODEUNIT(*s)), 1;
    if (*s - SA > SB - SA) goto ilseq;
    wut = *s++ - SA;
    wut = MAX(0, MIN(ARRAYLEN(kMbBittab) - 1, wut));
    c = kMbBittab[wut];
    n--;
  }
  if (n) {
    if (OOB(c, *s)) goto ilseq;
  loop:
    c = c << 6 | (*s++ - 0x80);
    n--;
    if (!(c & (1U << 31))) {
      *(unsigned *)st = 0;
      *wc = c;
      return N - n;
    }
    if (n) {
      if (*s - 0x80u >= 0x40) goto ilseq;
      goto loop;
    }
  }
  *(unsigned *)st = c;
  return -2;
ilseq:
  *(unsigned *)st = 0;
  errno = EILSEQ;
  return -1;
}
