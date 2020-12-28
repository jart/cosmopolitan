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
#include "libc/macros.h"
#include "libc/testlib/testlib.h"
#include "test/tool/build/lib/numbers.h"
#include "tool/build/lib/bitscan.h"
#include "tool/build/lib/flags.h"

#define OSZ  00000000040
#define REXW 00000000100

struct Machine m[1];
struct XedDecodedInst xedd[1];

void SetUp(void) {
  m->xedd = xedd;
}

TEST(bsr64, test) {
  bool zf;
  uint64_t i, w, x, a, b;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    a = AluBsr(m, REXW, x);
    asm("bsrq\t%2,%0" : "=r"(b), "=@ccz"(zf) : "r"(x));
    ASSERT_EQ(zf, GetFlag(m->flags, FLAGS_ZF));
    if (!zf) ASSERT_EQ(a, b);
  }
}

TEST(bsr32, test) {
  bool zf;
  uint32_t i, w, x, a, b;
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
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    a = AluBsr(m, OSZ, x);
    asm("bsrw\t%2,%0" : "=r"(b), "=@ccz"(zf) : "r"(x));
    ASSERT_EQ(zf, GetFlag(m->flags, FLAGS_ZF));
    if (!zf) ASSERT_EQ(a, b);
  }
}

TEST(bsf64, test) {
  bool zf;
  uint64_t i, w, x, a, b;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    a = AluBsf(m, REXW, x);
    asm("bsfq\t%2,%0" : "=r"(b), "=@ccz"(zf) : "r"(x));
    ASSERT_EQ(zf, GetFlag(m->flags, FLAGS_ZF));
    if (!zf) ASSERT_EQ(a, b);
  }
}

TEST(bsf32, test) {
  bool zf;
  uint32_t i, w, x, a, b;
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
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    a = AluBsf(m, OSZ, x);
    asm("bsfw\t%2,%0" : "=r"(b), "=@ccz"(zf) : "r"(x));
    ASSERT_EQ(zf, GetFlag(m->flags, FLAGS_ZF));
    if (!zf) ASSERT_EQ(a, b, "%#lx", x);
  }
}
