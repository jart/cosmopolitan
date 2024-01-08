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
#include "libc/errno.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  touch("conftest.a", 0644);
  symlink("conftest.a", "conftest.l");
  mkdir("conftest.d", 0755);
}

TEST(realpath, test1) {
  char *name = gc(realpath("conftest.a", NULL));
  ASSERT_TRUE(name && *name == '/');
}

TEST(realpath, test2) {
  ASSERT_EQ(NULL, gc(realpath("conftest.b/../conftest.a", NULL)));
}

TEST(realpath, test3) {
  char *name = gc(realpath("conftest.l/../conftest.a", NULL));
  if (IsWindows()) {
    // WIN32 acts as a flat namespace, rather than linear inode crawl.
    // GNU ./configure scripts consider this outcome to be acceptable.
    ASSERT_NE(NULL, name);
  } else {
    // Every other OS FS is a UNIX inode crawl.
    ASSERT_SYS(ENOTDIR, NULL, name);
  }
}

TEST(realpath, test4) {
  ASSERT_SYS(ENOTDIR, NULL, gc(realpath("conftest.a/", NULL)));
}

TEST(realpath, test5) {
  char *name1 = gc(realpath(".", NULL));
  char *name2 = gc(realpath("conftest.d//./..", NULL));
  ASSERT_NE(NULL, name1);
  ASSERT_NE(NULL, name2);
  ASSERT_STREQ(name1, name2);
}

TEST(realpath, test6) {
  // musl libc fails this test
  // comment say // is special but doesn't say why :/
  char *name = gc(realpath("//", NULL));
  ASSERT_NE(NULL, name);
  EXPECT_STREQ("/", name);
}
