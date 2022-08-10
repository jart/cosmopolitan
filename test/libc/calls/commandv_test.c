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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

uint64_t i;
char *oldpath;
char tmp[PATH_MAX];
char pathbuf[PATH_MAX];
char testlib_enable_tmp_setup_teardown;

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

void SetUp(void) {
  static int x;
  mkdir("bin", 0755);
  mkdir("home", 0755);
  oldpath = strdup(nulltoempty(getenv("PATH")));
  CHECK_NE(-1, setenv("PATH", "bin", true));
}

void TearDown(void) {
  CHECK_NE(-1, setenv("PATH", oldpath, true));
  free(oldpath);
}

TEST(commandv, testPathSearch) {
  EXPECT_NE(-1, touch("bin/sh", 0755));
  EXPECT_STREQ("bin/sh", commandv("sh", pathbuf, sizeof(pathbuf)));
}

TEST(commandv, testPathSearch_appendsComExtension) {
  EXPECT_NE(-1, touch("bin/sh.com", 0755));
  EXPECT_STREQ("bin/sh.com", commandv("sh", pathbuf, sizeof(pathbuf)));
}

TEST(commandv, testSlashes_wontSearchPath_butChecksAccess) {
  EXPECT_NE(-1, touch("home/sh", 0755));
  i = __syscount;
  EXPECT_STREQ("home/sh", commandv("home/sh", pathbuf, sizeof(pathbuf)));
  if (!IsWindows()) EXPECT_EQ(i + 1, __syscount);
}

TEST(commandv, testSlashes_wontSearchPath_butStillAppendsComExtension) {
  EXPECT_NE(-1, touch("home/sh.com", 0755));
  i = __syscount;
  EXPECT_STREQ("home/sh.com", commandv("home/sh", pathbuf, sizeof(pathbuf)));
  if (!IsWindows()) EXPECT_EQ(i + 2, __syscount);
}

TEST(commandv, testSameDir_doesntHappenByDefaultUnlessItsWindows) {
  EXPECT_NE(-1, touch("bog", 0755));
  if (IsWindows()) {
    EXPECT_STREQ("./bog", commandv("bog", pathbuf, sizeof(pathbuf)));
  } else {
    EXPECT_EQ(NULL, commandv("bog", pathbuf, sizeof(pathbuf)));
  }
}

TEST(commandv, testSameDir_willHappenWithColonBlank) {
  CHECK_NE(-1, setenv("PATH", "bin:", true));
  EXPECT_NE(-1, touch("bog", 0755));
  if (IsWindows()) {
    EXPECT_STREQ("./bog", commandv("bog", pathbuf, sizeof(pathbuf)));
  } else {
    EXPECT_STREQ("bog", commandv("bog", pathbuf, sizeof(pathbuf)));
  }
}

TEST(commandv, testSameDir_willHappenWithColonBlank2) {
  CHECK_NE(-1, setenv("PATH", ":bin", true));
  EXPECT_NE(-1, touch("bog", 0755));
  if (IsWindows()) {
    EXPECT_STREQ("./bog", commandv("bog", pathbuf, sizeof(pathbuf)));
  } else {
    EXPECT_STREQ("bog", commandv("bog", pathbuf, sizeof(pathbuf)));
  }
}
