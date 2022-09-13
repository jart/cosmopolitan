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
#include "libc/mem/mem.h"
#include "libc/mem/gc.internal.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(realloc_in_place, test) {
  char *x = malloc(16);
  EXPECT_EQ(x, realloc_in_place(x, 0));
  EXPECT_EQ(x, realloc_in_place(x, 1));
  *x = 2;
  free(x);
}

BENCH(realloc_in_place, bench) {
  volatile int i = 1000;
  volatile char *x = malloc(i);
  EZBENCH2("malloc", donothing, free(malloc(i)));
  EZBENCH2("malloc", donothing, free(malloc(i)));
  EZBENCH2("memalign", donothing, free(memalign(16, i)));
  EZBENCH2("memalign", donothing, free(memalign(32, i)));
  EZBENCH2("realloc", donothing, x = realloc(x, --i));
  EZBENCH2("realloc_in_place", donothing, realloc_in_place(x, --i));
  free(x);
}
