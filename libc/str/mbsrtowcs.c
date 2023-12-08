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

size_t mbsrtowcs(wchar_t *ws, const char **src, size_t wn, mbstate_t *st) {
  const unsigned char *s = (const void *)*src;
  size_t wn0 = wn;
  unsigned c = 0;
  if (st && (c = *(unsigned *)st)) {
    if (ws) {
      *(unsigned *)st = 0;
      goto resume;
    } else {
      goto resume0;
    }
  }
  if (MB_CUR_MAX == 1) {
    if (!ws) return strlen((const char *)s);
    for (;;) {
      if (!wn) {
        *src = (const void *)s;
        return wn0;
      }
      if (!*s) break;
      c = *s++;
      *ws++ = CODEUNIT(c);
      wn--;
    }
    *ws = 0;
    *src = 0;
    return wn0 - wn;
  }
  if (!ws)
    for (;;) {
      if (*s - 1u < 0x7f) {
        s++;
        wn--;
        continue;
      }
      if (*s - SA > SB - SA) break;
      c = kMbBittab[*s++ - SA];
    resume0:
      if (OOB(c, *s)) {
        s--;
        break;
      }
      s++;
      if (c & (1U << 25)) {
        if (*s - 0x80u >= 0x40) {
          s -= 2;
          break;
        }
        s++;
        if (c & (1U << 19)) {
          if (*s - 0x80u >= 0x40) {
            s -= 3;
            break;
          }
          s++;
        }
      }
      wn--;
      c = 0;
    }
  else
    for (;;) {
      if (!wn) {
        *src = (const void *)s;
        return wn0;
      }
      if (*s - 1u < 0x7f) {
        *ws++ = *s++;
        wn--;
        continue;
      }
      if (*s - SA > SB - SA) break;
      c = kMbBittab[*s++ - SA];
    resume:
      if (OOB(c, *s)) {
        s--;
        break;
      }
      c = (c << 6) | (*s++ - 0x80);
      if (c & (1U << 31)) {
        if (*s - 0x80u >= 0x40) {
          s -= 2;
          break;
        }
        c = (c << 6) | (*s++ - 0x80);
        if (c & (1U << 31)) {
          if (*s - 0x80u >= 0x40) {
            s -= 3;
            break;
          }
          c = (c << 6) | (*s++ - 0x80);
        }
      }
      *ws++ = c;
      wn--;
      c = 0;
    }
  if (!c && !*s) {
    if (ws) {
      *ws = 0;
      *src = 0;
    }
    return wn0 - wn;
  }
  errno = EILSEQ;
  if (ws) *src = (const void *)s;
  return -1;
}
