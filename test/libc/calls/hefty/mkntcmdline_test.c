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
#include "libc/calls/hefty/ntspawn.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(mkntcmdline, emptyArgvList_isntValidCommandLine) {
  char *argv[] = {NULL};
  EXPECT_EQ(NULL, gc(mkntcmdline(argv)));
  EXPECT_EQ(EINVAL, errno);
}

TEST(mkntcmdline, emptyArgs_isEmptyString) {
  char *argv[] = {"", NULL};
  EXPECT_STREQ(u"", gc(mkntcmdline(argv)));
}

TEST(mkntcmdline, ignoranceIsBliss) {
  char *argv[] = {"echo", "hello", "world", NULL};
  EXPECT_STREQ(u"echo hello world", gc(mkntcmdline(argv)));
}

TEST(mkntcmdline, spaceInArgument_getQuotesWrappedAround) {
  char *argv[] = {"echo", "hello there", "world", NULL};
  EXPECT_STREQ(u"echo \"hello there\" world", gc(mkntcmdline(argv)));
}

TEST(mkntcmdline, justQuote) {
  char *argv[] = {"\"", NULL};
  EXPECT_STREQ(u"\"\\\"\"", gc(mkntcmdline(argv)));
}

TEST(mkntcmdline, justSlash) {
  char *argv[] = {"\\", NULL};
  EXPECT_STREQ(u"\\", gc(mkntcmdline(argv)));
}

TEST(mkntcmdline, justSlashQuote) {
  char *argv[] = {"\\\"", NULL};
  EXPECT_STREQ(u"\"\\\\\\\"\"" /* "\\\"" */, gc(mkntcmdline(argv)));
}

TEST(mkntcmdline, basicQuoting) {
  char *argv[] = {"a\"b c", "d", NULL};
  EXPECT_STREQ(u"\"a\\\"b c\" d" /* "a\"b c" d */, gc(mkntcmdline(argv)));
}

TEST(mkntcmdline, testUnicode) {
  char *argv1[] = {
      gc(strdup("(╯°□°)╯")),
      gc(strdup("要依法治国是赞美那些谁是公义的和惩罚恶人。 - 韩非")),
      NULL,
  };
  EXPECT_STREQ(
      u"(╯°□°)╯ \"要依法治国是赞美那些谁是公义的和惩罚恶人。 - 韩非\"",
      gc(mkntcmdline(memcpy(gc(malloc(sizeof(argv1))), argv1, sizeof(argv1)))));
}
