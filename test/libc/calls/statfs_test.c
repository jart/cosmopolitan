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
#include "libc/calls/struct/statfs.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"

struct statfs f;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(statfs, testFile) {
  EXPECT_SYS(0, 0, touch("foo", 0644));
  EXPECT_SYS(0, 0, statfs("foo", &f));
}

TEST(statfs, testDirectory) {
  EXPECT_SYS(0, 0, statfs(".", &f));
}

TEST(statfs, testFdDirectory) {
  EXPECT_SYS(0, 3, open(".", O_RDONLY | O_DIRECTORY));
  EXPECT_SYS(0, 0, fstatfs(3, &f));
  EXPECT_SYS(0, 0, close(3));
}

TEST(statfs, testFdFile) {
  EXPECT_SYS(0, 0, touch("foo", 0644));
  EXPECT_SYS(0, 3, open("foo", O_RDONLY));
  EXPECT_SYS(0, 0, fstatfs(3, &f));
  EXPECT_SYS(0, 0, close(3));
}
