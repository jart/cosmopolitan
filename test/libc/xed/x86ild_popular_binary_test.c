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

TEST(x86ild, test_4883EC00) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_IMMb
    ISA_SET: I86
    SHORT: sub rsp, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâ∞ "));
}

TEST(x86ild, test_483D00000000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_OrAX_IMMz
    ISA_SET: I86
    SHORT: cmp rax, 0x0
  */
  EXPECT_EQ(6, ild(u"H=    "));
}

TEST(x86ild, test_48F7F6) {
  /*
    ICLASS: DIV
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DIV_GPRv
    ISA_SET: I86
    SHORT: div rsi
  */
  EXPECT_EQ(3, ild(u"H≈÷"));
}

TEST(x86ild, test_4839F1) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rcx, rsi
  */
  EXPECT_EQ(3, ild(u"H9±"));
}

TEST(x86ild, test_4839F0) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rax, rsi
  */
  EXPECT_EQ(3, ild(u"H9≡"));
}

TEST(x86ild, test_807B0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMb_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp byte ptr [rbx], 0x0
  */
  EXPECT_EQ(4, ild(u"Ç{  "));
}

TEST(x86ild, test_4839D0) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rax, rdx
  */
  EXPECT_EQ(3, ild(u"H9╨"));
}

TEST(x86ild, test_3C00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_AL_IMMb
    ISA_SET: I86
    SHORT: cmp al, 0x0
  */
  EXPECT_EQ(2, ild(u"< "));
}

TEST(x86ild, test_4883F800) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp rax, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâ° "));
}

TEST(x86ild, test_FEC8) {
  /*
    ICLASS: DEC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DEC_GPR8
    ISA_SET: I86
    SHORT: dec al
  */
  EXPECT_EQ(2, ild(u"■╚"));
}

TEST(x86ild, test_4801D0) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rax, rdx
  */
  EXPECT_EQ(3, ild(u"H☺╨"));
}

TEST(x86ild, test_83F800) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp eax, 0x0
  */
  EXPECT_EQ(3, ild(u"â° "));
}

TEST(x86ild, test_4883FA00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp rdx, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâ· "));
}

TEST(x86ild, test_3D00000000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_OrAX_IMMz
    ISA_SET: I86
    SHORT: cmp eax, 0x0
  */
  EXPECT_EQ(5, ild(u"=    "));
}

TEST(x86ild, test_6683F800) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp ax, 0x0
  */
  EXPECT_EQ(4, ild(u"fâ° "));
}

TEST(x86ild, test_80FA00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp dl, 0x0
  */
  EXPECT_EQ(3, ild(u"Ç· "));
}

TEST(x86ild, test_6683FA00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp dx, 0x0
  */
  EXPECT_EQ(4, ild(u"fâ· "));
}

TEST(x86ild, test_83FA00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp edx, 0x0
  */
  EXPECT_EQ(3, ild(u"â· "));
}

TEST(x86ild, test_663D0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_OrAX_IMMz
    ISA_SET: I86
    SHORT: cmp ax, 0x0
  */
  EXPECT_EQ(4, ild(u"f=  "));
}

TEST(x86ild, test_83FE00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp esi, 0x0
  */
  EXPECT_EQ(3, ild(u"â■ "));
}

TEST(x86ild, test_4883C400) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_IMMb
    ISA_SET: I86
    SHORT: add rsp, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâ─ "));
}

TEST(x86ild, test_01D2) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add edx, edx
  */
  EXPECT_EQ(2, ild(u"☺╥"));
}

TEST(x86ild, test_80F900) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp cl, 0x0
  */
  EXPECT_EQ(3, ild(u"Ç∙ "));
}

TEST(x86ild, test_4801C8) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rax, rcx
  */
  EXPECT_EQ(3, ild(u"H☺╚"));
}

TEST(x86ild, test_4883F900) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp rcx, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâ∙ "));
}

TEST(x86ild, test_4839C2) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rdx, rax
  */
  EXPECT_EQ(3, ild(u"H9┬"));
}

TEST(x86ild, test_01C0) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add eax, eax
  */
  EXPECT_EQ(2, ild(u"☺└"));
}

TEST(x86ild, test_83FF00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp edi, 0x0
  */
  EXPECT_EQ(3, ild(u"âλ "));
}

TEST(x86ild, test_4183FC00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp r12d, 0x0
  */
  EXPECT_EQ(4, ild(u"Aâⁿ "));
}

TEST(x86ild, test_01D0) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add eax, edx
  */
  EXPECT_EQ(2, ild(u"☺╨"));
}

TEST(x86ild, test_6683F900) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp cx, 0x0
  */
  EXPECT_EQ(4, ild(u"fâ∙ "));
}

TEST(x86ild, test_4883C300) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_IMMb
    ISA_SET: I86
    SHORT: add rbx, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâ├ "));
}

