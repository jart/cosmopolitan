/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(bzero, test) {
  int n;
  char *a, *b;
  a = gc(malloc(128));
  b = gc(malloc(128));
  for (n = 0; n < 128; ++n) {
    rngset(a, 128, _rand64, -1);
    memcpy(b, a, 128);
    bzero(a, n);
    memset(b, 0, n);
    ASSERT_EQ(0, memcmp(a, b, 128), "%d", n);
  }
}

void explicit_memset(void *, int, size_t) asm("memset");

char doge[129];

BENCH(bzero, bench) {
  EZBENCH2("bzero 0", donothing, explicit_bzero(doge, 0));
  EZBENCH2("memset 0", donothing, explicit_memset(doge, 0, 0));
  EZBENCH2("bzero 1", donothing, explicit_bzero(doge, 1));
  EZBENCH2("memset 1", donothing, explicit_memset(doge, 0, 1));
  EZBENCH2("bzero 2", donothing, explicit_bzero(doge, 2));
  EZBENCH2("memset 2", donothing, explicit_memset(doge, 0, 2));
  EZBENCH2("bzero 4", donothing, explicit_bzero(doge, 4));
  EZBENCH2("memset 4", donothing, explicit_memset(doge, 0, 4));
  EZBENCH2("bzero 8", donothing, explicit_bzero(doge, 8));
  EZBENCH2("memset 8", donothing, explicit_memset(doge, 0, 8));
  EZBENCH2("bzero 16", donothing, explicit_bzero(doge, 16));
  EZBENCH2("memset 16", donothing, explicit_memset(doge, 0, 16));
  EZBENCH2("bzero 31", donothing, explicit_bzero(doge, 31));
  EZBENCH2("memset 31", donothing, explicit_memset(doge, 0, 31));
  EZBENCH2("bzero 32", donothing, explicit_bzero(doge, 32));
  EZBENCH2("memset 32", donothing, explicit_memset(doge, 0, 32));
  EZBENCH2("bzero 33", donothing, explicit_bzero(doge, 33));
  EZBENCH2("memset 33", donothing, explicit_memset(doge, 0, 33));
  EZBENCH2("bzero 64", donothing, explicit_bzero(doge, 64));
  EZBENCH2("memset 64", donothing, explicit_memset(doge, 0, 64));
  EZBENCH2("bzero 127", donothing, explicit_bzero(doge, 127));
  EZBENCH2("memset 127", donothing, explicit_memset(doge, 0, 127));
  EZBENCH2("bzero 128", donothing, explicit_bzero(doge, 128));
  EZBENCH2("memset 128", donothing, explicit_memset(doge, 0, 128));
  EZBENCH2("bzero 0 [misalign]", donothing, explicit_bzero(doge + 1, 0));
  EZBENCH2("memset 0 [misalign]", donothing, explicit_memset(doge + 1, 0, 0));
  EZBENCH2("bzero 1 [misalign]", donothing, explicit_bzero(doge + 1, 1));
  EZBENCH2("memset 1 [misalign]", donothing, explicit_memset(doge + 1, 0, 1));
  EZBENCH2("bzero 2 [misalign]", donothing, explicit_bzero(doge + 1, 2));
  EZBENCH2("memset 2 [misalign]", donothing, explicit_memset(doge + 1, 0, 2));
  EZBENCH2("bzero 4 [misalign]", donothing, explicit_bzero(doge + 1, 4));
  EZBENCH2("memset 4 [misalign]", donothing, explicit_memset(doge + 1, 0, 4));
  EZBENCH2("bzero 8 [misalign]", donothing, explicit_bzero(doge + 1, 8));
  EZBENCH2("memset 8 [misalign]", donothing, explicit_memset(doge + 1, 0, 8));
  EZBENCH2("bzero 16 [misalign]", donothing, explicit_bzero(doge + 1, 16));
  EZBENCH2("memset 16 [misalign]", donothing, explicit_memset(doge + 1, 0, 16));
  EZBENCH2("bzero 31 [misalign]", donothing, explicit_bzero(doge + 1, 31));
  EZBENCH2("memset 31 [misalign]", donothing, explicit_memset(doge + 1, 0, 31));
  EZBENCH2("bzero 32 [misalign]", donothing, explicit_bzero(doge + 1, 32));
  EZBENCH2("memset 32 [misalign]", donothing, explicit_memset(doge + 1, 0, 32));
  EZBENCH2("bzero 33 [misalign]", donothing, explicit_bzero(doge + 1, 33));
  EZBENCH2("memset 33 [misalign]", donothing, explicit_memset(doge + 1, 0, 33));
  EZBENCH2("bzero 64 [misalign]", donothing, explicit_bzero(doge + 1, 64));
  EZBENCH2("memset 64 [misalign]", donothing, explicit_memset(doge + 1, 0, 64));
  EZBENCH2("bzero 127 [misalign]", donothing, explicit_bzero(doge + 1, 127));
  EZBENCH2("memset 127 [misalign]", donothing,
           explicit_memset(doge + 1, 0, 127));
  EZBENCH2("bzero 128 [misalign]", donothing, explicit_bzero(doge + 1, 128));
  EZBENCH2("memset 128 [misalign]", donothing,
           explicit_memset(doge + 1, 0, 128));
}
