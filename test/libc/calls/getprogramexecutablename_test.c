/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

static char *self;

void SetUpOnce(void) {
  self = GetProgramExecutableName();
  testlib_enable_tmp_setup_teardown();
}

__attribute__((__constructor__)) static void Child(int argc, char *argv[]) {
  static bool skiparg0tests;
  if (!__program_executable_name && !IsFreebsd() && !IsNetbsd()) {
    skiparg0tests = true;
    if (argc < 2) {
      fprintf(stderr, "warning: old/no loader; skipping argv[0] tests\n");
    }
  }
  if (argc >= 2 && !strcmp(argv[1], "Child")) {
    ASSERT_EQ(argc, 4);
    EXPECT_STREQ(argv[2], GetProgramExecutableName());
    if (!skiparg0tests) {
      EXPECT_STREQ(argv[3], argv[0]);
    }
    exit(g_testlib_failed);
  }
}

TEST(GetProgramExecutableName, ofThisFile) {
  EXPECT_EQ('/', *self);
  EXPECT_TRUE(
      endswith(self, "test/libc/calls/getprogramexecutablename_test.com"));
}

TEST(GetProgramExecutableName, nullEnv) {
  SPAWN(fork);
  execve(self, (char *[]){self, "Child", self, self, 0}, (char *[]){0});
  abort();
  EXITS(0);
}

TEST(GetProramExecutableName, weirdArgv0NullEnv) {
  SPAWN(fork);
  execve(self, (char *[]){"hello", "Child", self, "hello", 0}, (char *[]){0});
  abort();
  EXITS(0);
}

TEST(GetProgramExecutableName, weirdArgv0CosmoVar) {
  char buf[32 + PATH_MAX];
  stpcpy(stpcpy(buf, "COSMOPOLITAN_PROGRAM_EXECUTABLE="), self);
  SPAWN(fork);
  execve(self, (char *[]){"hello", "Child", self, "hello", 0},
         (char *[]){buf, 0});
  abort();
  EXITS(0);
}

TEST(GetProgramExecutableName, weirdArgv0WrongCosmoVar) {
  char *bad = "COSMOPOLITAN_PROGRAM_EXECUTABLE=hi";
  SPAWN(fork);
  execve(self, (char *[]){"hello", "Child", self, "hello", 0},
         (char *[]){bad, 0});
  abort();
  EXITS(0);
}

TEST(GetProgramExecutableName, movedSelf) {
  char buf[BUFSIZ];
  ASSERT_SYS(0, 3, open(GetProgramExecutableName(), O_RDONLY));
  ASSERT_SYS(0, 4, creat("test", 0755));
  ssize_t rc;
  while ((rc = read(3, buf, BUFSIZ)) > 0) {
    ASSERT_SYS(0, rc, write(4, buf, rc));
  }
  ASSERT_EQ(0, rc);
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_NE(NULL, getcwd(buf, BUFSIZ - 5));
  stpcpy(buf + strlen(buf), "/test");
  SPAWN(fork);
  execve(buf, (char *[]){"hello", "Child", buf, "hello", 0}, (char *[]){0});
  abort();
  EXITS(0);
}