TEST(x86ild, test_0000) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_MEMb_GPR8
    ISA_SET: I86
    SHORT: add byte ptr [rax], al
  */
  EXPECT_EQ(2, ild(u"  "));
}

TEST(x86ild, test_FECA) {
  /*
    ICLASS: DEC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DEC_GPR8
    ISA_SET: I86
    SHORT: dec dl
  */
  EXPECT_EQ(2, ild(u"■╩"));
}

TEST(x86ild, test_4801C2) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rdx, rax
  */
  EXPECT_EQ(3, ild(u"H☺┬"));
}

TEST(x86ild, test_807F0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMb_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp byte ptr [rdi], 0x0
  */
  EXPECT_EQ(4, ild(u"Ç⌂  "));
}

TEST(x86ild, test_4801C1) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rcx, rax
  */
  EXPECT_EQ(3, ild(u"H☺┴"));
}

TEST(x86ild, test_FFC3) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc ebx
  */
  EXPECT_EQ(2, ild(u"λ├"));
}

TEST(x86ild, test_83F900) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp ecx, 0x0
  */
  EXPECT_EQ(3, ild(u"â∙ "));
}

TEST(x86ild, test_83E800) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_IMMb
    ISA_SET: I86
    SHORT: sub eax, 0x0
  */
  EXPECT_EQ(3, ild(u"âΦ "));
}

TEST(x86ild, test_FFCE) {
  /*
    ICLASS: DEC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DEC_GPRv_FFr1
    ISA_SET: I86
    SHORT: dec esi
  */
  EXPECT_EQ(2, ild(u"λ╬"));
}

TEST(x86ild, test_83780000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMv_IMMb
    ISA_SET: I86
    SHORT: cmp dword ptr [rax], 0x0
  */
  EXPECT_EQ(4, ild(u"âx  "));
}

TEST(x86ild, test_81FF00000000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMz
    ISA_SET: I86
    SHORT: cmp edi, 0x0
  */
  EXPECT_EQ(6, ild(u"üλ    "));
}

TEST(x86ild, test_FFC8) {
  /*
    ICLASS: DEC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DEC_GPRv_FFr1
    ISA_SET: I86
    SHORT: dec eax
  */
  EXPECT_EQ(2, ild(u"λ╚"));
}

TEST(x86ild, test_80780000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMb_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp byte ptr [rax], 0x0
  */
  EXPECT_EQ(4, ild(u"Çx  "));
}

TEST(x86ild, test_4839C8) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rax, rcx
  */
  EXPECT_EQ(3, ild(u"H9╚"));
}

TEST(x86ild, test_803800) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMb_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp byte ptr [rax], 0x0
  */
  EXPECT_EQ(3, ild(u"Ç8 "));
}

TEST(x86ild, test_4883E800) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_IMMb
    ISA_SET: I86
    SHORT: sub rax, 0x0
  */
  EXPECT_EQ(4, ild(u"HâΦ "));
}

TEST(x86ild, test_4080FE00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp sil, 0x0
  */
  EXPECT_EQ(4, ild(u"@Ç■ "));
}

TEST(x86ild, test_FFC5) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc ebp
  */
  EXPECT_EQ(2, ild(u"λ┼"));
}

TEST(x86ild, test_807D0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMb_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp byte ptr [rbp], 0x0
  */
  EXPECT_EQ(4, ild(u"Ç}  "));
}

TEST(x86ild, test_4883C000) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_IMMb
    ISA_SET: I86
    SHORT: add rax, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâ└ "));
}

TEST(x86ild, test_480500000000) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_OrAX_IMMz
    ISA_SET: I86
    SHORT: add rax, 0x0
  */
  EXPECT_EQ(6, ild(u"H♣    "));
}

TEST(x86ild, test_00FF) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPR8_GPR8_00
    ISA_SET: I86
    SHORT: add bh, bh
  */
  EXPECT_EQ(2, ild(u" λ"));
}

TEST(x86ild, test_FFC1) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc ecx
  */
  EXPECT_EQ(2, ild(u"λ┴"));
}

TEST(x86ild, test_83B80000000000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMv_IMMb
    ISA_SET: I86
    SHORT: cmp dword ptr [rax], 0x0
  */
  EXPECT_EQ(7, ild(u"â╕     "));
}

TEST(x86ild, test_48FFC0) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc rax
  */
  EXPECT_EQ(3, ild(u"Hλ└"));
}

TEST(x86ild, test_4881FF00000000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMz
    ISA_SET: I86
    SHORT: cmp rdi, 0x0
  */
  EXPECT_EQ(7, ild(u"Hüλ    "));
}

TEST(x86ild, test_4839CA) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rdx, rcx
  */
  EXPECT_EQ(3, ild(u"H9╩"));
}

