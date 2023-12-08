/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Gavin Arthur Hayes                                            │
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
#include "libc/calls/groups.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/testlib/testlib.h"

TEST(DescribeGidList, test) {
  uint32_t list[] = {8, 67, 530, 9};
  uint32_t biglist[] = {8, 67, 530, 9, 8, 67, 530, 9, 8, 67, 530, 9,
                     8, 67, 530, 9, 8, 67, 530, 9, 8, 67, 530, 9,
                     8, 67, 530, 9, 8, 67, 530, 9, 8, 67, 530, 9};
  EXPECT_STREQ("n/a",
               DescribeGidList(-1, sizeof(list) / sizeof(list[0]), list));
  EXPECT_STREQ("n/a", DescribeGidList(0, -1, list));
  EXPECT_STREQ("{}", DescribeGidList(0, 0, list));
  EXPECT_STREQ("NULL", DescribeGidList(0, 9001, NULL));
  EXPECT_STREQ("{8, 67, 530, 9}",
               DescribeGidList(0, sizeof(list) / sizeof(list[0]), list));
  EXPECT_STREQ("{8, 67, 530}",
               DescribeGidList(0, sizeof(list) / sizeof(list[0]) - 1, list));
  EXPECT_STREQ(
      "{8, 67, 530, 9, 8, 67, 530, 9, 8, 67, 530, 9, 8, 67, 530, 9, 8, 67, "
      "530, 9, 8, 67, 530, 9, 8, 67, 530, 9, 8, 67, 530, 9, 8, ...",
      DescribeGidList(0, sizeof(biglist) / sizeof(biglist[0]), biglist));
}
