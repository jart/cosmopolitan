/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/fmt/bing.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "libc/str/varint.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

#define TV(X, B, I) __FILE__, __LINE__, #X, X, #B, B, #I, I

static const struct VarintTestVectors {
  const char *file;
  int line;
  const char *xcode;
  uint64_t x;
  const char *bcode;
  uint16_t b[11];
  const char *icode;
  uint8_t i;
} kv[] = {
    {TV(0x0000000000000000ul, u"          ", 1)},
    {TV(0x0000000000000001ul, u"☺         ", 1)},
    {TV(0x0000000000000003ul, u"♥         ", 1)},
    {TV(0x0000000000000007ul, u"•         ", 1)},
    {TV(0x000000000000000ful, u"☼         ", 1)},
    {TV(0x000000000000001ful, u"▼         ", 1)},
    {TV(0x000000000000003ful, u"⁇         ", 1)},
    {TV(0x000000000000007ful, u"⌂         ", 1)},
    {TV(0x00000000000000fful, u"λ☺        ", 2)},
    {TV(0x00000000000001fful, u"λ♥        ", 2)},
    {TV(0x00000000000003fful, u"λ•        ", 2)},
    {TV(0x00000000000007fful, u"λ☼        ", 2)},
    {TV(0x0000000000000ffful, u"λ▼        ", 2)},
    {TV(0x0000000000001ffful, u"λ⁇        ", 2)},
    {TV(0x0000000000003ffful, u"λ⌂        ", 2)},
    {TV(0x0000000000007ffful, u"λλ☺       ", 3)},
    {TV(0x000000000000fffful, u"λλ♥       ", 3)},
    {TV(0x000000000001fffful, u"λλ•       ", 3)},
    {TV(0x000000000003fffful, u"λλ☼       ", 3)},
    {TV(0x000000000007fffful, u"λλ▼       ", 3)},
    {TV(0x00000000000ffffful, u"λλ⁇       ", 3)},
    {TV(0x00000000001ffffful, u"λλ⌂       ", 3)},
    {TV(0x00000000003ffffful, u"λλλ☺      ", 4)},
    {TV(0x00000000007ffffful, u"λλλ♥      ", 4)},
    {TV(0x0000000000fffffful, u"λλλ•      ", 4)},
    {TV(0x0000000001fffffful, u"λλλ☼      ", 4)},
    {TV(0x0000000003fffffful, u"λλλ▼      ", 4)},
    {TV(0x0000000007fffffful, u"λλλ⁇      ", 4)},
    {TV(0x000000000ffffffful, u"λλλ⌂      ", 4)},
    {TV(0x000000001ffffffful, u"λλλλ☺     ", 5)},
    {TV(0x000000003ffffffful, u"λλλλ♥     ", 5)},
    {TV(0x000000007ffffffful, u"λλλλ•     ", 5)},
    {TV(0x00000000fffffffful, u"λλλλ☼     ", 5)},
    {TV(0x00000001fffffffful, u"λλλλ▼     ", 5)},
    {TV(0x00000003fffffffful, u"λλλλ⁇     ", 5)},
    {TV(0x00000007fffffffful, u"λλλλ⌂     ", 5)},
    {TV(0x0000000ffffffffful, u"λλλλλ☺    ", 6)},
    {TV(0x0000001ffffffffful, u"λλλλλ♥    ", 6)},
    {TV(0x0000003ffffffffful, u"λλλλλ•    ", 6)},
    {TV(0x0000007ffffffffful, u"λλλλλ☼    ", 6)},
    {TV(0x000000fffffffffful, u"λλλλλ▼    ", 6)},
    {TV(0x000001fffffffffful, u"λλλλλ⁇    ", 6)},
    {TV(0x000003fffffffffful, u"λλλλλ⌂    ", 6)},
    {TV(0x000007fffffffffful, u"λλλλλλ☺   ", 7)},
    {TV(0x00000ffffffffffful, u"λλλλλλ♥   ", 7)},
    {TV(0x00001ffffffffffful, u"λλλλλλ•   ", 7)},
    {TV(0x00003ffffffffffful, u"λλλλλλ☼   ", 7)},
    {TV(0x00007ffffffffffful, u"λλλλλλ▼   ", 7)},
    {TV(0x0000fffffffffffful, u"λλλλλλ⁇   ", 7)},
    {TV(0x0001fffffffffffful, u"λλλλλλ⌂   ", 7)},
    {TV(0x0003fffffffffffful, u"λλλλλλλ☺  ", 8)},
    {TV(0x0007fffffffffffful, u"λλλλλλλ♥  ", 8)},
    {TV(0x000ffffffffffffful, u"λλλλλλλ•  ", 8)},
    {TV(0x001ffffffffffffful, u"λλλλλλλ☼  ", 8)},
    {TV(0x003ffffffffffffful, u"λλλλλλλ▼  ", 8)},
    {TV(0x007ffffffffffffful, u"λλλλλλλ⁇  ", 8)},
    {TV(0x00fffffffffffffful, u"λλλλλλλ⌂  ", 8)},
    {TV(0x01fffffffffffffful, u"λλλλλλλλ☺ ", 9)},
    {TV(0x03fffffffffffffful, u"λλλλλλλλ♥ ", 9)},
    {TV(0x07fffffffffffffful, u"λλλλλλλλ• ", 9)},
    {TV(0x0ffffffffffffffful, u"λλλλλλλλ☼ ", 9)},
    {TV(0x1ffffffffffffffful, u"λλλλλλλλ▼ ", 9)},
    {TV(0x3ffffffffffffffful, u"λλλλλλλλ⁇ ", 9)},
    {TV(0x7ffffffffffffffful, u"λλλλλλλλ⌂ ", 9)},
    {TV(0xfffffffffffffffful, u"λλλλλλλλλ☺", 10)},
};

