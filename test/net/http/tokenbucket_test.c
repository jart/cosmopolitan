/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
  ASSERT_FALSE(AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ASSERT_FALSE(AcquireToken(tok.b, 0x7f000002, TB_CIDR));
  ReplenishTokens(tok.w, TB_WORDS);
  ReplenishTokens(tok.w, TB_WORDS);
  ASSERT_TRUE(AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ASSERT_TRUE(AcquireToken(tok.b, 0x7f000002, TB_CIDR));
  ASSERT_FALSE(AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ASSERT_TRUE(AcquireToken(tok.b, 0x08080808, TB_CIDR));
  ReplenishTokens(tok.w, TB_WORDS);
  ReplenishTokens(tok.w, TB_WORDS);
  ASSERT_TRUE(AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ASSERT_TRUE(AcquireToken(tok.b, 0x7f000001, TB_CIDR));
  ASSERT_FALSE(AcquireToken(tok.b, 0x7f000002, TB_CIDR));
  ASSERT_TRUE(AcquireToken(tok.b, 0x08080808, TB_CIDR));
}

BENCH(tokenbucket, bench) {
  struct timespec t1, t2;
  clock_gettime(0, &t1);
  ReplenishTokens(tok.w, TB_WORDS);
  clock_gettime(0, &t2);
  kprintf("token bucket replenish took %'ld us\n",
          _timespec_tomicros(_timespec_sub(t2, t1)));
}
