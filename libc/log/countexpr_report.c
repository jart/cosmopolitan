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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/log/countexpr.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/runtime/runtime.h"
#include "libc/stdckdint.h"
#include "libc/stdio/stdio.h"

#ifdef __x86_64__

static long GetLongSum(const long *h, size_t n) {
  long t;
  size_t i;
  for (t = i = 0; i < n; ++i) {
    if (ckd_add(&t, t, h[i])) {
      t = LONG_MAX;
      break;
    }
  }
  return t;
}

static size_t GetRowCount(const long *h, size_t n) {
  while (n && !h[n - 1]) --n;
  return n;
}

static void PrintHistogram(const long *h, size_t n, long t) {
  size_t i;
  long j, p;
  char s[101];
  unsigned long logos;
  for (i = 0; i < n; ++i) {
    p = (h[i] * 10000 + (t >> 1)) / t;
    unassert(0 <= p && p <= 10000);
    if (p) {
      for (j = 0; j < p / 100; ++j) s[j] = '#';
      s[j] = 0;
      logos = i ? 1ul << (i - 1) : 0;
      kprintf("%'12lu %'16ld %3d.%02d%% %s\n", logos, h[i], p / 100, p % 100,
              s);
    }
  }
}

void countexpr_report(void) {
  long hits;
  struct countexpr *p;
  for (p = countexpr_data; p->line; ++p) {
    if ((hits = GetLongSum(p->logos, 64))) {
      kprintf("%s:%d: %s(%s) %'ld hits\n", p->file, p->line, p->macro, p->code,
              hits);
      PrintHistogram(p->logos, 64, hits);
    }
  }
}

static textstartup void countexpr_init() {
  atexit(countexpr_report);
}

const void *const countexpr_ctor[] initarray = {
    countexpr_init,
};

#endif /* __x86_64__ */