TEST(x86ild, test_4839C1) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rcx, rax
  */
  EXPECT_EQ(3, ild(u"H9┴"));
}

TEST(x86ild, test_4801DF) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rdi, rbx
  */
  EXPECT_EQ(3, ild(u"H☺▀"));
}

TEST(x86ild, test_19C0) {
  /*
    ICLASS: SBB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SBB_GPRv_GPRv_19
    ISA_SET: I86
    SHORT: sbb eax, eax
  */
  EXPECT_EQ(2, ild(u"↓└"));
}

TEST(x86ild, test_833D0000000000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMv_IMMb
    ISA_SET: I86
    SHORT: cmp dword ptr [rip], 0x0
  */
  EXPECT_EQ(7, ild(u"â=     "));
}

TEST(x86ild, test_4801F0) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rax, rsi
  */
  EXPECT_EQ(3, ild(u"H☺≡"));
}

TEST(x86ild, test_4801C0) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rax, rax
  */
  EXPECT_EQ(3, ild(u"H☺└"));
}

TEST(x86ild, test_FFCA) {
  /*
    ICLASS: DEC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DEC_GPRv_FFr1
    ISA_SET: I86
    SHORT: dec edx
  */
  EXPECT_EQ(2, ild(u"λ╩"));
}

TEST(x86ild, test_FEC9) {
  /*
    ICLASS: DEC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DEC_GPR8
    ISA_SET: I86
    SHORT: dec cl
  */
  EXPECT_EQ(2, ild(u"■╔"));
}

TEST(x86ild, test_83FB00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp ebx, 0x0
  */
  EXPECT_EQ(3, ild(u"â√ "));
}

TEST(x86ild, test_48FFC8) {
  /*
    ICLASS: DEC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DEC_GPRv_FFr1
    ISA_SET: I86
    SHORT: dec rax
  */
  EXPECT_EQ(3, ild(u"Hλ╚"));
}

TEST(x86ild, test_48FFC2) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc rdx
  */
  EXPECT_EQ(3, ild(u"Hλ┬"));
}

TEST(x86ild, test_48F7F7) {
  /*
    ICLASS: DIV
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DIV_GPRv
    ISA_SET: I86
    SHORT: div rdi
  */
  EXPECT_EQ(3, ild(u"H≈≈"));
}

TEST(x86ild, test_4883FB00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp rbx, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâ√ "));
}

TEST(x86ild, test_4839F8) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rax, rdi
  */
  EXPECT_EQ(3, ild(u"H9°"));
}

TEST(x86ild, test_4801D1) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rcx, rdx
  */
  EXPECT_EQ(3, ild(u"H☺╤"));
}

TEST(x86ild, test_4801CA) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rdx, rcx
  */
  EXPECT_EQ(3, ild(u"H☺╩"));
}

TEST(x86ild, test_4080FF00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp dil, 0x0
  */
  EXPECT_EQ(4, ild(u"@Çλ "));
}

TEST(x86ild, test_FFC2) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc edx
  */
  EXPECT_EQ(2, ild(u"λ┬"));
}

TEST(x86ild, test_FFC0) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc eax
  */
  EXPECT_EQ(2, ild(u"λ└"));
}

TEST(x86ild, test_83FD00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp ebp, 0x0
  */
  EXPECT_EQ(3, ild(u"â² "));
}

TEST(x86ild, test_83C200) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_IMMb
    ISA_SET: I86
    SHORT: add edx, 0x0
  */
  EXPECT_EQ(3, ild(u"â┬ "));
}

TEST(x86ild, test_83C100) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_IMMb
    ISA_SET: I86
    SHORT: add ecx, 0x0
  */
  EXPECT_EQ(3, ild(u"â┴ "));
}

TEST(x86ild, test_4881EC00000000) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_IMMz
    ISA_SET: I86
    SHORT: sub rsp, 0x0
  */
  EXPECT_EQ(7, ild(u"Hü∞    "));
}

TEST(x86ild, test_4881C400000000) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_IMMz
    ISA_SET: I86
    SHORT: add rsp, 0x0
  */
  EXPECT_EQ(7, ild(u"Hü─    "));
}

TEST(x86ild, test_482D00000000) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_OrAX_IMMz
    ISA_SET: I86
    SHORT: sub rax, 0x0
  */
  EXPECT_EQ(6, ild(u"H-    "));
}

TEST(x86ild, test_4801C6) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rsi, rax
  */
  EXPECT_EQ(3, ild(u"H☺╞"));
}

TEST(x86ild, test_4439F3) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ebx, r14d
  */
  EXPECT_EQ(3, ild(u"D9≤"));
}

