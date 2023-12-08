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
#include "libc/serialize.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.h"
#include "libc/stdio/strlist.internal.h"
#include "libc/testlib/testlib.h"

struct StrList sl;

void TearDown(void) {
  FreeStrList(&sl);
}

TEST(strlist, test) {
  int i;
  char *b = 0;
  ASSERT_NE(-1, (i = AppendStrList(&sl)));
  ASSERT_NE(-1, appends(&sl.p[i], "world"));
  ASSERT_NE(-1, (i = AppendStrList(&sl)));
  ASSERT_NE(-1, appends(&sl.p[i], "hello"));
  SortStrList(&sl);
  ASSERT_NE(-1, JoinStrList(&sl, &b, READ16LE(", ")));
  EXPECT_STREQ("hello, world", b);
  free(b);
}

TEST(strlist, testNumbers) {
  int i;
  char *b = 0;
  ASSERT_NE(-1, (i = AppendStrList(&sl)));
  ASSERT_NE(-1, appends(&sl.p[i], "2"));
  ASSERT_NE(-1, (i = AppendStrList(&sl)));
  ASSERT_NE(-1, appends(&sl.p[i], "1"));
  SortStrList(&sl);
  ASSERT_NE(-1, JoinStrList(&sl, &b, ':'));
  EXPECT_STREQ("1:2", b);
  free(b);
}
