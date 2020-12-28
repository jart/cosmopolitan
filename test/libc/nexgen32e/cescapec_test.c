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
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(cescapec, test) {
  EXPECT_EQ(' ', cescapec(0x20));
  EXPECT_EQ('~', cescapec(0x7E));
  EXPECT_EQ('\\' | 'r' << 8, cescapec('\r'));
  EXPECT_EQ('\\' | 'n' << 8, cescapec('\n'));
  EXPECT_EQ('\\' | '0' << 8 | '0' << 16 | '0' << 24, cescapec(0));
  EXPECT_EQ('\\' | '0' << 8 | '3' << 16 | '3' << 24, cescapec('\e'));
  EXPECT_EQ('\\' | '1' << 8 | '7' << 16 | '7' << 24, cescapec(0x7F));
  EXPECT_EQ('\\' | '3' << 8 | '7' << 16 | '7' << 24, cescapec(0xFF));
  EXPECT_EQ('\\' | '3' << 8 | '7' << 16 | '7' << 24, cescapec(0xFFFF));
  EXPECT_EQ('\\' | '3' << 8 | '7' << 16 | '7' << 24, cescapec(-1));
}
