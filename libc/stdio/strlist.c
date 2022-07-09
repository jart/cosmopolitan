/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/alg/alg.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/strlist.internal.h"
#include "libc/str/str.h"

static int CompareStrings(const void *p1, const void *p2) {
  const char **a = p1;
  const char **b = p2;
  return strcmp(*a, *b);
}

void FreeStrList(struct StrList *sl) {
  int i;
  for (i = 0; i < sl->i; ++i) {
    free(sl->p[i]);
  }
  free(sl->p);
  sl->p = 0;
  sl->i = 0;
  sl->n = 0;
}

int AppendStrList(struct StrList *sl) {
  int n2;
  char **p2;
  if (sl->i == sl->n) {
    n2 = sl->n;
    if (!n2) n2 = 2;
    n2 += n2 >> 1;
    if ((p2 = realloc(sl->p, n2 * sizeof(*p2)))) {
      sl->p = p2;
      sl->n = n2;
    } else {
      return -1;
    }
  }
  sl->p[sl->i] = 0;
  appendr(&sl->p[sl->i], 0);
  return sl->i++;
}

void SortStrList(struct StrList *sl) {
  qsort(sl->p, sl->i, sizeof(*sl->p), CompareStrings);
}

int JoinStrList(struct StrList *sl, char **buf, uint64_t sep) {
  int i;
  if (!*buf && !sl->i) {
    return appendr(buf, 0);
  }
  for (i = 0; i < sl->i; ++i) {
    if (i) {
      if (appendw(buf, sep) == -1) {
        return -1;
      }
    }
    if (appends(buf, sl->p[i]) == -1) {
      return -1;
    }
  }
  return 0;
}
