/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/tab.internal.h"
#include "libc/testlib/testlib.h"

TEST(kBase36, test) {
  EXPECT_EQ(0, kBase36[0]);
  EXPECT_EQ(0, kBase36[255]);
  EXPECT_EQ(0, kBase36['!']);
  EXPECT_EQ(0 + 1, kBase36['0']);
  EXPECT_EQ(1 + 1, kBase36['1']);
  EXPECT_EQ(9 + 1, kBase36['9']);
  EXPECT_EQ(10 + 1, kBase36['a']);
  EXPECT_EQ(10 + 1, kBase36['A']);
  EXPECT_EQ(35 + 1, kBase36['z']);
  EXPECT_EQ(35 + 1, kBase36['Z']);
  EXPECT_EQ(0, kBase36['a' - 1]);
  EXPECT_EQ(0, kBase36['A' - 1]);
  EXPECT_EQ(0, kBase36['z' + 1]);
  EXPECT_EQ(0, kBase36['Z' + 1]);
}
