/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/atomic.h"
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "net/http/http.h"
#include "net/http/tokenbucket.h"

#define TB_CIDR  22
#define TB_BYTES (1u << TB_CIDR)
#define TB_WORDS (TB_BYTES / 8)

union TokenBucket {
  atomic_schar *b;
  atomic_uint_fast64_t *w;
} tok;

void SetUp(void) {
  ASSERT_NE(NULL, (tok.b = calloc(TB_BYTES, 1)));
}

void TearDown(void) {
  free(tok.b);
}

TEST(tokenbucket, test) {
  ASSERT_EQ(0, AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ASSERT_EQ(0, AcquireToken(tok.b, 0x7f000002, TB_CIDR));
  ReplenishTokens(tok.w, TB_WORDS);
  ReplenishTokens(tok.w, TB_WORDS);
  ASSERT_EQ(2, AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ASSERT_EQ(1, AcquireToken(tok.b, 0x7f000002, TB_CIDR));
  ASSERT_EQ(0, AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ASSERT_EQ(0, AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ASSERT_EQ(2, AcquireToken(tok.b, 0x08080808, TB_CIDR));
  ReplenishTokens(tok.w, TB_WORDS);
  ASSERT_EQ(1, AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ReplenishTokens(tok.w, TB_WORDS);
  ASSERT_EQ(1, AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ASSERT_EQ(0, AcquireToken(tok.b, 0x7f000002, TB_CIDR));
  ASSERT_EQ(3, AcquireToken(tok.b, 0x08080808, TB_CIDR));
  for (int i = 0; i < 130; ++i) ReplenishTokens(tok.w, TB_WORDS);
  ASSERT_EQ(127, AcquireToken(tok.b, 0x08080808, TB_CIDR));
}

void NaiveReplenishTokens(atomic_schar *b, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    int x = atomic_load_explicit(b + i, memory_order_relaxed);
    if (x == 127) continue;
    atomic_fetch_add_explicit(b + i, 1, memory_order_acq_rel);
  }
}

BENCH(tokenbucket, bench) {
  struct timespec t1, t2;

  clock_gettime(0, &t1);
  NaiveReplenishTokens(tok.b, TB_BYTES);
  clock_gettime(0, &t2);
  kprintf("NaiveReplenishTokens took %'ld us\n",
          timespec_tomicros(timespec_sub(t2, t1)));

  clock_gettime(0, &t1);
  ReplenishTokens(tok.w, TB_WORDS);
  clock_gettime(0, &t2);
  kprintf("ReplenishTokens empty took %'ld us\n",
          timespec_tomicros(timespec_sub(t2, t1)));

  memset(tok.b, 127, TB_BYTES);
  clock_gettime(0, &t1);
  ReplenishTokens(tok.w, TB_WORDS);
  clock_gettime(0, &t2);
  kprintf("ReplenishTokens full took %'ld us\n",
          timespec_tomicros(timespec_sub(t2, t1)));
}
