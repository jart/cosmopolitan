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
#include "libc/testlib/testlib.h"

TEST(getline, testEmpty) {
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  char *line = NULL;
  size_t linesize = 0;
  EXPECT_EQ(-1, getline(&line, &linesize, f));
  EXPECT_TRUE(feof(f));
  EXPECT_FALSE(ferror(f));
  EXPECT_EQ(0, fclose(f));
  free(line);
}

TEST(getline, testOneWithoutLineFeed) {
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(5, fwrite("hello", 1, 5, f));
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
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(12, fwrite("hello\nthere\n", 1, 12, f));
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
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  fwrite("he\0\3o\n", 1, 6, f);
  char *line = NULL;
  size_t linesize = 0;
  ASSERT_EQ(6, getline(&line, &linesize, f));
  EXPECT_BINEQ(u"he ♥o◙ ", line);
  fclose(f);
  free(line);
}
