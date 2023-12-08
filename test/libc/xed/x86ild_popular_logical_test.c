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

TEST(x86ild, test_85C0) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test eax, eax
  */
  ASSERT_EQ(2, ild(u"à└"));
}

TEST(x86ild, test_31D2) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor edx, edx
  */
  ASSERT_EQ(2, ild(u"1╥"));
}

TEST(x86ild, test_81E600000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and esi, 0x0
  */
  ASSERT_EQ(6, ild(u"üμ    "));
}

TEST(x86ild, test_31C0) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor eax, eax
  */
  ASSERT_EQ(2, ild(u"1└"));
}

TEST(x86ild, test_83E100) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and ecx, 0x0
  */
  ASSERT_EQ(3, ild(u"âß "));
}

TEST(x86ild, test_83E000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and eax, 0x0
  */
  ASSERT_EQ(3, ild(u"âα "));
}

TEST(x86ild, test_83E200) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and edx, 0x0
  */
  ASSERT_EQ(3, ild(u"âΓ "));
}

TEST(x86ild, test_F6430000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMb_IMMb_F6r0
    ISA_SET: I86
    SHORT: test byte ptr [rbx], 0x0
  */
  ASSERT_EQ(4, ild(u"÷C  "));
}

TEST(x86ild, test_66250000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_OrAX_IMMz
    ISA_SET: I86
    SHORT: and ax, 0x0
  */
  ASSERT_EQ(4, ild(u"f%  "));
}

TEST(x86ild, test_2500000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_OrAX_IMMz
    ISA_SET: I86
    SHORT: and eax, 0x0
  */
  ASSERT_EQ(5, ild(u"%    "));
}

TEST(x86ild, test_09D0) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or eax, edx
  */
  ASSERT_EQ(2, ild(u"○╨"));
}

TEST(x86ild, test_48234300) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_MEMv
    ISA_SET: I86
    SHORT: and rax, qword ptr [rbx]
  */
  ASSERT_EQ(4, ild(u"H#C "));
}

TEST(x86ild, test_83C800) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_IMMb
    ISA_SET: I86
    SHORT: or eax, 0x0
  */
  ASSERT_EQ(3, ild(u"â╚ "));
}

TEST(x86ild, test_0C00) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_AL_IMMb
    ISA_SET: I86
    SHORT: or al, 0x0
  */
  ASSERT_EQ(2, ild(u"♀ "));
}

TEST(x86ild, test_31C9) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor ecx, ecx
  */
  ASSERT_EQ(2, ild(u"1╔"));
}

TEST(x86ild, test_804F0000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_MEMb_IMMb_80r1
    ISA_SET: I86
    SHORT: or byte ptr [rdi], 0x0
  */
  ASSERT_EQ(4, ild(u"ÇO  "));
}

TEST(x86ild, test_09F1) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or ecx, esi
  */
  ASSERT_EQ(2, ild(u"○±"));
}

TEST(x86ild, test_6681E20000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and dx, 0x0
  */
  ASSERT_EQ(5, ild(u"füΓ  "));
}

TEST(x86ild, test_09C8) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or eax, ecx
  */
  ASSERT_EQ(2, ild(u"○╚"));
}

TEST(x86ild, test_83E600) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and esi, 0x0
  */
  ASSERT_EQ(3, ild(u"âμ "));
}

TEST(x86ild, test_80CC00) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPR8_IMMb_80r1
    ISA_SET: I86
    SHORT: or ah, 0x0
  */
  ASSERT_EQ(3, ild(u"Ç╠ "));
}

TEST(x86ild, test_09C2) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or edx, eax
  */
  ASSERT_EQ(2, ild(u"○┬"));
}

TEST(x86ild, test_6681E60000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and si, 0x0
  */
  ASSERT_EQ(5, ild(u"füμ  "));
}

TEST(x86ild, test_660D0000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_OrAX_IMMz
    ISA_SET: I86
    SHORT: or ax, 0x0
  */
  ASSERT_EQ(4, ild(u"f♪  "));
}

TEST(x86ild, test_09C6) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or esi, eax
  */
  ASSERT_EQ(2, ild(u"○╞"));
}

