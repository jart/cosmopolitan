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
#include "libc/fmt/bing.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/fpu.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"

const uint8_t kPi80[] = {
    0xd9, 0xe8,                          // fld1
    0xb8, 0x0a, 0x00, 0x00, 0x00,        // mov    $0xa,%eax
    0x31, 0xd2,                          // xor    %edx,%edx
    0xd9, 0xee,                          // fldz
    0x48, 0x98,                          // cltq
    0x48, 0x39, 0xc2,                    // cmp    %rax,%rdx
    0xd9, 0x05, 0x1a, 0x00, 0x00, 0x00,  // flds   0x1a(%rip)
    0x7d, 0x13,                          // jge    2b <pi80+0x2b>
    0xde, 0xc1,                          // faddp
    0x48, 0xff, 0xc2,                    // inc    %rdx
    0xd9, 0xfa,                          // fsqrt
    0xd9, 0x05, 0x0f, 0x00, 0x00, 0x00,  // flds   15(%rip)
    0xd8, 0xc9,                          // fmul   %st(1),%st
    0xde, 0xca,                          // fmulp  %st,%st(2)
    0xeb, 0xe2,                          // jmp    d <pi80+0xd>
    0xdd, 0xd9,                          // fstp   %st(1)
    0xde, 0xf1,                          // fdivp
    0xf4,                                // hlt
    0x00, 0x00, 0x00, 0x40,              // .float 2.0
    0x00, 0x00, 0x00, 0x3f,              // .float 0.5
};

const uint8_t kTenthprime[] = {
    0x31, 0xd2,                    // xor    %edx,%edx
    0x45, 0x31, 0xc0,              // xor    %r8d,%r8d
    0x31, 0xc9,                    // xor    %ecx,%ecx
    0xbe, 0x03, 0x00, 0x00, 0x00,  // mov    $0x3,%esi
    0x41, 0xff, 0xc0,              // inc    %r8d
    0x44, 0x89, 0xc0,              // mov    %r8d,%eax
    0x83, 0xf9, 0x0a,              // cmp    $0xa,%ecx
    0x74, 0x0b,                    // je     20
    0x99,                          // cltd
    0xf7, 0xfe,                    // idiv   %esi
    0x83, 0xfa, 0x01,              // cmp    $0x1,%edx
    0x83, 0xd9, 0xff,              // sbb    $-1,%ecx
    0xeb, 0xea,                    // jmp    a
    0xf4,                          // hlt
};

const uint8_t kTenthprime2[] = {
    0xE8, 0x11, 0x00, 0x00, 0x00,  //
    0xF4,                          //
    0x89, 0xF8,                    //
    0xB9, 0x03, 0x00, 0x00, 0x00,  //
    0x99,                          //
    0xF7, 0xF9,                    //
    0x85, 0xD2,                    //
    0x0F, 0x95, 0xC0,              //
    0xC3,                          //
    0x55,                          //
    0x48, 0x89, 0xE5,              //
    0x31, 0xF6,                    //
    0x45, 0x31, 0xC0,              //
    0x44, 0x89, 0xC7,              //
    0xE8, 0xDF, 0xFF, 0xFF, 0xFF,  //
    0x0F, 0xB6, 0xC0,              //
    0x66, 0x83, 0xF8, 0x01,        //
    0x83, 0xDE, 0xFF,              //
    0x41, 0xFF, 0xC0,              //
    0x83, 0xFE, 0x0A,              //
    0x75, 0xE6,                    //
    0x44, 0x89, 0xC0,              //
    0x5D,                          //
    0xC3,                          //
};

struct Machine *m;

void SetUp(void) {
  m = NewMachine();
  m->mode = XED_MACHINE_MODE_LONG_64;
  m->cr3 = AllocateLinearPage(m);
  ReserveVirtual(m, 0, 4096, 0x0207);
  ASSERT_EQ(0x1007, Read64(m->real.p + 0x0000));  // PML4T
  ASSERT_EQ(0x2007, Read64(m->real.p + 0x1000));  // PDPT
  ASSERT_EQ(0x3007, Read64(m->real.p + 0x2000));  // PDE
  ASSERT_EQ(0x0207, Read64(m->real.p + 0x3000));  // PT
  ASSERT_EQ(0x4000, m->real.i);
  ASSERT_EQ(1, m->memstat.reserved);
  ASSERT_EQ(4, m->memstat.committed);
  ASSERT_EQ(4, m->memstat.allocated);
  ASSERT_EQ(3, m->memstat.pagetables);
  Write64(m->sp, 4096);
}

