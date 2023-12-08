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
 * @fileoverview GCC's popular i386+ instruction w/ NexGen32e encoding.
 */

TEST(x86ild, test_400FB6C5) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx eax, bpl
  */
  ASSERT_EQ(4, ild(u"@☼╢┼"));
}

TEST(x86ild, test_0FB6C0) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx eax, al
  */
  ASSERT_EQ(3, ild(u"☼╢└"));
}

TEST(x86ild, test_0FB6E8) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx ebp, al
  */
  ASSERT_EQ(3, ild(u"☼╢Φ"));
}

TEST(x86ild, test_0F95C0) {
  /*
    ICLASS: SETNZ
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETNZ_GPR8
    ISA_SET: I386
    SHORT: setnz al
  */
  ASSERT_EQ(3, ild(u"☼ò└"));
}

TEST(x86ild, test_0F94C0) {
  /*
    ICLASS: SETZ
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETZ_GPR8
    ISA_SET: I386
    SHORT: setz al
  */
  ASSERT_EQ(3, ild(u"☼ö└"));
}

TEST(x86ild, test_400FB6D5) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx edx, bpl
  */
  ASSERT_EQ(4, ild(u"@☼╢╒"));
}

TEST(x86ild, test_0FB6D0) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx edx, al
  */
  ASSERT_EQ(3, ild(u"☼╢╨"));
}

TEST(x86ild, test_0F95C2) {
  /*
    ICLASS: SETNZ
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETNZ_GPR8
    ISA_SET: I386
    SHORT: setnz dl
  */
  ASSERT_EQ(3, ild(u"☼ò┬"));
}

TEST(x86ild, test_0F94C2) {
  /*
    ICLASS: SETZ
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETZ_GPR8
    ISA_SET: I386
    SHORT: setz dl
  */
  ASSERT_EQ(3, ild(u"☼ö┬"));
}

TEST(x86ild, test_0FB6D2) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx edx, dl
  */
  ASSERT_EQ(3, ild(u"☼╢╥"));
}

TEST(x86ild, test_0FBE841200000000) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx eax, byte ptr [rdx+rdx*1]
  */
  ASSERT_EQ(8, ild(u"☼╛ä↕    "));
}

TEST(x86ild, test_0FB64000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rax]
  */
  ASSERT_EQ(4, ild(u"☼╢@ "));
}

TEST(x86ild, test_0FBE04D500000000) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx eax, byte ptr [rdx*8]
  */
  ASSERT_EQ(8, ild(u"☼╛♦╒    "));
}

TEST(x86ild, test_0FBE8000000000) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx eax, byte ptr [rax]
  */
  ASSERT_EQ(7, ild(u"☼╛Ç    "));
}

TEST(x86ild, test_0FB64800) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ecx, byte ptr [rax]
  */
  ASSERT_EQ(4, ild(u"☼╢H "));
}

TEST(x86ild, test_0FB64700) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"☼╢G "));
}

TEST(x86ild, test_0FB65700) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edx, byte ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"☼╢W "));
}

TEST(x86ild, test_0FB6C2) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx eax, dl
  */
  ASSERT_EQ(3, ild(u"☼╢┬"));
}

TEST(x86ild, test_0FB67000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx esi, byte ptr [rax]
  */
  ASSERT_EQ(4, ild(u"☼╢p "));
}

TEST(x86ild, test_0FB68000000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rax]
  */
  ASSERT_EQ(7, ild(u"☼╢Ç    "));
}

TEST(x86ild, test_0FB6C9) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx ecx, cl
  */
  ASSERT_EQ(3, ild(u"☼╢╔"));
}

TEST(x86ild, test_0FBE4100) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx eax, byte ptr [rcx]
  */
  ASSERT_EQ(4, ild(u"☼╛A "));
}

TEST(x86ild, test_0F94C1) {
  /*
    ICLASS: SETZ
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETZ_GPR8
    ISA_SET: I386
    SHORT: setz cl
  */
  ASSERT_EQ(3, ild(u"☼ö┴"));
}

TEST(x86ild, test_0FB6841200000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rdx+rdx*1]
  */
  ASSERT_EQ(8, ild(u"☼╢ä↕    "));
}

TEST(x86ild, test_400FB6F6) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx esi, sil
  */
  ASSERT_EQ(4, ild(u"@☼╢÷"));
}