TEST(x86ild, test_83CA00) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_IMMb
    ISA_SET: I86
    SHORT: or edx, 0x0
  */
  ASSERT_EQ(3, ild(u"â╩ "));
}

TEST(x86ild, test_6681E10000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and cx, 0x0
  */
  ASSERT_EQ(5, ild(u"füß  "));
}

TEST(x86ild, test_80E200) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPR8_IMMb_80r4
    ISA_SET: I86
    SHORT: and dl, 0x0
  */
  ASSERT_EQ(3, ild(u"ÇΓ "));
}

TEST(x86ild, test_09FE) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or esi, edi
  */
  ASSERT_EQ(2, ild(u"○■"));
}

TEST(x86ild, test_F6470000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMb_IMMb_F6r0
    ISA_SET: I86
    SHORT: test byte ptr [rdi], 0x0
  */
  ASSERT_EQ(4, ild(u"÷G  "));
}

TEST(x86ild, test_4885C0) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test rax, rax
  */
  ASSERT_EQ(3, ild(u"Hà└"));
}

TEST(x86ild, test_83F200) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_IMMb
    ISA_SET: I86
    SHORT: xor edx, 0x0
  */
  ASSERT_EQ(3, ild(u"â≥ "));
}

TEST(x86ild, test_0D00000000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_OrAX_IMMz
    ISA_SET: I86
    SHORT: or eax, 0x0
  */
  ASSERT_EQ(5, ild(u"♪    "));
}

TEST(x86ild, test_84C0) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPR8_GPR8
    ISA_SET: I86
    SHORT: test al, al
  */
  ASSERT_EQ(2, ild(u"ä└"));
}

TEST(x86ild, test_A800) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_AL_IMMb
    ISA_SET: I86
    SHORT: test al, 0x0
  */
  ASSERT_EQ(2, ild(u"¿ "));
}

TEST(x86ild, test_81E200000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and edx, 0x0
  */
  ASSERT_EQ(6, ild(u"üΓ    "));
}

TEST(x86ild, test_F6450000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMb_IMMb_F6r0
    ISA_SET: I86
    SHORT: test byte ptr [rbp], 0x0
  */
  ASSERT_EQ(4, ild(u"÷E  "));
}

TEST(x86ild, test_80670000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_MEMb_IMMb_80r4
    ISA_SET: I86
    SHORT: and byte ptr [rdi], 0x0
  */
  ASSERT_EQ(4, ild(u"Çg  "));
}

TEST(x86ild, test_4885D2) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test rdx, rdx
  */
  ASSERT_EQ(3, ild(u"Hà╥"));
}

TEST(x86ild, test_83F100) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_IMMb
    ISA_SET: I86
    SHORT: xor ecx, 0x0
  */
  ASSERT_EQ(3, ild(u"â± "));
}

TEST(x86ild, test_81E100000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and ecx, 0x0
  */
  ASSERT_EQ(6, ild(u"üß    "));
}

TEST(x86ild, test_80E100) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPR8_IMMb_80r4
    ISA_SET: I86
    SHORT: and cl, 0x0
  */
  ASSERT_EQ(3, ild(u"Çß "));
}

TEST(x86ild, test_84D2) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPR8_GPR8
    ISA_SET: I86
    SHORT: test dl, dl
  */
  ASSERT_EQ(2, ild(u"ä╥"));
}

TEST(x86ild, test_6681CA0000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_IMMz
    ISA_SET: I86
    SHORT: or dx, 0x0
  */
  ASSERT_EQ(5, ild(u"fü╩  "));
}

TEST(x86ild, test_09C1) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or ecx, eax
  */
  ASSERT_EQ(2, ild(u"○┴"));
}

TEST(x86ild, test_804B0000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_MEMb_IMMb_80r1
    ISA_SET: I86
    SHORT: or byte ptr [rbx], 0x0
  */
  ASSERT_EQ(4, ild(u"ÇK  "));
}

TEST(x86ild, test_4885FF) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test rdi, rdi
  */
  ASSERT_EQ(3, ild(u"Hàλ"));
}

