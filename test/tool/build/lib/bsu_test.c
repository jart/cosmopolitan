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
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "test/tool/build/lib/numbers.h"
#include "test/tool/build/lib/optest.h"
#include "tool/build/lib/alu.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/machine.h"

#define NATIVE_ALU2(MODE, INSTRUCTION)              \
  asm("pushf\n\t"                                   \
      "andl\t%3,(%%rsp)\n\t"                        \
      "orl\t%4,(%%rsp)\n\t"                         \
      "popf\n\t" INSTRUCTION "\t%b2,%" MODE "0\n\t" \
      "pushf\n\t"                                   \
      "pop\t%q1"                                    \
      : "+r"(x), "=r"(*f)                           \
      : "c"(y), "i"(~FMASK), "r"(*f & FMASK)        \
      : "cc")

#define NATIVE_ALU2_ANYBITS(INSTRUCTION, MUTATING) \
  switch (w) {                                     \
    case 0:                                        \
      NATIVE_ALU2("b", INSTRUCTION);               \
      if (MUTATING) x &= 0xff;                     \
      return x;                                    \
    case 1:                                        \
      NATIVE_ALU2("w", INSTRUCTION);               \
      if (MUTATING) x &= 0xffff;                   \
      return x;                                    \
    case 2:                                        \
      NATIVE_ALU2("k", INSTRUCTION);               \
      return x;                                    \
    case 3:                                        \
      NATIVE_ALU2("q", INSTRUCTION);               \
      return x;                                    \
    default:                                       \
      abort();                                     \
  }

int64_t Bsu(int w, int h, uint64_t x, uint64_t y, uint32_t *f) {
  switch (h & 7) {
    case BSU_SHR:
      switch (w) {
        case 0:
          return Shr8(x, y, f);
        case 1:
          return Shr16(x, y, f);
        case 2:
          return Shr32(x, y, f);
        case 3:
          return Shr64(x, y, f);
        default:
          unreachable;
      }
    case BSU_SAL:
    case BSU_SHL:
      switch (w) {
        case 0:
          return Shl8(x, y, f);
        case 1:
          return Shl16(x, y, f);
        case 2:
          return Shl32(x, y, f);
        case 3:
          return Shl64(x, y, f);
        default:
          unreachable;
      }
    case BSU_SAR:
      switch (w) {
        case 0:
          return Sar8(x, y, f);
        case 1:
          return Sar16(x, y, f);
        case 2:
          return Sar32(x, y, f);
        case 3:
          return Sar64(x, y, f);
        default:
          unreachable;
      }
    case BSU_ROL:
      switch (w) {
        case 0:
          return Rol8(x, y, f);
        case 1:
          return Rol16(x, y, f);
        case 2:
          return Rol32(x, y, f);
        case 3:
          return Rol64(x, y, f);
        default:
          unreachable;
      }
    case BSU_ROR:
      switch (w) {
        case 0:
          return Ror8(x, y, f);
        case 1:
          return Ror16(x, y, f);
        case 2:
          return Ror32(x, y, f);
        case 3:
          return Ror64(x, y, f);
        default:
          unreachable;
      }
    case BSU_RCR:
      switch (w) {
        case 0:
          return Rcr8(x, y, f);
        case 1:
          return Rcr16(x, y, f);
        case 2:
          return Rcr32(x, y, f);
        case 3:
          return Rcr64(x, y, f);
        default:
          unreachable;
      }
    case BSU_RCL:
      switch (w) {
        case 0:
          return Rcl8(x, y, f);
        case 1:
          return Rcl16(x, y, f);
        case 2:
          return Rcl32(x, y, f);
        case 3:
          return Rcl64(x, y, f);
        default:
          unreachable;
      }
    default:
      unreachable;
  }
}

int64_t RunGolden(char w, int h, uint64_t x, uint64_t y, uint32_t *f) {
  switch (h & 7) {
    case BSU_ROR:
      NATIVE_ALU2_ANYBITS("ror", true);
    case BSU_SHL:
      NATIVE_ALU2_ANYBITS("shl", true);
    case BSU_SAL:
      NATIVE_ALU2_ANYBITS("sal", true);
    case BSU_RCR:
      NATIVE_ALU2_ANYBITS("rcr", true);
    case BSU_SAR:
      NATIVE_ALU2_ANYBITS("sar", true);
    case BSU_SHR:
      NATIVE_ALU2_ANYBITS("shr", true);
    case BSU_RCL:
      NATIVE_ALU2_ANYBITS("rcl", true);
    case BSU_ROL:
      NATIVE_ALU2_ANYBITS("rol", true);
    default:
      abort();
  }
}

const uint8_t kBsuOps[] = {
    BSU_SHR, BSU_SHL, BSU_ROL, BSU_ROR, BSU_RCL,
    BSU_RCR, BSU_SHL, BSU_SAL, BSU_SAR,
};

const char *const kBsuNames[] = {
    [BSU_ROL] = "rol", [BSU_ROR] = "ror", [BSU_RCL] = "rcl", [BSU_RCR] = "rcr",
    [BSU_SHL] = "shl", [BSU_SHR] = "shr", [BSU_SAL] = "sal", [BSU_SAR] = "sar",
};

