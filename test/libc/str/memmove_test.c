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
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

forceinline void *Memmove(void *restrict d, const void *restrict s, size_t n) {
  return __builtin___memmove_chk(d, s, n, __builtin_object_size(d, 0));
}

void foo1(void *p, void *q) {
  Memmove(p, q, 19);
}

TEST(memmove, supposedPythonBug_test1) {
  char a[32] = "123456789000000000";
  foo1(&a[9], a);
  EXPECT_STREQ("123456789123456789000000000", a);
  foo1(a, &a[9]);
  EXPECT_STREQ("123456789000000000", a);
}

void foo2(void *p, void *q) {
  memmove(p, q, 19);
}

TEST(memmove, supposedPythonBug_test2) {
  char a[32] = "123456789000000000";
  foo2(&a[9], a);
  EXPECT_STREQ("123456789123456789000000000", a);
  foo2(a, &a[9]);
  EXPECT_STREQ("123456789000000000", a);
}
