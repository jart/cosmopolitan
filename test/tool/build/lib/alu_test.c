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
#include "libc/assert.h"
#include "libc/bits/progn.internal.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "test/tool/build/lib/optest.h"
#include "tool/build/lib/alu.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/flags.h"

#define ALU_TEST 8

#define NATIVE_ALU2(MODE, INSTRUCTION)                     \
  asm("pushf\n\t"                                          \
      "andl\t%3,(%%rsp)\n\t"                               \
      "orl\t%4,(%%rsp)\n\t"                                \
      "popf\n\t" INSTRUCTION "\t%" MODE "2,%" MODE "0\n\t" \
      "pushf\n\t"                                          \
      "pop\t%q1"                                           \
      : "+r"(x), "=rm"(*f)                                 \
      : "r"(y), "i"(~FMASK), "r"(*f & FMASK)               \
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

int64_t RunGolden(char w, int h, uint64_t x, uint64_t y, uint32_t *f) {
  bool rw;
  rw = !(h & ALU_TEST);
  switch (h & 7) {
    case ALU_OR:
      NATIVE_ALU2_ANYBITS("or", rw);
    case ALU_AND:
      if (rw) {
        NATIVE_ALU2_ANYBITS("and", rw);
      } else {
        NATIVE_ALU2_ANYBITS("test", rw);
      }
    case ALU_XOR:
      NATIVE_ALU2_ANYBITS("xor", rw);
    case ALU_SBB:
      NATIVE_ALU2_ANYBITS("sbb", rw);
    case ALU_CMP:
      rw = false;
      NATIVE_ALU2_ANYBITS("cmp", rw);
    case ALU_SUB:
      NATIVE_ALU2_ANYBITS("sub", rw);
    case ALU_ADC:
      NATIVE_ALU2_ANYBITS("adc", rw);
    case ALU_ADD:
      NATIVE_ALU2_ANYBITS("add", rw);
    default:
      abort();
  }
}

const uint8_t kAluOps[] = {
    ALU_ADD, ALU_OR, ALU_ADC, ALU_SBB, ALU_AND, ALU_SUB, ALU_XOR, ALU_CMP, ALU_AND | ALU_TEST,
};

const char *const kAluNames[] = {
    [ALU_ADD] = "add", [ALU_OR] = "or",   [ALU_ADC] = "adc",
    [ALU_SBB] = "sbb", [ALU_AND] = "and", [ALU_SUB] = "sub",
    [ALU_XOR] = "xor", [ALU_CMP] = "cmp", [ALU_AND | ALU_TEST] = "test",
};

int64_t Alu(int w, int h, uint64_t x, uint64_t y, uint32_t *flags) {
  if (h < ALU_CMP) {
    return kAlu[h][w](x, y, flags);
  } else {
    kAlu[h & 7][w](x, y, flags);
    return x;
  }
}

int64_t RunOpTest(char w, int h, uint64_t x, uint64_t y, uint32_t *f) {
  return Alu(w, h, x, y, f);
}

TEST(alu, test) {
  RunOpTests(kAluOps, ARRAYLEN(kAluOps), kAluNames);
}
