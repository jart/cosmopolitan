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
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/testlib/testlib.h"

void *_memset(void *, int, size_t) asm("memset");

TEST(memset, size0_doesNothing) {
  _memset(NULL, 0, 0);
}

TEST(memset, size1) {
  char *b = gc(malloc(1));
  _memset(b, 7, 1);
  EXPECT_EQ(7, b[0]);
}

TEST(memset, size2) {
  char *b = gc(malloc(2));
  _memset(b, 7, 2);
  EXPECT_EQ(7, b[0]);
  EXPECT_EQ(7, b[1]);
}

TEST(memset, size3) {
  char *b = gc(malloc(3));
  _memset(b, 7, 3);
  EXPECT_EQ(7, b[0]);
  EXPECT_EQ(7, b[1]);
  EXPECT_EQ(7, b[2]);
}

TEST(memset, size4) {
  char *b = gc(malloc(4));
  _memset(b, 7, 4);
  EXPECT_EQ(7, b[0]);
  EXPECT_EQ(7, b[1]);
  EXPECT_EQ(7, b[2]);
  EXPECT_EQ(7, b[3]);
}

TEST(memset, size5) {
  char *b = gc(malloc(5));
  _memset(b, 7, 5);
  EXPECT_EQ(7, b[0]);
  EXPECT_EQ(7, b[1]);
  EXPECT_EQ(7, b[2]);
  EXPECT_EQ(7, b[3]);
  EXPECT_EQ(7, b[4]);
}

TEST(memset, wut) {
  char buf[128];
  _memset(buf, -1, sizeof(buf));
  EXPECT_EQ(255, buf[8] & 255);
}