TEST(x86ild, test_85F6) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test esi, esi
  */
  ASSERT_EQ(2, ild(u"à÷"));
}

TEST(x86ild, test_4585ED) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test r13d, r13d
  */
  ASSERT_EQ(3, ild(u"Eàφ"));
}

TEST(x86ild, test_84C9) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPR8_GPR8
    ISA_SET: I86
    SHORT: test cl, cl
  */
  ASSERT_EQ(2, ild(u"ä╔"));
}

TEST(x86ild, test_83F000) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_IMMb
    ISA_SET: I86
    SHORT: xor eax, 0x0
  */
  ASSERT_EQ(3, ild(u"â≡ "));
}

TEST(x86ild, test_09F2) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or edx, esi
  */
  ASSERT_EQ(2, ild(u"○≥"));
}

TEST(x86ild, test_4183E000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and r8d, 0x0
  */
  ASSERT_EQ(4, ild(u"Aâα "));
}

TEST(x86ild, test_85D2) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test edx, edx
  */
  ASSERT_EQ(2, ild(u"à╥"));
}

TEST(x86ild, test_83E700) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and edi, 0x0
  */
  ASSERT_EQ(3, ild(u"âτ "));
}

TEST(x86ild, test_31F6) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor esi, esi
  */
  ASSERT_EQ(2, ild(u"1÷"));
}

TEST(x86ild, test_668167000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_MEMv_IMMz
    ISA_SET: I86
    SHORT: and word ptr [rdi], 0x0
  */
  ASSERT_EQ(6, ild(u"füg   "));
}

TEST(x86ild, test_09F0) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or eax, esi
  */
  ASSERT_EQ(2, ild(u"○≡"));
}

TEST(x86ild, test_81E700000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and edi, 0x0
  */
  ASSERT_EQ(6, ild(u"üτ    "));
}

TEST(x86ild, test_66F743000000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMv_IMMz_F7r0
    ISA_SET: I86
    SHORT: test word ptr [rbx], 0x0
  */
  ASSERT_EQ(6, ild(u"f≈C   "));
}

TEST(x86ild, test_40F6C500) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPR8_IMMb_F6r0
    ISA_SET: I86
    SHORT: test bpl, 0x0
  */
  ASSERT_EQ(4, ild(u"@÷┼ "));
}

TEST(x86ild, test_80CA00) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPR8_IMMb_80r1
    ISA_SET: I86
    SHORT: or dl, 0x0
  */
  ASSERT_EQ(3, ild(u"Ç╩ "));
}

TEST(x86ild, test_48235300) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_MEMv
    ISA_SET: I86
    SHORT: and rdx, qword ptr [rbx]
  */
  ASSERT_EQ(4, ild(u"H#S "));
}

TEST(x86ild, test_31DB) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor ebx, ebx
  */
  ASSERT_EQ(2, ild(u"1█"));
}

TEST(x86ild, test_25000083E2) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_OrAX_IMMz
    ISA_SET: I86
    SHORT: and eax, 0xe2830000
  */
  ASSERT_EQ(5, ild(u"%  âΓ"));
}

TEST(x86ild, test_24D5) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_AL_IMMb
    ISA_SET: I86
    SHORT: and al, 0xd5
  */
  ASSERT_EQ(2, ild(u"$╒"));
}

TEST(x86ild, test_F6C200) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPR8_IMMb_F6r0
    ISA_SET: I86
    SHORT: test dl, 0x0
  */
  ASSERT_EQ(3, ild(u"÷┬ "));
}

TEST(x86ild, test_80630000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_MEMb_IMMb_80r4
    ISA_SET: I86
    SHORT: and byte ptr [rbx], 0x0
  */
  ASSERT_EQ(4, ild(u"Çc  "));
}

TEST(x86ild, test_4409D0) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or eax, r10d
  */
  ASSERT_EQ(3, ild(u"D○╨"));
}

TEST(x86ild, test_4409C0) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or eax, r8d
  */
  ASSERT_EQ(3, ild(u"D○└"));
}

TEST(x86ild, test_4183F000) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_IMMb
    ISA_SET: I86
    SHORT: xor r8d, 0x0
  */
  ASSERT_EQ(4, ild(u"Aâ≡ "));
}

