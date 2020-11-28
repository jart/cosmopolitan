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
#include "libc/bits/bits.h"
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

TEST(fgetwc, testUnicode_spuriousContChars_synchronizedBeforeRead) {
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  EXPECT_EQ(0x90, fputc(0x90, f));
  EXPECT_EQ(0x90, fputc(0x90, f));
  EXPECT_EQ(0xF0, fputc(0xF0, f));
  EXPECT_EQ(0x90, fputc(0x90, f));
  EXPECT_EQ(0x8C, fputc(0x8C, f));
  EXPECT_EQ(0xB0, fputc(0xB0, f));
  rewind(f);
  EXPECT_EQ(L'𐌰', fgetwc(f));
  fclose(f);
}
