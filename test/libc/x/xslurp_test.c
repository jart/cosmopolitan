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
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(xslurp, testEmptyWithNulTerminatedStringBehavior) {
  size_t got;
  const char *data;
  const char *path = gc(xjoinpaths(kTmpPath, "xslurp.txt"));
  ASSERT_NE(-1, xbarf(path, "", -1));
  ASSERT_NE(NULL, (data = gc(xslurp(path, &got))));
  ASSERT_EQ(0, got);
  ASSERT_STREQ("", data);
  ASSERT_NE(-1, unlink(path));
}

TEST(xslurp, testHyperion) {
  size_t got;
  const char *data;
  const char *path = gc(xjoinpaths(kTmpPath, "xslurp.txt"));
  ASSERT_NE(-1, xbarf(path, kHyperion, kHyperionSize));
  ASSERT_NE(NULL, (data = gc(xslurp(path, &got))));
  ASSERT_EQ(kHyperionSize, got);
  ASSERT_EQ(0, memcmp(data, kHyperion, kHyperionSize));
  ASSERT_NE(-1, unlink(path));
}
