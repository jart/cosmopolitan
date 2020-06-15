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
#include "libc/runtime/internal.h"
#include "libc/testlib/testlib.h"

TEST(getdosargv, empty) {
  size_t max = 4;
  size_t size = ARG_MAX;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(0, getdosargv(u"", buf, size, argv, max));
  EXPECT_EQ(NULL, argv[0]);
  tfree(argv);
  tfree(buf);
}

TEST(getdosargv, emptyish) {
  size_t max = 4;
  size_t size = ARG_MAX;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(0, getdosargv(u"  ", buf, size, argv, max));
  EXPECT_EQ(NULL, argv[0]);
  tfree(argv);
  tfree(buf);
}

TEST(getdosargv, basicUsage) {
  size_t max = 4;
  size_t size = ARG_MAX;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(3, getdosargv(u"a\t \"b  c\"  d ", buf, size, argv, max));
  EXPECT_STREQ("a", argv[0]);
  EXPECT_STREQ("b  c", argv[1]);
  EXPECT_STREQ("d", argv[2]);
  EXPECT_EQ(NULL, argv[3]);
  tfree(argv);
  tfree(buf);
}

TEST(getdosargv, advancedUsage) {
  size_t max = 4;
  size_t size = ARG_MAX;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(2, getdosargv(u"(╯°□°)╯︵ ┻━┻", buf, size, argv, max));
  EXPECT_STREQ("(╯°□°)╯︵", argv[0]);
  EXPECT_STREQ("┻━┻", argv[1]);
  EXPECT_EQ(NULL, argv[2]);
  tfree(argv);
  tfree(buf);
}

TEST(getdosargv, testAegeanGothicSupplementaryPlanes) {
  size_t max = 4; /* these symbols are almost as old as dos */
  size_t size = ARG_MAX;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(2, getdosargv(u"𐄷𐄸𐄹𐄺𐄻𐄼 𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷", buf, size, argv, max));
  EXPECT_STREQ("𐄷𐄸𐄹𐄺𐄻𐄼", argv[0]);
  EXPECT_STREQ("𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷", argv[1]);
  EXPECT_EQ(NULL, argv[2]);
  tfree(argv);
  tfree(buf);
}

TEST(getdosargv, realWorldUsage) {
  size_t max = 512;
  size_t size = ARG_MAX;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(5, getdosargv(u"C:\\Users\\jtunn\\printargs.com oh yes yes yes",
                          buf, size, argv, max));
  EXPECT_STREQ("C:\\Users\\jtunn\\printargs.com", argv[0]);
  EXPECT_STREQ("oh", argv[1]);
  EXPECT_STREQ("yes", argv[2]);
  EXPECT_STREQ("yes", argv[3]);
  EXPECT_STREQ("yes", argv[4]);
  EXPECT_EQ(NULL, argv[5]);
  tfree(argv);
  tfree(buf);
}

TEST(getdosargv, bufferOverrun_countIsStillAccurate_truncatesMemoryWithGrace) {
  size_t max = 3;
  size_t size = 7;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(3, getdosargv(u"a\t \"b  c\"  d ", buf, size, argv, max));
  EXPECT_STREQ("a", argv[0]);
  EXPECT_STREQ("b  c", argv[1]);
  EXPECT_EQ(NULL, argv[2]);
  tfree(argv);
  tfree(buf);
}

TEST(getdosargv, pureScanningMode) {
  size_t max = 0;
  size_t size = 0;
  char *buf = NULL;
  char **argv = NULL;
  EXPECT_EQ(3, getdosargv(u"a b c", buf, size, argv, max));
}

TEST(getdosargv, justSlashQuote) {
  size_t max = 4, size = 16;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(1, getdosargv(u"\"\\\\\\\"\"", buf, size, argv, max));
  EXPECT_STREQ("\\\"", argv[0]);
  tfree(argv);
  tfree(buf);
}

TEST(getdosargv, quoteInMiddleOfArg_wontSplitArg) {
  size_t max = 4, size = 16;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(1, getdosargv(u"hi\"\"there", buf, size, argv, max));
  EXPECT_STREQ("hithere", argv[0]);
  max = 4, size = 16;
  EXPECT_EQ(1, getdosargv(u"hi\" \"there", buf, size, argv, max));
  EXPECT_STREQ("hi there", argv[0]);
  tfree(argv);
  tfree(buf);
}

TEST(getdosargv, waqQuoting1) {
  size_t max = 4;
  size_t size = ARG_MAX;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(2,
            getdosargv(u"a\\\\\"\"\"\"\"\"\"\"b c\" d", buf, size, argv, max));
  EXPECT_STREQ("a\\\"\"b", argv[0]);
  EXPECT_STREQ("c d", argv[1]);
  EXPECT_EQ(NULL, argv[2]);
  tfree(argv);
  tfree(buf);
}

TEST(getdosargv, waqQuoting2) {
  size_t max = 4;
  size_t size = ARG_MAX;
  char *buf = tmalloc(size * sizeof(char));
  char **argv = tmalloc(max * sizeof(char *));
  EXPECT_EQ(2, getdosargv(u"\"a\\\"b c\" d", buf, size, argv, max));
  EXPECT_STREQ("a\"b c", argv[0]);
  EXPECT_STREQ("d", argv[1]);
  EXPECT_EQ(NULL, argv[2]);
  tfree(argv);
  tfree(buf);
}
