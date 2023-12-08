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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(strtok_r, testEmpty) {
  static const char *const kInput = "";
  static const char *const kSeparators = "/";
  char *s = strcpy(malloc(strlen(kInput) + 1), kInput);
  char *state;
  EXPECT_EQ(NULL, strtok_r(s, kSeparators, &state));
  free(s);
}

TEST(strtok_r, test) {
  static const char *const kInput = ".,lol..cat..";
  static const char *const kSeparators = ".,";
  char *s = strcpy(malloc(strlen(kInput) + 1), kInput);
  char *state;
  EXPECT_STREQ("lol", strtok_r(s, kSeparators, &state));
  EXPECT_STREQ("cat", strtok_r(NULL, kSeparators, &state));
  EXPECT_EQ(NULL, strtok_r(NULL, kSeparators, &state));
  EXPECT_EQ(NULL, strtok_r(NULL, kSeparators, &state));
  free(s);
}

TEST(strtok, test) {
  static const char *const kInput = ".,lol..cat..";
  static const char *const kSeparators = ".,";
  char *s = strcpy(malloc(strlen(kInput) + 1), kInput);
  EXPECT_STREQ("lol", strtok(s, kSeparators));
  EXPECT_STREQ("cat", strtok(NULL, kSeparators));
  EXPECT_EQ(NULL, strtok(NULL, kSeparators));
  EXPECT_EQ(NULL, strtok(NULL, kSeparators));
  free(s);
}

TEST(strtok_r, testHostsTxtLine) {
  static const char *const kInput = "203.0.113.1     lol.example. lol";
  static const char *const kSeparators = " \t";
  char *s = strcpy(malloc(strlen(kInput) + 1), kInput);
  char *state;
  EXPECT_STREQ("203.0.113.1", strtok_r(s, kSeparators, &state));
  EXPECT_STREQ("lol.example.", strtok_r(NULL, kSeparators, &state));
  EXPECT_STREQ("lol", strtok_r(NULL, kSeparators, &state));
  EXPECT_EQ(NULL, strtok_r(NULL, kSeparators, &state));
  free(s);
}
