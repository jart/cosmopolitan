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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

void *memccpy2(void *, const void *, int, size_t);

TEST(memccpy, testStringCopy) {
  char buf[16];
  EXPECT_EQ(buf + 3, memccpy(buf, "hi", '\0', sizeof(buf)));
  EXPECT_STREQ("hi", buf);
}

TEST(memccpy, testOverflow) {
  char buf[1];
  EXPECT_EQ(NULL, memccpy(buf, "hi", '\0', sizeof(buf)));
}

TEST(memccpy, testZeroLength_doesNothing) {
  char buf[1];
  EXPECT_EQ(NULL, memccpy(buf, "hi", '\0', 0));
}

TEST(memccpy, memcpy) {
  unsigned n, n2;
  char *b1, *b2, *b3, *e1, *e2;
  for (n = 0; n < 1026; ++n) {
    b1 = tmalloc(n);
    b2 = tmalloc(n);
    b3 = tmalloc(n);
    e1 = tinymemccpy(b2, b1, 31337, n);
    e2 = memccpy(b3, b1, 31337, n);
    n2 = e1 ? e1 - b1 : n;
    ASSERT_EQ(e1, e2);
    ASSERT_EQ(0, memcmp(b2, b3, n2));
    tfree(b3);
    tfree(b2);
    tfree(b1);
  }
}