TEST(x86ild, test_40F6C600) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPR8_IMMb_F6r0
    ISA_SET: I86
    SHORT: test sil, 0x0
  */
  ASSERT_EQ(4, ild(u"@÷╞ "));
}

TEST(x86ild, test_4080E600) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPR8_IMMb_80r4
    ISA_SET: I86
    SHORT: and sil, 0x0
  */
  ASSERT_EQ(4, ild(u"@Çμ "));
}

TEST(x86ild, test_09CA) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or edx, ecx
  */
  ASSERT_EQ(2, ild(u"○╩"));
}

TEST(x86ild, test_85ED) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test ebp, ebp
  */
  ASSERT_EQ(2, ild(u"àφ"));
}

TEST(x86ild, test_83E500) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and ebp, 0x0
  */
  ASSERT_EQ(3, ild(u"âσ "));
}

TEST(x86ild, test_834A0000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_MEMv_IMMb
    ISA_SET: I86
    SHORT: or dword ptr [rdx], 0x0
  */
  ASSERT_EQ(4, ild(u"âJ  "));
}

TEST(x86ild, test_4183F100) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_IMMb
    ISA_SET: I86
    SHORT: xor r9d, 0x0
  */
  ASSERT_EQ(4, ild(u"Aâ± "));
}

TEST(x86ild, test_4183E200) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and r10d, 0x0
  */
  ASSERT_EQ(4, ild(u"AâΓ "));
}

TEST(x86ild, test_4180E100) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPR8_IMMb_80r4
    ISA_SET: I86
    SHORT: and r9b, 0x0
  */
  ASSERT_EQ(4, ild(u"AÇß "));
}

TEST(x86ild, test_4180E000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPR8_IMMb_80r4
    ISA_SET: I86
    SHORT: and r8b, 0x0
  */
  ASSERT_EQ(4, ild(u"AÇα "));
}

TEST(x86ild, test_85DB) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test ebx, ebx
  */
  ASSERT_EQ(2, ild(u"à█"));
}

TEST(x86ild, test_81480000000000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_MEMv_IMMz
    ISA_SET: I86
    SHORT: or dword ptr [rax], 0x0
  */
  ASSERT_EQ(7, ild(u"üH     "));
}

TEST(x86ild, test_80CE00) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPR8_IMMb_80r1
    ISA_SET: I86
    SHORT: or dh, 0x0
  */
  ASSERT_EQ(3, ild(u"Ç╬ "));
}

TEST(x86ild, test_66F747000000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMv_IMMz_F7r0
    ISA_SET: I86
    SHORT: test word ptr [rdi], 0x0
  */
  ASSERT_EQ(6, ild(u"f≈G   "));
}

TEST(x86ild, test_48F7D2) {
  /*
    ICLASS: NOT
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: NOT_GPRv
    ISA_SET: I86
    SHORT: not rdx
  */
  ASSERT_EQ(3, ild(u"H≈╥"));
}

TEST(x86ild, test_4585F6) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test r14d, r14d
  */
  ASSERT_EQ(3, ild(u"Eà÷"));
}

TEST(x86ild, test_4183E100) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and r9d, 0x0
  */
  ASSERT_EQ(4, ild(u"Aâß "));
}

TEST(x86ild, test_4084F6) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPR8_GPR8
    ISA_SET: I86
    SHORT: test sil, sil
  */
  ASSERT_EQ(3, ild(u"@ä÷"));
}

TEST(x86ild, test_31ED) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor ebp, ebp
  */
  ASSERT_EQ(2, ild(u"1φ"));
}

TEST(x86ild, test_09F8) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or eax, edi
  */
  ASSERT_EQ(2, ild(u"○°"));
}

TEST(x86ild, test_85FF) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test edi, edi
  */
  ASSERT_EQ(2, ild(u"àλ"));
}

TEST(x86ild, test_83F600) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_IMMb
    ISA_SET: I86
    SHORT: xor esi, 0x0
  */
  ASSERT_EQ(3, ild(u"â÷ "));
}

