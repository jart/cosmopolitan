/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/progn.internal.h"
#include "libc/intrin/mpsadbw.h"
#include "libc/intrin/pabsb.h"
#include "libc/intrin/pabsd.h"
#include "libc/intrin/pabsw.h"
#include "libc/intrin/packssdw.h"
#include "libc/intrin/packsswb.h"
#include "libc/intrin/packusdw.h"
#include "libc/intrin/packuswb.h"
#include "libc/intrin/paddb.h"
#include "libc/intrin/paddd.h"
#include "libc/intrin/paddq.h"
#include "libc/intrin/paddsb.h"
#include "libc/intrin/paddsw.h"
#include "libc/intrin/paddusb.h"
#include "libc/intrin/paddusw.h"
#include "libc/intrin/paddw.h"
#include "libc/intrin/pand.h"
#include "libc/intrin/pandn.h"
#include "libc/intrin/pavgb.h"
#include "libc/intrin/pavgw.h"
#include "libc/intrin/pcmpeqb.h"
#include "libc/intrin/pcmpeqd.h"
#include "libc/intrin/pcmpeqw.h"
#include "libc/intrin/pcmpgtb.h"
#include "libc/intrin/pcmpgtd.h"
#include "libc/intrin/pcmpgtw.h"
#include "libc/intrin/pdep.h"
#include "libc/intrin/pext.h"
#include "libc/intrin/phaddd.h"
#include "libc/intrin/phaddsw.h"
#include "libc/intrin/phaddw.h"
#include "libc/intrin/phsubd.h"
#include "libc/intrin/phsubsw.h"
#include "libc/intrin/phsubw.h"
#include "libc/intrin/pmaddubsw.h"
#include "libc/intrin/pmaddwd.h"
#include "libc/intrin/pmaxsw.h"
#include "libc/intrin/pmaxub.h"
#include "libc/intrin/pminsw.h"
#include "libc/intrin/pminub.h"
#include "libc/intrin/pmulhrsw.h"
#include "libc/intrin/pmulhuw.h"
#include "libc/intrin/pmulhw.h"
#include "libc/intrin/pmulld.h"
#include "libc/intrin/pmullw.h"
#include "libc/intrin/pmuludq.h"
#include "libc/intrin/por.h"
#include "libc/intrin/psadbw.h"
#include "libc/intrin/pshufb.h"
#include "libc/intrin/pshufd.h"
#include "libc/intrin/pshufhw.h"
#include "libc/intrin/pshuflw.h"
#include "libc/intrin/pshufw.h"
#include "libc/intrin/psignb.h"
#include "libc/intrin/psignd.h"
#include "libc/intrin/psignw.h"
#include "libc/intrin/pslld.h"
#include "libc/intrin/pslldq.h"
#include "libc/intrin/psllq.h"
#include "libc/intrin/psllw.h"
#include "libc/intrin/psrad.h"
#include "libc/intrin/psraw.h"
#include "libc/intrin/psrld.h"
#include "libc/intrin/psrldq.h"
#include "libc/intrin/psrlq.h"
#include "libc/intrin/psrlw.h"
#include "libc/intrin/psubb.h"
#include "libc/intrin/psubq.h"
#include "libc/intrin/psubsb.h"
#include "libc/intrin/psubsw.h"
#include "libc/intrin/psubusb.h"
#include "libc/intrin/psubusw.h"
#include "libc/intrin/psubw.h"
#include "libc/intrin/punpckhbw.h"
#include "libc/intrin/punpckhdq.h"
#include "libc/intrin/punpckhqdq.h"
#include "libc/intrin/punpckhwd.h"
#include "libc/intrin/punpcklbw.h"
#include "libc/intrin/punpckldq.h"
#include "libc/intrin/punpcklqdq.h"
#include "libc/intrin/punpcklwd.h"
#include "libc/intrin/pxor.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/rand/lcg.internal.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/viz/lib/formatstringtable-testlib.h"

uint64_t g_rando = 1;

forceinline uint64_t Rando(void) {
  return KnuthLinearCongruentialGenerator(&g_rando) >> 32 << 32 |
         KnuthLinearCongruentialGenerator(&g_rando) >> 32;
}

noinline void RngSet(void *mem, size_t size) {
  uint64_t coin;
  DCHECK(size % 8 == 0);
  for (size >>= 3; size--;) {
    coin = Rando();
    memcpy((char *)mem + size * 8, &coin, 8);
  }
}

FIXTURE(intrin, disableHardwareExtensions) {
  memset((/*unconst*/ void *)kCpuids, 0, sizeof(kCpuids));
}

TEST(punpcklwd, test) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  uint16_t c[8];
  punpcklwd(c, a, b);
  ASSERT_EQ(1, c[0]);
  ASSERT_EQ(9, c[1]);
  ASSERT_EQ(2, c[2]);
  ASSERT_EQ(10, c[3]);
  ASSERT_EQ(3, c[4]);
  ASSERT_EQ(11, c[5]);
  ASSERT_EQ(4, c[6]);
  ASSERT_EQ(12, c[7]);
}

TEST(punpcklwd, pure) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  uint16_t c[8];
  punpcklwd(c, a, b);
  ASSERT_EQ(1, c[0]);
  ASSERT_EQ(9, c[1]);
  ASSERT_EQ(2, c[2]);
  ASSERT_EQ(10, c[3]);
  ASSERT_EQ(3, c[4]);
  ASSERT_EQ(11, c[5]);
  ASSERT_EQ(4, c[6]);
  ASSERT_EQ(12, c[7]);
}

TEST(punpcklwd, testAlias) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  punpcklwd(a, a, b);
  ASSERT_EQ(1, a[0]);
  ASSERT_EQ(9, a[1]);
  ASSERT_EQ(2, a[2]);
  ASSERT_EQ(10, a[3]);
  ASSERT_EQ(3, a[4]);
  ASSERT_EQ(11, a[5]);
  ASSERT_EQ(4, a[6]);
  ASSERT_EQ(12, a[7]);
}

TEST(punpcklwd, pureAlias) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  (punpcklwd)(a, a, b);
  ASSERT_EQ(1, a[0]);
  ASSERT_EQ(9, a[1]);
  ASSERT_EQ(2, a[2]);
  ASSERT_EQ(10, a[3]);
  ASSERT_EQ(3, a[4]);
  ASSERT_EQ(11, a[5]);
  ASSERT_EQ(4, a[6]);
  ASSERT_EQ(12, a[7]);
}