TEST(x86ild, test_41FFC8) {
  /*
    ICLASS: DEC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DEC_GPRv_FFr1
    ISA_SET: I86
    SHORT: dec r8d
  */
  EXPECT_EQ(3, ild(u"Aλ╚"));
}

TEST(x86ild, test_41FFC4) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc r12d
  */
  EXPECT_EQ(3, ild(u"Aλ─"));
}

TEST(x86ild, test_4183FD00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp r13d, 0x0
  */
  EXPECT_EQ(4, ild(u"Aâ² "));
}

TEST(x86ild, test_4180F800) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp r8b, 0x0
  */
  EXPECT_EQ(4, ild(u"AÇ° "));
}

TEST(x86ild, test_39F1) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ecx, esi
  */
  EXPECT_EQ(2, ild(u"9±"));
}

TEST(x86ild, test_39F0) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp eax, esi
  */
  EXPECT_EQ(2, ild(u"9≡"));
}

TEST(x86ild, test_FFC6) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc esi
  */
  EXPECT_EQ(2, ild(u"λ╞"));
}

TEST(x86ild, test_837D0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMv_IMMb
    ISA_SET: I86
    SHORT: cmp dword ptr [rbp], 0x0
  */
  EXPECT_EQ(4, ild(u"â}  "));
}

TEST(x86ild, test_833CD50000000000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMv_IMMb
    ISA_SET: I86
    SHORT: cmp dword ptr [rdx*8], 0x0
  */
  EXPECT_EQ(8, ild(u"â<╒     "));
}

TEST(x86ild, test_81EB00000000) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_IMMz
    ISA_SET: I86
    SHORT: sub ebx, 0x0
  */
  EXPECT_EQ(6, ild(u"üδ    "));
}

TEST(x86ild, test_807A0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMb_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp byte ptr [rdx], 0x0
  */
  EXPECT_EQ(4, ild(u"Çz  "));
}

TEST(x86ild, test_4C01C0) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rax, r8
  */
  EXPECT_EQ(3, ild(u"L☺└"));
}

TEST(x86ild, test_49FFC0) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc r8
  */
  EXPECT_EQ(3, ild(u"Iλ└"));
}

TEST(x86ild, test_4901D1) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add r9, rdx
  */
  EXPECT_EQ(3, ild(u"I☺╤"));
}

TEST(x86ild, test_48FFC7) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc rdi
  */
  EXPECT_EQ(3, ild(u"Hλ╟"));
}

TEST(x86ild, test_48FFC1) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc rcx
  */
  EXPECT_EQ(3, ild(u"Hλ┴"));
}

TEST(x86ild, test_4883FF00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp rdi, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâλ "));
}

TEST(x86ild, test_4883EB00) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_IMMb
    ISA_SET: I86
    SHORT: sub rbx, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâδ "));
}

TEST(x86ild, test_4883C200) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_IMMb
    ISA_SET: I86
    SHORT: add rdx, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâ┬ "));
}

TEST(x86ild, test_4839D8) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rax, rbx
  */
  EXPECT_EQ(3, ild(u"H9╪"));
}

TEST(x86ild, test_48034700) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_MEMv
    ISA_SET: I86
    SHORT: add rax, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"H♥G "));
}

TEST(x86ild, test_4801F2) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rdx, rsi
  */
  EXPECT_EQ(3, ild(u"H☺≥"));
}

TEST(x86ild, test_4539FE) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r14d, r15d
  */
  EXPECT_EQ(3, ild(u"E9■"));
}

TEST(x86ild, test_4429E1) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub ecx, r12d
  */
  EXPECT_EQ(3, ild(u"D)ß"));
}

TEST(x86ild, test_41807C240000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMb_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp byte ptr [r12], 0x0
  */
  EXPECT_EQ(6, ild(u"AÇ|$  "));
}

TEST(x86ild, test_39D0) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp eax, edx
  */
  EXPECT_EQ(2, ild(u"9╨"));
}

TEST(x86ild, test_39C2) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp edx, eax
  */
  EXPECT_EQ(2, ild(u"9┬"));
}

TEST(x86ild, test_39C1) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ecx, eax
  */
  EXPECT_EQ(2, ild(u"9┴"));
}

TEST(x86ild, test_0FAFC2) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv
    ISA_SET: I86
    SHORT: imul eax, edx
  */
  EXPECT_EQ(3, ild(u"☼»┬"));
}

TEST(x86ild, test_01C1) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add ecx, eax
  */
  EXPECT_EQ(2, ild(u"☺┴"));
}

TEST(x86ild, test_FE4300) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_MEMb
    ISA_SET: I86
    SHORT: inc byte ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"■C "));
}

TEST(x86ild, test_F7DB) {
  /*
    ICLASS: NEG
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: NEG_GPRv
    ISA_SET: I86
    SHORT: neg ebx
  */
  EXPECT_EQ(2, ild(u"≈█"));
}

