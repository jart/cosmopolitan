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

TEST(x86ild, test_0F45C2) {
  /*
    ICLASS: CMOVNZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVNZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovnz eax, edx
  */
  ASSERT_EQ(3, ild(u"☼E┬"));
}

TEST(x86ild, test_0F47F8) {
  /*
    ICLASS: CMOVNBE
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVNBE_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovnbe edi, eax
  */
  ASSERT_EQ(3, ild(u"☼G°"));
}

TEST(x86ild, test_480F44F0) {
  /*
    ICLASS: CMOVZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovz rsi, rax
  */
  ASSERT_EQ(4, ild(u"H☼D≡"));
}

TEST(x86ild, test_0F44C2) {
  /*
    ICLASS: CMOVZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovz eax, edx
  */
  ASSERT_EQ(3, ild(u"☼D┬"));
}

TEST(x86ild, test_0F47E8) {
  /*
    ICLASS: CMOVNBE
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVNBE_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovnbe ebp, eax
  */
  ASSERT_EQ(3, ild(u"☼GΦ"));
}

TEST(x86ild, test_0F45C3) {
  /*
    ICLASS: CMOVNZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVNZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovnz eax, ebx
  */
  ASSERT_EQ(3, ild(u"☼E├"));
}

TEST(x86ild, test_0F44C7) {
  /*
    ICLASS: CMOVZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovz eax, edi
  */
  ASSERT_EQ(3, ild(u"☼D╟"));
}

TEST(x86ild, test_480F44F2) {
  /*
    ICLASS: CMOVZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovz rsi, rdx
  */
  ASSERT_EQ(4, ild(u"H☼D≥"));
}

TEST(x86ild, test_480F44C7) {
  /*
    ICLASS: CMOVZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovz rax, rdi
  */
  ASSERT_EQ(4, ild(u"H☼D╟"));
}

TEST(x86ild, test_440F45F2) {
  /*
    ICLASS: CMOVNZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVNZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovnz r14d, edx
  */
  ASSERT_EQ(4, ild(u"D☼E≥"));
}

TEST(x86ild, test_440F45F0) {
  /*
    ICLASS: CMOVNZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVNZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovnz r14d, eax
  */
  ASSERT_EQ(4, ild(u"D☼E≡"));
}

TEST(x86ild, test_0F4DC8) {
  /*
    ICLASS: CMOVNL
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVNL_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovnl ecx, eax
  */
  ASSERT_EQ(3, ild(u"☼M╚"));
}

TEST(x86ild, test_0F46D8) {
  /*
    ICLASS: CMOVBE
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVBE_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovbe ebx, eax
  */
  ASSERT_EQ(3, ild(u"☼F╪"));
}

TEST(x86ild, test_0F46CF) {
  /*
    ICLASS: CMOVBE
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVBE_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovbe ecx, edi
  */
  ASSERT_EQ(3, ild(u"☼F╧"));
}

TEST(x86ild, test_0F46C1) {
  /*
    ICLASS: CMOVBE
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVBE_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovbe eax, ecx
  */
  ASSERT_EQ(3, ild(u"☼F┴"));
}

TEST(x86ild, test_0F45C6) {
  /*
    ICLASS: CMOVNZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVNZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovnz eax, esi
  */
  ASSERT_EQ(3, ild(u"☼E╞"));
}

TEST(x86ild, test_0F44D0) {
  /*
    ICLASS: CMOVZ
    CATEGORY: CMOV
    EXTENSION: BASE
    IFORM: CMOVZ_GPRv_GPRv
    ISA_SET: CMOV
    SHORT: cmovz edx, eax
  */
  ASSERT_EQ(3, ild(u"☼D╨"));
}
