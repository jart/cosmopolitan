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
#include "libc/macros.h"
#include "libc/testlib/testlib.h"
#include "test/tool/build/lib/numbers.h"
#include "tool/build/lib/bitscan.h"
#include "tool/build/lib/flags.h"

#define OSZ  00000000040
#define REXW 00000004000

struct Machine m[1];
struct XedDecodedInst xedd[1];

void SetUp(void) {
  m->xedd = xedd;
}

TEST(bsr64, test) {
  bool zf;
  uint64_t i, w, x, a, b;
  m->xedd->op.rde = REXW;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    a = AluBsr(m, 0, x);
    asm("bsrq\t%2,%0" : "=r"(b), "=@ccz"(zf) : "r"(x));
    ASSERT_EQ(zf, GetFlag(m->flags, FLAGS_ZF));
    if (!zf) ASSERT_EQ(a, b);
  }
}

TEST(bsr32, test) {
  bool zf;
  uint32_t i, w, x, a, b;
  m->xedd->op.rde = 0;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    a = AluBsr(m, 0, x);
    asm("bsrl\t%2,%0" : "=r"(b), "=@ccz"(zf) : "r"(x));
    ASSERT_EQ(zf, GetFlag(m->flags, FLAGS_ZF));
    if (!zf) ASSERT_EQ(a, b);
  }
}

TEST(bsr16, test) {
  bool zf;
  uint16_t i, w, x, a, b;
  m->xedd->op.rde = OSZ;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    a = AluBsr(m, 0, x);
    asm("bsrw\t%2,%0" : "=r"(b), "=@ccz"(zf) : "r"(x));
    ASSERT_EQ(zf, GetFlag(m->flags, FLAGS_ZF));
    if (!zf) ASSERT_EQ(a, b);
  }
}

TEST(bsf64, test) {
  bool zf;
  uint64_t i, w, x, a, b;
  m->xedd->op.rde = REXW;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    a = AluBsf(m, 0, x);
    asm("bsfq\t%2,%0" : "=r"(b), "=@ccz"(zf) : "r"(x));
    ASSERT_EQ(zf, GetFlag(m->flags, FLAGS_ZF));
    if (!zf) ASSERT_EQ(a, b);
  }
}

TEST(bsf32, test) {
  bool zf;
  uint32_t i, w, x, a, b;
  m->xedd->op.rde = 0;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    a = AluBsf(m, 0, x);
    asm("bsfl\t%2,%0" : "=r"(b), "=@ccz"(zf) : "r"(x));
    ASSERT_EQ(zf, GetFlag(m->flags, FLAGS_ZF));
    if (!zf) ASSERT_EQ(a, b);
  }
}

TEST(bsf16, test) {
  bool zf;
  uint16_t i, w, x, a, b;
  m->xedd->op.rde = OSZ;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    a = AluBsf(m, 0, x);
    asm("bsfw\t%2,%0" : "=r"(b), "=@ccz"(zf) : "r"(x));
    ASSERT_EQ(zf, GetFlag(m->flags, FLAGS_ZF));
    if (!zf) ASSERT_EQ(a, b, "%#lx", x);
  }
}