TEST(x86ild, test_81CA00000000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_IMMz
    ISA_SET: I86
    SHORT: or edx, 0x0
  */
  ASSERT_EQ(6, ild(u"ü╩    "));
}

TEST(x86ild, test_81490000000000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_MEMv_IMMz
    ISA_SET: I86
    SHORT: or dword ptr [rcx], 0x0
  */
  ASSERT_EQ(7, ild(u"üI     "));
}

TEST(x86ild, test_4D85ED) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test r13, r13
  */
  ASSERT_EQ(3, ild(u"Màφ"));
}

TEST(x86ild, test_4D85E4) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test r12, r12
  */
  ASSERT_EQ(3, ild(u"MàΣ"));
}

TEST(x86ild, test_48F7D0) {
  /*
    ICLASS: NOT
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: NOT_GPRv
    ISA_SET: I86
    SHORT: not rax
  */
  ASSERT_EQ(3, ild(u"H≈╨"));
}

TEST(x86ild, test_4885F6) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test rsi, rsi
  */
  ASSERT_EQ(3, ild(u"Hà÷"));
}

TEST(x86ild, test_4585C0) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test r8d, r8d
  */
  ASSERT_EQ(3, ild(u"Eà└"));
}

TEST(x86ild, test_4183F200) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_IMMb
    ISA_SET: I86
    SHORT: xor r10d, 0x0
  */
  ASSERT_EQ(4, ild(u"Aâ≥ "));
}

TEST(x86ild, test_31FF) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor edi, edi
  */
  ASSERT_EQ(2, ild(u"1λ"));
}

TEST(x86ild, test_2400) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_AL_IMMb
    ISA_SET: I86
    SHORT: and al, 0x0
  */
  ASSERT_EQ(2, ild(u"$ "));
}

TEST(x86ild, test_F7D1) {
  /*
    ICLASS: NOT
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: NOT_GPRv
    ISA_SET: I86
    SHORT: not ecx
  */
  ASSERT_EQ(2, ild(u"≈╤"));
}

TEST(x86ild, test_F7430000000000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMv_IMMz_F7r0
    ISA_SET: I86
    SHORT: test dword ptr [rbx], 0x0
  */
  ASSERT_EQ(7, ild(u"≈C     "));
}

TEST(x86ild, test_83C900) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_IMMb
    ISA_SET: I86
    SHORT: or ecx, 0x0
  */
  ASSERT_EQ(3, ild(u"â╔ "));
}

TEST(x86ild, test_814E0000000000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_MEMv_IMMz
    ISA_SET: I86
    SHORT: or dword ptr [rsi], 0x0
  */
  ASSERT_EQ(7, ild(u"üN     "));
}

TEST(x86ild, test_80C900) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPR8_IMMb_80r1
    ISA_SET: I86
    SHORT: or cl, 0x0
  */
  ASSERT_EQ(3, ild(u"Ç╔ "));
}

TEST(x86ild, test_668163000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_MEMv_IMMz
    ISA_SET: I86
    SHORT: and word ptr [rbx], 0x0
  */
  ASSERT_EQ(6, ild(u"füc   "));
}

TEST(x86ild, test_4883C900) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_IMMb
    ISA_SET: I86
    SHORT: or rcx, 0x0
  */
  ASSERT_EQ(4, ild(u"Hâ╔ "));
}

TEST(x86ild, test_4585E4) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test r12d, r12d
  */
  ASSERT_EQ(3, ild(u"EàΣ"));
}

TEST(x86ild, test_4531F6) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor r14d, r14d
  */
  ASSERT_EQ(3, ild(u"E1÷"));
}

TEST(x86ild, test_4531ED) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor r13d, r13d
  */
  ASSERT_EQ(3, ild(u"E1φ"));
}

TEST(x86ild, test_4531E4) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor r12d, r12d
  */
  ASSERT_EQ(3, ild(u"E1Σ"));
}

TEST(x86ild, test_4531D2) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor r10d, r10d
  */
  ASSERT_EQ(3, ild(u"E1╥"));
}

TEST(x86ild, test_4531C9) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor r9d, r9d
  */
  ASSERT_EQ(3, ild(u"E1╔"));
}

