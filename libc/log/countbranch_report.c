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
#include "libc/calls/calls.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/countbranch.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/alg.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

#ifdef __x86_64__

static double GetTotal(const struct countbranch *p) {
  return p->total;
}

static double GetPercent(const struct countbranch *p) {
  if (p->total) {
    return (double)p->taken / p->total * 100;
  } else {
    return 50;
  }
}

static int RankCounter(const struct countbranch *p) {
  double x;
  x = GetPercent(p);
  x = MIN(x, 100 - x);
  return x;
}

static int CompareCounters(const void *a, const void *b) {
  double x, y;
  x = RankCounter(a);
  y = RankCounter(b);
  if (x > y) return +1;
  if (x < y) return -1;
  if (GetTotal(a) < GetTotal(b)) return +1;
  if (GetTotal(a) > GetTotal(b)) return -1;
  return 0;
}

static size_t CountCounters(void) {
  size_t n;
  struct countbranch *p;
  for (n = 0, p = countbranch_data; p->total >= 0; ++p) ++n;
  return n;
}

static void SortCounters(size_t n) {
  qsort(countbranch_data, n, sizeof(*countbranch_data), CompareCounters);
}

void countbranch_report(void) {
  double r;
  size_t i, n;
  int pct, nines;
  struct countbranch *p;
  n = CountCounters();
  SortCounters(n);
  for (i = n; i--;) {
    p = countbranch_data + i;
    if (p->total) {
      r = (double)p->taken / p->total;
      pct = floor(r * 100);
      nines = floor(fmod(r * 100, 1) * 100000);
    } else {
      pct = 0;
      nines = 0;
    }
    if (strcmp(p->code, p->xcode)) {
      kprintf("%s:%-4d: %3d.%05d%% taken (%'ld/%'ld) %s [%s]\n", p->file,
              p->line, pct, nines, p->taken, p->total, p->code, p->xcode);
    } else {
      kprintf("%s:%-4d: %3d.%05d%% taken (%'ld/%'ld) %s\n", p->file, p->line,
              pct, nines, p->taken, p->total, p->code);
    }
  }
}

static textstartup void countbranch_init() {
  atexit(countbranch_report);
}

const void *const countbranch_ctor[] initarray = {
    countbranch_init,
};

#endif /* __x86_64__ */