void TearDown(void) {
  FreeVirtual(m, 0, 4096);
  ASSERT_EQ(0x1007, Read64(m->real.p + 0x0000));  // PML4T
  ASSERT_EQ(0x2007, Read64(m->real.p + 0x1000));  // PDPT
  ASSERT_EQ(0x3007, Read64(m->real.p + 0x2000));  // PDE
  ASSERT_EQ(0x0000, Read64(m->real.p + 0x3000));  // PT
  FreeMachine(m);
}

int ExecuteUntilHalt(struct Machine *m) {
  int rc;
  if (!(rc = setjmp(m->onhalt))) {
    for (;;) {
      LoadInstruction(m);
      ExecuteInstruction(m);
    }
  } else {
    return rc;
  }
}

TEST(machine, test) {
  VirtualRecv(m, 0, kTenthprime, sizeof(kTenthprime));
  ASSERT_EQ(kMachineHalt, ExecuteUntilHalt(m));
  ASSERT_EQ(15, Read32(m->ax));
}

TEST(machine, testFpu) {
  VirtualRecv(m, 0, kPi80, sizeof(kPi80));
  ASSERT_EQ(kMachineHalt, ExecuteUntilHalt(m));
  ASSERT_TRUE(fabs(3.14159 - FpuPop(m)) < 0.0001);
  m->ip = 0;
  ASSERT_EQ(kMachineHalt, ExecuteUntilHalt(m));
  ASSERT_TRUE(fabs(3.14159 - FpuPop(m)) < 0.0001);
}

BENCH(machine, benchPrimeNumberPrograms) {
  VirtualRecv(m, 0, kTenthprime2, sizeof(kTenthprime2));
  EZBENCH2("tenthprime2", m->ip = 0, ExecuteUntilHalt(m));
  ASSERT_EQ(15, Read32(m->ax));
  VirtualRecv(m, 0, kTenthprime, sizeof(kTenthprime));
  EZBENCH2("tenthprime", m->ip = 0, ExecuteUntilHalt(m));
  ASSERT_EQ(15, Read32(m->ax));
}

BENCH(machine, benchFpu) {
  VirtualRecv(m, 0, kPi80, sizeof(kPi80));
  EZBENCH2("pi80", m->ip = 0, PROGN(ExecuteUntilHalt(m), FpuPop(m)));
}

