/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/safemacros.internal.h"
#include "libc/dns/dns.h"
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
 * @see test/libc/dns/dnsnamecmp_test.c (the code that matters)
 */
int dnsnamecmp(const char *A, const char *B) {
  if (A == B) return 0;
  size_t n = strlen(A);
  size_t m = strlen(B);
  if (!n || !m || ((A[n - 1] == '.') ^ (B[m - 1] == '.'))) {
    if (n && m && A[n - 1] == '.' && strchr(B, '.')) {
      --m;
    } else if (n && m && B[m - 1] == '.' && strchr(A, '.')) {
      --n;
    } else {
      return A[n ? n - 1 : 0] - B[m ? m - 1 : 0];
    }
  }
  size_t i = n;
  size_t j = m;
  bool first = true;
  for (;;) {
    FindDnsLabel(A, &i, &n);
    FindDnsLabel(B, &j, &m);
    if (first) {
      first = false;
      if (!i && j) return 1;
      if (!j && i) return -1;
    }
    int res;
    if ((res = strncasecmp(&A[i], &B[j], min(n - i + 1, m - j + 1)))) {
      return res;
    }
    if (!i || !j) {
      return i - j;
    }
    n = i;
    m = j;
  }
}
