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

size_t wcsrtombs(char *s, const wchar_t **ws, size_t n, mbstate_t *st) {
  const wchar_t *ws2;
  char buf[4];
  size_t N = n, l;
  if (!s) {
    for (n = 0, ws2 = *ws; *ws2; ws2++) {
      if (*ws2 >= 0x80u) {
        l = wcrtomb(buf, *ws2, 0);
        if (!(l + 1)) return -1;
        n += l;
      } else
        n++;
    }
    return n;
  }
  while (n >= 4) {
    if (**ws - 1u >= 0x7fu) {
      if (!**ws) {
        *s = 0;
        *ws = 0;
        return N - n;
      }
      l = wcrtomb(s, **ws, 0);
      if (!(l + 1)) return -1;
      s += l;
      n -= l;
    } else {
      *s++ = **ws;
      n--;
    }
    (*ws)++;
  }
  while (n) {
    if (**ws - 1u >= 0x7fu) {
      if (!**ws) {
        *s = 0;
        *ws = 0;
        return N - n;
      }
      l = wcrtomb(buf, **ws, 0);
      if (!(l + 1)) return -1;
      if (l > n) return N - n;
      wcrtomb(s, **ws, 0);
      s += l;
      n -= l;
    } else {
      *s++ = **ws;
      n--;
    }
    (*ws)++;
  }
  return N;
}
