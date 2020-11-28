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
#include "libc/bits/bits.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(getline, testEmpty) {
  FILE *f = fmemopen("", 0, "r+");
  char *line = NULL;
  size_t linesize = 0;
  EXPECT_EQ(-1, getline(&line, &linesize, f));
  EXPECT_TRUE(feof(f));
  EXPECT_FALSE(ferror(f));
  EXPECT_EQ(0, fclose(f));
  free(line);
}

TEST(getline, testOneWithoutLineFeed) {
  FILE *f = fmemopen("hello", 5, "r+");
  char *line = NULL;
  size_t linesize = 0;
  ASSERT_EQ(5, getline(&line, &linesize, f));
  EXPECT_STREQ("hello", line);
  EXPECT_TRUE(feof(f));
  EXPECT_FALSE(ferror(f));
  ASSERT_EQ(-1, getline(&line, &linesize, f));
  EXPECT_EQ(0, fclose(f));
  free(line);
}

TEST(getline, testTwoLines) {
  const char *s = "hello\nthere\n";
  FILE *f = fmemopen(s, strlen(s), "r+");
  char *line = NULL;
  size_t linesize = 0;
  ASSERT_EQ(6, getline(&line, &linesize, f));
  EXPECT_STREQ("hello\n", line);
  EXPECT_FALSE(feof(f) | ferror(f));
  ASSERT_EQ(6, getline(&line, &linesize, f));
  EXPECT_STREQ("there\n", line);
  ASSERT_EQ(-1, getline(&line, &linesize, f));
  EXPECT_STREQ("", line);
  EXPECT_EQ(0, fclose(f));
  free(line);
}

TEST(getline, testBinaryLine_countExcludesOnlyTheBonusNul) {
  const char s[] = "he\0\3o\n";
  FILE *f = fmemopen(s, sizeof(s), "r+");
  char *line = NULL;
  size_t linesize = 0;
  ASSERT_EQ(6, getline(&line, &linesize, f));
  EXPECT_BINEQ(u"he ♥o◙ ", line);
  fclose(f);
  free(line);
}
