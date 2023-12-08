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
#include "tool/build/lib/getargs.h"
#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(getargs, test) {
  struct GetArgs ga;
  char *args[] = {"@a1", "yo", "@a2", "dawg", "@a3", 0};
  ASSERT_SYS(0, 3, creat("a1", 0644));
  ASSERT_SYS(0, 9, write(3, " hi  mog\n", 9));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, creat("a2", 0644));
  ASSERT_SYS(0, 1, write(3, "\n", 1));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, creat("a3", 0644));
  ASSERT_SYS(0, 4, write(3, "fun\t", 4));
  ASSERT_SYS(0, 0, close(3));
  getargs_init(&ga, args);
  EXPECT_STREQ("hi", getargs_next(&ga));
  EXPECT_STREQ("mog", getargs_next(&ga));
  EXPECT_STREQ("yo", getargs_next(&ga));
  EXPECT_STREQ("dawg", getargs_next(&ga));
  EXPECT_STREQ("fun", getargs_next(&ga));
  EXPECT_EQ(NULL, getargs_next(&ga));
  EXPECT_EQ(NULL, getargs_next(&ga));
  getargs_destroy(&ga);
}

void GetAllArgs(char **args) {
  size_t i = 0;
  struct GetArgs ga;
  getargs_init(&ga, args);
  while (getargs_next(&ga)) ++i;
  getargs_destroy(&ga);
  ASSERT_EQ(2 + 13790, i);
}

BENCH(getargs, bench) {
  int i;
  char *args[] = {"-o", "doge.txt", "@args.txt", 0};
  ASSERT_SYS(0, 3, creat("args.txt", 0644));
  for (i = 0; i < 13790; ++i)
    ASSERT_NE(-1, write(3, __argv[0], strlen(__argv[0]) + 1));
  ASSERT_SYS(0, 0, close(3));
  EZBENCH2("getargs", donothing, GetAllArgs(args));
}