TEST(x86ild, test_F77500) {
  /*
    ICLASS: DIV
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DIV_MEMv
    ISA_SET: I86
    SHORT: div dword ptr [rbp]
  */
  EXPECT_EQ(3, ild(u"≈u "));
}

TEST(x86ild, test_83EA00) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_IMMb
    ISA_SET: I86
    SHORT: sub edx, 0x0
  */
  EXPECT_EQ(3, ild(u"âΩ "));
}

TEST(x86ild, test_83E900) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_IMMb
    ISA_SET: I86
    SHORT: sub ecx, 0x0
  */
  EXPECT_EQ(3, ild(u"âΘ "));
}

TEST(x86ild, test_83C700) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_IMMb
    ISA_SET: I86
    SHORT: add edi, 0x0
  */
  EXPECT_EQ(3, ild(u"â╟ "));
}

TEST(x86ild, test_837F0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMv_IMMb
    ISA_SET: I86
    SHORT: cmp dword ptr [rdi], 0x0
  */
  EXPECT_EQ(4, ild(u"â⌂  "));
}

TEST(x86ild, test_833C2400) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMv_IMMb
    ISA_SET: I86
    SHORT: cmp dword ptr [rsp], 0x0
  */
  EXPECT_EQ(4, ild(u"â<$ "));
}

TEST(x86ild, test_81FA00000000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMz
    ISA_SET: I86
    SHORT: cmp edx, 0x0
  */
  EXPECT_EQ(6, ild(u"ü·    "));
}

TEST(x86ild, test_803D0000000000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMb_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp byte ptr [rip], 0x0
  */
  EXPECT_EQ(7, ild(u"Ç=     "));
}

TEST(x86ild, test_6683780000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMv_IMMb
    ISA_SET: I86
    SHORT: cmp word ptr [rax], 0x0
  */
  EXPECT_EQ(5, ild(u"fâx  "));
}

TEST(x86ild, test_6681FD0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMz
    ISA_SET: I86
    SHORT: cmp bp, 0x0
  */
  EXPECT_EQ(5, ild(u"fü²  "));
}

TEST(x86ild, test_6681FA0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMz
    ISA_SET: I86
    SHORT: cmp dx, 0x0
  */
  EXPECT_EQ(5, ild(u"fü·  "));
}

TEST(x86ild, test_6681C20000) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_IMMz
    ISA_SET: I86
    SHORT: add dx, 0x0
  */
  EXPECT_EQ(5, ild(u"fü┬  "));
}

TEST(x86ild, test_6639D0) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ax, dx
  */
  EXPECT_EQ(3, ild(u"f9╨"));
}

TEST(x86ild, test_6639CA) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp dx, cx
  */
  EXPECT_EQ(3, ild(u"f9╩"));
}

TEST(x86ild, test_4C29C0) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub rax, r8
  */
  EXPECT_EQ(3, ild(u"L)└"));
}

TEST(x86ild, test_4C034F00) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_MEMv
    ISA_SET: I86
    SHORT: add r9, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"L♥O "));
}

TEST(x86ild, test_4C01E8) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rax, r13
  */
  EXPECT_EQ(3, ild(u"L☺Φ"));
}

TEST(x86ild, test_4C01E0) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rax, r12
  */
  EXPECT_EQ(3, ild(u"L☺α"));
}

TEST(x86ild, test_49FFC5) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc r13
  */
  EXPECT_EQ(3, ild(u"Iλ┼"));
}

TEST(x86ild, test_49F7F1) {
  /*
    ICLASS: DIV
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DIV_GPRv
    ISA_SET: I86
    SHORT: div r9
  */
  EXPECT_EQ(3, ild(u"I≈±"));
}

TEST(x86ild, test_49F7DC) {
  /*
    ICLASS: NEG
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: NEG_GPRv
    ISA_SET: I86
    SHORT: neg r12
  */
  EXPECT_EQ(3, ild(u"I≈▄"));
}

TEST(x86ild, test_4929C2) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub r10, rax
  */
  EXPECT_EQ(3, ild(u"I)┬"));
}

TEST(x86ild, test_48FFC9) {
  /*
    ICLASS: DEC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DEC_GPRv_FFr1
    ISA_SET: I86
    SHORT: dec rcx
  */
  EXPECT_EQ(3, ild(u"Hλ╔"));
}

TEST(x86ild, test_48FFC6) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc rsi
  */
  EXPECT_EQ(3, ild(u"Hλ╞"));
}

TEST(x86ild, test_48FFC5) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc rbp
  */
  EXPECT_EQ(3, ild(u"Hλ┼"));
}

