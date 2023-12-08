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
#include "libc/macros.internal.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(hexpcpy, test) {
  char buf[] = {0x00, 0x02, 0x20, 0x80, 0xf5, 0xff};
  char str[ARRAYLEN(buf) * 2 + 1];
  rngset(str, sizeof(str), _rand64, -1);
  EXPECT_EQ(str + ARRAYLEN(buf) * 2, hexpcpy(str, buf, ARRAYLEN(buf)));
  EXPECT_STREQ("00022080f5ff", str);
}

TEST(hexpcpy, emptyBuf_writesNulTerminator) {
  char buf[1];
  char str[1];
  rngset(str, sizeof(str), _rand64, -1);
  EXPECT_EQ(str, hexpcpy(str, buf, 0));
  EXPECT_STREQ("", str);
}
