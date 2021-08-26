/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/log/check.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

char testlib_enable_tmp_setup_teardown;

static textstartup void TestInit(int argc, char **argv) {
  int fd;
  if (argc == 2 && !strcmp(argv[1], "boop")) {
    if ((fd = open("/dev/null", O_RDWR | O_CLOEXEC)) == 3) {
      _exit(72);
    } else {
      _exit(fd + 1);
    }
  }
}

const void *const TestCtor[] initarray = {TestInit};

TEST(dup, clearsCloexecFlag) {
  int ws;
  ASSERT_SYS(0, 0, close(creat("file", 0644)));
  ASSERT_SYS(0, 3, open("file", O_RDWR | O_CLOEXEC));
  ASSERT_NE(-1, (ws = xspawn(0)));
  if (ws == -2) {
    dup2(3, 0);
    execv(program_executable_name,
          (char *const[]){program_executable_name, "boop", 0});
    _exit(127);
  }
  ASSERT_EQ(72, WEXITSTATUS(ws));
  ASSERT_SYS(0, 0, close(3));
}