const struct ZigzagTestVectors {
  const char *file;
  int line;
  const char *xcode;
  int64_t x;
  const char *bcode;
  uint16_t b[11];
  const char *icode;
  uint8_t i;
} kz[] = {
    {TV(0, u" ", 1)},
    {TV(1, u"☻", 1)},
    {TV(-1, u"☺", 1)},
    {TV(2, u"♦", 1)},
    {TV(-2, u"♥", 1)},
    {TV(4, u"◘", 1)},
    {TV(-4, u"•", 1)},
    {TV(8, u"►", 1)},
    {TV(-8, u"☼", 1)},
    {TV(16, u" ", 1)},
    {TV(-16, u"▼", 1)},
    {TV(32, u"@", 1)},
    {TV(-32, u"⁇", 1)},
    {TV(64, u"Ç☺", 2)},
    {TV(-64, u"⌂", 1)},
    {TV(128, u"Ç☻", 2)},
    {TV(-128, u"λ☺", 2)},
    {TV(256, u"Ç♦", 2)},
    {TV(-256, u"λ♥", 2)},
    {TV(512, u"Ç◘", 2)},
    {TV(-512, u"λ•", 2)},
    {TV(1024, u"Ç►", 2)},
    {TV(-1024, u"λ☼", 2)},
    {TV(2048, u"Ç ", 2)},
    {TV(-2048, u"λ▼", 2)},
    {TV(4096, u"Ç@", 2)},
    {TV(-4096, u"λ⁇", 2)},
    {TV(8192, u"ÇÇ☺", 3)},
    {TV(-8192, u"λ⌂", 2)},
    {TV(16384, u"ÇÇ☻", 3)},
    {TV(-16384, u"λλ☺", 3)},
    {TV(32768, u"ÇÇ♦", 3)},
    {TV(-32768, u"λλ♥", 3)},
    {TV(65536, u"ÇÇ◘", 3)},
    {TV(-65536, u"λλ•", 3)},
    {TV(131072, u"ÇÇ►", 3)},
    {TV(-131072, u"λλ☼", 3)},
    {TV(262144, u"ÇÇ ", 3)},
    {TV(-262144, u"λλ▼", 3)},
    {TV(524288, u"ÇÇ@", 3)},
    {TV(-524288, u"λλ⁇", 3)},
    {TV(1048576, u"ÇÇÇ☺", 4)},
    {TV(-1048576, u"λλ⌂", 3)},
    {TV(2097152, u"ÇÇÇ☻", 4)},
    {TV(-2097152, u"λλλ☺", 4)},
    {TV(4194304, u"ÇÇÇ♦", 4)},
    {TV(-4194304, u"λλλ♥", 4)},
    {TV(8388608, u"ÇÇÇ◘", 4)},
    {TV(-8388608, u"λλλ•", 4)},
    {TV(16777216, u"ÇÇÇ►", 4)},
    {TV(-16777216, u"λλλ☼", 4)},
    {TV(33554432, u"ÇÇÇ ", 4)},
    {TV(-33554432, u"λλλ▼", 4)},
    {TV(67108864, u"ÇÇÇ@", 4)},
    {TV(-67108864, u"λλλ⁇", 4)},
    {TV(134217728, u"ÇÇÇÇ☺", 5)},
    {TV(-134217728, u"λλλ⌂", 4)},
    {TV(268435456, u"ÇÇÇÇ☻", 5)},
    {TV(-268435456, u"λλλλ☺", 5)},
    {TV(536870912, u"ÇÇÇÇ♦", 5)},
    {TV(-536870912, u"λλλλ♥", 5)},
    {TV(1073741824, u"ÇÇÇÇ◘", 5)},
    {TV(-1073741824, u"λλλλ•", 5)},
    {TV(2147483648, u"ÇÇÇÇ►", 5)},
    {TV(-2147483648, u"λλλλ☼", 5)},
    {TV(4294967296, u"ÇÇÇÇ ", 5)},
    {TV(-4294967296, u"λλλλ▼", 5)},
    {TV(8589934592, u"ÇÇÇÇ@", 5)},
    {TV(-8589934592, u"λλλλ⁇", 5)},
    {TV(17179869184, u"ÇÇÇÇÇ☺", 6)},
    {TV(-17179869184, u"λλλλ⌂", 5)},
    {TV(34359738368, u"ÇÇÇÇÇ☻", 6)},
    {TV(-34359738368, u"λλλλλ☺", 6)},
    {TV(68719476736, u"ÇÇÇÇÇ♦", 6)},
    {TV(-68719476736, u"λλλλλ♥", 6)},
    {TV(137438953472, u"ÇÇÇÇÇ◘", 6)},
    {TV(-137438953472, u"λλλλλ•", 6)},
    {TV(274877906944, u"ÇÇÇÇÇ►", 6)},
    {TV(-274877906944, u"λλλλλ☼", 6)},
    {TV(549755813888, u"ÇÇÇÇÇ ", 6)},
    {TV(-549755813888, u"λλλλλ▼", 6)},
    {TV(1099511627776, u"ÇÇÇÇÇ@", 6)},
    {TV(-1099511627776, u"λλλλλ⁇", 6)},
    {TV(2199023255552, u"ÇÇÇÇÇÇ☺", 7)},
    {TV(-2199023255552, u"λλλλλ⌂", 6)},
    {TV(4398046511104, u"ÇÇÇÇÇÇ☻", 7)},
    {TV(-4398046511104, u"λλλλλλ☺", 7)},
    {TV(8796093022208, u"ÇÇÇÇÇÇ♦", 7)},
    {TV(-8796093022208, u"λλλλλλ♥", 7)},
    {TV(17592186044416, u"ÇÇÇÇÇÇ◘", 7)},
    {TV(-17592186044416, u"λλλλλλ•", 7)},
    {TV(35184372088832, u"ÇÇÇÇÇÇ►", 7)},
    {TV(-35184372088832, u"λλλλλλ☼", 7)},
    {TV(70368744177664, u"ÇÇÇÇÇÇ ", 7)},
    {TV(-70368744177664, u"λλλλλλ▼", 7)},
    {TV(140737488355328, u"ÇÇÇÇÇÇ@", 7)},
    {TV(-140737488355328, u"λλλλλλ⁇", 7)},
    {TV(281474976710656, u"ÇÇÇÇÇÇÇ☺", 8)},
    {TV(-281474976710656, u"λλλλλλ⌂", 7)},
    {TV(562949953421312, u"ÇÇÇÇÇÇÇ☻", 8)},
    {TV(-562949953421312, u"λλλλλλλ☺", 8)},
    {TV(1125899906842624, u"ÇÇÇÇÇÇÇ♦", 8)},
    {TV(-1125899906842624, u"λλλλλλλ♥", 8)},
    {TV(2251799813685248, u"ÇÇÇÇÇÇÇ◘", 8)},
    {TV(-2251799813685248, u"λλλλλλλ•", 8)},
    {TV(4503599627370496, u"ÇÇÇÇÇÇÇ►", 8)},
    {TV(-4503599627370496, u"λλλλλλλ☼", 8)},
    {TV(9007199254740992, u"ÇÇÇÇÇÇÇ ", 8)},
    {TV(-9007199254740992, u"λλλλλλλ▼", 8)},
    {TV(18014398509481984, u"ÇÇÇÇÇÇÇ@", 8)},
    {TV(-18014398509481984, u"λλλλλλλ⁇", 8)},
    {TV(36028797018963968, u"ÇÇÇÇÇÇÇÇ☺", 9)},
    {TV(-36028797018963968, u"λλλλλλλ⌂", 8)},
    {TV(72057594037927936, u"ÇÇÇÇÇÇÇÇ☻", 9)},
    {TV(-72057594037927936, u"λλλλλλλλ☺", 9)},
    {TV(144115188075855872, u"ÇÇÇÇÇÇÇÇ♦", 9)},
    {TV(-144115188075855872, u"λλλλλλλλ♥", 9)},
    {TV(288230376151711744, u"ÇÇÇÇÇÇÇÇ◘", 9)},
    {TV(-288230376151711744, u"λλλλλλλλ•", 9)},
    {TV(576460752303423488, u"ÇÇÇÇÇÇÇÇ►", 9)},
    {TV(-576460752303423488, u"λλλλλλλλ☼", 9)},
    {TV(1152921504606846976, u"ÇÇÇÇÇÇÇÇ ", 9)},
    {TV(-1152921504606846976, u"λλλλλλλλ▼", 9)},
    {TV(2305843009213693952, u"ÇÇÇÇÇÇÇÇ@", 9)},
    {TV(-2305843009213693952, u"λλλλλλλλ⁇", 9)},
    {TV(4611686018427387904, u"ÇÇÇÇÇÇÇÇÇ☺", 10)},
    {TV(-4611686018427387904, u"λλλλλλλλ⌂", 9)},
    {TV(INT64_MIN, u"λλλλλλλλλ☺", 10)},
    {TV(INT64_MAX, u"■λλλλλλλλ☺", 10)}, /* wut */
};

