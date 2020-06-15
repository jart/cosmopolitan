/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(strtok_r, testEmpty) {
  static const char *const kInput = "";
  static const char *const kSeparators = "/";
  char *s = strcpy(tmalloc(strlen(kInput) + 1), kInput);
  char *state;
  EXPECT_EQ(NULL, strtok_r(s, kSeparators, &state));
  tfree(s);
}

TEST(strtok_r, test) {
  static const char *const kInput = ".,lol..cat..";
  static const char *const kSeparators = ".,";
  char *s = strcpy(tmalloc(strlen(kInput) + 1), kInput);
  char *state;
  EXPECT_STREQ("lol", strtok_r(s, kSeparators, &state));
  EXPECT_STREQ("cat", strtok_r(NULL, kSeparators, &state));
  EXPECT_EQ(NULL, strtok_r(NULL, kSeparators, &state));
  EXPECT_EQ(NULL, strtok_r(NULL, kSeparators, &state));
  tfree(s);
}

TEST(strtok, test) {
  static const char *const kInput = ".,lol..cat..";
  static const char *const kSeparators = ".,";
  char *s = strcpy(tmalloc(strlen(kInput) + 1), kInput);
  EXPECT_STREQ("lol", strtok(s, kSeparators));
  EXPECT_STREQ("cat", strtok(NULL, kSeparators));
  EXPECT_EQ(NULL, strtok(NULL, kSeparators));
  EXPECT_EQ(NULL, strtok(NULL, kSeparators));
  tfree(s);
}

TEST(strtok_r, testHostsTxtLine) {
  static const char *const kInput = "203.0.113.1     lol.example. lol";
  static const char *const kSeparators = " \t";
  char *s = strcpy(tmalloc(strlen(kInput) + 1), kInput);
  char *state;
  EXPECT_STREQ("203.0.113.1", strtok_r(s, kSeparators, &state));
  EXPECT_STREQ("lol.example.", strtok_r(NULL, kSeparators, &state));
  EXPECT_STREQ("lol", strtok_r(NULL, kSeparators, &state));
  EXPECT_EQ(NULL, strtok_r(NULL, kSeparators, &state));
  tfree(s);
}
