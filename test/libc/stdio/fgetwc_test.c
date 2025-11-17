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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/stdio/internal.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(fgetwc, testAscii_oneChar) {
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  EXPECT_EQ('A', fputc('A', f));
  rewind(f);
  EXPECT_EQ('A', fgetc(f));
  fclose(f);
}

TEST(fgetwc, testAscii_twoChar) {
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  EXPECT_EQ('A', fputc('A', f));
  EXPECT_EQ('B', fputc('B', f));
  rewind(f);
  EXPECT_EQ('A', fgetc(f));
  EXPECT_EQ('B', fgetc(f));
  fclose(f);
}

TEST(fgetwc, testUnicode_oneChar) {
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  EXPECT_EQ(L'𐌰', fputwc(L'𐌰', f));
  rewind(f);
  EXPECT_EQ(L'𐌰', fgetwc(f));
  fclose(f);
}

TEST(fgetwc, testUnicode_oneChar_writtenAsRawUtf8) {
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  EXPECT_EQ(0xF0, fputc(0xF0, f));
  EXPECT_EQ(0x90, fputc(0x90, f));
  EXPECT_EQ(0x8C, fputc(0x8C, f));
  EXPECT_EQ(0xB0, fputc(0xB0, f));
  rewind(f);
  EXPECT_EQ(L'𐌰', fgetwc(f));
  fclose(f);
}

TEST(fgetwc, strayContBytes) {
  FILE *f = fmemopen(NULL, 2, "r+");
  ASSERT_EQ(0, ftell(f));
  ASSERT_SYS(0, 0200, fputc(0200, f));
  ASSERT_EQ(1, ftell(f));
  ASSERT_SYS(0, 'x', fputc('x', f));
  rewind(f);
  ASSERT_SYS(EILSEQ, WEOF, fgetwc(f));
  ASSERT_FALSE(ferror(f));
  ASSERT_FALSE(feof(f));
  ASSERT_EQ(1, ftell(f));
  ASSERT_SYS(0, 'x', fgetwc(f));
  ASSERT_FALSE(ferror(f));
  ASSERT_FALSE(feof(f));
  ASSERT_SYS(0, WEOF, fgetwc(f));
  ASSERT_FALSE(ferror(f));
  ASSERT_TRUE(feof(f));
  fclose(f);
}

TEST(fgetwc, incompleteSequence_failsButDoesntErrorStream) {
#ifndef __GLIBC__  // wut
  FILE *f;
  ASSERT_TRUE(!!(f = fmemopen(NULL, 100, "r+")));
  ASSERT_SYS(0, 0300, fputc(0300, f));
  ASSERT_SYS(0, 'x', fputc('x', f));
  ASSERT_SYS(0, 2, ftell(f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 2, ftell(f));
  rewind(f);
  ASSERT_SYS(EILSEQ, WEOF, fgetwc(f));
  ASSERT_SYS(0, 1, ftell(f));
  ASSERT_FALSE(ferror(f));
  ASSERT_FALSE(feof(f));
  errno = 0;
  ASSERT_SYS(0, 'x', fgetwc(f));
  ASSERT_SYS(0, 2, ftell(f));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_END));
  ASSERT_SYS(0, 100, ftell(f));
  ASSERT_FALSE(ferror(f));
  ASSERT_FALSE(feof(f));
  ASSERT_SYS(0, WEOF, fgetwc(f));
  ASSERT_FALSE(ferror(f));
  ASSERT_TRUE(feof(f));
  ASSERT_SYS(0, 0, fclose(f));
#endif
}
