/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Gavin Arthur Hayes                                            │
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
#include "libc/testlib/testlib.h"

TEST(getgroups, test) {
  int rc;
  uint32_t res[1000];
  rc = getgroups(0, NULL);
  if (IsLinux() || IsNetbsd() || IsOpenbsd() || IsFreebsd() || IsXnu()) {
    EXPECT_NE(-1, rc);
    EXPECT_NE(-1, getgroups(sizeof(res) / sizeof(res[0]), res));
  } else {
    EXPECT_EQ(-1, rc);
    EXPECT_EQ(ENOSYS, errno);
  }
}

TEST(setgroups, test) {
  int rc;
  uint32_t src[5];
  EXPECT_EQ(-1, setgroups(0, NULL));
  if (IsLinux() || IsNetbsd() || IsOpenbsd() || IsFreebsd() || IsXnu()) {
    EXPECT_EQ(EPERM, errno);
    EXPECT_EQ(-1, setgroups(sizeof(src) / sizeof(src[0]), src));
  } else {
    EXPECT_EQ(ENOSYS, errno);
  }
}