TEST(writevint, test) {
  size_t i;
  uint8_t b[10], *p;
  for (i = 0; i < ARRAYLEN(kv); ++i) {
    memset(b, 0, sizeof(b));
    p = writevint(b, kv[i].x);
    __TEST_EQ(assert, kz[i].file, kz[i].line, __FUNCTION__, kz[i].icode,
              kz[i].xcode, kv[i].i, p - b);
    assertBinaryEquals$cp437(kz[i].file, kz[i].line, __FUNCTION__, kv[i].b, b,
                             strlen(kv[i].b), "", false);
  }
}

TEST(readvint, test) {
  size_t i;
  uint64_t x;
  uint8_t *b, *p;
  for (i = 0; i < ARRAYLEN(kv); ++i) {
    b = gc(unbingstr(kv[i].b));
    p = readvint(b, b + strlen(kv[i].b), &x);
    __TEST_EQ(assert, kv[i].file, kv[i].line, __FUNCTION__, kv[i].icode,
              kv[i].xcode, kv[i].i, (intptr_t)(p - b));
    assertBinaryEquals$cp437(kv[i].file, kv[i].line, __FUNCTION__, kv[i].b, b,
                             strlen(kv[i].b), "", false);
  }
}

TEST(writesint, test) {
  size_t i;
  uint8_t b[10], *p;
  for (i = 0; i < ARRAYLEN(kz); ++i) {
    memset(b, 0, sizeof(b));
    p = writesint(b, kz[i].x);
    __TEST_EQ(assert, kz[i].file, kz[i].line, __FUNCTION__, kz[i].icode,
              kz[i].xcode, kz[i].i, (intptr_t)(p - b));
    assertBinaryEquals$cp437(kz[i].file, kz[i].line, __FUNCTION__, kz[i].b, b,
                             strlen(kz[i].b), "", false);
  }
}

TEST(readsint, test) {
  size_t i;
  int64_t x;
  uint8_t *b, *p;
  for (i = 0; i < ARRAYLEN(kz); ++i) {
    b = gc(unbingstr(kz[i].b));
    p = readsint(b, b + strlen(kz[i].b), &x);
    __TEST_EQ(assert, kz[i].file, kz[i].line, __FUNCTION__, kz[i].icode,
              kz[i].xcode, kz[i].i, (intptr_t)(p - b));
    assertBinaryEquals$cp437(kz[i].file, kz[i].line, __FUNCTION__, kz[i].b, b,
                             strlen(kz[i].b), "", false);
  }
}
