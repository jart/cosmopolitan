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
#include "test/libc/xed/lib.h"
#include "third_party/xed/x86.h"

/**
 * @fileoverview Hefty NOPs.
 */

TEST(x86ild, test_nop9_660F1F840000000000) {
  /*
    ICLASS: NOP
    CATEGORY: WIDENOP
    EXTENSION: BASE
    IFORM: NOP_MEMv_GPRv_0F1F
    ISA_SET: FAT_NOP
    SHORT: nop word ptr [rax+rax*1], ax
  */
  ASSERT_EQ(9, ild(u"f☼▼ä     "));
}

TEST(x86ild, test_nop8_0F1F840000000000) {
  /* nopl 0x00000000(%rax,%rax,1) */
  ASSERT_EQ(8, ild(u"☼▼ä     "));
}

TEST(x86ild, test_nop7_0F1F8000000000) {
  /* nopl 0x00000000(%rax) */
  ASSERT_EQ(7, ild(u"☼▼Ç    "));
}

TEST(x86ild, test_nop6_660F1F440000) {
  /*
    ICLASS: NOP
    CATEGORY: WIDENOP
    EXTENSION: BASE
    IFORM: NOP_MEMv_GPRv_0F1F
    ISA_SET: FAT_NOP
    SHORT: nop word ptr [rax+rax*1], ax
  */
  ASSERT_EQ(6, ild(u"f☼▼D  "));
}

TEST(x86ild, test_nop5_0F1F440000) {
  /*
    ICLASS: NOP
    CATEGORY: WIDENOP
    EXTENSION: BASE
    IFORM: NOP_MEMv_GPRv_0F1F
    ISA_SET: FAT_NOP
    SHORT: nop dword ptr [rax+rax*1], eax
  */
  ASSERT_EQ(5, ild(u"☼▼D  "));
}

TEST(x86ild, test_nop4_0F1F4000) {
  /*
    ICLASS: NOP
    CATEGORY: WIDENOP
    EXTENSION: BASE
    IFORM: NOP_MEMv_GPRv_0F1F
    ISA_SET: FAT_NOP
    SHORT: nop dword ptr [rax], eax
  */
  ASSERT_EQ(4, ild(u"☼▼@ "));
}

TEST(x86ild, test_nop3_0F1F00) {
  /*
    ICLASS: NOP
    CATEGORY: WIDENOP
    EXTENSION: BASE
    IFORM: NOP_MEMv_GPRv_0F1F
    ISA_SET: FAT_NOP
    SHORT: nop dword ptr [rax], eax
  */
  ASSERT_EQ(3, ild(u"☼▼ "));
}

TEST(x86ild, test_nop2_6690) {
  /* xchg %ax,%ax */
  ASSERT_EQ(2, ild(u"fÉ"));
}

TEST(x86ild, test_nop1_90) {
  /* xchg %eax,%eax */
  ASSERT_EQ(1, ild(u"É"));
}