TEST(punpcklwd, testAlias2) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  punpcklwd(b, a, b);
  ASSERT_EQ(1, b[0]);
  ASSERT_EQ(9, b[1]);
  ASSERT_EQ(2, b[2]);
  ASSERT_EQ(10, b[3]);
  ASSERT_EQ(3, b[4]);
  ASSERT_EQ(11, b[5]);
  ASSERT_EQ(4, b[6]);
  ASSERT_EQ(12, b[7]);
}

TEST(punpcklwd, pureAlias2) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  (punpcklwd)(b, a, b);
  ASSERT_EQ(1, b[0]);
  ASSERT_EQ(9, b[1]);
  ASSERT_EQ(2, b[2]);
  ASSERT_EQ(10, b[3]);
  ASSERT_EQ(3, b[4]);
  ASSERT_EQ(11, b[5]);
  ASSERT_EQ(4, b[6]);
  ASSERT_EQ(12, b[7]);
}

TEST(punpcklqdq, test) {
  uint64_t a[2] = {1, 2};
  uint64_t b[2] = {3, 4};
  uint64_t c[2];
  punpcklqdq(c, a, b);
  ASSERT_EQ(1, c[0]);
  ASSERT_EQ(3, c[1]);
}

TEST(punpcklqdq, pure) {
  uint64_t a[2] = {1, 2};
  uint64_t b[2] = {3, 4};
  uint64_t c[2];
  (punpcklqdq)(c, a, b);
  ASSERT_EQ(1, c[0]);
  ASSERT_EQ(3, c[1]);
}

TEST(punpcklqdq, testAlias) {
  uint64_t a[2] = {1, 2};
  uint64_t b[2] = {3, 4};
  punpcklqdq(a, a, b);
  ASSERT_EQ(1, a[0]);
  ASSERT_EQ(3, a[1]);
}

TEST(punpcklqdq, pureAlias) {
  uint64_t a[2] = {1, 2};
  uint64_t b[2] = {3, 4};
  (punpcklqdq)(a, a, b);
  ASSERT_EQ(1, a[0]);
  ASSERT_EQ(3, a[1]);
}

TEST(punpckldq, test) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  uint32_t c[4];
  punpckldq(c, a, b);
  ASSERT_EQ(1, c[0]);
  ASSERT_EQ(5, c[1]);
  ASSERT_EQ(2, c[2]);
  ASSERT_EQ(6, c[3]);
}

TEST(punpckldq, pure) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  uint32_t c[4];
  punpckldq(c, a, b);
  ASSERT_EQ(1, c[0]);
  ASSERT_EQ(5, c[1]);
  ASSERT_EQ(2, c[2]);
  ASSERT_EQ(6, c[3]);
}

TEST(punpckldq, testAlias) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  punpckldq(a, a, b);
  ASSERT_EQ(1, a[0]);
  ASSERT_EQ(5, a[1]);
  ASSERT_EQ(2, a[2]);
  ASSERT_EQ(6, a[3]);
}

TEST(punpckldq, pureAlias) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  (punpckldq)(a, a, b);
  ASSERT_EQ(1, a[0]);
  ASSERT_EQ(5, a[1]);
  ASSERT_EQ(2, a[2]);
  ASSERT_EQ(6, a[3]);
}

TEST(punpckldq, testAlias2) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  punpckldq(b, a, b);
  ASSERT_EQ(1, b[0]);
  ASSERT_EQ(5, b[1]);
  ASSERT_EQ(2, b[2]);
  ASSERT_EQ(6, b[3]);
}

TEST(punpckldq, pureAlias2) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  (punpckldq)(b, a, b);
  ASSERT_EQ(1, b[0]);
  ASSERT_EQ(5, b[1]);
  ASSERT_EQ(2, b[2]);
  ASSERT_EQ(6, b[3]);
}

TEST(punpcklqdq, fuzz) {
  int i, j;
  uint64_t x[2], y[2], a[2], b[2];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    punpcklqdq(a, x, y);
    (punpcklqdq)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(punpckldq, fuzz) {
  int i, j;
  uint32_t x[4], y[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    punpckldq(a, x, y);
    (punpckldq)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(punpcklbw, fuzz) {
  int i, j;
  uint8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    punpcklbw(a, x, y);
    (punpcklbw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(punpckhwd, test) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  uint16_t c[8];
  punpckhwd(c, a, b);
  EXPECT_EQ(5, c[0]);
  EXPECT_EQ(13, c[1]);
  EXPECT_EQ(6, c[2]);
  EXPECT_EQ(14, c[3]);
  EXPECT_EQ(7, c[4]);
  EXPECT_EQ(15, c[5]);
  EXPECT_EQ(8, c[6]);
  EXPECT_EQ(16, c[7]);
}

TEST(punpckhwd, pure) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  uint16_t c[8];
  punpckhwd(c, a, b);
  EXPECT_EQ(5, c[0]);
  EXPECT_EQ(13, c[1]);
  EXPECT_EQ(6, c[2]);
  EXPECT_EQ(14, c[3]);
  EXPECT_EQ(7, c[4]);
  EXPECT_EQ(15, c[5]);
  EXPECT_EQ(8, c[6]);
  EXPECT_EQ(16, c[7]);
}

TEST(punpckhwd, testAlias) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  punpckhwd(a, a, b);
  EXPECT_EQ(5, a[0]);
  EXPECT_EQ(13, a[1]);
  EXPECT_EQ(6, a[2]);
  EXPECT_EQ(14, a[3]);
  EXPECT_EQ(7, a[4]);
  EXPECT_EQ(15, a[5]);
  EXPECT_EQ(8, a[6]);
  EXPECT_EQ(16, a[7]);
}

TEST(punpckhwd, pureAlias) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  (punpckhwd)(a, a, b);
  EXPECT_EQ(5, a[0]);
  EXPECT_EQ(13, a[1]);
  EXPECT_EQ(6, a[2]);
  EXPECT_EQ(14, a[3]);
  EXPECT_EQ(7, a[4]);
  EXPECT_EQ(15, a[5]);
  EXPECT_EQ(8, a[6]);
  EXPECT_EQ(16, a[7]);
}

TEST(punpckhwd, testAlias2) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  punpckhwd(b, a, b);
  EXPECT_EQ(5, b[0]);
  EXPECT_EQ(13, b[1]);
  EXPECT_EQ(6, b[2]);
  EXPECT_EQ(14, b[3]);
  EXPECT_EQ(7, b[4]);
  EXPECT_EQ(15, b[5]);
  EXPECT_EQ(8, b[6]);
  EXPECT_EQ(16, b[7]);
}