TEST(x86ild, test_0FBAE500) {
  /*
    ICLASS: BT
    CATEGORY: BITBYTE
    EXTENSION: BASE
    IFORM: BT_GPRv_IMMb
    ISA_SET: I386
    SHORT: bt ebp, 0x0
  */
  ASSERT_EQ(4, ild(u"☼║σ "));
}

TEST(x86ild, test_0FB6C1) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx eax, cl
  */
  ASSERT_EQ(3, ild(u"☼╢┴"));
}

TEST(x86ild, test_0FBE4600) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx eax, byte ptr [rsi]
  */
  ASSERT_EQ(4, ild(u"☼╛F "));
}

TEST(x86ild, test_0FB77800) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx edi, word ptr [rax]
  */
  ASSERT_EQ(4, ild(u"☼╖x "));
}

TEST(x86ild, test_0FB68700000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rdi]
  */
  ASSERT_EQ(7, ild(u"☼╢ç    "));
}

TEST(x86ild, test_0FB67F00) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edi, byte ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"☼╢⌂ "));
}

TEST(x86ild, test_0FB64F00) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ecx, byte ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"☼╢O "));
}

TEST(x86ild, test_0FB604D500000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rdx*8]
  */
  ASSERT_EQ(8, ild(u"☼╢♦╒    "));
}

TEST(x86ild, test_0FB78000000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx eax, word ptr [rax]
  */
  ASSERT_EQ(7, ild(u"☼╖Ç    "));
}

TEST(x86ild, test_0FB74700) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx eax, word ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"☼╖G "));
}

TEST(x86ild, test_0F96C0) {
  /*
    ICLASS: SETBE
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETBE_GPR8
    ISA_SET: I386
    SHORT: setbe al
  */
  ASSERT_EQ(3, ild(u"☼û└"));
}

TEST(x86ild, test_0FB74500) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx eax, word ptr [rbp]
  */
  ASSERT_EQ(4, ild(u"☼╖E "));
}

TEST(x86ild, test_0FB6C8) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx ecx, al
  */
  ASSERT_EQ(3, ild(u"☼╢╚"));
}

TEST(x86ild, test_440FB67500) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r14d, byte ptr [rbp]
  */
  ASSERT_EQ(5, ild(u"D☼╢u "));
}

TEST(x86ild, test_0FBE5600) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx edx, byte ptr [rsi]
  */
  ASSERT_EQ(4, ild(u"☼╛V "));
}

TEST(x86ild, test_0FB7F6) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR16
    ISA_SET: I386
    SHORT: movzx esi, si
  */
  ASSERT_EQ(3, ild(u"☼╖÷"));
}

TEST(x86ild, test_0FB7840000000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx eax, word ptr [rax+rax*1]
  */
  ASSERT_EQ(8, ild(u"☼╖ä     "));
}

TEST(x86ild, test_0FB67D00) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edi, byte ptr [rbp]
  */
  ASSERT_EQ(4, ild(u"☼╢} "));
}

TEST(x86ild, test_0FB64B00) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ecx, byte ptr [rbx]
  */
  ASSERT_EQ(4, ild(u"☼╢K "));
}

TEST(x86ild, test_0FB64300) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rbx]
  */
  ASSERT_EQ(4, ild(u"☼╢C "));
}

TEST(x86ild, test_0FB64100) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rcx]
  */
  ASSERT_EQ(4, ild(u"☼╢A "));
}

TEST(x86ild, test_F3AB) {
  /*
    ICLASS: REP_STOSD
    CATEGORY: STRINGOP
    EXTENSION: BASE
    IFORM: REP_STOSD
    ISA_SET: I386
    SHORT: rep stosd dword ptr [rdi]
  */
  ASSERT_EQ(2, ild(u"≤½"));
}

TEST(x86ild, test_480FBE04D500000000) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx rax, byte ptr [rdx*8]
  */
  ASSERT_EQ(9, ild(u"H☼╛♦╒    "));
}

TEST(x86ild, test_400FB6CD) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx ecx, bpl
  */
  ASSERT_EQ(4, ild(u"@☼╢═"));
}

TEST(x86ild, test_400FB6C7) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx eax, dil
  */
  ASSERT_EQ(4, ild(u"@☼╢╟"));
}

