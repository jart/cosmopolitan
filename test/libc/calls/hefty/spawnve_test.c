/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/hefty/spawn.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/testlib/testlib.h"

#define CMD (IsWindows() ? "cmd" : "sh")
#define ARG (IsWindows() ? "/c" : "-c")
#define COD (IsWindows() ? "echo %BOOP%" : "echo $BOOP")
#define OUT (IsWindows() ? u"hello♪◙" : u"hello◙")

TEST(spawnve, testIpc) {
  ssize_t got;
  int pid, wstatus;
  char buf[16] = {0};
  int tubes[3] = {STDIN_FILENO, -1, STDERR_FILENO};
  errno = 0;
  setenv("BOOP", "hello", true);
  ASSERT_EQ(0, errno);
  ASSERT_NE(-1, (pid = spawnlp(0, tubes, CMD, CMD, ARG, COD, NULL)));
  ASSERT_EQ(0, errno);
  ASSERT_NE(-1, (got = read(tubes[1], buf, sizeof(buf))));
  ASSERT_EQ(0, errno);
  ASSERT_NE(-1, waitpid(pid, &wstatus, 0));
  ASSERT_EQ(0, errno);
  EXPECT_EQ(0, WEXITSTATUS(wstatus));
  EXPECT_BINEQ(OUT, buf);
}

TEST(spawnve, testExit) {
  int pid, wstatus;
  ASSERT_NE(-1, (pid = spawnlp(0, NULL, CMD, CMD, ARG, "exit 42", NULL)));
  ASSERT_NE(-1, waitpid(pid, &wstatus, 0));
  EXPECT_EQ(42, WEXITSTATUS(wstatus));
}

TEST(spawnve, testSpawnSelf) {
  int pid, wstatus;
  ASSERT_NE(-1,
            (pid = spawnlp(0, NULL, (const char *)getauxval(AT_EXECFN),
                           program_invocation_name, "--testSpawnSelf", NULL)));
  ASSERT_NE(-1, waitpid(pid, &wstatus, 0));
  EXPECT_EQ(123, WEXITSTATUS(wstatus));
}
static textstartup void onspawnself() {
  if (g_argc > 1 && strcmp(g_argv[1], "--testSpawnSelf") == 0) {
    _exit(123);
  }
}
const void *const onspawnself_ctor[] initarray = {onspawnself};
