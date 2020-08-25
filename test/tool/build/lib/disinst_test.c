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
#include "libc/testlib/testlib.h"
#include "tool/build/lib/dis.h"

char b1[64];
char b2[64];
struct Dis d[1];
struct DisBuilder b = {d, d->xedd, 0};

TEST(DisInst, testInt3) {
  uint8_t op[] = {0xcc};
  xed_decoded_inst_zero_set_mode(d->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(d->xedd, op, sizeof(op)));
  DisInst(b, b1, DisSpec(d->xedd, b2));
  EXPECT_STREQ("int3    ", b1);
}

TEST(DisInst, testImmMem_needsSuffix) {
  uint8_t op[] = {0x80, 0x3c, 0x07, 0x00};
  xed_decoded_inst_zero_set_mode(d->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(d->xedd, op, sizeof(op)));
  DisInst(b, b1, DisSpec(d->xedd, b2));
  EXPECT_STREQ("cmpb    $0,(%rdi,%rax)", b1);
}

TEST(DisInst, testImmReg_doesntNeedSuffix) {
  uint8_t op[] = {0xb8, 0x08, 0x70, 0x40, 0x00};
  xed_decoded_inst_zero_set_mode(d->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(d->xedd, op, sizeof(op)));
  DisInst(b, b1, DisSpec(d->xedd, b2));
  EXPECT_STREQ("mov     $0x407008,%eax", b1);
}

TEST(DisInst, testPuttingOnTheRiz) {
  static uint8_t ops[][15] = {
      {0x8d, 0b00110100, 0b00100110},
      {0x67, 0x8d, 0b00110100, 0b11100110},
      {0x67, 0x8d, 0b10110100, 0b11100101, 0, 0, 0, 0},
      {0x8d, 0b00110100, 0b11100101, 0x37, 0x13, 0x03, 0},
      {0x8d, 0b10110100, 0b11100101, 0, 0, 0, 0},
  };
  xed_decoded_inst_zero_set_mode(d->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(d->xedd, ops[0], sizeof(ops[0])));
  DisInst(b, b1, DisSpec(d->xedd, b2));
  EXPECT_STREQ("lea     (%rsi),%esi", b1);
  xed_decoded_inst_zero_set_mode(d->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(d->xedd, ops[1], sizeof(ops[1])));
  DisInst(b, b1, DisSpec(d->xedd, b2));
  EXPECT_STREQ("lea     (%esi,%eiz,8),%esi", b1);
  xed_decoded_inst_zero_set_mode(d->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(d->xedd, ops[2], sizeof(ops[2])));
  DisInst(b, b1, DisSpec(d->xedd, b2));
  EXPECT_STREQ("lea     0(%ebp,%eiz,8),%esi", b1);
  xed_decoded_inst_zero_set_mode(d->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(d->xedd, ops[3], sizeof(ops[3])));
  DisInst(b, b1, DisSpec(d->xedd, b2));
  EXPECT_STREQ("lea     0x31337,%esi", b1);
  xed_decoded_inst_zero_set_mode(d->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(d->xedd, ops[4], sizeof(ops[4])));
  DisInst(b, b1, DisSpec(d->xedd, b2));
  EXPECT_STREQ("lea     0(%rbp,%riz,8),%esi", b1);
}

TEST(DisInst, testSibIndexOnly) {
  uint8_t op[] = {76, 141, 4, 141, 0, 0, 0, 0}; /* lea 0x0(,%rcx,4),%r8 */
  xed_decoded_inst_zero_set_mode(d->xedd, XED_MACHINE_MODE_LONG_64);
  ASSERT_EQ(0, xed_instruction_length_decode(d->xedd, op, sizeof(op)));
  DisInst(b, b1, DisSpec(d->xedd, b2));
  EXPECT_STREQ("lea     0(,%rcx,4),%r8", b1);
}
