/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Gabriel Ravier                                                │
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
#include "libc/calls/struct/rlimit.h"
#include "libc/errno.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/testlib/testlib.h"

static void limit_memory_to_1mb() {
  struct rlimit limit = {};
  ASSERT_GE(getrlimit(RLIMIT_AS, &limit), 0);

  if (limit.rlim_max > 1000000 || limit.rlim_max == RLIM_INFINITY) {
    limit.rlim_max = 1000000;
    limit.rlim_cur = limit.rlim_max;
    ASSERT_GE(setrlimit(RLIMIT_AS, &limit), 0);
  }
}

static void check_double_format_enomem(const char *fmt) {
  errno = 0;
  int result = printf(fmt, 1.0);
  ASSERT_LE(result, 0);
  ASSERT_EQ(errno, ENOMEM);
}

static void check_long_double_format_enomem(const char *fmt) {
  errno = 0;
  int result = printf(fmt, 1.0L);
  ASSERT_LE(result, 0);
  ASSERT_EQ(errno, ENOMEM);
}

TEST(snprintf, enomemFloat) {
  limit_memory_to_1mb();

  check_double_format_enomem("%.1000000f");
  check_double_format_enomem("%.1000000g");
  check_double_format_enomem("%.1000000e");

  check_long_double_format_enomem("%.1000000Lf");
  check_long_double_format_enomem("%.1000000Lg");
  check_long_double_format_enomem("%.1000000Le");
}
