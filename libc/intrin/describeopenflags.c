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
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sol.h"

#define N (4096 / 2 / sizeof(struct DescribeFlags))

/**
 * Describes clock_gettime() clock argument.
 */
const char *(DescribeOpenFlags)(char buf[128], int x) {
  char *p;
  int i, n;
  const char *pipe;
  struct DescribeFlags d[N];
  if (x == -1) return "-1";
  p = buf;
  switch (x & O_ACCMODE) {
    case O_RDONLY:
      p = stpcpy(p, "O_RDONLY");
      x &= ~O_ACCMODE;
      pipe = "|";
      break;
    case O_WRONLY:
      p = stpcpy(p, "O_WRONLY");
      x &= ~O_ACCMODE;
      pipe = "|";
      break;
    case O_RDWR:
      p = stpcpy(p, "O_RDWR");
      x &= ~O_ACCMODE;
      pipe = "|";
      break;
    default:
      pipe = "";
      break;
  }
  if (x) {
    p = stpcpy(p, pipe);
    for (n = 0; kOpenFlags[n].x != MAGNUM_TERMINATOR; ++n) {
      if (n == N) notpossible;
    }
    for (i = 0; i < n; ++i) {
      d[i].flag = MAGNUM_NUMBER(kOpenFlags, i);
      d[i].name = MAGNUM_STRING(kOpenFlags, i);
    }
    DescribeFlags(p, 128 - (p - buf), d, n, "O_", x);
  }
  return buf;
}
