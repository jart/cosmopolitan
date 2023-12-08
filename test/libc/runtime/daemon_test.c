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
#include "libc/dce.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(daemon, test) {
  char buf[512] = {0};
  SPAWN(fork);
  ASSERT_SYS(0, 3, open(".", O_RDONLY | O_DIRECTORY));
  ASSERT_SYS(0, 0, daemon(false, false));
  ASSERT_SYS(0, 4, openat(3, "ok", O_WRONLY | O_CREAT | O_TRUNC, 0644));
  ASSERT_NE(NULL, getcwd(buf, sizeof(buf)));
  ASSERT_SYS(0, IsWindows() ? 3 : 1, write(4, buf, strlen(buf)));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
  EXITS(0);
  for (int i = 0; i < 13; ++i) {
    bzero(buf, 512);
    open("ok", O_RDONLY);
    read(3, buf, 511);
    close(3);
    if (!strcmp(IsWindows() ? "/C/" : "/", buf)) {
      return;
    }
    usleep(1000L << i);
  }
}
