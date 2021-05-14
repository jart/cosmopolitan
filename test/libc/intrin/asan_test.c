/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/mem/mem.h"
#include "libc/testlib/testlib.h"

TEST(asan, test) {
  char *p;
  if (!IsAsan()) return;
  p = gc(malloc(3));
  EXPECT_TRUE(__asan_is_valid(p, 3));
  EXPECT_FALSE(__asan_is_valid(p, 4));
  EXPECT_TRUE(__asan_is_valid(p + 1, 2));
  EXPECT_FALSE(__asan_is_valid(p + 1, 3));
  p = gc(malloc(8 + 3));
  EXPECT_TRUE(__asan_is_valid(p, 8 + 3));
  EXPECT_FALSE(__asan_is_valid(p, 8 + 4));
  EXPECT_TRUE(__asan_is_valid(p + 1, 8 + 2));
  EXPECT_FALSE(__asan_is_valid(p + 1, 8 + 3));
  p = gc(malloc(64 + 3));
  EXPECT_TRUE(__asan_is_valid(p, 64 + 3));
  EXPECT_FALSE(__asan_is_valid(p, 64 + 4));
  EXPECT_TRUE(__asan_is_valid(p + 1, 64 + 2));
  EXPECT_FALSE(__asan_is_valid(p + 1, 64 + 3));
}
