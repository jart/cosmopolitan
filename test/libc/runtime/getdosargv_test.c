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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"

char testlib_enable_tmp_setup_teardown;

void SetUpOnce(void) {
  if (!IsWindows()) {
    // TODO(jart): mock out that win32 i/o call
    tinyprint(2, program_invocation_name, ": skipping on non-windows\n", NULL);
    exit(0);
  }
}

TEST(GetDosArgv, empty) {
  size_t max = 4;
  size_t size = ARG_MAX / 2;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(0, GetDosArgv(u"", buf, size, argv, max));
  EXPECT_EQ(NULL, argv[0]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, emptyish) {
  size_t max = 4;
  size_t size = ARG_MAX / 2;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(0, GetDosArgv(u"  ", buf, size, argv, max));
  EXPECT_EQ(NULL, argv[0]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, basicUsage) {
  size_t max = 4;
  size_t size = ARG_MAX / 2;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(3, GetDosArgv(u"a\t \"b  c\"  d ", buf, size, argv, max));
  EXPECT_STREQ("a", argv[0]);
  EXPECT_STREQ("b  c", argv[1]);
  EXPECT_STREQ("d", argv[2]);
  EXPECT_EQ(NULL, argv[3]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, advancedUsage) {
  size_t max = 4;
  size_t size = ARG_MAX / 2;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(2, GetDosArgv(u"(╯°□°)╯︵ ┻━┻", buf, size, argv, max));
  EXPECT_STREQ("(╯°□°)╯︵", argv[0]);
  EXPECT_STREQ("┻━┻", argv[1]);
  EXPECT_EQ(NULL, argv[2]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, testAegeanGothicSupplementaryPlanes) {
  size_t max = 4; /* these symbols are almost as old as dos */
  size_t size = ARG_MAX / 2;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(2, GetDosArgv(u"𐄷𐄸𐄹𐄺𐄻𐄼 𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷", buf, size, argv, max));
  EXPECT_STREQ("𐄷𐄸𐄹𐄺𐄻𐄼", argv[0]);
  EXPECT_STREQ("𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷", argv[1]);
  EXPECT_EQ(NULL, argv[2]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, realWorldUsage) {
  size_t max = 512;
  size_t size = ARG_MAX / 2;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(5, GetDosArgv(u"C:\\Users\\jtunn\\printargs.com oh yes yes yes",
                          buf, size, argv, max));
  EXPECT_STREQ("C:\\Users\\jtunn\\printargs.com", argv[0]);
  EXPECT_STREQ("oh", argv[1]);
  EXPECT_STREQ("yes", argv[2]);
  EXPECT_STREQ("yes", argv[3]);
  EXPECT_STREQ("yes", argv[4]);
  EXPECT_EQ(NULL, argv[5]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, bufferOverrun_countIsStillAccurate_truncatesMemoryWithGrace) {
  size_t max = 3;
  size_t size = 7;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(3, GetDosArgv(u"a\t \"b  c\"  d ", buf, size, argv, max));
  EXPECT_STREQ("a", argv[0]);
  EXPECT_STREQ("b  c", argv[1]);
  EXPECT_EQ(NULL, argv[2]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, pureScanningMode) {
  size_t max = 0;
  size_t size = 0;
  char *buf = NULL;
  char **argv = NULL;
  EXPECT_EQ(3, GetDosArgv(u"a b c", buf, size, argv, max));
}

TEST(GetDosArgv, justSlashQuote) {
  size_t max = 4, size = 16;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(1, GetDosArgv(u"\"\\\\\\\"\"", buf, size, argv, max));
  EXPECT_STREQ("\\\"", argv[0]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, quoteInMiddleOfArg_wontSplitArg) {
  size_t max = 4, size = 16;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(1, GetDosArgv(u"hi\"\"there", buf, size, argv, max));
  EXPECT_STREQ("hithere", argv[0]);
  max = 4, size = 16;
  EXPECT_EQ(1, GetDosArgv(u"hi\" \"there", buf, size, argv, max));
  EXPECT_STREQ("hi there", argv[0]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, waqQuoting1) {
  size_t max = 4;
  size_t size = ARG_MAX / 2;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(2,
            GetDosArgv(u"a\\\\\"\"\"\"\"\"\"\"b c\" d", buf, size, argv, max));
  EXPECT_STREQ("a\\\"\"b", argv[0]);
  EXPECT_STREQ("c d", argv[1]);
  EXPECT_EQ(NULL, argv[2]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, waqQuoting2) {
  size_t max = 4;
  size_t size = ARG_MAX / 2;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(2, GetDosArgv(u"\"a\\\"b c\" d", buf, size, argv, max));
  EXPECT_STREQ("a\"b c", argv[0]);
  EXPECT_STREQ("d", argv[1]);
  EXPECT_EQ(NULL, argv[2]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, cmdToil) {
  size_t max = 4;
  size_t size = ARG_MAX / 2;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  EXPECT_EQ(3, GetDosArgv(u"cmd.exe /C \"echo hi >\"\"\"𝑓𝑜𝑜 bar.txt\"\"\"\"",
                          buf, size, argv, max));
  EXPECT_STREQ("cmd.exe", argv[0]);
  EXPECT_STREQ("/C", argv[1]);
  EXPECT_STREQ("echo hi >\"𝑓𝑜𝑜 bar.txt\"", argv[2]);
  EXPECT_EQ(NULL, argv[3]);
  free(argv);
  free(buf);
}

TEST(GetDosArgv, canonicalizesCurrentDirectoryCommandPath) {
  size_t max = 4;
  size_t size = ARG_MAX / 2;
  char *buf = malloc(size * sizeof(char));
  char **argv = malloc(max * sizeof(char *));
  ASSERT_SYS(0, 0, touch("emacs.com", 0755));
  EXPECT_EQ(1, GetDosArgv(u"emacs.com", buf, size, argv, max));
  EXPECT_STREQ(".\\emacs.com", argv[0]);
  EXPECT_EQ(NULL, argv[1]);
  free(argv);
  free(buf);
}