TEST(x86ild, test_48F7F1) {
  /*
    ICLASS: DIV
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DIV_GPRv
    ISA_SET: I86
    SHORT: div rcx
  */
  EXPECT_EQ(3, ild(u"H≈±"));
}

TEST(x86ild, test_48F7DB) {
  /*
    ICLASS: NEG
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: NEG_GPRv
    ISA_SET: I86
    SHORT: neg rbx
  */
  EXPECT_EQ(3, ild(u"H≈█"));
}

TEST(x86ild, test_4883FE00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp rsi, 0x0
  */
  EXPECT_EQ(4, ild(u"Hâ■ "));
}

TEST(x86ild, test_48837F0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMv_IMMb
    ISA_SET: I86
    SHORT: cmp qword ptr [rdi], 0x0
  */
  EXPECT_EQ(5, ild(u"Hâ⌂  "));
}

TEST(x86ild, test_4881EE00000000) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_IMMz
    ISA_SET: I86
    SHORT: sub rsi, 0x0
  */
  EXPECT_EQ(7, ild(u"Hüε    "));
}

TEST(x86ild, test_4881EB00000000) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_IMMz
    ISA_SET: I86
    SHORT: sub rbx, 0x0
  */
  EXPECT_EQ(7, ild(u"Hüδ    "));
}

TEST(x86ild, test_4839F2) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rdx, rsi
  */
  EXPECT_EQ(3, ild(u"H9≥"));
}

TEST(x86ild, test_4839D6) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rsi, rdx
  */
  EXPECT_EQ(3, ild(u"H9╓"));
}

TEST(x86ild, test_4839C7) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp rdi, rax
  */
  EXPECT_EQ(3, ild(u"H9╟"));
}

TEST(x86ild, test_4829DA) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub rdx, rbx
  */
  EXPECT_EQ(3, ild(u"H)┌"));
}

TEST(x86ild, test_480FAFD0) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv
    ISA_SET: I86
    SHORT: imul rdx, rax
  */
  EXPECT_EQ(4, ild(u"H☼»╨"));
}

TEST(x86ild, test_480FAFCA) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv
    ISA_SET: I86
    SHORT: imul rcx, rdx
  */
  EXPECT_EQ(4, ild(u"H☼»╩"));
}

TEST(x86ild, test_480FAFC8) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv
    ISA_SET: I86
    SHORT: imul rcx, rax
  */
  EXPECT_EQ(4, ild(u"H☼»╚"));
}

TEST(x86ild, test_48035300) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_MEMv
    ISA_SET: I86
    SHORT: add rdx, qword ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"H♥S "));
}

TEST(x86ild, test_4801F8) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rax, rdi
  */
  EXPECT_EQ(3, ild(u"H☺°"));
}

TEST(x86ild, test_4801EF) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rdi, rbp
  */
  EXPECT_EQ(3, ild(u"H☺∩"));
}

TEST(x86ild, test_4801DE) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rsi, rbx
  */
  EXPECT_EQ(3, ild(u"H☺▐"));
}

TEST(x86ild, test_4801D2) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add rdx, rdx
  */
  EXPECT_EQ(3, ild(u"H☺╥"));
}

TEST(x86ild, test_4539F4) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r12d, r14d
  */
  EXPECT_EQ(3, ild(u"E9⌠"));
}

TEST(x86ild, test_4539EF) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r15d, r13d
  */
  EXPECT_EQ(3, ild(u"E9∩"));
}

TEST(x86ild, test_4539EE) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r14d, r13d
  */
  EXPECT_EQ(3, ild(u"E9ε"));
}

TEST(x86ild, test_4529C4) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub r12d, r8d
  */
  EXPECT_EQ(3, ild(u"E)─"));
}

TEST(x86ild, test_4501E4) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add r12d, r12d
  */
  EXPECT_EQ(3, ild(u"E☺Σ"));
}

TEST(x86ild, test_4501E2) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add r10d, r12d
  */
  EXPECT_EQ(3, ild(u"E☺Γ"));
}

TEST(x86ild, test_4439E8) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp eax, r13d
  */
  EXPECT_EQ(3, ild(u"D9Φ"));
}

TEST(x86ild, test_4439E5) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ebp, r12d
  */
  EXPECT_EQ(3, ild(u"D9σ"));
}

TEST(x86ild, test_4439E3) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ebx, r12d
  */
  EXPECT_EQ(3, ild(u"D9π"));
}

TEST(x86ild, test_4439E1) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ecx, r12d
  */
  EXPECT_EQ(3, ild(u"D9ß"));
}

TEST(x86ild, test_4439C9) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ecx, r9d
  */
  EXPECT_EQ(3, ild(u"D9╔"));
}

TEST(x86ild, test_4439C7) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp edi, r8d
  */
  EXPECT_EQ(3, ild(u"D9╟"));
}

