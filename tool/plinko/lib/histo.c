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
#include "libc/assert.h"
#include "tool/plinko/lib/histo.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/printf.h"

void PrintHistogram(int fd, const char *s, const long *h, size_t n) {
  long t;
  bool islast;
  int j, p, m;
  char buf[101];
  size_t i, logos;
  if (!(t = GetLongSum(h, n))) return;
  Fprintf(fd, "%s%n", s);
  for (i = 0; i < n; ++i) {
    if (!h[i]) continue;
    p = h[i] * 1000000 / t;
    assert(0 <= p && p <= 1000000);
    for (j = 0, m = p / 10000; j < m; ++j) {
      buf[j] = '#';
    }
    buf[j] = 0;
    if (i) {
      logos = 1;
      logos <<= i - 1;
    } else {
      logos = 0;
    }
    islast = i == n - 1;
    Fprintf(fd, "%'16lu%s %'*ld %3d.%04d%% %s%n", logos, islast ? "+" : "",
            16 - islast, h[i], p / 10000, p % 10000, buf);
  }
}