TEST(x86ild, test_4531C0) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor r8d, r8d
  */
  ASSERT_EQ(3, ild(u"E1└"));
}

TEST(x86ild, test_4409D8) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or eax, r11d
  */
  ASSERT_EQ(3, ild(u"D○╪"));
}

TEST(x86ild, test_4409C7) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or edi, r8d
  */
  ASSERT_EQ(3, ild(u"D○╟"));
}

TEST(x86ild, test_4183E400) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and r12d, 0x0
  */
  ASSERT_EQ(4, ild(u"AâΣ "));
}

TEST(x86ild, test_4181E600000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and r14d, 0x0
  */
  ASSERT_EQ(7, ild(u"Aüμ    "));
}

TEST(x86ild, test_4180E200) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPR8_IMMb_80r4
    ISA_SET: I86
    SHORT: and r10b, 0x0
  */
  ASSERT_EQ(4, ild(u"AÇΓ "));
}

TEST(x86ild, test_4109C0) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or r8d, eax
  */
  ASSERT_EQ(3, ild(u"A○└"));
}

TEST(x86ild, test_21C2) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_GPRv_21
    ISA_SET: I86
    SHORT: and edx, eax
  */
  ASSERT_EQ(2, ild(u"!┬"));
}

TEST(x86ild, test_09FA) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or edx, edi
  */
  ASSERT_EQ(2, ild(u"○·"));
}

TEST(x86ild, test_09E8) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or eax, ebp
  */
  ASSERT_EQ(2, ild(u"○Φ"));
}

TEST(x86ild, test_F7470000000000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMv_IMMz_F7r0
    ISA_SET: I86
    SHORT: test dword ptr [rdi], 0x0
  */
  ASSERT_EQ(7, ild(u"≈G     "));
}

TEST(x86ild, test_F7400000000000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMv_IMMz_F7r0
    ISA_SET: I86
    SHORT: test dword ptr [rax], 0x0
  */
  ASSERT_EQ(7, ild(u"≈@     "));
}

TEST(x86ild, test_F6040800) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMb_IMMb_F6r0
    ISA_SET: I86
    SHORT: test byte ptr [rax+rcx*1], 0x0
  */
  ASSERT_EQ(4, ild(u"÷♦◘ "));
}

TEST(x86ild, test_85C9) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test ecx, ecx
  */
  ASSERT_EQ(2, ild(u"à╔"));
}

TEST(x86ild, test_83F700) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_IMMb
    ISA_SET: I86
    SHORT: xor edi, 0x0
  */
  ASSERT_EQ(3, ild(u"â≈ "));
}

TEST(x86ild, test_83490000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_MEMv_IMMb
    ISA_SET: I86
    SHORT: or dword ptr [rcx], 0x0
  */
  ASSERT_EQ(4, ild(u"âI  "));
}

TEST(x86ild, test_81670000000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_MEMv_IMMz
    ISA_SET: I86
    SHORT: and dword ptr [rdi], 0x0
  */
  ASSERT_EQ(7, ild(u"üg     "));
}

TEST(x86ild, test_81630000000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_MEMv_IMMz
    ISA_SET: I86
    SHORT: and dword ptr [rbx], 0x0
  */
  ASSERT_EQ(7, ild(u"üc     "));
}

TEST(x86ild, test_66A90000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_OrAX_IMMz
    ISA_SET: I86
    SHORT: test ax, 0x0
  */
  ASSERT_EQ(4, ild(u"f⌐  "));
}

TEST(x86ild, test_6685F6) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test si, si
  */
  ASSERT_EQ(3, ild(u"fà÷"));
}

TEST(x86ild, test_6685D2) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test dx, dx
  */
  ASSERT_EQ(3, ild(u"fà╥"));
}

TEST(x86ild, test_6685C0) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test ax, ax
  */
  ASSERT_EQ(3, ild(u"fà└"));
}

TEST(x86ild, test_6681E70000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and di, 0x0
  */
  ASSERT_EQ(5, ild(u"füτ  "));
}

TEST(x86ild, test_66350000) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_OrAX_IMMz
    ISA_SET: I86
    SHORT: xor ax, 0x0
  */
  ASSERT_EQ(4, ild(u"f5  "));
}

