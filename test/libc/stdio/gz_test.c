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
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "third_party/zlib/zlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(gz, test) {
  int fd;
  gzFile f;
  char buf[64] = {0};

  ASSERT_NE(NULL, (f = gzopen("hello.txt.gz", "wb")));
  ASSERT_EQ(5, gzwrite(f, "hello", 5));
  ASSERT_EQ(Z_OK, gzclose(f));

  ASSERT_NE(NULL, (f = gzopen("hello.txt.gz", "rb")));
  ASSERT_EQ(5, gzread(f, buf, sizeof(buf)));
  ASSERT_STREQ("hello", buf);
  ASSERT_EQ(Z_OK, gzclose(f));

  ASSERT_NE(NULL, (fd = open("hello.txt.gz", O_RDONLY)));
  ASSERT_EQ(25, read(fd, buf, sizeof(buf)));
  EXPECT_BINEQ(u"▼ï◘      ♥╦H═╔╔• åª►6♣   ", buf);
  ASSERT_SYS(0, 0, close(fd));
}
