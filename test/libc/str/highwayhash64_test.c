/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2017 Google LLC                                                    │
│                                                                              │
│ Licensed under the Apache License, Version 2.0 (the "License");              │
│ you may not use this file except in compliance with the License.             │
│ You may obtain a copy of the License at                                      │
│                                                                              │
│     http://www.apache.org/licenses/LICENSE-2.0                               │
│                                                                              │
│ Unless required by applicable law or agreed to in writing, software          │
│ distributed under the License is distributed on an "AS IS" BASIS,            │
│ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     │
│ See the License for the specific language governing permissions and          │
│ limitations under the License.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/str/highwayhash64.h"
#include "libc/inttypes.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "third_party/zlib/zlib.h"

#define kMaxSize 64

static const uint64_t kTestKey1[4] = {
    0x0706050403020100,
    0x0F0E0D0C0B0A0908,
    0x1716151413121110,
    0x1F1E1D1C1B1A1918,
};

static const uint64_t kTestKey2[4] = {1, 2, 3, 4};

const uint64_t kExpected64[kMaxSize + 1] = {
    0x907A56DE22C26E53, 0x7EAB43AAC7CDDD78, 0xB8D0569AB0B53D62,
    0x5C6BEFAB8A463D80, 0xF205A46893007EDA, 0x2B8A1668E4A94541,
    0xBD4CCC325BEFCA6F, 0x4D02AE1738F59482, 0xE1205108E55F3171,
    0x32D2644EC77A1584, 0xF6E10ACDB103A90B, 0xC3BBF4615B415C15,
    0x243CC2040063FA9C, 0xA89A58CE65E641FF, 0x24B031A348455A23,
    0x40793F86A449F33B, 0xCFAB3489F97EB832, 0x19FE67D2C8C5C0E2,
    0x04DD90A69C565CC2, 0x75D9518E2371C504, 0x38AD9B1141D3DD16,
    0x0264432CCD8A70E0, 0xA9DB5A6288683390, 0xD7B05492003F028C,
    0x205F615AEA59E51E, 0xEEE0C89621052884, 0x1BFC1A93A7284F4F,
    0x512175B5B70DA91D, 0xF71F8976A0A2C639, 0xAE093FEF1F84E3E7,
    0x22CA92B01161860F, 0x9FC7007CCF035A68, 0xA0C964D9ECD580FC,
    0x2C90F73CA03181FC, 0x185CF84E5691EB9E, 0x4FC1F5EF2752AA9B,
    0xF5B7391A5E0A33EB, 0xB9B84B83B4E96C9C, 0x5E42FE712A5CD9B4,
    0xA150F2F90C3F97DC, 0x7FA522D75E2D637D, 0x181AD0CC0DFFD32B,
    0x3889ED981E854028, 0xFB4297E8C586EE2D, 0x6D064A45BB28059C,
    0x90563609B3EC860C, 0x7AA4FCE94097C666, 0x1326BAC06B911E08,
    0xB926168D2B154F34, 0x9919848945B1948D, 0xA2A98FC534825EBE,
    0xE9809095213EF0B6, 0x582E5483707BC0E9, 0x086E9414A88A6AF5,
    0xEE86B98D20F6743D, 0xF89B7FF609B1C0A7, 0x4C7D9CC19E22C3E8,
    0x9A97005024562A6F, 0x5DD41CF423E6EBEF, 0xDF13609C0468E227,
    0x6E0DA4F64188155A, 0xB755BA4B50D7D4A1, 0x887A3484647479BD,
    0xAB8EEBE9BF2139A0, 0x75542C5D4CD2A6FF,
};

uint32_t KnuthMultiplicativeHash32(const void *buf, size_t size) {
  size_t i;
  uint32_t h;
  const uint32_t kPhiPrime = 0x9e3779b1;
  const unsigned char *p = (const unsigned char *)buf;
  for (h = i = 0; i < size; i++) h = (p[i] + h) * kPhiPrime;
  return h;
}

void TestHash64(uint64_t expected, const uint8_t *data, size_t size,
                const uint64_t *key) {
  uint64_t hash = HighwayHash64(data, size, key);
  if (expected != hash) {
    printf("Test failed: expected %016" PRIx64 ", got %016" PRIx64
           ", size: %d\n",
           expected, hash, (int)size);
    exit(1);
  }
}

TEST(highwayhash64, test) {
  int i;
  uint8_t data[kMaxSize + 1] = {0};
  for (i = 0; i <= kMaxSize; i++) {
    data[i] = i;
    TestHash64(kExpected64[i], data, i, kTestKey1);
  }
  for (i = 0; i < 33; i++) {
    data[i] = 128 + i;
  }
  TestHash64(0x53c516cce478cad7ull, data, 33, kTestKey2);
}

BENCH(highwayhash64, newbench) {
  char fun[256];
  rngset(fun, 256, _rand64, -1);
  EZBENCH_N("highwayhash64", 0, HighwayHash64(0, 0, kTestKey1));
  EZBENCH_N("highwayhash64", 8, HighwayHash64("helloooo", 8, kTestKey1));
  EZBENCH_N("highwayhash64", 31, HighwayHash64(fun, 31, kTestKey1));
  EZBENCH_N("highwayhash64", 32, HighwayHash64(fun, 32, kTestKey1));
  EZBENCH_N("highwayhash64", 63, HighwayHash64(fun, 63, kTestKey1));
  EZBENCH_N("highwayhash64", 64, HighwayHash64(fun, 64, kTestKey1));
  EZBENCH_N("highwayhash64", 128, HighwayHash64(fun, 128, kTestKey1));
  EZBENCH_N("highwayhash64", 256, HighwayHash64(fun, 256, kTestKey1));
  EZBENCH_N("highwayhash64", kHyperionSize,
            HighwayHash64(kHyperion, kHyperionSize, kTestKey1));
}

BENCH(highwayhash64, bench) {
  EZBENCH2("knuth small", donothing,
           __expropriate(KnuthMultiplicativeHash32(__veil("r", "hello"), 5)));
  EZBENCH2("crc32c small", donothing, __expropriate(crc32c(0, "hello", 5)));
  EZBENCH2("crc32 small", donothing,
           __expropriate(crc32_z(0, __veil("r", "hello"), 5)));
  EZBENCH2("highwayhash64 small", donothing,
           HighwayHash64((void *)"hello", 5, kTestKey1));
  EZBENCH2("crc32 big", donothing,
           __expropriate(crc32_z(0, kHyperion, kHyperionSize)));
  EZBENCH2("crc32c big", donothing,
           __expropriate(crc32c(0, kHyperion, kHyperionSize)));
  EZBENCH2("highwayhash64 big", donothing,
           HighwayHash64((void *)kHyperion, kHyperionSize, kTestKey1));
  EZBENCH2("knuth big", donothing,
           __expropriate(KnuthMultiplicativeHash32(__veil("r", kHyperion),
                                                   kHyperionSize)));
}
