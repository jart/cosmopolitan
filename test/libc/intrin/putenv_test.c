/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Gavin Arthur Hayes                                            │
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
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(putenv, test) {
  EXPECT_EQ(0, clearenv());
  EXPECT_EQ(0, putenv("hi=there"));
  EXPECT_STREQ("there", getenv("hi"));
  EXPECT_EQ(0, clearenv());
  EXPECT_EQ(0, putenv("hi=theretwo"));
  EXPECT_STREQ("theretwo", getenv("hi"));
  EXPECT_EQ(0, clearenv());
  EXPECT_EQ(0, setenv("hi", "therethree", 0));
  EXPECT_STREQ("therethree", getenv("hi"));
}

TEST(putenv, usesProvidedMemory) {
  char kv[32] = "hi=hello";
  EXPECT_EQ(0, putenv(kv));
  EXPECT_STREQ("hello", getenv("hi"));
  strcpy(kv, "hi=there");
  EXPECT_STREQ("there", getenv("hi"));
  EXPECT_EQ(0, unsetenv("hi"));
  EXPECT_EQ(0, unsetenv("hi"));
  EXPECT_EQ(0, getenv("hi"));
  EXPECT_EQ(0, clearenv());
}

TEST(putenv, keyonly) {
  EXPECT_EQ(0, clearenv());
  EXPECT_EQ(0, putenv("hi"));
  EXPECT_STREQ("", getenv("hi"));
  EXPECT_STREQ("hi", environ[0]);
  EXPECT_EQ(0, environ[1]);
  EXPECT_EQ(0, unsetenv("hi"));
  EXPECT_EQ(0, getenv("hi"));
  EXPECT_EQ(0, environ[0]);
  EXPECT_EQ(0, environ[1]);
}

TEST(putenv, environ) {
  char *s = strdup("hi=there");
  EXPECT_EQ(0, clearenv());
  EXPECT_EQ(0, putenv(s));
  EXPECT_EQ(0, putenv(s));
  EXPECT_EQ(s, environ[0]);
  EXPECT_EQ(0, environ[1]);
  EXPECT_EQ(0, clearenv());
  free(s);
}
