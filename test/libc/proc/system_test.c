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
#include "libc/calls/calls.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#ifdef __x86_64__

#define GETEXITSTATUS(x)                                                \
  ({                                                                    \
    int status_ = (x);                                                  \
    if (WIFSIGNALED(status_)) {                                         \
      kprintf("%s:%d: %s terminated with %G\n", __FILE__, __LINE__, #x, \
              WTERMSIG(status_));                                       \
      exit(1);                                                          \
    }                                                                   \
    WEXITSTATUS(status_);                                               \
  })

__static_yoink("_tr");
__static_yoink("glob");

int pipefd[2];
int stdoutBack;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void CaptureStdout(void) {
  ASSERT_NE(-1, (stdoutBack = dup(1)));
  ASSERT_SYS(0, 0, pipe(pipefd));
  ASSERT_NE(-1, dup2(pipefd[1], 1));
}

void RestoreStdout(void) {
  ASSERT_SYS(0, 1, dup2(stdoutBack, 1));
  ASSERT_SYS(0, 0, close(stdoutBack));
  ASSERT_SYS(0, 0, close(pipefd[1]));
  ASSERT_SYS(0, 0, close(pipefd[0]));
}

TEST(system, haveShell) {
  ASSERT_TRUE(system(0));
}

TEST(system, echo) {
  ASSERT_EQ(0, system("echo hello >\"hello there.txt\""));
  EXPECT_STREQ("hello\n", gc(xslurp("hello there.txt", 0)));
}

TEST(system, exit) {
  ASSERT_EQ(123, GETEXITSTATUS(system("exit 123")));
}

TEST(system, testStdoutRedirect) {
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_EQ(0, system("./echo.com hello >hello.txt"));
  EXPECT_STREQ("hello\n", gc(xslurp("hello.txt", 0)));
}

TEST(system, testStdoutRedirect_withSpacesInFilename) {
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_EQ(0, system("./echo.com hello >\"hello there.txt\""));
  EXPECT_STREQ("hello\n", gc(xslurp("hello there.txt", 0)));
}

TEST(system, testStderrRedirect_toStdout) {
  CaptureStdout();
  int stderrBack = dup(2);
  ASSERT_NE(-1, stderrBack);
  char buf[5] = {0};
  ASSERT_NE(-1, dup2(1, 2));
  bool success = false;
  if (GETEXITSTATUS(system("echo aaa 2>&1")) == 0) {
    success = read(pipefd[0], buf, 4) == (4);
  }
  ASSERT_NE(-1, dup2(stderrBack, 2));
  ASSERT_EQ(true, success);
  ASSERT_STREQ("aaa\n", buf);
  buf[0] = 0;
  buf[1] = 0;
  buf[2] = 0;
  buf[3] = 0;
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_NE(-1, dup2(1, 2));
  success = false;
  if (GETEXITSTATUS(system("./echo.com aaa 2>&1")) == 0) {
    success = read(pipefd[0], buf, 4) == (4);
  }
  ASSERT_NE(-1, dup2(stderrBack, 2));
  ASSERT_EQ(true, success);
  ASSERT_STREQ("aaa\n", buf);
  RestoreStdout();
}

TEST(system, and) {
  ASSERT_EQ(1, GETEXITSTATUS(system("false && false")));
  ASSERT_EQ(1, GETEXITSTATUS(system("true&& false")));
  ASSERT_EQ(1, GETEXITSTATUS(system("false &&true")));
  ASSERT_EQ(0, GETEXITSTATUS(system("true&&true")));
}

TEST(system, or) {
  ASSERT_EQ(1, GETEXITSTATUS(system("false || false")));
  ASSERT_EQ(0, GETEXITSTATUS(system("true|| false")));
  ASSERT_EQ(0, GETEXITSTATUS(system("false ||true")));
  ASSERT_EQ(0, GETEXITSTATUS(system("true||true")));
}

TEST(system, async1) {
  ASSERT_EQ(123, GETEXITSTATUS(system("exit 123 & "
                                      "wait $!")));
}

