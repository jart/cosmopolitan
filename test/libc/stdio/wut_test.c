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
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/mem/mem.h"
#include "libc/stdio/posix_spawn.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

char testlib_enable_tmp_setup_teardown;

TEST(posix_spawn, torture) {
  int ws, pid;
  short flags;
  sigset_t allsig;
  posix_spawnattr_t attr;
  posix_spawn_file_actions_t fa;
  sigfillset(&allsig);
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  // XXX: NetBSD doesn't seem to let us set the scheduler to itself ;_;
  ASSERT_EQ(0, posix_spawnattr_init(&attr));
  ASSERT_EQ(0, posix_spawnattr_setflags(
                   &attr, POSIX_SPAWN_RESETIDS | POSIX_SPAWN_SETSIGDEF |
                              POSIX_SPAWN_SETSIGDEF | POSIX_SPAWN_SETPGROUP |
                              POSIX_SPAWN_SETSIGMASK |
                              (IsNetbsd() ? 0 : POSIX_SPAWN_SETSCHEDULER)));
  ASSERT_EQ(0, posix_spawnattr_setsigmask(&attr, &allsig));
  ASSERT_EQ(0, posix_spawnattr_setsigdefault(&attr, &allsig));
  ASSERT_EQ(0, posix_spawn_file_actions_init(&fa));
  ASSERT_EQ(0, posix_spawn_file_actions_addclose(&fa, 0));
  ASSERT_EQ(
      0, posix_spawn_file_actions_addopen(&fa, 1, "/dev/null", O_WRONLY, 0644));
  ASSERT_EQ(0, posix_spawn_file_actions_adddup2(&fa, 1, 0));
  for (int i = 0; i < 15; ++i) {
    char *volatile zzz = malloc(13);
    char *args[] = {"./echo.com", NULL};
    char *envs[] = {NULL};
    ASSERT_EQ(0, posix_spawn(&pid, "./echo.com", &fa, &attr, args, envs));
    ASSERT_NE(-1, waitpid(pid, &ws, 0));
    ASSERT_TRUE(WIFEXITED(ws));
    ASSERT_EQ(0, WEXITSTATUS(ws));
    free(zzz);
  }
  ASSERT_EQ(0, posix_spawn_file_actions_destroy(&fa));
  ASSERT_EQ(0, posix_spawnattr_destroy(&attr));
}
