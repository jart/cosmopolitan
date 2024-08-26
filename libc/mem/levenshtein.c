/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

#define MIN3(a, b, c) \
  ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

/**
 * Computes similarity between two strings.
 */
double levenshtein(const char *s0, const char *s1) {
  int n0 = strlen(s0) + 1;
  int n1 = strlen(s1) + 1;
  int *col = (int *)malloc(n1 * sizeof(int));
  int *pol = (int *)malloc(n1 * sizeof(int));
  for (int i = 0; i < n1; i++)
    pol[i] = i;
  for (int i = 0; i < n0; i++) {
    col[0] = i;
    for (int j = 1; j < n1; j++)
      col[j] = MIN3(1 + col[j - 1], 1 + pol[j],
                    pol[j - 1] + !(i > 0 && s0[i - 1] == s1[j - 1]));
    int *t = col;
    col = pol;
    pol = t;
  }
  int dist = pol[n1 - 1];
  free(pol);
  free(col);
  return 1 - dist / ((n0 > n1 ? n0 : n1) - 1.);
}
