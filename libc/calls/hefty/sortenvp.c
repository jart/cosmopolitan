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
#include "libc/alg/alg.h"
#include "libc/alg/arraylist.h"
#include "libc/calls/hefty/ntspawn.h"
#include "libc/dce.h"
#include "libc/nexgen32e/tinystrcmp.h"
#include "libc/str/str.h"

static int CompareStrings(const char *l, const char *r) {
  size_t i = 0;
  while (l[i] == r[i] && r[i]) ++i;
  return (l[i] & 0xff) - (r[i] & 0xff);
}

static void SortStrings(char **a, size_t n) {
  char *t;
  size_t i, j;
  for (i = 1; i < n; ++i) {
    for (t = a[i], j = i; j > 0 && CompareStrings(t, a[j - 1]) < 0; --j) {
      a[j] = a[j - 1];
    }
    a[j] = t;
  }
}

/**
 * Copies environment variable pointers and sorts them.
 *
 * This is useful for (a) binary searching; and (b) keeping the NT
 * Executive happy, which wants strings to be ordered by UNICODE
 * codepoint identifiers. That's basically what uint8_t comparisons on
 * UTF8-encoded data gives us.
 *
 * @param envp is a NULL-terminated string array
 * @return newly allocated sorted copy of envp pointer array
 */
hidden textwindows nodiscard char **sortenvp(char *const envp[]) {
  char **copy;
  size_t n, size;
  n = 0;
  while (envp[n]) n++;
  size = (n + 1) * sizeof(char *);
  if ((copy = malloc(size))) {
    memcpy(copy, envp, size);
    SortStrings(copy, n);
  }
  return copy;
}
