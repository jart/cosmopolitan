/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
