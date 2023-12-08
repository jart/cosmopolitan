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
#include "libc/calls/calls.h"
#include "libc/calls/struct/rusage.h"
#include "libc/log/appendresourcereport.internal.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/testlib/testlib.h"

TEST(AppendResourceReport, testEmpty_doesNothing) {
  char *b = 0;
  struct rusage ru;
  bzero(&ru, sizeof(ru));
  AppendResourceReport(&b, &ru, " / ");
  EXPECT_EQ(NULL, b);
}

TEST(AppendResourceReport, testMemory_balloons) {
  char *b = 0;
  struct rusage ru;
  bzero(&ru, sizeof(ru));
  ru.ru_maxrss = 1;
  AppendResourceReport(&b, &ru, "");
  EXPECT_STREQ("ballooned to 1kb in size", b);
  free(b);
}

#if 0
TEST(AppendResourceReport, impure) {
  char *b = 0;
  struct rusage ru;
  EXPECT_EQ(0, getrusage(RUSAGE_SELF, &ru));
  AppendResourceReport(&b, &ru, "\n");
  EXPECT_STREQ("ballooned to 1kb in size", b);
  free(b);
}
#endif
