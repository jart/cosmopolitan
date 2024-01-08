/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/mem/gc.h"
#include "libc/testlib/testlib.h"
#if SupportsWindows()

char16_t p[PATH_MAX];

TEST(mkntpath, testEmpty) {
  EXPECT_EQ(0, __mkntpath("", p));
  EXPECT_STREQ(u"", p);
}

TEST(mkntpath, testSlashes) {
  /*
   * The Windows command prompt works fine with all reasonable
   * unix-style paths. There only seems to be one exception, and that's
   * all it takes to make the feature entirely useless to us, similar to
   * the law of noncontradiction. We address the issue as follows:
   */
  EXPECT_EQ(9, __mkntpath("o/foo.com", p));
  EXPECT_STREQ(u"o\\foo.com", p);
}

TEST(mkntpath, testUnicode) {
  EXPECT_EQ(20, __mkntpath("C:\\𐌰𐌱𐌲𐌳\\𐌴𐌵𐌶𐌷", p));
  EXPECT_STREQ(u"C:\\𐌰𐌱𐌲𐌳\\𐌴𐌵𐌶𐌷", p);
}

TEST(mkntpath, testRemoveDoubleSlash) {
  EXPECT_EQ(21, __mkntpath("C:\\Users\\jart\\\\.config", p));
  EXPECT_STREQ(u"C:\\Users\\jart\\.config", p);
}

#endif /* SupportsWindows() */
