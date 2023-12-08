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
#include "libc/fmt/libgen.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

static char dup[128];

#define BASENAME(x) basename(strcpy(dup, x))

TEST(basename, testRegularExamples) {
  EXPECT_STREQ("lib", BASENAME("/usr/lib"));
  EXPECT_STREQ("lib", BASENAME("usr/lib"));
  EXPECT_STREQ("usr", BASENAME("/usr/"));
  EXPECT_STREQ("usr", BASENAME("usr"));
  EXPECT_STREQ("/", BASENAME("/"));
  EXPECT_STREQ(".", BASENAME("."));
  EXPECT_STREQ("..", BASENAME(".."));
}

TEST(basename, testIrregularExamples) {
  EXPECT_STREQ(".", basename(0));
  EXPECT_STREQ(".", basename(""));
}

TEST(basename, testTrailingSlash_isIgnored) {
  EXPECT_STREQ("foo", BASENAME("foo/"));
  EXPECT_STREQ("foo", BASENAME("foo//"));
}

TEST(basename, testOnlySlashes_oneSlashOnlyVasily) {
  EXPECT_STREQ("/", BASENAME("///"));
}