TEST(x86ild, test_4D85D2) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test r10, r10
  */
  ASSERT_EQ(3, ild(u"Mà╥"));
}

TEST(x86ild, test_4D21E8) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_GPRv_21
    ISA_SET: I86
    SHORT: and r8, r13
  */
  ASSERT_EQ(3, ild(u"M!Φ"));
}

TEST(x86ild, test_4C31C0) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor rax, r8
  */
  ASSERT_EQ(3, ild(u"L1└"));
}

TEST(x86ild, test_4921C2) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_GPRv_21
    ISA_SET: I86
    SHORT: and r10, rax
  */
  ASSERT_EQ(3, ild(u"I!┬"));
}

TEST(x86ild, test_48F7D1) {
  /*
    ICLASS: NOT
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: NOT_GPRv
    ISA_SET: I86
    SHORT: not rcx
  */
  ASSERT_EQ(3, ild(u"H≈╤"));
}

TEST(x86ild, test_48859000000000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMv_GPRv
    ISA_SET: I86
    SHORT: test qword ptr [rax], rdx
  */
  ASSERT_EQ(7, ild(u"HàÉ    "));
}

TEST(x86ild, test_48854200) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMv_GPRv
    ISA_SET: I86
    SHORT: test qword ptr [rdx], rax
  */
  ASSERT_EQ(4, ild(u"HàB "));
}

TEST(x86ild, test_488514C7) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMv_GPRv
    ISA_SET: I86
    SHORT: test qword ptr [rdi+rax*8], rdx
  */
  ASSERT_EQ(4, ild(u"Hà¶╟"));
}

TEST(x86ild, test_488504D500000000) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_MEMv_GPRv
    ISA_SET: I86
    SHORT: test qword ptr [rdx*8], rax
  */
  ASSERT_EQ(8, ild(u"Hà♦╒    "));
}

TEST(x86ild, test_4883E400) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and rsp, 0x0
  */
  ASSERT_EQ(4, ild(u"HâΣ "));
}

TEST(x86ild, test_4883CB00) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_IMMb
    ISA_SET: I86
    SHORT: or rbx, 0x0
  */
  ASSERT_EQ(4, ild(u"Hâ╦ "));
}

TEST(x86ild, test_4883CA00) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_IMMb
    ISA_SET: I86
    SHORT: or rdx, 0x0
  */
  ASSERT_EQ(4, ild(u"Hâ╩ "));
}

TEST(x86ild, test_4831ED) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor rbp, rbp
  */
  ASSERT_EQ(3, ild(u"H1φ"));
}

TEST(x86ild, test_48234B00) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_MEMv
    ISA_SET: I86
    SHORT: and rcx, qword ptr [rbx]
  */
  ASSERT_EQ(4, ild(u"H#K "));
}

TEST(x86ild, test_482306) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_MEMv
    ISA_SET: I86
    SHORT: and rax, qword ptr [rsi]
  */
  ASSERT_EQ(3, ild(u"H#♠"));
}

TEST(x86ild, test_4821F8) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_GPRv_21
    ISA_SET: I86
    SHORT: and rax, rdi
  */
  ASSERT_EQ(3, ild(u"H!°"));
}

TEST(x86ild, test_480B5700) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_MEMv
    ISA_SET: I86
    SHORT: or rdx, qword ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"H♂W "));
}

TEST(x86ild, test_4809D0) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or rax, rdx
  */
  ASSERT_EQ(3, ild(u"H○╨"));
}

TEST(x86ild, test_4585FF) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPRv_GPRv
    ISA_SET: I86
    SHORT: test r15d, r15d
  */
  ASSERT_EQ(3, ild(u"Eàλ"));
}

TEST(x86ild, test_4584D2) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPR8_GPR8
    ISA_SET: I86
    SHORT: test r10b, r10b
  */
  ASSERT_EQ(3, ild(u"Eä╥"));
}

TEST(x86ild, test_4531FF) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor r15d, r15d
  */
  ASSERT_EQ(3, ild(u"E1λ"));
}