int64_t RunOpTest(char w, int h, uint64_t x, uint64_t y, uint32_t *f) {
  return Bsu(w, h, x, y, f);
}

void FixupUndefOpTestFlags(char w, int h, uint64_t x, uint64_t y,
                           uint32_t goldenflags, uint32_t *flags) {
  y &= w == 3 ? 0x3F : 0x1F;
  if (y != 0 && y != 1) {
    *flags = SetFlag(*flags, FLAGS_OF, GetFlag(goldenflags, FLAGS_OF));
  }
}

TEST(bsu, test) {
  RunOpTests(kBsuOps, ARRAYLEN(kBsuOps), kBsuNames);
}

TEST(shrd32, smoke) {
  int i, j, k;
  uint8_t b, cf, of;
  uint32_t x, y, z1, f, z2, unflag;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    for (j = 0; j < ARRAYLEN(kNumbers); ++j) {
      y = kNumbers[j];
      for (k = 0; k < ARRAYLEN(kNumbers); ++k) {
        f = 0;
        cf = 0;
        of = 0;
        b = kNumbers[k];
        z1 = BsuDoubleShift(2, x, y, b, true, &f);
        z2 = x;
        asm("xor\t%1,%1\n\t"
            "shrd\t%5,%4,%0"
            : "+rm"(z2), "=&r"(unflag), "=@ccc"(cf), "=@cco"(of)
            : "r"(y), "c"(b)
            : "cc");
        ASSERT_EQ(z2, z1);
        ASSERT_EQ(cf, GetFlag(f, FLAGS_CF));
        if (b <= 1) ASSERT_EQ(of, GetFlag(f, FLAGS_OF));
      }
    }
  }
}

TEST(shld16, smoke) {
  uint32_t f;
  int i, j, k;
  uint8_t b, cf, of;
  uint16_t x, y, z1, z2, unflag;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    for (j = 0; j < ARRAYLEN(kNumbers); ++j) {
      y = kNumbers[j];
      for (k = 0; k < ARRAYLEN(kNumbers); ++k) {
        f = 0;
        cf = 0;
        of = 0;
        b = kNumbers[k];
        if (b > 16) continue;
        z1 = BsuDoubleShift(1, x, y, b, false, &f);
        z2 = x;
        asm("xor\t%1,%1\n\t"
            "shldw\t%5,%4,%0"
            : "+rm"(z2), "=&r"(unflag), "=@ccc"(cf), "=@cco"(of)
            : "r"(y), "c"(b)
            : "cc");
        ASSERT_EQ(z2, z1);
        ASSERT_EQ(cf, GetFlag(f, FLAGS_CF));
        if (b <= 1) ASSERT_EQ(of, GetFlag(f, FLAGS_OF));
      }
    }
  }
}

TEST(shld32, smoke) {
  int i, j, k;
  uint8_t b, cf, of;
  uint32_t x, y, z1, f, z2, unflag;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    for (j = 0; j < ARRAYLEN(kNumbers); ++j) {
      y = kNumbers[j];
      for (k = 0; k < ARRAYLEN(kNumbers); ++k) {
        f = 0;
        cf = 0;
        of = 0;
        b = kNumbers[k];
        if (b > 32) continue;
        z1 = BsuDoubleShift(2, x, y, b, false, &f);
        z2 = x;
        asm("xor\t%1,%1\n\t"
            "shld\t%5,%4,%0"
            : "+rm"(z2), "=&r"(unflag), "=@ccc"(cf), "=@cco"(of)
            : "r"(y), "c"(b)
            : "cc");
        ASSERT_EQ(z2, z1);
        ASSERT_EQ(cf, GetFlag(f, FLAGS_CF));
        if (b <= 1) ASSERT_EQ(of, GetFlag(f, FLAGS_OF));
      }
    }
  }
}

TEST(shld64, smoke) {
  uint32_t f;
  int i, j, k;
  uint8_t b, cf, of;
  uint64_t x, y, z1, z2, unflag;
  for (i = 0; i < ARRAYLEN(kNumbers); ++i) {
    x = kNumbers[i];
    for (j = 0; j < ARRAYLEN(kNumbers); ++j) {
      y = kNumbers[j];
      for (k = 0; k < ARRAYLEN(kNumbers); ++k) {
        f = 0;
        cf = 0;
        of = 0;
        b = kNumbers[k];
        if (b > 64) continue;
        z1 = BsuDoubleShift(3, x, y, b, false, &f);
        z2 = x;
        asm("xor\t%1,%1\n\t"
            "shldq\t%5,%4,%0"
            : "+rm"(z2), "=&r"(unflag), "=@ccc"(cf), "=@cco"(of)
            : "r"(y), "c"(b)
            : "cc");
        ASSERT_EQ(z2, z1);
        ASSERT_EQ(cf, GetFlag(f, FLAGS_CF));
        if (b <= 1) ASSERT_EQ(of, GetFlag(f, FLAGS_OF));
      }
    }
  }
}