TEST(x86ild, test_4439C5) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ebp, r8d
  */
  EXPECT_EQ(3, ild(u"D9┼"));
}

TEST(x86ild, test_4439C2) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp edx, r8d
  */
  EXPECT_EQ(3, ild(u"D9┬"));
}

TEST(x86ild, test_4438C1) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_GPR8_38
    ISA_SET: I86
    SHORT: cmp cl, r8b
  */
  EXPECT_EQ(3, ild(u"D8┴"));
}

TEST(x86ild, test_4429DD) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub ebp, r11d
  */
  EXPECT_EQ(3, ild(u"D)▌"));
}

TEST(x86ild, test_4429D1) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub ecx, r10d
  */
  EXPECT_EQ(3, ild(u"D)╤"));
}

TEST(x86ild, test_4429C0) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub eax, r8d
  */
  EXPECT_EQ(3, ild(u"D)└"));
}

TEST(x86ild, test_4401E8) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add eax, r13d
  */
  EXPECT_EQ(3, ild(u"D☺Φ"));
}

TEST(x86ild, test_4401C8) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add eax, r9d
  */
  EXPECT_EQ(3, ild(u"D☺╚"));
}

TEST(x86ild, test_41FFCF) {
  /*
    ICLASS: DEC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: DEC_GPRv_FFr1
    ISA_SET: I86
    SHORT: dec r15d
  */
  EXPECT_EQ(3, ild(u"Aλ╧"));
}

TEST(x86ild, test_41FFC7) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc r15d
  */
  EXPECT_EQ(3, ild(u"Aλ╟"));
}

TEST(x86ild, test_41FFC6) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc r14d
  */
  EXPECT_EQ(3, ild(u"Aλ╞"));
}

TEST(x86ild, test_41FFC2) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc r10d
  */
  EXPECT_EQ(3, ild(u"Aλ┬"));
}

TEST(x86ild, test_41FFC1) {
  /*
    ICLASS: INC
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: INC_GPRv_FFr0
    ISA_SET: I86
    SHORT: inc r9d
  */
  EXPECT_EQ(3, ild(u"Aλ┴"));
}

TEST(x86ild, test_4183FE00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp r14d, 0x0
  */
  EXPECT_EQ(4, ild(u"Aâ■ "));
}

TEST(x86ild, test_4183FB00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp r11d, 0x0
  */
  EXPECT_EQ(4, ild(u"Aâ√ "));
}

TEST(x86ild, test_4183F900) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp r9d, 0x0
  */
  EXPECT_EQ(4, ild(u"Aâ∙ "));
}

TEST(x86ild, test_4183F800) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMb
    ISA_SET: I86
    SHORT: cmp r8d, 0x0
  */
  EXPECT_EQ(4, ild(u"Aâ° "));
}

TEST(x86ild, test_4181F800000000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_IMMz
    ISA_SET: I86
    SHORT: cmp r8d, 0x0
  */
  EXPECT_EQ(7, ild(u"Aü°    "));
}

TEST(x86ild, test_4180F900) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp r9b, 0x0
  */
  EXPECT_EQ(4, ild(u"AÇ∙ "));
}

TEST(x86ild, test_41803C0000) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMb_IMMb_80r7
    ISA_SET: I86
    SHORT: cmp byte ptr [r8+rax*1], 0x0
  */
  EXPECT_EQ(5, ild(u"AÇ<  "));
}

TEST(x86ild, test_4139F5) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r13d, esi
  */
  EXPECT_EQ(3, ild(u"A9⌡"));
}

TEST(x86ild, test_4139EE) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r14d, ebp
  */
  EXPECT_EQ(3, ild(u"A9ε"));
}

TEST(x86ild, test_4139EC) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r12d, ebp
  */
  EXPECT_EQ(3, ild(u"A9∞"));
}

TEST(x86ild, test_4139DD) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r13d, ebx
  */
  EXPECT_EQ(3, ild(u"A9▌"));
}

TEST(x86ild, test_4139D1) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r9d, edx
  */
  EXPECT_EQ(3, ild(u"A9╤"));
}

TEST(x86ild, test_4139D0) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r8d, edx
  */
  EXPECT_EQ(3, ild(u"A9╨"));
}

TEST(x86ild, test_4139C6) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r14d, eax
  */
  EXPECT_EQ(3, ild(u"A9╞"));
}

TEST(x86ild, test_4139C4) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r12d, eax
  */
  EXPECT_EQ(3, ild(u"A9─"));
}

TEST(x86ild, test_4139C1) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp r9d, eax
  */
  EXPECT_EQ(3, ild(u"A9┴"));
}

TEST(x86ild, test_4138ED) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_GPR8_38
    ISA_SET: I86
    SHORT: cmp r13b, bpl
  */
  EXPECT_EQ(3, ild(u"A8φ"));
}

