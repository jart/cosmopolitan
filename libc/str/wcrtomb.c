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

size_t wcrtomb(char *s, wchar_t wc, mbstate_t *st) {
  if (!s) return 1;
  if ((unsigned)wc < 0x80) {
    *s = wc;
    return 1;
  } else if (MB_CUR_MAX == 1) {
    if (!IS_CODEUNIT(wc)) {
      errno = EILSEQ;
      return -1;
    }
    *s = wc;
    return 1;
  } else if ((unsigned)wc < 0x800) {
    *s++ = 0xc0 | (wc >> 6);
    *s = 0x80 | (wc & 0x3f);
    return 2;
  } else if ((unsigned)wc < 0xd800 || (unsigned)wc - 0xe000 < 0x2000) {
    *s++ = 0xe0 | (wc >> 12);
    *s++ = 0x80 | ((wc >> 6) & 0x3f);
    *s = 0x80 | (wc & 0x3f);
    return 3;
  } else if ((unsigned)wc - 0x10000 < 0x100000) {
    *s++ = 0xf0 | (wc >> 18);
    *s++ = 0x80 | ((wc >> 12) & 0x3f);
    *s++ = 0x80 | ((wc >> 6) & 0x3f);
    *s = 0x80 | (wc & 0x3f);
    return 4;
  }
  errno = EILSEQ;
  return -1;
}
