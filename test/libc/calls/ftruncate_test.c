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
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

int64_t fd;
struct stat st;
const char *path;

TEST(ftruncate, test) {
  mkdir("o", 0755);
  mkdir("o/tmp", 0755);
  path = gc(xasprintf("o/tmp/%s.%d", program_invocation_short_name, getpid()));
  ASSERT_NE(-1, (fd = creat(path, 0755)));
  ASSERT_EQ(5, write(fd, "hello", 5));
  errno = 31337;
  ASSERT_NE(-1, ftruncate(fd, 31337));
  EXPECT_EQ(31337, errno);
  ASSERT_EQ(5, write(fd, "world", 5));
  ASSERT_NE(-1, close(fd));
  ASSERT_NE(-1, stat(path, &st));
  ASSERT_EQ(31337, st.st_size);
  ASSERT_BINEQ(u"helloworld", gc(xslurp(path, 0)));
  unlink(path);
}