TEST(x86ild, test_0FBE4E00) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx ecx, byte ptr [rsi]
  */
  ASSERT_EQ(4, ild(u"☼╛N "));
}

TEST(x86ild, test_0FBE0CD500000000) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx ecx, byte ptr [rdx*8]
  */
  ASSERT_EQ(8, ild(u"☼╛♀╒    "));
}

TEST(x86ild, test_0FB7C0) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR16
    ISA_SET: I386
    SHORT: movzx eax, ax
  */
  ASSERT_EQ(3, ild(u"☼╖└"));
}

TEST(x86ild, test_0FB6CA) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx ecx, dl
  */
  ASSERT_EQ(3, ild(u"☼╢╩"));
}

TEST(x86ild, test_0FB68200000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rdx]
  */
  ASSERT_EQ(7, ild(u"☼╢é    "));
}

TEST(x86ild, test_0FB67300) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx esi, byte ptr [rbx]
  */
  ASSERT_EQ(4, ild(u"☼╢s "));
}

TEST(x86ild, test_0FB65300) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edx, byte ptr [rbx]
  */
  ASSERT_EQ(4, ild(u"☼╢S "));
}

TEST(x86ild, test_450FB6C0) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx r8d, r8b
  */
  ASSERT_EQ(4, ild(u"E☼╢└"));
}

TEST(x86ild, test_0FBE8100000000) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx eax, byte ptr [rcx]
  */
  ASSERT_EQ(7, ild(u"☼╛ü    "));
}

TEST(x86ild, test_0FBE4F00) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx ecx, byte ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"☼╛O "));
}

TEST(x86ild, test_0FBE4700) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx eax, byte ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"☼╛G "));
}

TEST(x86ild, test_0FBE049500000000) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx eax, byte ptr [rdx*4]
  */
  ASSERT_EQ(8, ild(u"☼╛♦ò    "));
}

TEST(x86ild, test_0FB7C7) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR16
    ISA_SET: I386
    SHORT: movzx eax, di
  */
  ASSERT_EQ(3, ild(u"☼╖╟"));
}

TEST(x86ild, test_0FB75700) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx edx, word ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"☼╖W "));
}

TEST(x86ild, test_0FB6742400) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx esi, byte ptr [rsp]
  */
  ASSERT_EQ(5, ild(u"☼╢t$ "));
}

TEST(x86ild, test_0FB630) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx esi, byte ptr [rax]
  */
  ASSERT_EQ(3, ild(u"☼╢0"));
}

TEST(x86ild, test_0FB62F) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ebp, byte ptr [rdi]
  */
  ASSERT_EQ(3, ild(u"☼╢/"));
}

TEST(x86ild, test_0FB600) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rax]
  */
  ASSERT_EQ(3, ild(u"☼╢ "));
}

TEST(x86ild, test_66400FBEC7) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movsx ax, dil
  */
  ASSERT_EQ(5, ild(u"f@☼╛╟"));
}

TEST(x86ild, test_480FBFF6) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_GPR16
    ISA_SET: I386
    SHORT: movsx rsi, si
  */
  ASSERT_EQ(4, ild(u"H☼┐÷"));
}

TEST(x86ild, test_480FBFC7) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_GPR16
    ISA_SET: I386
    SHORT: movsx rax, di
  */
  ASSERT_EQ(4, ild(u"H☼┐╟"));
}

TEST(x86ild, test_480FBF12) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movsx rdx, word ptr [rdx]
  */
  ASSERT_EQ(4, ild(u"H☼┐↕"));
}

TEST(x86ild, test_480FBEF6) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movsx rsi, sil
  */
  ASSERT_EQ(4, ild(u"H☼╛÷"));
}

TEST(x86ild, test_480FBEC7) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movsx rax, dil
  */
  ASSERT_EQ(4, ild(u"H☼╛╟"));
}

TEST(x86ild, test_480FBE12) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx rdx, byte ptr [rdx]
  */
  ASSERT_EQ(4, ild(u"H☼╛↕"));
}

TEST(x86ild, test_480FA3D1) {
  /*
    ICLASS: BT
    CATEGORY: BITBYTE
    EXTENSION: BASE
    IFORM: BT_GPRv_GPRv
    ISA_SET: I386
    SHORT: bt rcx, rdx
  */
  ASSERT_EQ(4, ild(u"H☼ú╤"));
}

