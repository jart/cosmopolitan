/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/mem/mem.h"
#include "libc/mem/gc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(utf16to8, test) {
  EXPECT_STREQ("hello☻♥", gc(utf16to8(u"hello☻♥", -1, 0)));
  EXPECT_STREQ("hello☻♥hello☻♥h", gc(utf16to8(u"hello☻♥hello☻♥h", -1, 0)));
  EXPECT_STREQ("hello☻♥hello☻♥hi", gc(utf16to8(u"hello☻♥hello☻♥hi", -1, 0)));
  EXPECT_STREQ("hello☻♥hello☻♥hello☻♥hello☻♥hello☻♥",
               gc(utf16to8(u"hello☻♥hello☻♥hello☻♥hello☻♥hello☻♥", -1, 0)));
  EXPECT_STREQ("hello--hello--h", gc(utf16to8(u"hello--hello--h", -1, 0)));
  EXPECT_STREQ("hello--hello--hi", gc(utf16to8(u"hello--hello--hi", -1, 0)));
  EXPECT_STREQ("hello--hello--hello--hello--hello--",
               gc(utf16to8(u"hello--hello--hello--hello--hello--", -1, 0)));
}

BENCH(utf16to8, bench) {
  size_t n;
  char16_t *h;
  h = gc(utf8to16(kHyperion, kHyperionSize, &n));
  EZBENCH2("utf16to8", donothing, free(utf16to8(h, n, 0)));
}
