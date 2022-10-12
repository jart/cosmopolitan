/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/dce.h"
#include "libc/mem/copyfd.internal.h"
#include "libc/mem/gc.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

char testlib_enable_tmp_setup_teardown;

TEST(system, haveShell) {
  ASSERT_TRUE(system(0));
}

TEST(system, echo) {
  ASSERT_EQ(0, system("echo hello >\"hello there.txt\""));
  EXPECT_STREQ("hello\n", _gc(xslurp("hello there.txt", 0)));
}

TEST(system, exit) {
  ASSERT_EQ(123, WEXITSTATUS(system("exit 123")));
}

TEST(system, testStdoutRedirect) {
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_EQ(0, system("./echo.com hello >hello.txt"));
  EXPECT_STREQ("hello\n", _gc(xslurp("hello.txt", 0)));
}

TEST(system, testStdoutRedirect_withSpacesInFilename) {
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_EQ(0, system("./echo.com hello >\"hello there.txt\""));
  EXPECT_STREQ("hello\n", _gc(xslurp("hello there.txt", 0)));
}

BENCH(system, bench) {
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  EZBENCH2("system cmd", donothing, system("./echo.com hi >/dev/null"));
  EZBENCH2("cocmd echo", donothing, system("echo hi >/dev/null"));
  EZBENCH2("cocmd exit", donothing, system("exit"));
}

TEST(system, and) {
  ASSERT_EQ(1, WEXITSTATUS(system("false && false")));
  ASSERT_EQ(1, WEXITSTATUS(system("true&& false")));
  ASSERT_EQ(1, WEXITSTATUS(system("false &&true")));
  ASSERT_EQ(0, WEXITSTATUS(system("true&&true")));
}

TEST(system, or) {
  ASSERT_EQ(1, WEXITSTATUS(system("false || false")));
  ASSERT_EQ(0, WEXITSTATUS(system("true|| false")));
  ASSERT_EQ(0, WEXITSTATUS(system("false ||true")));
  ASSERT_EQ(0, WEXITSTATUS(system("true||true")));
}

TEST(system, async1) {
  ASSERT_EQ(123, WEXITSTATUS(system("exit 123 & "
                                    "wait $!")));
}

TEST(system, async4) {
  ASSERT_EQ(0, WEXITSTATUS(system("echo a >a & "
                                  "echo b >b & "
                                  "echo c >c & "
                                  "echo d >d & "
                                  "wait")));
  ASSERT_TRUE(fileexists("a"));
  ASSERT_TRUE(fileexists("b"));
  ASSERT_TRUE(fileexists("c"));
  ASSERT_TRUE(fileexists("d"));
}

TEST(system, equals) {
  setenv("var", "wand", true);
  ASSERT_EQ(0, WEXITSTATUS(system("test a = a")));
  ASSERT_EQ(1, WEXITSTATUS(system("test a = b")));
  ASSERT_EQ(0, WEXITSTATUS(system("test x$var = xwand")));
  ASSERT_EQ(0, WEXITSTATUS(system("[ a = a ]")));
  ASSERT_EQ(1, WEXITSTATUS(system("[ a = b ]")));
}

TEST(system, notequals) {
  ASSERT_EQ(1, WEXITSTATUS(system("test a != a")));
  ASSERT_EQ(0, WEXITSTATUS(system("test a != b")));
}

TEST(system, usleep) {
  ASSERT_EQ(0, WEXITSTATUS(system("usleep & kill $!")));
}

TEST(system, kill) {
  int ws = system("kill -TERM $$; usleep");
  if (!IsWindows()) ASSERT_EQ(SIGTERM, WTERMSIG(ws));
}