TEST(system, async4) {
  ASSERT_EQ(0, GETEXITSTATUS(system("echo a >a & "
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
  ASSERT_EQ(0, GETEXITSTATUS(system("test a = a")));
  ASSERT_EQ(1, GETEXITSTATUS(system("test a = b")));
  ASSERT_EQ(0, GETEXITSTATUS(system("test x$var = xwand")));
  ASSERT_EQ(0, GETEXITSTATUS(system("[ a = a ]")));
  ASSERT_EQ(1, GETEXITSTATUS(system("[ a = b ]")));
}

TEST(system, notequals) {
  ASSERT_EQ(1, GETEXITSTATUS(system("test a != a")));
  ASSERT_EQ(0, GETEXITSTATUS(system("test a != b")));
}

TEST(system, usleep) {
  ASSERT_EQ(0, GETEXITSTATUS(system("usleep & kill $!")));
}

TEST(system, kill) {
  int ws = system("kill -TERM $$; usleep");
  if (!IsWindows()) ASSERT_EQ(SIGTERM, WTERMSIG(ws));
}

TEST(system, exitStatusPreservedAfterSemiColon) {
  testlib_extract("/zip/false.com", "false.com", 0755);
  ASSERT_EQ(1, GETEXITSTATUS(system("false;")));
  ASSERT_EQ(1, GETEXITSTATUS(system("false; ")));
  ASSERT_EQ(1, GETEXITSTATUS(system("./false.com;")));
  ASSERT_EQ(1, GETEXITSTATUS(system("./false.com;")));
  CaptureStdout();
  ASSERT_EQ(0, GETEXITSTATUS(system("false; echo $?")));
  char buf[9] = {0};
  ASSERT_EQ(2, read(pipefd[0], buf, 8));
  ASSERT_STREQ("1\n", buf);
  ASSERT_EQ(0, GETEXITSTATUS(system("./false.com; echo $?")));
  ASSERT_EQ(2, read(pipefd[0], buf, 8));
  ASSERT_STREQ("1\n", buf);
  ASSERT_EQ(0, GETEXITSTATUS(system("echo -n hi")));
  EXPECT_EQ(2, read(pipefd[0], buf, 8));
  ASSERT_STREQ("hi", buf);
  RestoreStdout();
}

TEST(system, devStdout) {
  CaptureStdout();
  ASSERT_EQ(0, GETEXITSTATUS(system("echo sup >/dev/stdout")));
  char buf[16] = {0};
  ASSERT_EQ(4, read(pipefd[0], buf, 16));
  ASSERT_STREQ("sup\n", buf);
  RestoreStdout();
}

TEST(system, globio) {
  char buf[9] = {0};
  CaptureStdout();
  ASSERT_SYS(0, 0, touch("a", 0644));
  ASSERT_SYS(0, 0, touch("b", 0644));
  ASSERT_EQ(0, GETEXITSTATUS(system("echo *")));
  EXPECT_EQ(4, read(pipefd[0], buf, 8));
  ASSERT_STREQ("a b\n", buf);
  RestoreStdout();
}

TEST(system, allowsLoneCloseCurlyBrace) {
  CaptureStdout();
  char buf[6] = {0};
  ASSERT_EQ(0, GETEXITSTATUS(system("echo \"aaa\"}")));
  ASSERT_EQ(5, read(pipefd[0], buf, 5));
  ASSERT_STREQ("aaa}\n", buf);
  bzero(buf, 6);
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_EQ(0, GETEXITSTATUS(system("./echo.com \"aaa\"}")));
  ASSERT_EQ(5, read(pipefd[0], buf, 5));
  ASSERT_STREQ("aaa}\n", buf);
  RestoreStdout();
}

TEST(system, glob) {
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_EQ(0, system("./ec*.com aaa"));
  ASSERT_EQ(0, system("./ec?o.com aaa"));
}

TEST(system, env) {
  ASSERT_EQ(0, GETEXITSTATUS(system("env - a=b c=d >res")));
  ASSERT_STREQ("a=b\nc=d\n", gc(xslurp("res", 0)));
  ASSERT_EQ(0, GETEXITSTATUS(system("env -i -0 a=b c=d >res")));
  ASSERT_STREQN("a=b\0c=d\0", gc(xslurp("res", 0)), 8);
  ASSERT_EQ(0, GETEXITSTATUS(system("env -i0 a=b c=d >res")));
  ASSERT_STREQN("a=b\0c=d\0", gc(xslurp("res", 0)), 8);
  ASSERT_EQ(0, GETEXITSTATUS(system("env - a=b c=d -u a z=g >res")));
  ASSERT_STREQ("c=d\nz=g\n", gc(xslurp("res", 0)));
  ASSERT_EQ(0, GETEXITSTATUS(system("env - a=b c=d -ua z=g >res")));
  ASSERT_STREQ("c=d\nz=g\n", gc(xslurp("res", 0)));
  ASSERT_EQ(0, GETEXITSTATUS(system("env - dope='show' >res")));
  ASSERT_STREQ("dope=show\n", gc(xslurp("res", 0)));
}

TEST(system, pipelineCanOutputToFile) {
  ASSERT_EQ(0, GETEXITSTATUS(system("echo hello | tr a-z A-Z >res")));
  ASSERT_STREQ("HELLO\n", gc(xslurp("res", 0)));
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_EQ(0, GETEXITSTATUS(system("./echo.com hello | tr a-z A-Z >res")));
  ASSERT_STREQ("HELLO\n", gc(xslurp("res", 0)));
}

TEST(system, pipelineCanOutputBackToSelf) {
  char buf[16] = {0};
  CaptureStdout();
  ASSERT_EQ(0, GETEXITSTATUS(system("echo hello | tr a-z A-Z")));
  ASSERT_SYS(0, 6, read(pipefd[0], buf, 16));
  ASSERT_STREQ("HELLO\n", buf);
  ASSERT_EQ(0, GETEXITSTATUS(system("echo hello | exec tr a-z A-Z")));
  ASSERT_SYS(0, 6, read(pipefd[0], buf, 16));
  ASSERT_STREQ("HELLO\n", buf);
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_EQ(0, GETEXITSTATUS(system("./echo.com hello | tr a-z A-Z")));
  ASSERT_SYS(0, 6, read(pipefd[0], buf, 16));
  ASSERT_STREQ("HELLO\n", buf);
  ASSERT_EQ(0, GETEXITSTATUS(system("./echo.com hello | exec tr a-z A-Z")));
  ASSERT_SYS(0, 6, read(pipefd[0], buf, 16));
  ASSERT_STREQ("HELLO\n", buf);
  RestoreStdout();
}

int system2(const char *);

BENCH(system, bench) {
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  EZBENCH2("system cmd", donothing, system("./echo.com hi >/dev/null"));
  EZBENCH2("systemvpe cmd", donothing,
           systemvpe("./echo.com", (char *[]){"./echo.com", "hi", 0}, 0));
  EZBENCH2("cocmd echo", donothing, system("echo hi >/dev/null"));
  EZBENCH2("cocmd exit", donothing, system("exit"));
}

#endif /* __x86_64__ */
