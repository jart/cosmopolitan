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
#include "libc/dce.h"
#include "libc/mem/mem.h"
#include "libc/mem/sortedints.internal.h"
#include "libc/str/str.h"

bool ContainsInt(const struct SortedInts *t, int k) {
  int l, m, r;
  l = 0;
  r = t->n - 1;
  while (l <= r) {
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    if (t->p[m] < k) {
      l = m + 1;
    } else if (t->p[m] > k) {
      r = m - 1;
    } else {
      return true;
    }
  }
  return false;
}

int LeftmostInt(const struct SortedInts *t, int k) {
  int l, m, r;
  l = 0;
  r = t->n;
  while (l < r) {
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    if (t->p[m] < k) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  unassert(l == 0 || k >= t->p[l - 1]);
  unassert(l == t->n || k <= t->p[l]);
  return l;
}

int CountInt(const struct SortedInts *t, int k) {
  int i, c;
  for (c = 0, i = LeftmostInt(t, k); i < t->n; ++i) {
    if (t->p[i] == k) {
      ++c;
    } else {
      break;
    }
  }
  return c;
}

bool InsertInt(struct SortedInts *t, int k, bool u) {
  int l;
  unassert(t->n >= 0);
  unassert(t->n <= t->c);
  if (t->n == t->c) {
    ++t->c;
    if (!IsModeDbg()) {
      t->c += t->c >> 1;
    }
    t->p = realloc(t->p, t->c * sizeof(*t->p));
  }
  l = LeftmostInt(t, k);
  if (l < t->n) {
    if (u && t->p[l] == k) {
      return false;
    }
    memmove(t->p + l + 1, t->p + l, (t->n - l) * sizeof(*t->p));
  }
  t->p[l] = k;
  t->n++;
  return true;
}