TEST(x86ild, test_480FA3D0) {
  /*
    ICLASS: BT
    CATEGORY: BITBYTE
    EXTENSION: BASE
    IFORM: BT_GPRv_GPRv
    ISA_SET: I386
    SHORT: bt rax, rdx
  */
  ASSERT_EQ(4, ild(u"H☼ú╨"));
}

TEST(x86ild, test_480FA3CE) {
  /*
    ICLASS: BT
    CATEGORY: BITBYTE
    EXTENSION: BASE
    IFORM: BT_GPRv_GPRv
    ISA_SET: I386
    SHORT: bt rsi, rcx
  */
  ASSERT_EQ(4, ild(u"H☼ú╬"));
}

TEST(x86ild, test_450FB7C0) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR16
    ISA_SET: I386
    SHORT: movzx r8d, r8w
  */
  ASSERT_EQ(4, ild(u"E☼╖└"));
}

TEST(x86ild, test_450FB67500) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r14d, byte ptr [r13]
  */
  ASSERT_EQ(5, ild(u"E☼╢u "));
}

TEST(x86ild, test_450FB63424) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r14d, byte ptr [r12]
  */
  ASSERT_EQ(5, ild(u"E☼╢4$"));
}

TEST(x86ild, test_450FB60C00) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r9d, byte ptr [r8+rax*1]
  */
  ASSERT_EQ(5, ild(u"E☼╢♀ "));
}

TEST(x86ild, test_440FB66B00) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r13d, byte ptr [rbx]
  */
  ASSERT_EQ(5, ild(u"D☼╢k "));
}

TEST(x86ild, test_440FB64700) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r8d, byte ptr [rdi]
  */
  ASSERT_EQ(5, ild(u"D☼╢G "));
}

TEST(x86ild, test_440FB62F) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r13d, byte ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"D☼╢/"));
}

TEST(x86ild, test_440FB62C08) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r13d, byte ptr [rax+rcx*1]
  */
  ASSERT_EQ(5, ild(u"D☼╢,◘"));
}

TEST(x86ild, test_440FB627) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r12d, byte ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"D☼╢‘"));
}

TEST(x86ild, test_440FB62430) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r12d, byte ptr [rax+rsi*1]
  */
  ASSERT_EQ(5, ild(u"D☼╢$0"));
}

TEST(x86ild, test_440FB620) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r12d, byte ptr [rax]
  */
  ASSERT_EQ(4, ild(u"D☼╢ "));
}

TEST(x86ild, test_440FB607) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx r8d, byte ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"D☼╢•"));
}

TEST(x86ild, test_410FB7442400) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx eax, word ptr [r12]
  */
  ASSERT_EQ(6, ild(u"A☼╖D$ "));
}

TEST(x86ild, test_410FB67C2C00) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edi, byte ptr [r12+rbp*1]
  */
  ASSERT_EQ(6, ild(u"A☼╢|, "));
}

TEST(x86ild, test_410FB6442400) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [r12]
  */
  ASSERT_EQ(6, ild(u"A☼╢D$ "));
}

TEST(x86ild, test_410FB63400) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx esi, byte ptr [r8+rax*1]
  */
  ASSERT_EQ(5, ild(u"A☼╢4 "));
}

TEST(x86ild, test_410FB62C10) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ebp, byte ptr [r8+rdx*1]
  */
  ASSERT_EQ(5, ild(u"A☼╢,►"));
}

TEST(x86ild, test_410F95C4) {
  /*
    ICLASS: SETNZ
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETNZ_GPR8
    ISA_SET: I386
    SHORT: setnz r12b
  */
  ASSERT_EQ(4, ild(u"A☼ò─"));
}

TEST(x86ild, test_410F94C0) {
  /*
    ICLASS: SETZ
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETZ_GPR8
    ISA_SET: I386
    SHORT: setz r8b
  */
  ASSERT_EQ(4, ild(u"A☼ö└"));
}

TEST(x86ild, test_400FBEC7) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movsx eax, dil
  */
  ASSERT_EQ(4, ild(u"@☼╛╟"));
}

TEST(x86ild, test_400FB6FF) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx edi, dil
  */
  ASSERT_EQ(4, ild(u"@☼╢λ"));
}

