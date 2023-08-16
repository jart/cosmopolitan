/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/runtime/zipos.internal.h"

static size_t __zipos_trimpath(char *s, int *isabs) {
  char *p = s, *q = s;
  for (; *q; ++q) {
    if (*q == '/') {
      while (q[1] == '/') ++q;
      if (q[1] == '.' && (q[2] == '/' || q[2] == '\0')) {
        ++q;
      } else {
        *p++ = '/';
      }
    } else {
      *p++ = *q;
    }
  }
  if (s < p && p[-1] == '.' && p[-2] == '.' && (p - 2 == s || p[-3] == '/')) {
    *p++ = '/';
  }
  *p = '\0';
  if (isabs) {
    *isabs = *s == '/';
  }
  return p - s;
}

size_t __zipos_normpath(char *s) {
  int isabs;
  char *p = s, *q = s;
  __zipos_trimpath(s, &isabs);
  if (!*s) return 0;
  for (; *q != '\0'; ++q) {
    if (q[0] == '/' && q[1] == '.' && q[2] == '.' &&
        (q[3] == '/' || q[3] == '\0')) {
      char *ep = p;
      while (s < ep && *--ep != '/') donothing;
      if (ep != p &&
          (p[-1] != '.' || p[-2] != '.' || (s < p - 3 && p[-3] != '/'))) {
        p = ep;
        q += 2;
        continue;
      } else if (ep == s && isabs) {
        q += 2;
        continue;
      }
    }
    if (q[0] != '/' || p != s || isabs) {
      *p++ = *q;
    }
  }
  if (p == s) {
    *p++ = isabs ? '/' : '.';
  }
  if (p == s + 1 && s[0] == '.') {
    *p++ = '/';
  }
  while (p - s > 1 && p[-1] == '/') {
    --p;
  }
  *p = '\0';
  return p - s;
}
