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

size_t mbsnrtowcs(wchar_t *wcs, const char **src, size_t n, size_t wn,
                  mbstate_t *st) {
  size_t l, cnt = 0, n2;
  wchar_t *ws, wbuf[256];
  const char *s = *src;
  const char *tmp_s;
  if (!wcs) {
    ws = wbuf, wn = sizeof(wbuf) / sizeof(*wbuf);
  } else {
    ws = wcs;
  }
  /* making sure output buffer size is at most n/4 will ensure
   * that mbsrtowcs never reads more than n input bytes. thus
   * we can use mbsrtowcs as long as it's practical.. */
  while (s && wn && ((n2 = n / 4) >= wn || n2 > 32)) {
    if (n2 >= wn) n2 = wn;
    tmp_s = s;
    l = mbsrtowcs(ws, &s, n2, st);
    if (!(l + 1)) {
      cnt = l;
      wn = 0;
      break;
    }
    if (ws != wbuf) {
      ws += l;
      wn -= l;
    }
    n = s ? n - (s - tmp_s) : 0;
    cnt += l;
  }
  if (s)
    while (wn && n) {
      l = mbrtowc(ws, s, n, st);
      if (l + 2 <= 2) {
        if (!(l + 1)) {
          cnt = l;
          break;
        }
        if (!l) {
          s = 0;
          break;
        }
        /* have to roll back partial character */
        *(unsigned *)st = 0;
        break;
      }
      s += l;
      n -= l;
      /* safe - this loop runs fewer than sizeof(wbuf)/8 times */
      ws++;
      wn--;
      cnt++;
    }
  if (wcs) *src = s;
  return cnt;
}
