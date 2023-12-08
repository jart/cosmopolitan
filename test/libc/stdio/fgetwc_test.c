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
#include "libc/stdio/stdio.h"
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

TEST(fgetwc, testUnicode_undecodableSequences_fallsBackToBinary) {
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  EXPECT_EQ(0200, fputc(0200, f));
  EXPECT_EQ(0220, fputc(0220, f));
  EXPECT_EQ(0xF0, fputc(0xF0, f));
  EXPECT_EQ(0x90, fputc(0x90, f));
  EXPECT_EQ(0x8C, fputc(0x8C, f));
  EXPECT_EQ(0xB0, fputc(0xB0, f));
  EXPECT_EQ(0304, fputc(0304, f));
  EXPECT_EQ('a', fputc('a', f));
  rewind(f);
  EXPECT_EQ(0200, fgetwc(f));
  EXPECT_EQ(0220, fgetwc(f));
  EXPECT_EQ(L'𐌰', fgetwc(f));
  EXPECT_EQ(0304, fgetwc(f));
  EXPECT_EQ('a', fgetwc(f));
  fclose(f);
}
