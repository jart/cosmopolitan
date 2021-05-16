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
#include "libc/dns/dns.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"

forceinline void FindDnsLabel(const char *A, size_t *i, size_t *n) {
  while (*i) {
    if (A[*i - 1] == '.') {
      if (*i == *n) {
        --(*n);
      } else {
        break;
      }
    }
    --(*i);
  }
}

/**
 * Compares DNS hostnames in reverse lexicographical asciibetical order.
 * @return <0, 0, or >0
 * @see test/libc/dns/comparednsnames_test.c (the code that matters)
 */
int CompareDnsNames(const char *A, const char *B) {
  int res;
  bool first;
  size_t n, m, i, j;
  if (A == B) return 0;
  n = strlen(A);
  m = strlen(B);
  if (!n || !m || ((A[n - 1] == '.') ^ (B[m - 1] == '.'))) {
    if (n && m && A[n - 1] == '.' && strchr(B, '.')) {
      --m;
    } else if (n && m && B[m - 1] == '.' && strchr(A, '.')) {
      --n;
    } else {
      return A[n ? n - 1 : 0] - B[m ? m - 1 : 0];
    }
  }
  i = n;
  j = m;
  first = true;
  for (;;) {
    FindDnsLabel(A, &i, &n);
    FindDnsLabel(B, &j, &m);
    if (first) {
      first = false;
      if (!i && j) return 1;
      if (!j && i) return -1;
    }
    if ((res = strncasecmp(&A[i], &B[j], MIN(n - i + 1, m - j + 1)))) {
      return res;
    }
    if (!i || !j) {
      return i - j;
    }
    n = i;
    m = j;
  }
}
