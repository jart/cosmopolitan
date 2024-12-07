/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(cosmo_args, normalize) {
  char *args[] = {0};
  char **argv = args;
  ASSERT_EQ(1, cosmo_args(0, &argv));
  ASSERT_STREQ(GetProgramExecutableName(), argv[0]);
}

TEST(cosmo_args, test) {
  xbarf(".args", "a b c", -1);
  char *args[] = {"prog", "arg", 0};
  char **argv = args;
  ASSERT_EQ(5, cosmo_args(".args", &argv));
  ASSERT_STREQ("prog", argv[0]);
  ASSERT_STREQ("a", argv[1]);
  ASSERT_STREQ("b", argv[2]);
  ASSERT_STREQ("c", argv[3]);
  ASSERT_STREQ("arg", argv[4]);
}

TEST(cosmo_args, perline) {
  xbarf(".args", "a\nb\nc\n", -1);
  char *args[] = {"prog", "arg", 0};
  char **argv = args;
  ASSERT_EQ(5, cosmo_args(".args", &argv));
  ASSERT_STREQ("prog", argv[0]);
  ASSERT_STREQ("a", argv[1]);
  ASSERT_STREQ("b", argv[2]);
  ASSERT_STREQ("c", argv[3]);
  ASSERT_STREQ("arg", argv[4]);
}

TEST(cosmo_args, dots_end) {
  xbarf(".args", "a b c ...", -1);
  char *args[] = {"prog", "arg", 0};
  char **argv = args;
  ASSERT_EQ(5, cosmo_args(".args", &argv));
  ASSERT_STREQ("prog", argv[0]);
  ASSERT_STREQ("a", argv[1]);
  ASSERT_STREQ("b", argv[2]);
  ASSERT_STREQ("c", argv[3]);
  ASSERT_STREQ("arg", argv[4]);
}

TEST(cosmo_args, dots_middle) {
  xbarf(".args", "a ... b c", -1);
  char *args[] = {"prog", "arg", 0};
  char **argv = args;
  ASSERT_EQ(5, cosmo_args(".args", &argv));
  ASSERT_STREQ("prog", argv[0]);
  ASSERT_STREQ("a", argv[1]);
  ASSERT_STREQ("arg", argv[2]);
  ASSERT_STREQ("b", argv[3]);
  ASSERT_STREQ("c", argv[4]);
}

TEST(cosmo_args, quote) {
  xbarf(".args", " 'hi \\n there'# ", -1);
  char *args[] = {0};
  char **argv = args;
  ASSERT_EQ(2, cosmo_args(".args", &argv));
  ASSERT_STREQ("hi \\n there#", argv[1]);
}

TEST(cosmo_args, dquote) {
  xbarf(".args", " \"hi \\a\\b\\t\\n\\v\\f\\r\\e\\0\\11 \\111 \xab there\"# ",
        -1);
  char *args[] = {0};
  char **argv = args;
  ASSERT_EQ(2, cosmo_args(".args", &argv));
  ASSERT_STREQ("hi \a\b\t\n\v\f\r\e\0\11 \111 \xab there#", argv[1]);
}

TEST(cosmo_args, comment) {
  xbarf(".args",
        "# comment\n"
        "a # hello there\n"
        "b # yup\n",
        -1);
  char *args[] = {0};
  char **argv = args;
  ASSERT_EQ(3, cosmo_args(".args", &argv));
  ASSERT_STREQ("a", argv[1]);
  ASSERT_STREQ("b", argv[2]);
}

TEST(cosmo_args, backslash_newline) {
  xbarf(".args",
        "a\\\n"
        "b\n",
        -1);
  char *args[] = {0};
  char **argv = args;
  ASSERT_EQ(2, cosmo_args(".args", &argv));
  ASSERT_STREQ("ab", argv[1]);
}

TEST(cosmo_args, dotz) {
  xbarf(".args", ". .. ...x", -1);
  char *args[] = {0};
  char **argv = args;
  ASSERT_EQ(4, cosmo_args(".args", &argv));
  ASSERT_STREQ(".", argv[1]);
  ASSERT_STREQ("..", argv[2]);
  ASSERT_STREQ("...x", argv[3]);
}

TEST(cosmo_args, env) {
  setenv("foo", "bar", true);
  xbarf(".args", "$foo x${foo}x \"$foo\" \"${foo}\" $foo", -1);
  char *args[] = {0};
  char **argv = args;
  ASSERT_EQ(6, cosmo_args(".args", &argv));
  ASSERT_STREQ("bar", argv[1]);
  ASSERT_STREQ("xbarx", argv[2]);
  ASSERT_STREQ("bar", argv[3]);
  ASSERT_STREQ("bar", argv[4]);
  ASSERT_STREQ("bar", argv[5]);
}

TEST(cosmo_args, dquote_backslash_newline) {
  setenv("foo", "bar", true);
  xbarf(".args",
        "-p \"\\\n"
        "hello\"\n",
        -1);
  char *args[] = {0};
  char **argv = args;
  ASSERT_EQ(3, cosmo_args(".args", &argv));
  ASSERT_STREQ("-p", argv[1]);
  ASSERT_STREQ("hello", argv[2]);
}

TEST(cosmo_args, dquote_plain_old_newline) {
  setenv("foo", "bar", true);
  xbarf(".args",
        "-p \"\n"
        "hello\"\n",
        -1);
  char *args[] = {0};
  char **argv = args;
  ASSERT_EQ(3, cosmo_args(".args", &argv));
  ASSERT_STREQ("-p", argv[1]);
  ASSERT_STREQ("\nhello", argv[2]);
}

#define LENGTH     128
#define ITERATIONS 5000
#define CHARSET    "abc#'\"$.\\{} \r\n"

TEST(cosmo_args, fuzz) {
  if (IsWindows())
    return;  // not worth it fs too slow
  char s[LENGTH + 1] = {0};
  for (int i = 0; i < ITERATIONS; ++i) {
    for (int j = 0; j < LENGTH; ++j)
      s[j] = CHARSET[rand() % (sizeof(CHARSET) - 1)];
    xbarf(".args", s, -1);
    char *args[] = {0};
    char **argv = args;
    cosmo_args(".args", &argv);
    for (int j = 0; argv[j]; ++j)
      free(argv[j]);
    argv[0] = 0;
  }
}