TEST(x86ild, test_4531DB) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor r11d, r11d
  */
  ASSERT_EQ(3, ild(u"E1█"));
}

TEST(x86ild, test_4509DA) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or r10d, r11d
  */
  ASSERT_EQ(3, ild(u"E○┌"));
}

TEST(x86ild, test_4509C8) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or r8d, r9d
  */
  ASSERT_EQ(3, ild(u"E○╚"));
}

TEST(x86ild, test_450801) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_MEMb_GPR8
    ISA_SET: I86
    SHORT: or byte ptr [r9], r8b
  */
  ASSERT_EQ(3, ild(u"E◘☺"));
}

TEST(x86ild, test_4431C0) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_GPRv_31
    ISA_SET: I86
    SHORT: xor eax, r8d
  */
  ASSERT_EQ(3, ild(u"D1└"));
}

TEST(x86ild, test_4421C1) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_GPRv_21
    ISA_SET: I86
    SHORT: and ecx, r8d
  */
  ASSERT_EQ(3, ild(u"D!┴"));
}

TEST(x86ild, test_4409C8) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or eax, r9d
  */
  ASSERT_EQ(3, ild(u"D○╚"));
}

TEST(x86ild, test_4183F300) {
  /*
    ICLASS: XOR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: XOR_GPRv_IMMb
    ISA_SET: I86
    SHORT: xor r11d, 0x0
  */
  ASSERT_EQ(4, ild(u"Aâ≤ "));
}

TEST(x86ild, test_4183E700) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and r15d, 0x0
  */
  ASSERT_EQ(4, ild(u"Aâτ "));
}

TEST(x86ild, test_4183E600) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and r14d, 0x0
  */
  ASSERT_EQ(4, ild(u"Aâμ "));
}

TEST(x86ild, test_4183E300) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMb
    ISA_SET: I86
    SHORT: and r11d, 0x0
  */
  ASSERT_EQ(4, ild(u"Aâπ "));
}

TEST(x86ild, test_4181E700000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and r15d, 0x0
  */
  ASSERT_EQ(7, ild(u"Aüτ    "));
}

TEST(x86ild, test_4181E500000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and r13d, 0x0
  */
  ASSERT_EQ(7, ild(u"Aüσ    "));
}

TEST(x86ild, test_4181E000000000) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_IMMz
    ISA_SET: I86
    SHORT: and r8d, 0x0
  */
  ASSERT_EQ(7, ild(u"Aüα    "));
}

TEST(x86ild, test_4109D4) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPRv_GPRv_09
    ISA_SET: I86
    SHORT: or r12d, edx
  */
  ASSERT_EQ(3, ild(u"A○╘"));
}

TEST(x86ild, test_4084FF) {
  /*
    ICLASS: TEST
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: TEST_GPR8_GPR8
    ISA_SET: I86
    SHORT: test dil, dil
  */
  ASSERT_EQ(3, ild(u"@äλ"));
}

TEST(x86ild, test_224300) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPR8_MEMb
    ISA_SET: I86
    SHORT: and al, byte ptr [rbx]
  */
  ASSERT_EQ(3, ild(u"“C "));
}

TEST(x86ild, test_21F9) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_GPRv_21
    ISA_SET: I86
    SHORT: and ecx, edi
  */
  ASSERT_EQ(2, ild(u"!∙"));
}

TEST(x86ild, test_21F8) {
  /*
    ICLASS: AND
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: AND_GPRv_GPRv_21
    ISA_SET: I86
    SHORT: and eax, edi
  */
  ASSERT_EQ(2, ild(u"!°"));
}

TEST(x86ild, test_093C8500000000) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_MEMv_GPRv
    ISA_SET: I86
    SHORT: or dword ptr [rax*4], edi
  */
  ASSERT_EQ(7, ild(u"○<à    "));
}

TEST(x86ild, test_08D1) {
  /*
    ICLASS: OR
    CATEGORY: LOGICAL
    EXTENSION: BASE
    IFORM: OR_GPR8_GPR8_08
    ISA_SET: I86
    SHORT: or cl, dl
  */
  ASSERT_EQ(2, ild(u"◘╤"));
}