TEST(x86ild, test_400FB6CE) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx ecx, sil
  */
  ASSERT_EQ(4, ild(u"@☼╢╬"));
}

TEST(x86ild, test_400F9EC5) {
  /*
    ICLASS: SETLE
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETLE_GPR8
    ISA_SET: I386
    SHORT: setle bpl
  */
  ASSERT_EQ(4, ild(u"@☼€┼"));
}

TEST(x86ild, test_400F94C7) {
  /*
    ICLASS: SETZ
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETZ_GPR8
    ISA_SET: I386
    SHORT: setz dil
  */
  ASSERT_EQ(4, ild(u"@☼ö╟"));
}

TEST(x86ild, test_400F94C6) {
  /*
    ICLASS: SETZ
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETZ_GPR8
    ISA_SET: I386
    SHORT: setz sil
  */
  ASSERT_EQ(4, ild(u"@☼ö╞"));
}

TEST(x86ild, test_0FBFF8) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_GPR16
    ISA_SET: I386
    SHORT: movsx edi, ax
  */
  ASSERT_EQ(3, ild(u"☼┐°"));
}

TEST(x86ild, test_0FBFC7) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_GPR16
    ISA_SET: I386
    SHORT: movsx eax, di
  */
  ASSERT_EQ(3, ild(u"☼┐╟"));
}

TEST(x86ild, test_0FBEF8) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movsx edi, al
  */
  ASSERT_EQ(3, ild(u"☼╛°"));
}

TEST(x86ild, test_0FBE4A00) {
  /*
    ICLASS: MOVSX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVSX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movsx ecx, byte ptr [rdx]
  */
  ASSERT_EQ(4, ild(u"☼╛J "));
}

TEST(x86ild, test_0FB7FA) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR16
    ISA_SET: I386
    SHORT: movzx edi, dx
  */
  ASSERT_EQ(3, ild(u"☼╖·"));
}

TEST(x86ild, test_0FB7D2) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR16
    ISA_SET: I386
    SHORT: movzx edx, dx
  */
  ASSERT_EQ(3, ild(u"☼╖╥"));
}

TEST(x86ild, test_0FB7742400) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx esi, word ptr [rsp]
  */
  ASSERT_EQ(5, ild(u"☼╖t$ "));
}

TEST(x86ild, test_0FB75000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx edx, word ptr [rax]
  */
  ASSERT_EQ(4, ild(u"☼╖P "));
}

TEST(x86ild, test_0FB74600) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx eax, word ptr [rsi]
  */
  ASSERT_EQ(4, ild(u"☼╖F "));
}

TEST(x86ild, test_0FB74200) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx eax, word ptr [rdx]
  */
  ASSERT_EQ(4, ild(u"☼╖B "));
}

TEST(x86ild, test_0FB739) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx edi, word ptr [rcx]
  */
  ASSERT_EQ(3, ild(u"☼╖9"));
}

TEST(x86ild, test_0FB700) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMw
    ISA_SET: I386
    SHORT: movzx eax, word ptr [rax]
  */
  ASSERT_EQ(3, ild(u"☼╖ "));
}

TEST(x86ild, test_0FB6F9) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx edi, cl
  */
  ASSERT_EQ(3, ild(u"☼╢∙"));
}

TEST(x86ild, test_0FB6F2) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx esi, dl
  */
  ASSERT_EQ(3, ild(u"☼╢≥"));
}

TEST(x86ild, test_0FB6F1) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx esi, cl
  */
  ASSERT_EQ(3, ild(u"☼╢±"));
}

TEST(x86ild, test_0FB6F0) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_GPR8
    ISA_SET: I386
    SHORT: movzx esi, al
  */
  ASSERT_EQ(3, ild(u"☼╢≡"));
}

TEST(x86ild, test_0FB69C2400000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ebx, byte ptr [rsp]
  */
  ASSERT_EQ(8, ild(u"☼╢£$    "));
}

TEST(x86ild, test_0FB6941100000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edx, byte ptr [rcx+rdx*1]
  */
  ASSERT_EQ(8, ild(u"☼╢ö◄    "));
}

TEST(x86ild, test_0FB68C1000000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ecx, byte ptr [rax+rdx*1]
  */
  ASSERT_EQ(8, ild(u"☼╢î►    "));
}

