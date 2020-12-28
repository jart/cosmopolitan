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
#include "libc/alg/alg.h"
#include "libc/alg/arraylist.internal.h"
#include "libc/calls/hefty/ntspawn.h"
#include "libc/dce.h"
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
