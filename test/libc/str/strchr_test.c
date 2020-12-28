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
#include "libc/bits/bits.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(strchr, blank) {
  const char *const blank = "";
  EXPECT_EQ(NULL, strchr(blank, '#'));
  EXPECT_EQ(blank, strchr(blank, '\0'));
}

TEST(strchr, text) {
  char buf[] = "hellothere";
  EXPECT_STREQ("there", strchr(buf, 't'));
}
TEST(strchr, testsse) {
  char buf[] = "hellohellohellohellohellohellohellohello"
               "theretheretheretheretheretheretherethere";
  EXPECT_STREQ("theretheretheretheretheretheretherethere", strchr(buf, 't'));
}
TEST(rawmemchr, text) {
  char buf[] = "hellothere";
  EXPECT_STREQ("there", rawmemchr(buf, 't'));
}
TEST(strchrnul, text) {
  char buf[] = "hellothere";
  EXPECT_STREQ("there", strchrnul(buf, 't'));
}

TEST(strchr, nulTerminator) {
  char buf[] = "hellothere";
  EXPECT_STREQ("", strchr(buf, '\0'));
}
TEST(rawmemchr, nulTerminator) {
  char buf[] = "hellothere";
  EXPECT_STREQ("", rawmemchr(buf, '\0'));
}
TEST(strchrnul, nulTerminator) {
  char buf[] = "hellothere";
  EXPECT_STREQ("", strchrnul(buf, '\0'));
}

TEST(strchr, notFound_returnsNul) {
  char buf[] = "hellothere";
  EXPECT_EQ(NULL, strchr(buf, 'z'));
}
TEST(strchrnul, notFound_returnsPointerToNulByte) {
  char buf[] = "hi";
  EXPECT_STREQ("", strchrnul(buf, 'z'));
  EXPECT_EQ(&buf[2], strchrnul(buf, 'z'));
}
