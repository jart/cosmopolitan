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
#include "libc/intrin/describeflags.internal.h"

const char *DescribeFlags(char *p, size_t n, const struct DescribeFlags *d,
                          size_t m, const char *prefix, unsigned x) {
  bool t;
  char b[21];
  size_t i, j, k;
  for (t = false, i = j = 0; j < m; ++j) {
    if (d[j].flag && d[j].flag != -1 && (x & d[j].flag) == d[j].flag) {
      x &= ~d[j].flag;
      if (t) {
        if (i + 1 < n) p[i++] = '|';
      } else {
        t = true;
      }
      for (k = 0; prefix && prefix[k]; ++k) {
        if (i + 1 < n) p[i++] = prefix[k];
      }
      for (k = 0; d[j].name[k]; ++k) {
        if (i + 1 < n) p[i++] = d[j].name[k];
      }
    }
  }
  if (x || !t) {
    if (t && i + 1 < n) p[i++] = '|';
    if (i + 1 < n) p[i++] = '0';
    if (x) {
      if (i + 1 < n) p[i++] = 'x';
      k = 0;
      do {
        if (i + 1 < n) b[k++] = "0123456789abcdef"[x % 16];
      } while ((x /= 16));
      while (k--) {
        if (i + 1 < n) p[i++] = b[k];
      }
    }
  }
  if (i < n) p[i] = 0;
  return p;
}
