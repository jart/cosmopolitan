/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/rlimit.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/pib.h"
#include "libc/testlib/testlib.h"

__attribute__((__constructor__)) static void init(void) {
  if (__argc == 2 && !strcmp(__argv[1], "--test1")) {
    char stack_mem[8] = {0};
    _Exit(kisdangerous(stack_mem));
  }
}

TEST(kisdangerous, stack) {
  char stack_mem[8] = {0};
  ASSERT_FALSE(kisdangerous(stack_mem));
}

TEST(kisdangerous, tinystack) {
  int pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    struct rlimit rlim;
    ASSERT_SYS(0, 0, getrlimit(RLIMIT_STACK, &rlim));
    rlim.rlim_cur = 64 * 1024;
    ASSERT_SYS(0, 0, setrlimit(RLIMIT_STACK, &rlim));
    execl(GetProgramExecutableName(), GetProgramExecutableName(), "--test1",
          NULL);
    kprintf("error %m\n");
    _Exit(127);
  }
  int ws;
  ASSERT_SYS(0, pid, wait(&ws));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}

TEST(kisdangerous, mprotect) {
  char *p;
  ASSERT_NE(NULL, (p = _mapanon(1)));
  ASSERT_FALSE(kisdangerous(p));
  ASSERT_SYS(0, 0, mprotect(p, 1, PROT_NONE));
  ASSERT_TRUE(kisdangerous(p));
  ASSERT_SYS(0, 0, mprotect(p, 1, PROT_READ));
  ASSERT_FALSE(kisdangerous(p));
  ASSERT_SYS(0, 0, mprotect(p, 1, PROT_WRITE));
  ASSERT_FALSE(kisdangerous(p));
  ASSERT_SYS(0, 0, munmap(p, 1));
  ASSERT_TRUE(kisdangerous(p));
}

TEST(kisdangerous, permalloc) {
  // hit __maps_balloc() hard enough to fill up bootstrap slab
  for (int i = 0; i < 20000; ++i)
    ASSERT_FALSE(kisdangerous(cosmo_permalloc(128)));
}
