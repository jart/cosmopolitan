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
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"
#include "tool/args/args.h"

void FreeZipArgs(void);
int LoadZipArgsImpl(int *, char ***, char *);

void TearDown(void) {
  FreeZipArgs();
}

TEST(LoadZipArgs, testNoFile_noCliArgs_doesNothing) {
  int argc = 1;
  char *args[] = {"prog", 0};
  char **argv = &args[0];
  EXPECT_EQ(0, LoadZipArgsImpl(&argc, &argv, 0));
  ASSERT_EQ(1, argc);
  EXPECT_STREQ("prog", argv[0]);
  EXPECT_EQ(NULL, argv[1]);
}

TEST(LoadZipArgs, testDefaultArgs_noCliArgs_replacesArgv) {
  int argc = 1;
  char *args[] = {"prog", 0};
  char **argv = &args[0];
  EXPECT_EQ(0, LoadZipArgsImpl(&argc, &argv, strdup("\
-x\r\n\
hello\r\n\
-y\r\n\
world\r\n\
")));
  ASSERT_EQ(5, argc);
  EXPECT_STREQ("prog", argv[0]);
  EXPECT_STREQ("-x", argv[1]);
  EXPECT_STREQ("hello", argv[2]);
  EXPECT_STREQ("-y", argv[3]);
  EXPECT_STREQ("world", argv[4]);
  EXPECT_EQ(NULL, argv[5]);
  EXPECT_EQ(5, __argc);
  EXPECT_STREQ("prog", __argv[0]);
  EXPECT_STREQ("-x", __argv[1]);
  EXPECT_STREQ("hello", __argv[2]);
  EXPECT_STREQ("-y", __argv[3]);
  EXPECT_STREQ("world", __argv[4]);
  EXPECT_EQ(NULL, __argv[5]);
}

TEST(LoadZipArgs, testDefaultArgs_hasCliArgs_impliedMergeThem) {
  int argc = 2;
  char *args[] = {"prog", "yo", 0};
  char **argv = &args[0];
  EXPECT_EQ(0, LoadZipArgsImpl(&argc, &argv, strdup("\
-x\r\n\
hello o\r\n\
-y\r\n\
world\r\n\
")));
  ASSERT_EQ(6, argc);
  EXPECT_STREQ("prog", argv[0]);
  EXPECT_STREQ("-x", argv[1]);
  EXPECT_STREQ("hello o", argv[2]);
  EXPECT_STREQ("-y", argv[3]);
  EXPECT_STREQ("world", argv[4]);
  EXPECT_STREQ("yo", argv[5]);
  EXPECT_EQ(NULL, argv[6]);
}

TEST(LoadZipArgs, testDots_hasCliArgs_mergesThem) {
  int argc = 3;
  char *args[] = {"prog", "yo", "dawg", 0};
  char **argv = &args[0];
  EXPECT_EQ(0, LoadZipArgsImpl(&argc, &argv, strdup("\
-x\r\n\
hell o\r\n\
-y\r\n\
world\r\n\
...\r\n\
")));
  ASSERT_EQ(7, argc);
  EXPECT_STREQ("prog", argv[0]);
  EXPECT_STREQ("-x", argv[1]);
  EXPECT_STREQ("hell o", argv[2]);
  EXPECT_STREQ("-y", argv[3]);
  EXPECT_STREQ("world", argv[4]);
  EXPECT_STREQ("yo", argv[5]);
  EXPECT_STREQ("dawg", argv[6]);
  EXPECT_EQ(NULL, argv[7]);
}
