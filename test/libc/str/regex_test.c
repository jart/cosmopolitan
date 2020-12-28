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
#include "third_party/regex/regex.h"

TEST(regex, test) {
  regex_t rx;
  EXPECT_EQ(REG_OK, regcomp(&rx, "^[A-Za-z\x7f-\uffff]{2}$", REG_EXTENDED));
  EXPECT_EQ(REG_OK, regexec(&rx, "AZ", 0, NULL, 0));
  EXPECT_EQ(REG_OK, regexec(&rx, "→→", 0, NULL, 0));
  EXPECT_EQ(REG_NOMATCH, regexec(&rx, "A", 0, NULL, 0));
  EXPECT_EQ(REG_NOMATCH, regexec(&rx, "→", 0, NULL, 0));
  EXPECT_EQ(REG_NOMATCH, regexec(&rx, "0", 0, NULL, 0));
  regfree(&rx);
}
