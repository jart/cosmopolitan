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
#include "dsp/tty/windex.h"
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

unsigned windex$k8(short *, size_t) hidden;
unsigned windex$avx2(short *, size_t) hidden;
unsigned windex$sse4(short *, size_t) hidden;

const short kW[64] forcealign(32) = {
    8281, 3883, 1365, 1786, 9006, 3681, 5563, 8013, 5787,   9063, 2923,
    3564, 6122, 32,   1436, 0741, 7957, 9219, 1320, 2083,   1904, 8905,
    2465, 9122, 9563, 1290, 4474, 3988, 9920, 8325, 1088,   2915, 33,
    1085, 7806, 3248, 1186, 1357, 6738, 1311, 1092, 6195,   7089, 6631,
    1261, 1364, 9007, 8289, 1409, 1090, 8358, 1502, 7658,   2668, 3522,
    1730, 2041, 7707, 5096, 6876, 1324, 1242, 5283, 0x7fff,
};

const short kW2[32] forcealign(32) = {
    8281, 1,    1365, 1786, 9006, 3681, 5563, 8013, 5787, 9063, 2923,
    3564, 6122, 32,   1436, 0741, 7957, 9219, 1320, 2083, 1904, 8905,
    2465, 9122, 9563, 1290, 4474, 3988, 9920, 8325, 1088, 2915,
};

const short kW3[64] forcealign(32) = {
    8281, 0x7fff, 1365, 1786, 9006, 3681, 5563, 8013, 5787,   9063, 2923,
    3564, 6122,   32,   1436, 0741, 7957, 9219, 1320, 2083,   1904, 8905,
    2465, 9122,   9563, 1290, 4474, 3988, 9920, 8325, 1088,   2915, 33,
    1085, 7806,   3248, 1186, 1357, 6738, 1311, 1092, 6195,   7089, 6631,
    1261, 1364,   9007, 8289, 1409, 1090, 8358, 1502, 7658,   2668, 3522,
    1730, 2041,   7707, 5096, 6876, 1324, 1242, 7,    0x7fff,
};

#define TestIt(impl, index, value, n, array)                    \
  ({                                                            \
    short *a = memcpy(tgc(tmemalign(32, n * 2)), array, n * 2); \
    unsigned res = impl(array, n);                              \
    ASSERT_EQ(index, res);                                      \
    ASSERT_EQ(value, a[res]);                                   \
  })

TEST(windex, testRealWorldPicks) {
  const short kPicks[96] forcealign(32) = {
      103,    85,     145,    146,    121,    103,    145, 187, 146,    189,
      121,    103,    139,    121,    63,     105,    105, 147, 60,     103,
      103,    146,    121,    103,    139,    121,    139, 121, 157,    139,
      199,    200,    163,    223,    164,    225,    81,  141, 105,    165,
      78,     139,    103,    164,    61,     103,    103, 145, 79,     139,
      103,    163,    21,     63,     21,     81,     63,  45,  105,    106,
      106,    107,    102,    103,    103,    104,    64,  107, 107,    150,
      82,     143,    107,    168,    108,    109,    109, 110, 21,     64,
      21,     82,     105,    106,    64,     46,     106, 107, 0x7fff, 0x7fff,
      0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff, 0x7fff,
  };
  /* multiple valid answers are fine if it's deterministic */
  TestIt(windex$k8, 52, 21, ARRAYLEN(kPicks), kPicks);
  if (X86_HAVE(AVX2)) TestIt(windex$avx2, 78, 21, ARRAYLEN(kPicks), kPicks);
  if (X86_HAVE(SSE4_2)) TestIt(windex$sse4, 80, 21, ARRAYLEN(kPicks), kPicks);
}

TEST(windex, test) {
  TestIt(windex, 13, 32, ARRAYLEN(kW), kW);
  TestIt(windex, 1, 1, ARRAYLEN(kW2), kW2);
  TestIt(windex, 62, 7, ARRAYLEN(kW3), kW3);
}

TEST(windex$avx2, test) {
  if (X86_HAVE(AVX2)) {
    TestIt(windex$avx2, 13, 32, ARRAYLEN(kW), kW);
    TestIt(windex$avx2, 1, 1, ARRAYLEN(kW2), kW2);
    TestIt(windex$avx2, 62, 7, ARRAYLEN(kW3), kW3);
  }
}

TEST(windex$sse4, test) {
  if (X86_HAVE(SSE4_2)) {
    TestIt(windex$sse4, 13, 32, ARRAYLEN(kW), kW);
    TestIt(windex$sse4, 1, 1, ARRAYLEN(kW2), kW2);
    TestIt(windex$sse4, 62, 7, ARRAYLEN(kW3), kW3);
  }
}

TEST(windex$k8, test) {
  TestIt(windex$k8, 13, 32, ARRAYLEN(kW), kW);
  TestIt(windex$k8, 1, 1, ARRAYLEN(kW2), kW2);
  TestIt(windex$k8, 62, 7, ARRAYLEN(kW3), kW3);
}

////////////////////////////////////////////////////////////////////////////////

BENCH(windex, bench) {
  EZBENCH(donothing, windex(kW, ARRAYLEN(kW)));
}
BENCH(windex$k8, bench) {
  EZBENCH(donothing, windex$k8(kW, ARRAYLEN(kW)));
}
BENCH(windex$avx2, bench) {
  if (X86_HAVE(AVX2)) {
    EZBENCH(donothing, windex$avx2(kW, ARRAYLEN(kW)));
  }
}
BENCH(windex$sse4, bench) {
  if (X86_HAVE(SSE4_2)) {
    EZBENCH(donothing, windex$sse4(kW, ARRAYLEN(kW)));
  }
}