TEST(x86ild, test_4129CC) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub r12d, ecx
  */
  EXPECT_EQ(3, ild(u"A)╠"));
}

TEST(x86ild, test_4038F1) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_GPR8_38
    ISA_SET: I86
    SHORT: cmp cl, sil
  */
  EXPECT_EQ(3, ild(u"@8±"));
}

TEST(x86ild, test_3A5300) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_MEMb
    ISA_SET: I86
    SHORT: cmp dl, byte ptr [rbx]
  */
  EXPECT_EQ(3, ild(u":S "));
}

TEST(x86ild, test_3A4B00) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_MEMb
    ISA_SET: I86
    SHORT: cmp cl, byte ptr [rbx]
  */
  EXPECT_EQ(3, ild(u":K "));
}

TEST(x86ild, test_3A4300) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPR8_MEMb
    ISA_SET: I86
    SHORT: cmp al, byte ptr [rbx]
  */
  EXPECT_EQ(3, ild(u":C "));
}

TEST(x86ild, test_39F9) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ecx, edi
  */
  EXPECT_EQ(2, ild(u"9∙"));
}

TEST(x86ild, test_39F7) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp edi, esi
  */
  EXPECT_EQ(2, ild(u"9≈"));
}

TEST(x86ild, test_39EB) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ebx, ebp
  */
  EXPECT_EQ(2, ild(u"9δ"));
}

TEST(x86ild, test_39D8) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp eax, ebx
  */
  EXPECT_EQ(2, ild(u"9╪"));
}

TEST(x86ild, test_39CA) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp edx, ecx
  */
  EXPECT_EQ(2, ild(u"9╩"));
}

TEST(x86ild, test_39C5) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ebp, eax
  */
  EXPECT_EQ(2, ild(u"9┼"));
}

TEST(x86ild, test_39C3) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_GPRv_GPRv_39
    ISA_SET: I86
    SHORT: cmp ebx, eax
  */
  EXPECT_EQ(2, ild(u"9├"));
}

TEST(x86ild, test_385700) {
  /*
    ICLASS: CMP
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: CMP_MEMb_GPR8
    ISA_SET: I86
    SHORT: cmp byte ptr [rdi], dl
  */
  EXPECT_EQ(3, ild(u"8W "));
}

TEST(x86ild, test_29E8) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub eax, ebp
  */
  EXPECT_EQ(2, ild(u")Φ"));
}

TEST(x86ild, test_29DD) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub ebp, ebx
  */
  EXPECT_EQ(2, ild(u")▌"));
}

TEST(x86ild, test_29C5) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub ebp, eax
  */
  EXPECT_EQ(2, ild(u")┼"));
}

TEST(x86ild, test_29C3) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub ebx, eax
  */
  EXPECT_EQ(2, ild(u")├"));
}

TEST(x86ild, test_29C2) {
  /*
    ICLASS: SUB
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: SUB_GPRv_GPRv_29
    ISA_SET: I86
    SHORT: sub edx, eax
  */
  EXPECT_EQ(2, ild(u")┬"));
}

TEST(x86ild, test_01FE) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add esi, edi
  */
  EXPECT_EQ(2, ild(u"☺■"));
}

TEST(x86ild, test_01F0) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add eax, esi
  */
  EXPECT_EQ(2, ild(u"☺≡"));
}

TEST(x86ild, test_01D6) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add esi, edx
  */
  EXPECT_EQ(2, ild(u"☺╓"));
}

TEST(x86ild, test_01C9) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add ecx, ecx
  */
  EXPECT_EQ(2, ild(u"☺╔"));
}

TEST(x86ild, test_01C8) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add eax, ecx
  */
  EXPECT_EQ(2, ild(u"☺╚"));
}

TEST(x86ild, test_01C5) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add ebp, eax
  */
  EXPECT_EQ(2, ild(u"☺┼"));
}

TEST(x86ild, test_01C2) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPRv_GPRv_01
    ISA_SET: I86
    SHORT: add edx, eax
  */
  EXPECT_EQ(2, ild(u"☺┬"));
}

TEST(x86ild, test_01B000000000) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_MEMv_GPRv
    ISA_SET: I86
    SHORT: add dword ptr [rax], esi
  */
  EXPECT_EQ(6, ild(u"☺░    "));
}

TEST(x86ild, test_00C1) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_GPR8_GPR8_00
    ISA_SET: I86
    SHORT: add cl, al
  */
  EXPECT_EQ(2, ild(u" ┴"));
}

TEST(x86ild, test_00B800000000) {
  /*
    ICLASS: ADD
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: ADD_MEMb_GPR8
    ISA_SET: I86
    SHORT: add byte ptr [rax], bh
  */
  EXPECT_EQ(6, ild(u" ╕    "));
}
