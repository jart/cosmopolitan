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
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

#define N 16

char *GenBuf(char buf[8], int x) {
  int i;
  bzero(buf, 8);
  for (i = 0; i < 7; ++i) {
    buf[i] = x & 127;  // nt doesn't respect invalid unicode?
    x >>= 1;
  }
  return buf;
}

__attribute__((__constructor__)) static void init(void) {
  char buf[8];
  if (__argc == 4 && !strcmp(__argv[1], "-")) {
    ASSERT_STREQ(GenBuf(buf, atoi(__argv[2])), __argv[3]);
    exit(0);
  }
}

TEST(execve, testArgPassing) {
  int i;
  char ibuf[12], buf[8];
  for (i = 0; i < N; ++i) {
    FormatInt32(ibuf, i);
    GenBuf(buf, i);
    SPAWN(vfork);
    execve(GetProgramExecutableName(),
           (char *const[]){GetProgramExecutableName(), "-", ibuf, buf, 0},
           (char *const[]){0});
    notpossible;
    EXITS(0);
  }
}

TEST(execve, ziposELF) {
  if (!IsLinux() && !IsFreebsd()) {
    EXPECT_SYS(ENOSYS, -1,
               execve("/zip/life.elf", (char *const[]){0}, (char *const[]){0}));
    return;
  }
  SPAWN(fork);
  execve("/zip/life.elf", (char *const[]){0}, (char *const[]){0});
  notpossible;
  EXITS(42);
}

TEST(execve, ziposAPE) {
  if (!IsLinux() && !IsFreebsd()) {
    EXPECT_EQ(-1, execve("/zip/life-nomod.com", (char *const[]){0},
                         (char *const[]){0}));
    return;
  }
  SPAWN(fork);
  execve("/zip/life-nomod.com", (char *const[]){0}, (char *const[]){0});
  notpossible;
  EXITS(42);
}
