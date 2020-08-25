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
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/testlib/testlib.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/modrm.h"

TEST(modrm, testAddressSizeOverride_isNotPresent_keepsWholeExpression) {
  struct Machine *m = gc(NewMachine());
  struct XedDecodedInst *xedd = gc(calloc(1, sizeof(struct XedDecodedInst)));
  uint8_t op[] = {0x8d, 0x04, 0x03}; /* lea (%rbx,%rax,1),%eax */
  m->xedd = xedd;
  Write64(m->bx, 0x2);
  Write64(m->ax, 0xffffffff);
  xed_decoded_inst_zero_set_mode(xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(xedd, op, sizeof(op)));
  EXPECT_EQ(0x100000001, ComputeAddress(m));
}

TEST(modrm, testAddressSizeOverride_isPresent_modulesWholeExpression) {
  struct Machine *m = gc(NewMachine());
  struct XedDecodedInst *xedd = gc(calloc(1, sizeof(struct XedDecodedInst)));
  uint8_t op[] = {0x67, 0x8d, 0x04, 0x03}; /* lea (%ebx,%eax,1),%eax */
  m->xedd = xedd;
  Write64(m->bx, 0x2);
  Write64(m->ax, 0xffffffff);
  xed_decoded_inst_zero_set_mode(xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(xedd, op, sizeof(op)));
  EXPECT_EQ(0x000000001, ComputeAddress(m));
}

TEST(modrm, testOverflow_doesntTriggerTooling) {
  struct Machine *m = gc(NewMachine());
  struct XedDecodedInst *xedd = gc(calloc(1, sizeof(struct XedDecodedInst)));
  uint8_t op[] = {0x8d, 0x04, 0x03}; /* lea (%rbx,%rax,1),%eax */
  m->xedd = xedd;
  Write64(m->bx, 0x0000000000000001);
  Write64(m->ax, 0x7fffffffffffffff);
  xed_decoded_inst_zero_set_mode(xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(xedd, op, sizeof(op)));
  EXPECT_EQ(0x8000000000000000ull, (uint64_t)ComputeAddress(m));
}

TEST(modrm, testPuttingOnTheRiz) {
  struct Machine *m = gc(NewMachine());
  static uint8_t ops[][15] = {
      {0x8d, 0b00110100, 0b00100110},        // lea (%rsi),%esi
      {0x67, 0x8d, 0b00110100, 0b11100110},  // lea (%esi,%eiz,8),%esi
      {103, 141, 180, 229, 55, 19, 3, 0},    // lea 0x31337(%ebp,%eiz,8),%esi
      {141, 52, 229, 55, 19, 3, 0},          // lea 0x31337,%esi
  };
  m->xedd = gc(calloc(1, sizeof(struct XedDecodedInst)));
  Write64(m->si, 0x100000001);
  Write64(m->bp, 0x200000002);
  xed_decoded_inst_zero_set_mode(m->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(m->xedd, ops[0], sizeof(ops[0])));
  EXPECT_EQ(0x100000001, ComputeAddress(m));
  xed_decoded_inst_zero_set_mode(m->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(m->xedd, ops[1], sizeof(ops[1])));
  EXPECT_EQ(0x000000001, ComputeAddress(m));
  xed_decoded_inst_zero_set_mode(m->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(m->xedd, ops[2], sizeof(ops[2])));
  EXPECT_EQ(0x31339, ComputeAddress(m));
  xed_decoded_inst_zero_set_mode(m->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(m->xedd, ops[3], sizeof(ops[3])));
  EXPECT_EQ(0x31337, ComputeAddress(m));
}

TEST(modrm, testSibIndexOnly) {
  //       mod                  = 0b00  (0)
  //       reg                  = 0b000 (0)
  //       rm                   = 0b100 (4)
  //       scale                = 0b10  (2)
  //       index                = 0b001 (1)
  //       base                 = 0b101 (5)
  struct Machine *m = gc(NewMachine());
  struct XedDecodedInst *xedd = gc(calloc(1, sizeof(struct XedDecodedInst)));
  uint8_t op[] = {76, 141, 4, 141, 0, 0, 0, 0}; /* lea 0x0(,%rcx,4),%r8 */
  m->xedd = xedd;
  Write64(m->bp, 0x123);
  Write64(m->cx, 0x123);
  xed_decoded_inst_zero_set_mode(xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(xedd, op, sizeof(op)));
  EXPECT_EQ(0x123 * 4, (uint64_t)ComputeAddress(m));
}