TEST(punpckhwd, pureAlias2) {
  uint16_t a[8] = {1, 02, 03, 04, 05, 06, 07, 8};
  uint16_t b[8] = {9, 10, 11, 12, 13, 14, 15, 16};
  (punpckhwd)(b, a, b);
  EXPECT_EQ(5, b[0]);
  EXPECT_EQ(13, b[1]);
  EXPECT_EQ(6, b[2]);
  EXPECT_EQ(14, b[3]);
  EXPECT_EQ(7, b[4]);
  EXPECT_EQ(15, b[5]);
  EXPECT_EQ(8, b[6]);
  EXPECT_EQ(16, b[7]);
}

TEST(punpckhqdq, test) {
  uint64_t a[2] = {1, 2};
  uint64_t b[2] = {3, 4};
  uint64_t c[2];
  punpckhqdq(c, a, b);
  EXPECT_EQ(2, c[0]);
  EXPECT_EQ(4, c[1]);
}

TEST(punpckhqdq, pure) {
  uint64_t a[2] = {1, 2};
  uint64_t b[2] = {3, 4};
  uint64_t c[2];
  (punpckhqdq)(c, a, b);
  EXPECT_EQ(2, c[0]);
  EXPECT_EQ(4, c[1]);
}

TEST(punpckhqdq, testAlias) {
  uint64_t a[2] = {1, 2};
  uint64_t b[2] = {3, 4};
  punpckhqdq(a, a, b);
  EXPECT_EQ(2, a[0]);
  EXPECT_EQ(4, a[1]);
}

TEST(punpckhqdq, pureAlias) {
  uint64_t a[2] = {1, 2};
  uint64_t b[2] = {3, 4};
  (punpckhqdq)(a, a, b);
  EXPECT_EQ(2, a[0]);
  EXPECT_EQ(4, a[1]);
}

TEST(punpckhdq, test) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  uint32_t c[4];
  punpckhdq(c, a, b);
  EXPECT_EQ(3, c[0]);
  EXPECT_EQ(7, c[1]);
  EXPECT_EQ(4, c[2]);
  EXPECT_EQ(8, c[3]);
}

TEST(punpckhdq, pure) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  uint32_t c[4];
  punpckhdq(c, a, b);
  EXPECT_EQ(3, c[0]);
  EXPECT_EQ(7, c[1]);
  EXPECT_EQ(4, c[2]);
  EXPECT_EQ(8, c[3]);
}

TEST(punpckhdq, testAlias) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  punpckhdq(a, a, b);
  EXPECT_EQ(3, a[0]);
  EXPECT_EQ(7, a[1]);
  EXPECT_EQ(4, a[2]);
  EXPECT_EQ(8, a[3]);
}

TEST(punpckhdq, pureAlias) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  (punpckhdq)(a, a, b);
  EXPECT_EQ(3, a[0]);
  EXPECT_EQ(7, a[1]);
  EXPECT_EQ(4, a[2]);
  EXPECT_EQ(8, a[3]);
}

TEST(punpckhdq, testAlias2) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  punpckhdq(b, a, b);
  EXPECT_EQ(3, b[0]);
  EXPECT_EQ(7, b[1]);
  EXPECT_EQ(4, b[2]);
  EXPECT_EQ(8, b[3]);
}

TEST(punpckhdq, pureAlias2) {
  uint32_t a[4] = {1, 2, 3, 4};
  uint32_t b[4] = {5, 6, 7, 8};
  (punpckhdq)(b, a, b);
  EXPECT_EQ(3, b[0]);
  EXPECT_EQ(7, b[1]);
  EXPECT_EQ(4, b[2]);
  EXPECT_EQ(8, b[3]);
}