TEST(x86ild, test_0FB67B00) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edi, byte ptr [rbx]
  */
  ASSERT_EQ(4, ild(u"☼╢{ "));
}

TEST(x86ild, test_0FB65F00) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ebx, byte ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"☼╢_ "));
}

TEST(x86ild, test_0FB65800) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ebx, byte ptr [rax]
  */
  ASSERT_EQ(4, ild(u"☼╢X "));
}

TEST(x86ild, test_0FB65200) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edx, byte ptr [rdx]
  */
  ASSERT_EQ(4, ild(u"☼╢R "));
}

TEST(x86ild, test_0FB65000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edx, byte ptr [rax]
  */
  ASSERT_EQ(4, ild(u"☼╢P "));
}

TEST(x86ild, test_0FB64600) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rsi]
  */
  ASSERT_EQ(4, ild(u"☼╢F "));
}

TEST(x86ild, test_0FB6443700) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rdi+rsi*1]
  */
  ASSERT_EQ(5, ild(u"☼╢D7 "));
}

TEST(x86ild, test_0FB63F) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edi, byte ptr [rdi]
  */
  ASSERT_EQ(3, ild(u"☼╢⁇"));
}

TEST(x86ild, test_0FB63C08) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edi, byte ptr [rax+rcx*1]
  */
  ASSERT_EQ(4, ild(u"☼╢<◘"));
}

TEST(x86ild, test_0FB63430) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx esi, byte ptr [rax+rsi*1]
  */
  ASSERT_EQ(4, ild(u"☼╢40"));
}

TEST(x86ild, test_0FB610) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx edx, byte ptr [rax]
  */
  ASSERT_EQ(3, ild(u"☼╢►"));
}

TEST(x86ild, test_0FB60F) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ecx, byte ptr [rdi]
  */
  ASSERT_EQ(3, ild(u"☼╢☼"));
}

TEST(x86ild, test_0FB60CD500000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ecx, byte ptr [rdx*8]
  */
  ASSERT_EQ(8, ild(u"☼╢♀╒    "));
}

TEST(x86ild, test_0FB60C08) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ecx, byte ptr [rax+rcx*1]
  */
  ASSERT_EQ(4, ild(u"☼╢♀◘"));
}

TEST(x86ild, test_0FB608) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx ecx, byte ptr [rax]
  */
  ASSERT_EQ(3, ild(u"☼╢◘"));
}

TEST(x86ild, test_0FB607) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rdi]
  */
  ASSERT_EQ(3, ild(u"☼╢•"));
}

TEST(x86ild, test_0FB6049500000000) {
  /*
    ICLASS: MOVZX
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOVZX_GPRv_MEMb
    ISA_SET: I386
    SHORT: movzx eax, byte ptr [rdx*4]
  */
  ASSERT_EQ(8, ild(u"☼╢♦ò    "));
}

TEST(x86ild, test_0FA3D0) {
  /*
    ICLASS: BT
    CATEGORY: BITBYTE
    EXTENSION: BASE
    IFORM: BT_GPRv_GPRv
    ISA_SET: I386
    SHORT: bt eax, edx
  */
  ASSERT_EQ(3, ild(u"☼ú╨"));
}

TEST(x86ild, test_0F9CC1) {
  /*
    ICLASS: SETL
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETL_GPR8
    ISA_SET: I386
    SHORT: setl cl
  */
  ASSERT_EQ(3, ild(u"☼£┴"));
}

TEST(x86ild, test_0F97C1) {
  /*
    ICLASS: SETNBE
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETNBE_GPR8
    ISA_SET: I386
    SHORT: setnbe cl
  */
  ASSERT_EQ(3, ild(u"☼ù┴"));
}

TEST(x86ild, test_0F96C2) {
  /*
    ICLASS: SETBE
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETBE_GPR8
    ISA_SET: I386
    SHORT: setbe dl
  */
  ASSERT_EQ(3, ild(u"☼û┬"));
}

TEST(x86ild, test_0F95C1) {
  /*
    ICLASS: SETNZ
    CATEGORY: SETCC
    EXTENSION: BASE
    IFORM: SETNZ_GPR8
    ISA_SET: I386
    SHORT: setnz cl
  */
  ASSERT_EQ(3, ild(u"☼ò┴"));
}
