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
#include "libc/assert.h"
#include "libc/log/libfatal.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(getline, testEmpty) {
  FILE *f = fmemopen("", 0, "r+");
  char *line = NULL;
  size_t linesize = 0;
  EXPECT_EQ(-1, getline(&line, &linesize, f));
  EXPECT_TRUE(feof(f));
  EXPECT_FALSE(ferror(f));
  EXPECT_EQ(0, fclose(f));
  free(line);
}

TEST(getline, testOneWithoutLineFeed) {
  FILE *f = fmemopen("hello", 5, "r+");
  char *line = NULL;
  size_t linesize = 0;
  ASSERT_EQ(5, getline(&line, &linesize, f));
  EXPECT_STREQ("hello", line);
  EXPECT_TRUE(feof(f));
  EXPECT_FALSE(ferror(f));
  ASSERT_EQ(-1, getline(&line, &linesize, f));
  EXPECT_EQ(0, fclose(f));
  free(line);
}

TEST(getline, testTwoLines) {
  const char *s = "hello\nthere\n";
  FILE *f = fmemopen((void *)s, strlen(s), "r+");
  char *line = NULL;
  size_t linesize = 0;
  ASSERT_EQ(6, getline(&line, &linesize, f));
  EXPECT_STREQ("hello\n", line);
  EXPECT_FALSE(feof(f) | ferror(f));
  ASSERT_EQ(6, getline(&line, &linesize, f));
  EXPECT_STREQ("there\n", line);
  ASSERT_EQ(-1, getline(&line, &linesize, f));
  EXPECT_STREQ("", line);
  EXPECT_EQ(0, fclose(f));
  free(line);
}

TEST(getline, testBinaryLine_countExcludesOnlyTheBonusNul) {
  const char s[] = "he\0\3o\n";
  FILE *f = fmemopen((void *)s, sizeof(s), "r+");
  char *line = NULL;
  size_t linesize = 0;
  ASSERT_EQ(6, getline(&line, &linesize, f));
  EXPECT_BINEQ(u"he ♥o◙ ", line);
  fclose(f);
  free(line);
}

void WriteHyperionFile(void) {
  FILE *f;
  ASSERT_NE(NULL, (f = fopen("hyperion.txt", "w")));
  EXPECT_EQ(1, fwrite(kHyperion, kHyperionSize, 1, f));
  EXPECT_NE(-1, fclose(f));
}

void ReadHyperionLines(void) {
  FILE *f;
  ssize_t rc;
  char *data = NULL;
  size_t size = 0;
  char *line = NULL;
  size_t linesize = 0;
  ASSERT_NE(NULL, (f = fopen("hyperion.txt", "r")));
  for (;;) {
    rc = getline(&line, &linesize, f);
    if (rc == -1) break;
    data = xrealloc(data, size + rc);
    memcpy(data + size, line, rc);
    size += rc;
  }
  EXPECT_TRUE(feof(f));
  ASSERT_EQ(kHyperionSize, size);
  EXPECT_EQ(0, memcmp(kHyperion, data, size));
  EXPECT_NE(-1, fclose(f));
  free(line);
  free(data);
}

TEST(getline, lotsOfLines_roundTrips) {
  WriteHyperionFile();
  ReadHyperionLines();
}

BENCH(getline, bench) {
  WriteHyperionFile();
  EZBENCH2("getline", donothing, ReadHyperionLines());
}