TEST(punpckhwd, fuzz) {
  int i, j;
  uint16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    punpckhwd(a, x, y);
    (punpckhwd)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(punpckhqdq, fuzz) {
  int i, j;
  uint64_t x[2], y[2], a[2], b[2];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    punpckhqdq(a, x, y);
    (punpckhqdq)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(punpckhdq, fuzz) {
  int i, j;
  uint32_t x[4], y[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    punpckhdq(a, x, y);
    (punpckhdq)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(punpckhbw, fuzz) {
  int i, j;
  uint8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    punpckhbw(a, x, y);
    (punpckhbw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psubq, fuzz) {
  int i, j;
  int64_t x[2], y[2], a[2], b[2];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psubq(a, x, y);
    (psubq)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psrawv, testSmallShift) {
  int16_t A[8] = {-1, -2, SHRT_MIN, 2};
  uint64_t B[2] = {1};
  psrawv(A, A, B);
  ASSERT_EQ(-1, A[0]);
  ASSERT_EQ(-1, A[1]);
  ASSERT_EQ(-16384, A[2]);
  ASSERT_EQ(1, A[3]);
  ASSERT_EQ(0, A[4]);
}

TEST(psraw, testSmallShift) {
  int16_t A[8] = {-1, -2, SHRT_MIN, 2};
  psraw(A, A, 1);
  ASSERT_EQ(-1, A[0]);
  ASSERT_EQ(-1, A[1]);
  ASSERT_EQ(-16384, A[2]);
  ASSERT_EQ(1, A[3]);
  ASSERT_EQ(0, A[4]);
}

TEST(psraw, pureSmallShift) {
  int16_t A[8] = {-1, -2, SHRT_MIN, 2};
  (psraw)(A, A, 1);
  ASSERT_EQ(-1, A[0]);
  ASSERT_EQ(-1, A[1]);
  ASSERT_EQ(-16384, A[2]);
  ASSERT_EQ(1, A[3]);
  ASSERT_EQ(0, A[4]);
}

TEST(psraw, testBigShift_saturatesCount) {
  int16_t A[8] = {-1, -2, SHRT_MIN, 2};
  psraw(A, A, 77);
  ASSERT_EQ(-1, A[0]);
  ASSERT_EQ(-1, A[1]);
  ASSERT_EQ(-1, A[2]);
  ASSERT_EQ(0, A[3]);
  ASSERT_EQ(0, A[4]);
}

TEST(psraw, pureBigShift_saturatesCount) {
  int16_t A[8] = {-1, -2, SHRT_MIN, 2};
  (psraw)(A, A, 77);
  ASSERT_EQ(-1, A[0]);
  ASSERT_EQ(-1, A[1]);
  ASSERT_EQ(-1, A[2]);
  ASSERT_EQ(0, A[3]);
  ASSERT_EQ(0, A[4]);
}

TEST(psradv, testSmallShift) {
  int32_t A[8] = {-1, -2, INT32_MIN, 2};
  uint64_t B[2] = {1};
  psradv(A, A, B);
  ASSERT_EQ(-1, A[0]);
  ASSERT_EQ(-1, A[1]);
  ASSERT_EQ(-1073741824, A[2]);
  ASSERT_EQ(1, A[3]);
  ASSERT_EQ(0, A[4]);
}

TEST(psradv, test) {
  int i, j;
  int32_t x[4], a[4], b[4];
  uint64_t y[2];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    for (j = 0; j < 2; ++j) {
      y[j] = Rando() % 64;
    }
    psradv(a, x, y);
    (psradv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psrad, testSmallShift) {
  int32_t A[4] = {-1, -2, INT32_MIN, 2};
  psrad(A, A, 1);
  ASSERT_EQ(-1, A[0]);
  ASSERT_EQ(-1, A[1]);
  ASSERT_EQ(-1073741824, A[2]);
  ASSERT_EQ(1, A[3]);
}

TEST(psrad, pureSmallShift) {
  int32_t A[4] = {-1, -2, INT32_MIN, 2};
  (psrad)(A, A, 1);
  ASSERT_EQ(-1, A[0]);
  ASSERT_EQ(-1, A[1]);
  ASSERT_EQ(-1073741824, A[2]);
  ASSERT_EQ(1, A[3]);
}

TEST(psrad, testBigShift_saturatesCount) {
  int32_t A[4] = {-1, -2, INT32_MIN, 2};
  psrad(A, A, 77);
  ASSERT_EQ(-1, A[0]);
  ASSERT_EQ(-1, A[1]);
  ASSERT_EQ(-1, A[2]);
  ASSERT_EQ(0, A[3]);
}

TEST(psrad, pureBigShift_saturatesCount) {
  int32_t A[4] = {-1, -2, INT32_MIN, 2};
  (psrad)(A, A, 77);
  ASSERT_EQ(-1, A[0]);
  ASSERT_EQ(-1, A[1]);
  ASSERT_EQ(-1, A[2]);
  ASSERT_EQ(0, A[3]);
}

TEST(psllwv, test) {
  int i, j;
  uint16_t x[8], a[8], b[8];
  uint64_t y[2];
  for (i = 0; i < 32; ++i) {
    RngSet(x, sizeof(x));
    for (j = 0; j < 2; ++j) {
      y[j] = Rando() % 300;
    }
    psllwv(a, x, y);
    (psllwv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psllw, testSmallShift) {
  uint16_t A[8] = {0, 1, 0xffff, 2};
  psllw(A, A, 1);
  ASSERT_EQ(0, A[0]);
  ASSERT_EQ(2, A[1]);
  ASSERT_EQ(0xfffe, A[2]);
  ASSERT_EQ(4, A[3]);
}

TEST(psllwv, testSmallShift) {
  uint16_t A[8] = {0, 1, 0xffff, 2};
  uint64_t B[2] = {1};
  psllwv(A, A, B);
  ASSERT_EQ(0, A[0]);
  ASSERT_EQ(2, A[1]);
  ASSERT_EQ(0xfffe, A[2]);
  ASSERT_EQ(4, A[3]);
}

TEST(pslldv, test) {
  int i, j;
  uint32_t x[4], a[4], b[4];
  uint64_t y[2];
  for (i = 0; i < 32; ++i) {
    RngSet(x, sizeof(x));
    for (j = 0; j < 2; ++j) {
      y[j] = Rando() % 300;
    }
    pslldv(a, x, y);
    (pslldv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pslld, testSmallShift) {
  uint32_t A[8] = {0, 1, 0xffffffff, 2};
  pslld(A, A, 1);
  ASSERT_EQ(0, A[0]);
  ASSERT_EQ(2, A[1]);
  ASSERT_EQ(0xfffffffe, A[2]);
  ASSERT_EQ(4, A[3]);
}

TEST(pslldv, testSmallShift) {
  uint32_t A[8] = {0, 1, 0xffffffff, 2};
  uint64_t B[2] = {1};
  pslldv(A, A, B);
  ASSERT_EQ(0, A[0]);
  ASSERT_EQ(2, A[1]);
  ASSERT_EQ(0xfffffffe, A[2]);
  ASSERT_EQ(4, A[3]);
}

TEST(pmulhuw, test) {
  uint16_t x[8] = {0, 0xffff, 0x0000, 0x0001, 0x8000};
  uint16_t y[8] = {0, 0xffff, 0xffff, 0xffff, 0x8000};
  uint16_t z[8];
  pmulhuw(z, x, y);
  ASSERT_EQ(0x0000 /*0000*/, z[0]);
  ASSERT_EQ(0xfffe /*0001*/, z[1]);
  ASSERT_EQ(0x0000 /*0000*/, z[2]);
  ASSERT_EQ(0x0000 /*ffff*/, z[3]);
  ASSERT_EQ(0x4000 /*0000*/, z[4]);
}

TEST(pmulhuw, pure) {
  uint16_t x[8] = {0, 0xffff, 0x0000, 0x0001, 0x8000};
  uint16_t y[8] = {0, 0xffff, 0xffff, 0xffff, 0x8000};
  uint16_t z[8];
  (pmulhuw)(z, x, y);
  ASSERT_EQ(0x0000 /*0000*/, z[0]);
  ASSERT_EQ(0xfffe /*0001*/, z[1]);
  ASSERT_EQ(0x0000 /*0000*/, z[2]);
  ASSERT_EQ(0x0000 /*ffff*/, z[3]);
  ASSERT_EQ(0x4000 /*0000*/, z[4]);
}

TEST(pmulhuw, fuzz) {
  int i, j;
  uint16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pmulhuw(a, x, y);
    (pmulhuw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pmulhuw(a, (void *)a, y);
    (pmulhuw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pmulhw, fuzz) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pmulhw(a, x, y);
    (pmulhw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pmulhw(a, (void *)a, y);
    (pmulhw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pmullw, fuzz) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pmullw(a, x, y);
    (pmullw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pmullw(a, (void *)a, y);
    (pmullw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pmulld, fuzz) {
  int i, j;
  int32_t x[4], y[4], a[4], b[4];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pmulld(a, x, y);
    (pmulld)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pmulld(a, (void *)a, y);
    (pmulld)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pmuludq, fuzz) {
  int i, j;
  uint32_t x[4], y[4];
  uint64_t a[2], b[2];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pmuludq(a, x, y);
    (pmuludq)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pmuludq(a, (void *)a, y);
    (pmuludq)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pmaddwd, fuzz) {
  int i, j;
  int16_t x[8], y[8];
  int32_t a[4], b[4];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pmaddwd(a, x, y);
    (pmaddwd)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pmaddwd(a, (void *)a, y);
    (pmaddwd)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(phaddw, fuzz) {
  int i, j;
  int16_t x[8], y[8];
  int16_t a[8], b[8];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    phaddw(a, x, y);
    (phaddw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    phaddw(a, (void *)a, y);
    (phaddw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(phaddd, fuzz) {
  int i, j;
  int32_t x[4], y[4];
  int32_t a[4], b[4];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    phaddd(a, x, y);
    (phaddd)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    phaddd(a, (void *)a, y);
    (phaddd)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(phsubw, fuzz) {
  int i, j;
  int16_t x[8], y[8];
  int16_t a[8], b[8];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    phsubw(a, x, y);
    (phsubw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    phsubw(a, (void *)a, y);
    (phsubw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(phsubd, fuzz) {
  int i, j;
  int32_t x[4], y[4];
  int32_t a[4], b[4];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    phsubd(a, x, y);
    (phsubd)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    phsubd(a, (void *)a, y);
    (phsubd)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(phaddsw, fuzz) {
  int i, j;
  int16_t x[8], y[8];
  int16_t a[8], b[8];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    phaddsw(a, x, y);
    (phaddsw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    phaddsw(a, (void *)a, y);
    (phaddsw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(phsubsw, fuzz) {
  int i, j;
  int16_t x[8], y[8];
  int16_t a[8], b[8];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    phsubsw(a, x, y);
    (phsubsw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    phsubsw(a, (void *)a, y);
    (phsubsw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(phaddw, testOverflow_wrapsAround) {
  short M[2][8] = {
      {0x7fff, 0, 0x7fff, 1, 13004, -30425, 20777, -16389},
      {-28040, 13318, -1336, -24798, -13876, 3599, -7346, -23575},
  };
  phaddw(M[0], M[0], M[1]);
  EXPECT_SHRTMATRIXEQ(2, 8, M, "\n\
 32767 -32768 -17421   4388 -14722 -26134 -10277 -30921\n\
-28040  13318  -1336 -24798 -13876   3599  -7346 -23575");
}

TEST(phaddw, testAliasing_isOk) {
  short M[1][8] = {
      {0, 1, 2, 3, 4, 5, 6, 7},
  };
  phaddw(M[0], M[0], M[0]);
  EXPECT_SHRTMATRIXEQ(1, 8, M, "\n\
 1  5  9 13  1  5  9 13");
}

TEST(phaddsw, testOverflow_saturates) {
  short M[2][8] = {
      {0x7fff, 0, 0x7fff, 1, 0x7fff, 0x7fff, 20777, -16389},
      {-28040, 13318, -1336, -24798, -13876, 3599, -7346, -23575},
  };
  phaddsw(M[0], M[0], M[1]);
  EXPECT_SHRTMATRIXEQ(2, 8, M, "\n\
 32767  32767  32767   4388 -14722 -26134 -10277 -30921\n\
-28040  13318  -1336 -24798 -13876   3599  -7346 -23575");
}

TEST(phaddsw, testAliasing_isOk) {
  short M[1][8] = {{0, 1, 2, 3, 4, 5, 6, 7}};
  phaddsw(M[0], M[0], M[0]);
  EXPECT_SHRTMATRIXEQ(1, 8, M, "\n\
 1  5  9 13  1  5  9 13");
}

TEST(pcmpgtb, test) {
  int i, j;
  int8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    static int count;
    pcmpgtb(a, x, y);
    (pcmpgtb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pcmpeqb, test) {
  int i, j;
  uint8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pcmpeqb(a, x, y);
    (pcmpeqb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pcmpeqd, test) {
  int i, j;
  int32_t x[4], y[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pcmpeqd(a, x, y);
    (pcmpeqd)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pcmpgtd, test) {
  int i, j;
  int32_t x[4], y[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pcmpgtd(a, x, y);
    (pcmpgtd)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pcmpeqw, test) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pcmpeqw(a, x, y);
    (pcmpeqw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pcmpgtw, test) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pcmpgtw(a, x, y);
    (pcmpgtw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(por, fuzz) {
  int i, j;
  uint64_t x[2], y[2], a[2], b[2];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 2; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando();
    por(a, x, y);
    (por)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    por(a, (void *)a, y);
    (por)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pxor, fuzz) {
  int i, j;
  uint64_t x[2], y[2], a[2], b[2];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 2; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando();
    pxor(a, x, y);
    (pxor)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pxor(a, (void *)a, y);
    (pxor)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pand, fuzz) {
  int i, j;
  uint64_t x[2], y[2], a[2], b[2];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 2; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando();
    pand(a, x, y);
    (pand)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pand(a, (void *)a, y);
    (pand)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pandn, fuzz) {
  int i, j;
  uint64_t x[2], y[2], a[2], b[2];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 2; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando();
    pandn(a, x, y);
    (pandn)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pandn(a, (void *)a, y);
    (pandn)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(paddq, fuzz) {
  int i, j;
  int64_t x[2], y[2], a[2], b[2];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 2; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando();
    paddq(a, x, y);
    (paddq)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    paddq(a, (void *)a, y);
    (paddq)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pavgb, fuzz) {
  int i, j;
  uint8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pavgb(a, x, y);
    (pavgb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pavgb(a, (void *)a, y);
    (pavgb)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pavgw, fuzz) {
  int i, j;
  uint16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pavgw(a, x, y);
    (pavgw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pavgw(a, (void *)a, y);
    (pavgw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(punpcklwd, fuzz) {
  int i, j;
  uint16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    punpcklwd(a, x, y);
    (punpcklwd)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    punpcklwd(a, a, y);
    (punpcklwd)(b, b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    punpcklwd(a, y, a);
    (punpcklwd)(b, y, b);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pminub, fuzz) {
  int i, j;
  uint8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pminub(a, x, y);
    (pminub)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pminub(a, (void *)a, y);
    (pminub)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pminsw, fuzz) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pminsw(a, x, y);
    (pminsw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pminsw(a, (void *)a, y);
    (pminsw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pmaxub, fuzz) {
  int i, j;
  uint8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pmaxub(a, x, y);
    (pmaxub)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pmaxub(a, (void *)a, y);
    (pmaxub)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pmaxsw, fuzz) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pmaxsw(a, x, y);
    (pmaxsw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pmaxsw(a, (void *)a, y);
    (pmaxsw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(paddw, test) {
  int16_t A[8] = {7};
  int16_t B[8] = {11};
  int16_t C[8];
  paddw(C, A, B);
  ASSERT_EQ(18, C[0]);
}

TEST(paddw, testOverflow_wrapsAround) {
  int16_t A[8] = {SHRT_MAX, SHRT_MIN};
  int16_t B[8] = {1, -1};
  paddw(A, A, B);
  ASSERT_EQ(SHRT_MIN, A[0]);
  ASSERT_EQ(SHRT_MAX, A[1]);
}

TEST(paddw, fuzz) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    paddw(a, x, y);
    (paddw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    paddw(a, (void *)a, y);
    (paddw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(paddsw, test) {
  int16_t A[8] = {7};
  int16_t B[8] = {11};
  int16_t C[8];
  paddsw(C, A, B);
  ASSERT_EQ(18, C[0]);
}

TEST(paddsw, testOverflow_saturates) {
  int16_t A[8] = {SHRT_MAX, SHRT_MIN};
  int16_t B[8] = {1, -1};
  paddsw(A, A, B);
  ASSERT_EQ(SHRT_MAX, A[0]);
  ASSERT_EQ(SHRT_MIN, A[1]);
}

TEST(paddusw, fuzz) {
  int i, j;
  uint16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    paddusw(a, x, y);
    (paddusw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    paddusw(a, (void *)a, y);
    (paddusw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psubb, fuzz) {
  int i, j;
  uint8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psubb(a, x, y);
    (psubb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psubb(a, (void *)a, y);
    (psubb)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psubw, fuzz) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psubw(a, x, y);
    (psubw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psubw(a, (void *)a, y);
    (psubw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psubusw, fuzz) {
  int i, j;
  uint16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psubusw(a, x, y);
    (psubusw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psubusw(a, (void *)a, y);
    (psubusw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(paddusb, fuzz) {
  int i, j;
  uint8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    paddusb(a, x, y);
    (paddusb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    paddusb(a, (void *)a, y);
    (paddusb)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psubusb, fuzz) {
  int i, j;
  uint8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psubusb(a, x, y);
    (psubusb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psubusb(a, (void *)a, y);
    (psubusb)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pabsb, fuzz) {
  int i, j;
  int8_t x[16];
  uint8_t a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    pabsb(a, x);
    (pabsb)(b, x);
    ASSERT_EQ(0, memcmp(a, b, 16), "%d\n\t%`#.16s\n\t%`#.16s\n\t%`#.16s", i, x,
              a, b);
  }
}

TEST(pabsw, fuzz) {
  int i, j;
  int16_t x[8];
  uint16_t a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    pabsw(a, x);
    (pabsw)(b, x);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pabsd, fuzz) {
  int i, j;
  int32_t x[4];
  uint32_t a[4], b[4];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    pabsd(a, x);
    (pabsd)(b, x);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psignb, fuzz) {
  int i, j;
  int8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psignb(a, x, y);
    (psignb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psignb(a, (void *)a, y);
    (psignb)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psignw, fuzz) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psignw(a, x, y);
    (psignw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psignw(a, (void *)a, y);
    (psignw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psignd, fuzz) {
  int i, j;
  int32_t x[4], y[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psignd(a, x, y);
    (psignd)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psignd(a, (void *)a, y);
    (psignd)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(paddb, fuzz) {
  int i, j;
  int8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    paddb(a, x, y);
    (paddb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    paddb(a, (void *)a, y);
    (paddb)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(paddsb, fuzz) {
  int i, j;
  int8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    paddsb(a, x, y);
    (paddsb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    paddsb(a, (void *)a, y);
    (paddsb)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(paddsw, fuzz) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    paddsw(a, x, y);
    (paddsw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    paddsw(a, (void *)a, y);
    (paddsw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psubsb, fuzz) {
  int i, j;
  int8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psubsb(a, x, y);
    (psubsb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psubsb(a, (void *)a, y);
    (psubsb)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psubsw, fuzz) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psubsw(a, x, y);
    (psubsw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psubsw(a, (void *)a, y);
    (psubsw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(paddd, fuzz) {
  int i, j;
  int32_t x[4], y[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    paddd(a, x, y);
    (paddd)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    paddd(a, (void *)a, y);
    (paddd)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pshufb, fuzz) {
  int i, j;
  uint8_t x[16], y[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pshufb(a, x, y);
    (pshufb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pshufb(a, (void *)a, y);
    (pshufb)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pshufd, fuzz) {
  int i, j;
  int32_t x[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 4; ++j) x[j] = Rando();
#define T(IMM)                    \
  pshufd(a, x, IMM);              \
  (pshufd)(b, x, IMM);            \
  ASSERT_EQ(0, memcmp(a, b, 16)); \
  pshufd(a, (void *)a, IMM);      \
  (pshufd)(b, (void *)b, IMM);    \
  ASSERT_EQ(0, memcmp(a, b, 16))
    T(0b00000011);
    T(0b00000110);
    T(0b00001100);
    T(0b00011000);
    T(0b00110000);
    T(0b01100000);
    T(0b11000000);
    T(0b10000000);
#undef T
  }
}

TEST(pshuflw, fuzz) {
  int i, j;
  int16_t x[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 8; ++j) x[j] = Rando();
#define T(IMM)                    \
  pshuflw(a, x, IMM);             \
  (pshuflw)(b, x, IMM);           \
  ASSERT_EQ(0, memcmp(a, b, 16)); \
  pshuflw(a, (void *)a, IMM);     \
  (pshuflw)(b, (void *)b, IMM);   \
  ASSERT_EQ(0, memcmp(a, b, 16))
    T(0b00000011);
    T(0b00000110);
    T(0b00001100);
    T(0b00011000);
    T(0b00110000);
    T(0b01100000);
    T(0b11000000);
    T(0b10000000);
#undef T
  }
}

TEST(pshufhw, fuzz) {
  int i, j;
  int16_t x[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 8; ++j) x[j] = Rando();
#define T(IMM)                    \
  pshufhw(a, x, IMM);             \
  (pshufhw)(b, x, IMM);           \
  ASSERT_EQ(0, memcmp(a, b, 16)); \
  pshufhw(a, (void *)a, IMM);     \
  (pshufhw)(b, (void *)b, IMM);   \
  ASSERT_EQ(0, memcmp(a, b, 16))
    T(0b00000011);
    T(0b00000110);
    T(0b00001100);
    T(0b00011000);
    T(0b00110000);
    T(0b01100000);
    T(0b11000000);
    T(0b10000000);
#undef T
  }
}

TEST(packuswb, test) {
  const short S[8] = {0, 128, -128, 255, SHRT_MAX, SHRT_MIN, 0, 0};
  unsigned char B[16] = {0};
  packuswb(B, S, S);
  ASSERT_EQ(0, B[0]);
  ASSERT_EQ(128, B[1]);
  ASSERT_EQ(0, B[2]);
  ASSERT_EQ(255, B[3]);
  ASSERT_EQ(255, B[4]);
  ASSERT_EQ(0, B[5]);
  ASSERT_EQ(0, B[6]);
  ASSERT_EQ(0, B[7]);
  ASSERT_EQ(0, B[8]);
  ASSERT_EQ(128, B[9]);
  ASSERT_EQ(0, B[10]);
  ASSERT_EQ(255, B[11]);
  ASSERT_EQ(255, B[12]);
  ASSERT_EQ(0, B[13]);
  ASSERT_EQ(0, B[14]);
  ASSERT_EQ(0, B[15]);
}

TEST(packsswb, test) {
  const short S[8] = {0, 128, -128, 255, SHRT_MAX, SHRT_MIN, 0, 0};
  signed char B[16] = {0};
  packsswb(B, S, S);
  ASSERT_EQ(0, B[0]);
  ASSERT_EQ(127, B[1]);
  ASSERT_EQ(-128, B[2]);
  ASSERT_EQ(127, B[3]);
  ASSERT_EQ(127, B[4]);
  ASSERT_EQ(-128, B[5]);
  ASSERT_EQ(0, B[6]);
  ASSERT_EQ(0, B[7]);
  ASSERT_EQ(0, B[8]);
  ASSERT_EQ(127, B[9]);
  ASSERT_EQ(-128, B[10]);
  ASSERT_EQ(127, B[11]);
  ASSERT_EQ(127, B[12]);
  ASSERT_EQ(-128, B[13]);
  ASSERT_EQ(0, B[14]);
  ASSERT_EQ(0, B[15]);
}

TEST(packssdw, testAlias) {
  int i, j;
  union {
    int16_t out[8];
    int32_t in1[4];
  } u;
  int16_t a[8], b[8];
  int32_t x[4], y[4];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    memcpy(u.in1, x, sizeof(x));
    packssdw(u.out, u.in1, y);
    memcpy(a, u.out, sizeof(u.out));
    memcpy(u.in1, x, sizeof(x));
    (packssdw)(u.out, u.in1, y);
    memcpy(b, u.out, sizeof(u.out));
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(packusdw, test) {
  int i, j;
  int32_t x[4], y[4];
  uint16_t a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    packusdw(a, x, y);
    (packusdw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(packuswb, fuzz) {
  int i, j;
  int16_t x[8], y[8];
  uint8_t a[16], b[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    packuswb(a, x, y);
    (packuswb)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    packuswb(a, x, x);
    (packuswb)(b, x, x);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(packssdw, test) {
  int i, j;
  int32_t x[4], y[4];
  int16_t a[8], b[8];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    packssdw(a, x, y);
    (packssdw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psllwv, fuzz) {
  int i, j;
  uint64_t y[2];
  uint16_t x[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 8; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando() % 64;
    psllwv(a, x, y);
    (psllwv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psllwv(a, (void *)a, y);
    (psllwv)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pslldv, fuzz) {
  int i, j;
  uint64_t y[2];
  uint32_t x[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 4; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando() % 64;
    pslldv(a, x, y);
    (pslldv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pslldv(a, (void *)a, y);
    (pslldv)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psllqv, fuzz) {
  int i, j;
  uint64_t y[2];
  uint64_t x[2], a[2], b[2];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 2; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando() % 64;
    psllqv(a, x, y);
    (psllqv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psllqv(a, (void *)a, y);
    (psllqv)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psrlwv, fuzz) {
  int i, j;
  uint64_t y[2];
  uint16_t x[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 8; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando() % 64;
    psrlwv(a, x, y);
    (psrlwv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psrlwv(a, (void *)a, y);
    (psrlwv)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psrldv, fuzz) {
  int i, j;
  uint64_t y[2];
  uint32_t x[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 4; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando() % 64;
    psrldv(a, x, y);
    (psrldv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psrldv(a, (void *)a, y);
    (psrldv)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psrlqv, fuzz) {
  int i, j;
  uint64_t y[2];
  uint64_t x[2], a[2], b[2];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 2; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando() % 64;
    psrlqv(a, x, y);
    (psrlqv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psrlqv(a, (void *)a, y);
    (psrlqv)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psrawv, fuzz) {
  int i, j;
  uint64_t y[2];
  int16_t x[8], a[8], b[8];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 8; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando() % 64;
    psrawv(a, x, y);
    (psrawv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psrawv(a, (void *)a, y);
    (psrawv)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psradv, fuzz) {
  int i, j;
  uint64_t y[2];
  int32_t x[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 4; ++j) x[j] = Rando();
    for (j = 0; j < 2; ++j) y[j] = Rando() % 64;
    psradv(a, x, y);
    (psradv)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    psradv(a, (void *)a, y);
    (psradv)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(psrldq, fuzz) {
  int i, n;
  uint8_t x[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    memset(a, -1, sizeof(a));
    memset(b, -1, sizeof(b));
    RngSet(x, sizeof(x));
    n = Rando() % 20;
    psrldq(a, x, n);
    (psrldq)(b, x, n);
    ASSERT_EQ(0, memcmp(a, b, 16), "%d\n\t%`#.16s\n\t%`#.16s\n\t%`#.16s", n, x,
              a, b);
    n = Rando() % 20;
    psrldq(a, a, n);
    (psrldq)(b, b, n);
    ASSERT_EQ(0, memcmp(a, b, 16), "%d\n\t%`#.16s\n\t%`#.16s\n\t%`#.16s", n, x,
              a, b);
  }
}

TEST(pslldq, fuzz) {
  int i, n;
  uint8_t x[16], a[16], b[16];
  for (i = 0; i < 100; ++i) {
    memset(a, -1, sizeof(a));
    memset(b, -1, sizeof(b));
    RngSet(x, sizeof(x));
    n = Rando() % 20;
    pslldq(a, x, n);
    (pslldq)(b, x, n);
    ASSERT_EQ(0, memcmp(a, b, 16), "%d\n\t%`#.16s\n\t%`#.16s\n\t%`#.16s", n, x,
              a, b);
    n = Rando() % 20;
    pslldq(a, a, n);
    (pslldq)(b, b, n);
    ASSERT_EQ(0, memcmp(a, b, 16), "%d\n\t%`#.16s\n\t%`#.16s\n\t%`#.16s", n, x,
              a, b);
  }
}

TEST(psadbw, test) {
  int i, j;
  uint64_t a[2], b[2];
  uint8_t x[16], y[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    psadbw(a, x, y);
    (psadbw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pmulhrsw, fuzz) {
  int i, j;
  int16_t x[8], y[8], a[8], b[8];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pmulhrsw(a, x, y);
    (pmulhrsw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pmulhrsw(a, (void *)a, y);
    (pmulhrsw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(mpsadbw, fuzz) {
  int i, j;
  uint16_t a[8], b[8];
  uint8_t x[16], y[16];
  for (i = 0; i < 100; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    for (j = 0; j < 8; ++j) {
      mpsadbw(a, x, y, j);
      (mpsadbw)(b, x, y, j);
      ASSERT_EQ(0, memcmp(a, b, 16), "%d %d", i, j);
    }
  }
}

TEST(pmaddubsw, fuzz) {
  int i, j;
  int8_t y[16];
  uint8_t x[16];
  int16_t a[8], b[8];
  for (i = 0; i < 1000; ++i) {
    RngSet(x, sizeof(x));
    RngSet(y, sizeof(y));
    pmaddubsw(a, x, y);
    (pmaddubsw)(b, x, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
    pmaddubsw(a, (void *)a, y);
    (pmaddubsw)(b, (void *)b, y);
    ASSERT_EQ(0, memcmp(a, b, 16));
  }
}

TEST(pshufw, fuzz) {
  int i, j;
  uint8_t y;
  int16_t x[4], a[4], b[4];
  for (i = 0; i < 100; ++i) {
    for (j = 0; j < 4; ++j) x[j] = Rando();
    pshufw(a, x, 0b10111111);
    (pshufw)(b, x, 0b10111111);
    ASSERT_EQ(0, memcmp(a, b, 8));
    pshufw(a, (void *)a, 0b10111111);
    (pshufw)(b, (void *)b, 0b10111111);
    ASSERT_EQ(0, memcmp(a, b, 8));
    pshufw(a, x, 0b00001000);
    (pshufw)(b, x, 0b00001000);
    ASSERT_EQ(0, memcmp(a, b, 8));
    pshufw(a, x, 0b00010001);
    (pshufw)(b, x, 0b00010001);
    ASSERT_EQ(0, memcmp(a, b, 8));
    pshufw(a, x, 0b01110100);
    (pshufw)(b, x, 0b01110100);
    ASSERT_EQ(0, memcmp(a, b, 8));
    pshufw(a, x, 0b01101101);
    (pshufw)(b, x, 0b01101101);
    ASSERT_EQ(0, memcmp(a, b, 8));
    pshufw(a, x, 0b10011011);
    (pshufw)(b, x, 0b10011011);
    ASSERT_EQ(0, memcmp(a, b, 8));
    pshufw(a, x, 0b10111000);
    (pshufw)(b, x, 0b10111000);
    ASSERT_EQ(0, memcmp(a, b, 8));
    pshufw(a, x, 0b11000111);
    (pshufw)(b, x, 0b11000111);
    ASSERT_EQ(0, memcmp(a, b, 8));
  }
}

TEST(pcmpeqw, test2) {
  int16_t kNumbers16[] = {0, 1, 2, 123, 0xffff, 0xfffe, 0x8000, 0x8001, 0x8080};
  int i, j, k;
  int16_t a[8], b[8], x[8], y[8];
  for (i = 0; i < ARRAYLEN(kNumbers16); ++i) {
    for (j = 0; j < ARRAYLEN(kNumbers16); ++j) {
      for (k = 0; k < 8; ++k) {
        x[k] = kNumbers16[(i + k) % ARRAYLEN(kNumbers16)];
        y[k] = kNumbers16[(j + k) % ARRAYLEN(kNumbers16)];
      }
      pcmpeqw(a, x, y);
      (pcmpeqw)(b, x, y);
      EXPECT_EQ(0, memcmp(a, b, 16));
    }
  }
}

TEST(pdep, fuzz) {
  int i;
  uint64_t x, y;
  for (i = 0; i < 1000; ++i) {
    x = rand64();
    y = rand64();
    ASSERT_EQ(pdep(x, y), (pdep)(x, y));
  }
}

TEST(pext, fuzz) {
  int i;
  uint64_t x, y;
  for (i = 0; i < 1000; ++i) {
    x = rand64();
    y = rand64();
    ASSERT_EQ(pext(x, y), (pext)(x, y));
  }
}

BENCH(psrldq, bench) {
  volatile uint8_t A[16];
  volatile uint8_t B[16];
  EZBENCH2("psrldq const 𝑖", donothing, PROGN(psrldq(A, B, 7)));
  EZBENCH2("psrldq var 𝑖", donothing, PROGN(psrldq(A, B, VEIL("r", 7))));
  EZBENCH2("psrldq ansi", donothing, PROGN((psrldq)(A, B, 7)));
}

BENCH(pslldq, bench) {
  volatile uint8_t A[16];
  volatile uint8_t B[16];
  EZBENCH2("pslldq const 𝑖", donothing, PROGN(pslldq(A, B, 7)));
  EZBENCH2("pslldq var 𝑖", donothing, PROGN(pslldq(A, B, VEIL("r", 7))));
  EZBENCH2("pslldq ansi", donothing, PROGN((pslldq)(A, B, 7)));
}