BENCH(machine, benchLoadExec2) {
  uint8_t kMovCode[] = {0xbe, 0x03, 0x00, 0x00, 0x00};
  VirtualRecv(m, 0, kMovCode, sizeof(kMovCode));
  LoadInstruction(m);
  EZBENCH2("mov", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchLoadExec3) {
  uint8_t kMovdCode[] = {0x66, 0x0f, 0x6e, 0xc0};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kMovdCode, sizeof(kMovdCode));
  LoadInstruction(m);
  EZBENCH2("movd", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchLoadExec4) {
  uint8_t kAddpsRegregCode[] = {0x0f, 0x58, 0xC0};
  uint8_t kAddpsMemregCode[] = {0x0f, 0x58, 0x00};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kAddpsRegregCode, sizeof(kAddpsRegregCode));
  LoadInstruction(m);
  EZBENCH2("addps reg reg", m->ip = 0, ExecuteInstruction(m));
  VirtualRecv(m, 0, kAddpsMemregCode, sizeof(kAddpsMemregCode));
  LoadInstruction(m);
  EZBENCH2("addps mem reg", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchLoadExec5) {
  uint8_t kPaddwRegregCode[] = {0x66, 0x0F, 0xFD, 0xC0};
  uint8_t kPaddwMemregCode[] = {0x66, 0x0F, 0xFD, 0x00};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kPaddwRegregCode, sizeof(kPaddwRegregCode));
  LoadInstruction(m);
  EZBENCH2("paddw", m->ip = 0, ExecuteInstruction(m));
  VirtualRecv(m, 0, kPaddwMemregCode, sizeof(kPaddwMemregCode));
  LoadInstruction(m);
  EZBENCH2("paddw mem", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchLoadExec6) {
  uint8_t kPsubqRegregCode[] = {0x66, 0x0F, 0xFB, 0xC0};
  uint8_t kPsubqMemregCode[] = {0x66, 0x0F, 0xFB, 0x00};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kPsubqRegregCode, sizeof(kPsubqRegregCode));
  LoadInstruction(m);
  EZBENCH2("psubq", m->ip = 0, ExecuteInstruction(m));
  VirtualRecv(m, 0, kPsubqMemregCode, sizeof(kPsubqMemregCode));
  LoadInstruction(m);
  EZBENCH2("psubq mem", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchAddqMem) {
  uint8_t kAddMemregCode[] = {0x48, 0x03, 0x08};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kAddMemregCode, sizeof(kAddMemregCode));
  LoadInstruction(m);
  EZBENCH2("addq mem", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchAddlMem) {
  uint8_t kAddMemregCode[] = {0x03, 0x08};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kAddMemregCode, sizeof(kAddMemregCode));
  LoadInstruction(m);
  EZBENCH2("addl mem", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchAddq) {
  uint8_t kAddqCode[] = {0x48, 0x01, 0xd8};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kAddqCode, sizeof(kAddqCode));
  LoadInstruction(m);
  EZBENCH2("addq", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchAddb) {
  uint8_t kAddbCode[] = {0x00, 0xd8};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kAddbCode, sizeof(kAddbCode));
  LoadInstruction(m);
  EZBENCH2("addb", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchXorReg) {
  VirtualRecv(m, 0, kTenthprime, sizeof(kTenthprime));
  LoadInstruction(m);
  EZBENCH2("xor", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchLoadExec8) {
  uint8_t kFchsCode[] = {0xd9, 0xe0};
  Write64(m->ax, 0);
  OpFinit(m);
  *FpuSt(m, 0) = M_PI;
  FpuSetTag(m, 0, kFpuTagValid);
  VirtualRecv(m, 0, kFchsCode, sizeof(kFchsCode));
  LoadInstruction(m);
  EZBENCH2("fchs", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchPushpop) {
  uint8_t kPushpop[] = {0x50, 0x58};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kPushpop, sizeof(kPushpop));
  EZBENCH2("pushpop", m->ip = 0,
           PROGN(LoadInstruction(m), ExecuteInstruction(m), LoadInstruction(m),
                 ExecuteInstruction(m)));
}

BENCH(machine, benchPause) {
  uint8_t kPause[] = {0xf3, 0x90};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kPause, sizeof(kPause));
  LoadInstruction(m);
  EZBENCH2("pause", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchClc) {
  uint8_t kClc[] = {0xf8};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kClc, sizeof(kClc));
  LoadInstruction(m);
  EZBENCH2("clc", m->ip = 0, ExecuteInstruction(m));
}

BENCH(machine, benchNop) {
  uint8_t kNop[] = {0x90};
  Write64(m->ax, 0);
  VirtualRecv(m, 0, kNop, sizeof(kNop));
  LoadInstruction(m);
  EZBENCH2("nop", m->ip = 0, ExecuteInstruction(m));
  EZBENCH2("nop w/ load", m->ip = 0,
           PROGN(LoadInstruction(m), ExecuteInstruction(m)));
}

TEST(x87, fprem1) {
  // 1 rem -1.5
  const uint8_t prog[] = {
      0xd9, 0x05, 0x05, 0x00, 0x00, 0x00,  // flds
      0xd9, 0xe8,                          // fld1
      0xd9, 0xf8,                          // fprem
      0xf4,                                // hlt
      0x00, 0x00, 0xc0, 0xbf,              // .float -1.5
  };
  VirtualRecv(m, 0, prog, sizeof(prog));
  ASSERT_EQ(kMachineHalt, ExecuteUntilHalt(m));
  ASSERT_LDBL_EQ(1, FpuPop(m));
}

TEST(x87, fprem2) {
  // 12300000000000000. rem .0000000000000123
  const uint8_t prog[] = {
      0xdd, 0x05, 0x11, 0x00, 0x00, 0x00,              // fldl
      0xdd, 0x05, 0x03, 0x00, 0x00, 0x00,              // fldl
      0xd9, 0xf8,                                      // fprem
      0xf4,                                            // hlt
      0x00, 0x60, 0x5e, 0x75, 0x64, 0xd9, 0x45, 0x43,  //
      0x5b, 0x14, 0xea, 0x9d, 0x77, 0xb2, 0x0b, 0x3d,  //
  };
  VirtualRecv(m, 0, prog, sizeof(prog));
  ASSERT_EQ(kMachineHalt, ExecuteUntilHalt(m));
  ASSERT_LDBL_EQ(1.1766221079117338e-14, FpuPop(m));
}

TEST(machine, sizeIsReasonable) {
  ASSERT_LE(sizeof(struct Machine), 65536 * 3);
}
