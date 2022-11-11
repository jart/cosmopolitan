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
#include "libc/assert.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/macros.internal.h"
#include "libc/sysv/consts/sol.h"

#ifdef DescribeOpenFlags
#undef DescribeOpenFlags
#endif

#define N (PAGESIZE / 2 / sizeof(struct DescribeFlags))

/**
 * Describes clock_gettime() clock argument.
 */
const char *DescribeOpenFlags(char buf[128], int x) {
  char *s;
  int i, n;
  struct DescribeFlags d[N];
  if (x == -1) return "-1";
  // TODO(jart): unify DescribeFlags and MagnumStr data structures
  for (n = 0; kOpenFlags[n].x != MAGNUM_TERMINATOR; ++n) {
    if (n == N) notpossible;
  }
  for (i = 0; i < n; ++i) {
    d[i].flag = MAGNUM_NUMBER(kOpenFlags, i);
    d[i].name = MAGNUM_STRING(kOpenFlags, i);
  }
  return DescribeFlags(buf, 128, d, n, "O_", x);
}
