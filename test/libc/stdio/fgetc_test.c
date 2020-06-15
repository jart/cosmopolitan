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
#include "libc/calls/calls.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

int pipefd[2];
FILE *f, *reader, *writer;

TEST(fgetc, testEnd) {
  f = fmemopen(NULL, BUFSIZ, "r+");
  EXPECT_EQ(EOF, fgetc(f));
  EXPECT_TRUE(feof(f));
  EXPECT_FALSE(ferror(f));
  EXPECT_EQ(0, fclose(f));
}

TEST(fgetwc, testEnd) {
  f = fmemopen(NULL, BUFSIZ, "r+");
  EXPECT_EQ(WEOF, fgetwc(f));
  EXPECT_TRUE(feof(f));
  EXPECT_FALSE(ferror(f));
  EXPECT_EQ(0, fclose(f));
}

TEST(fgetwc, testMultibyte) {
  f = fmemopen(NULL, BUFSIZ, "r+");
  EXPECT_EQ(L'𝑥', fputwc(L'𝑥', f));
  EXPECT_EQ(L'𝑦', fputwc(L'𝑦', f));
  EXPECT_EQ(L'𝑧', fputwc(L'𝑧', f));
  EXPECT_EQ(L'𝑥', fgetwc(f));
  EXPECT_EQ(L'𝑦', fgetwc(f));
  EXPECT_EQ(L'𝑧', fgetwc(f));
  EXPECT_EQ(WEOF, fgetwc(f));
  EXPECT_TRUE(feof(f));
  fclose(f);
}

TEST(fgetc, testPipe) {
  ASSERT_NE(-1, pipe(pipefd));
  writer = fdopen(pipefd[1], "w");
  reader = fdopen(pipefd[0], "r");
  EXPECT_EQ('a', fputc('a', writer));
  EXPECT_EQ('b', fputc('b', writer));
  EXPECT_EQ('c', fputc('c', writer));
  EXPECT_EQ(3, fflush(writer));
  EXPECT_EQ('a', fgetc(reader));
  EXPECT_EQ('b', fgetc(reader));
  EXPECT_EQ('c', fgetc(reader));
  EXPECT_EQ(0, fclose(reader));
  EXPECT_EQ(0, fclose(writer));
}
