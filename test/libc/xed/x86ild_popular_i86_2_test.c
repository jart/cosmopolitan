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

TEST(x86ild, test_4489E7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edi, r12d
  */
  EXPECT_EQ(3, ild(u"Dëτ"));
}

TEST(x86ild, test_4489E0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, r12d
  */
  EXPECT_EQ(3, ild(u"Dëα"));
}

TEST(x86ild, test_4489D9) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ecx, r11d
  */
  EXPECT_EQ(3, ild(u"Dë┘"));
}

TEST(x86ild, test_4488D0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov al, r10b
  */
  EXPECT_EQ(3, ild(u"Dê╨"));
}

TEST(x86ild, test_41BE00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov r14d, 0x0
  */
  EXPECT_EQ(6, ild(u"A╛    "));
}

TEST(x86ild, test_41B900000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov r9d, 0x0
  */
  EXPECT_EQ(6, ild(u"A╣    "));
}

TEST(x86ild, test_4189E8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r8d, ebp
  */
  EXPECT_EQ(3, ild(u"AëΦ"));
}

TEST(x86ild, test_4189D0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r8d, edx
  */
  EXPECT_EQ(3, ild(u"Aë╨"));
}

TEST(x86ild, test_4189C7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r15d, eax
  */
  EXPECT_EQ(3, ild(u"Aë╟"));
}

TEST(x86ild, test_4188C0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r8b, al
  */
  EXPECT_EQ(3, ild(u"Aê└"));
}

TEST(x86ild, test_408A7B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dil, byte ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"@è{ "));
}

TEST(x86ild, test_4088F1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov cl, sil
  */
  EXPECT_EQ(3, ild(u"@ê±"));
}

TEST(x86ild, test_4088D6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov sil, dl
  */
  EXPECT_EQ(3, ild(u"@ê╓"));
}

TEST(x86ild, test_4088C7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dil, al
  */
  EXPECT_EQ(3, ild(u"@ê╟"));
}

TEST(x86ild, test_0F8300000000) {
  /*
    ICLASS: JNB
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNB_RELBRd
    ISA_SET: I86
    SHORT: jnb 0x6
  */
  EXPECT_EQ(6, ild(u"☼â    "));
}

TEST(x86ild, test_FF24F500000000) {
  /*
    ICLASS: JMP
    CATEGORY: UNCOND_BR
    EXTENSION: BASE
    IFORM: JMP_MEMv
    ISA_SET: I86
    SHORT: jmp qword ptr [rsi*8]
  */
  EXPECT_EQ(7, ild(u"λ$⌡    "));
}

TEST(x86ild, test_E80083E200) {
  /*
    ICLASS: CALL_NEAR
    CATEGORY: CALL
    EXTENSION: BASE
    IFORM: CALL_NEAR_RELBRd
    ISA_SET: I86
    SHORT: call 0xe28305
  */
  EXPECT_EQ(5, ild(u"Φ âΓ "));
}

TEST(x86ild, test_D3E7) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_CL_D3r4
    ISA_SET: I86
    SHORT: shl edi, cl
  */
  EXPECT_EQ(2, ild(u"╙τ"));
}

TEST(x86ild, test_D2C1) {
  /*
    ICLASS: ROL
    CATEGORY: ROTATE
    EXTENSION: BASE
    IFORM: ROL_GPR8_CL
    ISA_SET: I86
    SHORT: rol cl, cl
  */
  EXPECT_EQ(2, ild(u"╥┴"));
}

TEST(x86ild, test_C784240000000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov dword ptr [rsp], 0x0
  */
  EXPECT_EQ(11, ild(u"╟ä$        "));
}

TEST(x86ild, test_C744240000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov dword ptr [rsp], 0x0
  */
  EXPECT_EQ(8, ild(u"╟D$     "));
}

TEST(x86ild, test_C6840E0000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rsi+rcx*1], 0x0
  */
  EXPECT_EQ(8, ild(u"╞ä♫     "));
}

TEST(x86ild, test_C6420000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rdx], 0x0
  */
  EXPECT_EQ(4, ild(u"╞B  "));
}

TEST(x86ild, test_C60700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rdi], 0x0
  */
  EXPECT_EQ(3, ild(u"╞• "));
}

TEST(x86ild, test_C60600) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rsi], 0x0
  */
  EXPECT_EQ(3, ild(u"╞♠ "));
}

TEST(x86ild, test_8BB200000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov esi, dword ptr [rdx]
  */
  EXPECT_EQ(6, ild(u"ï▓    "));
}

TEST(x86ild, test_8B7F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edi, dword ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"ï⌂ "));
}

TEST(x86ild, test_8B7C2400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edi, dword ptr [rsp]
  */
  EXPECT_EQ(4, ild(u"ï|$ "));
}

TEST(x86ild, test_8B7B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edi, dword ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"ï{ "));
}

TEST(x86ild, test_8B5100) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rcx]
  */
  EXPECT_EQ(3, ild(u"ïQ "));
}

TEST(x86ild, test_8B4C2400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ecx, dword ptr [rsp]
  */
  EXPECT_EQ(4, ild(u"ïL$ "));
}

TEST(x86ild, test_8B16) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rsi]
  */
  EXPECT_EQ(2, ild(u"ï▬"));
}

TEST(x86ild, test_8B10) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rax]
  */
  EXPECT_EQ(2, ild(u"ï►"));
}

TEST(x86ild, test_8B0C8500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ecx, dword ptr [rax*4]
  */
  EXPECT_EQ(7, ild(u"ï♀à    "));
}

TEST(x86ild, test_8B04FD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rdi*8]
  */
  EXPECT_EQ(7, ild(u"ï♦²    "));
}

TEST(x86ild, test_8B04C500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rax*8]
  */
  EXPECT_EQ(7, ild(u"ï♦┼    "));
}

TEST(x86ild, test_8B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rax]
  */
  EXPECT_EQ(2, ild(u"ï "));
}

TEST(x86ild, test_8A14C500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rax*8]
  */
  EXPECT_EQ(7, ild(u"è¶┼    "));
}

TEST(x86ild, test_8A0401) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rcx+rax*1]
  */
  EXPECT_EQ(3, ild(u"è♦☺"));
}

TEST(x86ild, test_89FB) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ebx, edi
  */
  EXPECT_EQ(2, ild(u"ë√"));
}

TEST(x86ild, test_89DF) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edi, ebx
  */
  EXPECT_EQ(2, ild(u"ë▀"));
}

TEST(x86ild, test_89D8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, ebx
  */
  EXPECT_EQ(2, ild(u"ë╪"));
}

TEST(x86ild, test_89CB) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ebx, ecx
  */
  EXPECT_EQ(2, ild(u"ë╦"));
}

TEST(x86ild, test_899A00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdx], ebx
  */
  EXPECT_EQ(6, ild(u"ëÜ    "));
}

TEST(x86ild, test_894B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rbx], ecx
  */
  EXPECT_EQ(3, ild(u"ëK "));
}

TEST(x86ild, test_890424) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rsp], eax
  */
  EXPECT_EQ(3, ild(u"ë♦$"));
}

TEST(x86ild, test_8810) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rax], dl
  */
  EXPECT_EQ(2, ild(u"ê►"));
}

TEST(x86ild, test_66C747000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov word ptr [rdi], 0x0
  */
  EXPECT_EQ(6, ild(u"f╟G   "));
}

TEST(x86ild, test_668910) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rax], dx
  */
  EXPECT_EQ(3, ild(u"fë►"));
}

TEST(x86ild, test_4D89EC) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r12, r13
  */
  EXPECT_EQ(3, ild(u"Më∞"));
}

TEST(x86ild, test_4C89FA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdx, r15
  */
  EXPECT_EQ(3, ild(u"Lë·"));
}

TEST(x86ild, test_4C89E6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rsi, r12
  */
  EXPECT_EQ(3, ild(u"Lëμ"));
}

TEST(x86ild, test_4C89D0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rax, r10
  */
  EXPECT_EQ(3, ild(u"Lë╨"));
}

TEST(x86ild, test_49D3E0) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_CL_D3r4
    ISA_SET: I86
    SHORT: shl r8, cl
  */
  EXPECT_EQ(3, ild(u"I╙α"));
}

TEST(x86ild, test_4989F4) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r12, rsi
  */
  EXPECT_EQ(3, ild(u"Ië⌠"));
}

TEST(x86ild, test_4989C4) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r12, rax
  */
  EXPECT_EQ(3, ild(u"Ië─"));
}

TEST(x86ild, test_48BF0000000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov rdi, 0x0
  */
  EXPECT_EQ(10, ild(u"H┐        "));
}

TEST(x86ild, test_488B9100000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rcx]
  */
  EXPECT_EQ(7, ild(u"Hïæ    "));
}

TEST(x86ild, test_488B7700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rsi, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"Hïw "));
}

TEST(x86ild, test_488B5200) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rdx]
  */
  EXPECT_EQ(4, ild(u"HïR "));
}

TEST(x86ild, test_488B04D0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rax+rdx*8]
  */
  EXPECT_EQ(4, ild(u"Hï♦╨"));
}

TEST(x86ild, test_4889E9) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rcx, rbp
  */
  EXPECT_EQ(3, ild(u"HëΘ"));
}

TEST(x86ild, test_4889E7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdi, rsp
  */
  EXPECT_EQ(3, ild(u"Hëτ"));
}

TEST(x86ild, test_4889D7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdi, rdx
  */
  EXPECT_EQ(3, ild(u"Hë╫"));
}

TEST(x86ild, test_4889CD) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rbp, rcx
  */
  EXPECT_EQ(3, ild(u"Hë═"));
}

TEST(x86ild, test_48895700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rdi], rdx
  */
  EXPECT_EQ(4, ild(u"HëW "));
}

TEST(x86ild, test_48894700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rdi], rax
  */
  EXPECT_EQ(4, ild(u"HëG "));
}

TEST(x86ild, test_48894300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rbx], rax
  */
  EXPECT_EQ(4, ild(u"HëC "));
}

TEST(x86ild, test_488910) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rax], rdx
  */
  EXPECT_EQ(3, ild(u"Hë►"));
}

TEST(x86ild, test_48890D00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rip], rcx
  */
  EXPECT_EQ(7, ild(u"Hë♪    "));
}

TEST(x86ild, test_488902) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rdx], rax
  */
  EXPECT_EQ(3, ild(u"Hë☻"));
}

TEST(x86ild, test_4863D2) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_GPR32
    ISA_SET: LONGMODE
    SHORT: movsxd rdx, edx
  */
  EXPECT_EQ(3, ild(u"Hc╥"));
}

TEST(x86ild, test_4589CA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r10d, r9d
  */
  EXPECT_EQ(3, ild(u"Eë╩"));
}

TEST(x86ild, test_4588C8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r8b, r9b
  */
  EXPECT_EQ(3, ild(u"Eê╚"));
}

TEST(x86ild, test_4489FA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edx, r15d
  */
  EXPECT_EQ(3, ild(u"Dë·"));
}

TEST(x86ild, test_4489F7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edi, r14d
  */
  EXPECT_EQ(3, ild(u"Dë≈"));
}

TEST(x86ild, test_4489EF) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edi, r13d
  */
  EXPECT_EQ(3, ild(u"Dë∩"));
}

TEST(x86ild, test_4489E1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ecx, r12d
  */
  EXPECT_EQ(3, ild(u"Dëß"));
}

TEST(x86ild, test_4489442400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rsp], r8d
  */
  EXPECT_EQ(5, ild(u"DëD$ "));
}

TEST(x86ild, test_4488CA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dl, r9b
  */
  EXPECT_EQ(3, ild(u"Dê╩"));
}

TEST(x86ild, test_44885300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], r10b
  */
  EXPECT_EQ(4, ild(u"DêS "));
}

TEST(x86ild, test_44884300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], r8b
  */
  EXPECT_EQ(4, ild(u"DêC "));
}

TEST(x86ild, test_428B148D00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [r9*4]
  */
  EXPECT_EQ(8, ild(u"Bï¶ì    "));
}

TEST(x86ild, test_41D3E0) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_CL_D3r4
    ISA_SET: I86
    SHORT: shl r8d, cl
  */
  EXPECT_EQ(3, ild(u"A╙α"));
}

TEST(x86ild, test_41BF00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov r15d, 0x0
  */
  EXPECT_EQ(6, ild(u"A┐    "));
}

TEST(x86ild, test_41BD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov r13d, 0x0
  */
  EXPECT_EQ(6, ild(u"A╜    "));
}

TEST(x86ild, test_41B800000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov r8d, 0x0
  */
  EXPECT_EQ(6, ild(u"A╕    "));
}

TEST(x86ild, test_4189F4) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r12d, esi
  */
  EXPECT_EQ(3, ild(u"Aë⌠"));
}

TEST(x86ild, test_4189F2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r10d, esi
  */
  EXPECT_EQ(3, ild(u"Aë≥"));
}

TEST(x86ild, test_4189C8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r8d, ecx
  */
  EXPECT_EQ(3, ild(u"Aë╚"));
}

TEST(x86ild, test_4088F9) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov cl, dil
  */
  EXPECT_EQ(3, ild(u"@ê∙"));
}

TEST(x86ild, test_4088F2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dl, sil
  */
  EXPECT_EQ(3, ild(u"@ê≥"));
}

TEST(x86ild, test_4088E8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov al, bpl
  */
  EXPECT_EQ(3, ild(u"@êΦ"));
}

TEST(x86ild, test_0F8900000000) {
  /*
    ICLASS: JNS
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNS_RELBRd
    ISA_SET: I86
    SHORT: jns 0x6
  */
  EXPECT_EQ(6, ild(u"☼ë    "));
}

TEST(x86ild, test_0F8600000000) {
  /*
    ICLASS: JBE
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JBE_RELBRd
    ISA_SET: I86
    SHORT: jbe 0x6
  */
  EXPECT_EQ(6, ild(u"☼å    "));
}

TEST(x86ild, test_FF14C500000000) {
  /*
    ICLASS: CALL_NEAR
    CATEGORY: CALL
    EXTENSION: BASE
    IFORM: CALL_NEAR_MEMv
    ISA_SET: I86
    SHORT: call qword ptr [rax*8]
  */
  EXPECT_EQ(7, ild(u"λ¶┼    "));
}

TEST(x86ild, test_F3C3) {
  /*
    ICLASS: RET_NEAR
    CATEGORY: RET
    EXTENSION: BASE
    IFORM: RET_NEAR
    ISA_SET: I86
    SHORT: ret
  */
  EXPECT_EQ(2, ild(u"≤├"));
}

TEST(x86ild, test_F3A4) {
  /*
    ICLASS: REP_MOVSB
    CATEGORY: STRINGOP
    EXTENSION: BASE
    IFORM: REP_MOVSB
    ISA_SET: I86
    SHORT: rep movsb byte ptr [rdi], byte ptr [rsi]
  */
  EXPECT_EQ(2, ild(u"≤ñ"));
}

TEST(x86ild, test_D3E2) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_CL_D3r4
    ISA_SET: I86
    SHORT: shl edx, cl
  */
  EXPECT_EQ(2, ild(u"╙Γ"));
}

TEST(x86ild, test_C6040300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rbx+rax*1], 0x0
  */
  EXPECT_EQ(4, ild(u"╞♦♥ "));
}

TEST(x86ild, test_C60100) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rcx], 0x0
  */
  EXPECT_EQ(3, ild(u"╞☺ "));
}

TEST(x86ild, test_B200) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_IMMb_B0
    ISA_SET: I86
    SHORT: mov dl, 0x0
  */
  EXPECT_EQ(2, ild(u"▓ "));
}

TEST(x86ild, test_8B8100000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rcx]
  */
  EXPECT_EQ(6, ild(u"ïü    "));
}

TEST(x86ild, test_8B7500) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov esi, dword ptr [rbp]
  */
  EXPECT_EQ(3, ild(u"ïu "));
}

TEST(x86ild, test_8B6800) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ebp, dword ptr [rax]
  */
  EXPECT_EQ(3, ild(u"ïh "));
}

TEST(x86ild, test_8B4800) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ecx, dword ptr [rax]
  */
  EXPECT_EQ(3, ild(u"ïH "));
}

TEST(x86ild, test_8B4500) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rbp]
  */
  EXPECT_EQ(3, ild(u"ïE "));
}

TEST(x86ild, test_8B3F) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edi, dword ptr [rdi]
  */
  EXPECT_EQ(2, ild(u"ï?"));
}

TEST(x86ild, test_8B3C24) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edi, dword ptr [rsp]
  */
  EXPECT_EQ(3, ild(u"ï<$"));
}

TEST(x86ild, test_8B348500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov esi, dword ptr [rax*4]
  */
  EXPECT_EQ(7, ild(u"ï4à    "));
}

TEST(x86ild, test_8B28) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ebp, dword ptr [rax]
  */
  EXPECT_EQ(2, ild(u"ï("));
}

TEST(x86ild, test_8B1E) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ebx, dword ptr [rsi]
  */
  EXPECT_EQ(2, ild(u"ï▲"));
}

TEST(x86ild, test_8B1500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rip]
  */
  EXPECT_EQ(6, ild(u"ï§    "));
}

TEST(x86ild, test_8B14F500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rsi*8]
  */
  EXPECT_EQ(7, ild(u"ï¶⌡    "));
}

TEST(x86ild, test_8B14C500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rax*8]
  */
  EXPECT_EQ(7, ild(u"ï¶┼    "));
}

TEST(x86ild, test_8B0CCD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ecx, dword ptr [rcx*8]
  */
  EXPECT_EQ(7, ild(u"ï♀═    "));
}

TEST(x86ild, test_8B0CBD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ecx, dword ptr [rdi*4]
  */
  EXPECT_EQ(7, ild(u"ï♀╜    "));
}

TEST(x86ild, test_8B0CB500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ecx, dword ptr [rsi*4]
  */
  EXPECT_EQ(7, ild(u"ï♀╡    "));
}

TEST(x86ild, test_8B0C8D00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ecx, dword ptr [rcx*4]
  */
  EXPECT_EQ(7, ild(u"ï♀ì    "));
}

TEST(x86ild, test_8B06) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rsi]
  */
  EXPECT_EQ(2, ild(u"ï♠"));
}

TEST(x86ild, test_8B04B500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rsi*4]
  */
  EXPECT_EQ(7, ild(u"ï♦╡    "));
}

TEST(x86ild, test_8A9200000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rdx]
  */
  EXPECT_EQ(6, ild(u"èÆ    "));
}

TEST(x86ild, test_8A8900000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rcx]
  */
  EXPECT_EQ(6, ild(u"èë    "));
}

TEST(x86ild, test_8A841000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rax+rdx*1]
  */
  EXPECT_EQ(7, ild(u"èä►    "));
}

TEST(x86ild, test_8A542400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rsp]
  */
  EXPECT_EQ(4, ild(u"èT$ "));
}

TEST(x86ild, test_8A4E00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rsi]
  */
  EXPECT_EQ(3, ild(u"èN "));
}

TEST(x86ild, test_8A4D00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rbp]
  */
  EXPECT_EQ(3, ild(u"èM "));
}

TEST(x86ild, test_8A14F500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rsi*8]
  */
  EXPECT_EQ(7, ild(u"è¶⌡    "));
}

TEST(x86ild, test_8A149500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rdx*4]
  */
  EXPECT_EQ(7, ild(u"è¶ò    "));
}

TEST(x86ild, test_8A1411) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rcx+rdx*1]
  */
  EXPECT_EQ(3, ild(u"è¶◄"));
}

TEST(x86ild, test_8A140A) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rdx+rcx*1]
  */
  EXPECT_EQ(3, ild(u"è¶◙"));
}

TEST(x86ild, test_8A1402) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rdx+rax*1]
  */
  EXPECT_EQ(3, ild(u"è¶☻"));
}

TEST(x86ild, test_8A0C01) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rcx+rax*1]
  */
  EXPECT_EQ(3, ild(u"è♀☺"));
}

TEST(x86ild, test_8A06) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rsi]
  */
  EXPECT_EQ(2, ild(u"è♠"));
}

TEST(x86ild, test_8A0406) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rsi+rax*1]
  */
  EXPECT_EQ(3, ild(u"è♦♠"));
}

TEST(x86ild, test_8A0402) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rdx+rax*1]
  */
  EXPECT_EQ(3, ild(u"è♦☻"));
}

TEST(x86ild, test_89F9) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ecx, edi
  */
  EXPECT_EQ(2, ild(u"ë∙"));
}

TEST(x86ild, test_89F3) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ebx, esi
  */
  EXPECT_EQ(2, ild(u"ë≤"));
}

TEST(x86ild, test_89E9) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ecx, ebp
  */
  EXPECT_EQ(2, ild(u"ëΘ"));
}

TEST(x86ild, test_89D7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edi, edx
  */
  EXPECT_EQ(2, ild(u"ë╫"));
}

TEST(x86ild, test_89D3) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ebx, edx
  */
  EXPECT_EQ(2, ild(u"ë╙"));
}

TEST(x86ild, test_89AA00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdx], ebp
  */
  EXPECT_EQ(6, ild(u"ë¬    "));
}

TEST(x86ild, test_89942400000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rsp], edx
  */
  EXPECT_EQ(7, ild(u"ëö$    "));
}

TEST(x86ild, test_898200000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdx], eax
  */
  EXPECT_EQ(6, ild(u"ëé    "));
}

TEST(x86ild, test_897C2400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rsp], edi
  */
  EXPECT_EQ(4, ild(u"ë|$ "));
}

TEST(x86ild, test_895D00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rbp], ebx
  */
  EXPECT_EQ(3, ild(u"ë] "));
}

TEST(x86ild, test_895800) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rax], ebx
  */
  EXPECT_EQ(3, ild(u"ëX "));
}

TEST(x86ild, test_894C2400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rsp], ecx
  */
  EXPECT_EQ(4, ild(u"ëL$ "));
}

TEST(x86ild, test_894500) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rbp], eax
  */
  EXPECT_EQ(3, ild(u"ëE "));
}

TEST(x86ild, test_893D00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rip], edi
  */
  EXPECT_EQ(6, ild(u"ë=    "));
}

TEST(x86ild, test_891500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rip], edx
  */
  EXPECT_EQ(6, ild(u"ë§    "));
}

TEST(x86ild, test_8903) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rbx], eax
  */
  EXPECT_EQ(2, ild(u"ë♥"));
}

TEST(x86ild, test_88540700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rdi+rax*1], dl
  */
  EXPECT_EQ(4, ild(u"êT• "));
}

TEST(x86ild, test_88540300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx+rax*1], dl
  */
  EXPECT_EQ(4, ild(u"êT♥ "));
}

TEST(x86ild, test_885100) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rcx], dl
  */
  EXPECT_EQ(3, ild(u"êQ "));
}

TEST(x86ild, test_88442400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rsp], al
  */
  EXPECT_EQ(4, ild(u"êD$ "));
}

TEST(x86ild, test_88441400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rsp+rdx*1], al
  */
  EXPECT_EQ(4, ild(u"êD¶ "));
}

TEST(x86ild, test_880C33) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx+rsi*1], cl
  */
  EXPECT_EQ(3, ild(u"ê♀3"));
}

TEST(x86ild, test_66C782000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov word ptr [rdx], 0x0
  */
  EXPECT_EQ(9, ild(u"f╟é      "));
}

TEST(x86ild, test_6690) {
  /*
    ICLASS: NOP
    CATEGORY: NOP
    EXTENSION: BASE
    IFORM: NOP_90
    ISA_SET: I86
    SHORT: data16 nop
  */
  EXPECT_EQ(2, ild(u"fÉ"));
}

TEST(x86ild, test_668B940000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov dx, word ptr [rax+rax*1]
  */
  EXPECT_EQ(8, ild(u"fïö     "));
}

TEST(x86ild, test_668B4B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov cx, word ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"fïK "));
}

TEST(x86ild, test_66897B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rbx], di
  */
  EXPECT_EQ(4, ild(u"fë{ "));
}

TEST(x86ild, test_66894B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rbx], cx
  */
  EXPECT_EQ(4, ild(u"fëK "));
}

TEST(x86ild, test_668902) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rdx], ax
  */
  EXPECT_EQ(3, ild(u"fë☻"));
}

TEST(x86ild, test_66448B7500) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r14w, word ptr [rbp]
  */
  EXPECT_EQ(5, ild(u"fDïu "));
}

TEST(x86ild, test_66448B6F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r13w, word ptr [rdi]
  */
  EXPECT_EQ(5, ild(u"fDïo "));
}

TEST(x86ild, test_66448B4700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r8w, word ptr [rdi]
  */
  EXPECT_EQ(5, ild(u"fDïG "));
}

TEST(x86ild, test_4D89C3) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r11, r8
  */
  EXPECT_EQ(3, ild(u"Më├"));
}

TEST(x86ild, test_4D63EE) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_GPR32
    ISA_SET: LONGMODE
    SHORT: movsxd r13, r14d
  */
  EXPECT_EQ(3, ild(u"Mcε"));
}

TEST(x86ild, test_4D63C0) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_GPR32
    ISA_SET: LONGMODE
    SHORT: movsxd r8, r8d
  */
  EXPECT_EQ(3, ild(u"Mc└"));
}

TEST(x86ild, test_4C8B7700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r14, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"Lïw "));
}

TEST(x86ild, test_4C8B6F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r13, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"Lïo "));
}

TEST(x86ild, test_4C8B6D00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r13, qword ptr [rbp]
  */
  EXPECT_EQ(4, ild(u"Lïm "));
}

TEST(x86ild, test_4C8B6700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r12, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"Lïg "));
}

TEST(x86ild, test_4C8B6300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r12, qword ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"Lïc "));
}

TEST(x86ild, test_4C8B4700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r8, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"LïG "));
}

TEST(x86ild, test_4C8B4300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r8, qword ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"LïC "));
}

TEST(x86ild, test_4C8B2500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r12, qword ptr [rip]
  */
  EXPECT_EQ(7, ild(u"Lï%    "));
}

TEST(x86ild, test_4C89EE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rsi, r13
  */
  EXPECT_EQ(3, ild(u"Lëε"));
}

TEST(x86ild, test_4C89EA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdx, r13
  */
  EXPECT_EQ(3, ild(u"LëΩ"));
}

TEST(x86ild, test_4C89CE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rsi, r9
  */
  EXPECT_EQ(3, ild(u"Lë╬"));
}

TEST(x86ild, test_4C89C6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rsi, r8
  */
  EXPECT_EQ(3, ild(u"Lë╞"));
}

TEST(x86ild, test_4C897300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rbx], r14
  */
  EXPECT_EQ(4, ild(u"Lës "));
}

TEST(x86ild, test_4C896300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rbx], r12
  */
  EXPECT_EQ(4, ild(u"Lëc "));
}

TEST(x86ild, test_4A8B34C500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rsi, qword ptr [r8*8]
  */
  EXPECT_EQ(8, ild(u"Jï4┼    "));
}

TEST(x86ild, test_49D3ED) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_CL
    ISA_SET: I86
    SHORT: shr r13, cl
  */
  EXPECT_EQ(3, ild(u"I╙φ"));
}

TEST(x86ild, test_4989FD) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r13, rdi
  */
  EXPECT_EQ(3, ild(u"Ië²"));
}

TEST(x86ild, test_4989F6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r14, rsi
  */
  EXPECT_EQ(3, ild(u"Ië÷"));
}

TEST(x86ild, test_4989F5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r13, rsi
  */
  EXPECT_EQ(3, ild(u"Ië⌡"));
}

TEST(x86ild, test_4989F2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r10, rsi
  */
  EXPECT_EQ(3, ild(u"Ië≥"));
}

TEST(x86ild, test_4989E4) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r12, rsp
  */
  EXPECT_EQ(3, ild(u"IëΣ"));
}

TEST(x86ild, test_4989D0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r8, rdx
  */
  EXPECT_EQ(3, ild(u"Ië╨"));
}

TEST(x86ild, test_4989CD) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r13, rcx
  */
  EXPECT_EQ(3, ild(u"Ië═"));
}

TEST(x86ild, test_4989C5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r13, rax
  */
  EXPECT_EQ(3, ild(u"Ië┼"));
}

TEST(x86ild, test_4963F7) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_GPR32
    ISA_SET: LONGMODE
    SHORT: movsxd rsi, r15d
  */
  EXPECT_EQ(3, ild(u"Ic≈"));
}

TEST(x86ild, test_48D3C0) {
  /*
    ICLASS: ROL
    CATEGORY: ROTATE
    EXTENSION: BASE
    IFORM: ROL_GPRv_CL
    ISA_SET: I86
    SHORT: rol rax, cl
  */
  EXPECT_EQ(3, ild(u"H╙└"));
}

TEST(x86ild, test_48C7C000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMz
    ISA_SET: I86
    SHORT: mov rax, 0x0
  */
  EXPECT_EQ(7, ild(u"H╟└    "));
}

TEST(x86ild, test_48C70300000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov qword ptr [rbx], 0x0
  */
  EXPECT_EQ(7, ild(u"H╟♥    "));
}

TEST(x86ild, test_488B9000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rax]
  */
  EXPECT_EQ(7, ild(u"HïÉ    "));
}

TEST(x86ild, test_488B8600000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rsi]
  */
  EXPECT_EQ(7, ild(u"Hïå    "));
}

TEST(x86ild, test_488B7B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdi, qword ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"Hï{ "));
}

TEST(x86ild, test_488B7300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rsi, qword ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"Hïs "));
}

TEST(x86ild, test_488B4500) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rbp]
  */
  EXPECT_EQ(4, ild(u"HïE "));
}

TEST(x86ild, test_488B4100) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rcx]
  */
  EXPECT_EQ(4, ild(u"HïA "));
}

TEST(x86ild, test_488B37) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rsi, qword ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"Hï7"));
}

TEST(x86ild, test_488B3424) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rsi, qword ptr [rsp]
  */
  EXPECT_EQ(4, ild(u"Hï4$"));
}

TEST(x86ild, test_488B33) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rsi, qword ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"Hï3"));
}

TEST(x86ild, test_488B1D00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rbx, qword ptr [rip]
  */
  EXPECT_EQ(7, ild(u"Hï↔    "));
}

TEST(x86ild, test_488B14CA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rdx+rcx*8]
  */
  EXPECT_EQ(4, ild(u"Hï¶╩"));
}

TEST(x86ild, test_488B13) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"Hï‼"));
}

TEST(x86ild, test_488B12) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rdx]
  */
  EXPECT_EQ(3, ild(u"Hï↕"));
}

TEST(x86ild, test_488B0D00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rcx, qword ptr [rip]
  */
  EXPECT_EQ(7, ild(u"Hï♪    "));
}

TEST(x86ild, test_488B07) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"Hï•"));
}

TEST(x86ild, test_488B04F500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rsi*8]
  */
  EXPECT_EQ(8, ild(u"Hï♦⌡    "));
}

TEST(x86ild, test_488B03) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"Hï♥"));
}

TEST(x86ild, test_488B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rax]
  */
  EXPECT_EQ(3, ild(u"Hï "));
}

TEST(x86ild, test_4889FA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdx, rdi
  */
  EXPECT_EQ(3, ild(u"Hë·"));
}

TEST(x86ild, test_4889F8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rax, rdi
  */
  EXPECT_EQ(3, ild(u"Hë°"));
}

TEST(x86ild, test_4889F1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rcx, rsi
  */
  EXPECT_EQ(3, ild(u"Hë±"));
}

TEST(x86ild, test_4889D6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rsi, rdx
  */
  EXPECT_EQ(3, ild(u"Hë╓"));
}

TEST(x86ild, test_4889D3) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rbx, rdx
  */
  EXPECT_EQ(3, ild(u"Hë╙"));
}

TEST(x86ild, test_4889CA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdx, rcx
  */
  EXPECT_EQ(3, ild(u"Hë╩"));
}

TEST(x86ild, test_4889C5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rbp, rax
  */
  EXPECT_EQ(3, ild(u"Hë┼"));
}

TEST(x86ild, test_48895300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rbx], rdx
  */
  EXPECT_EQ(4, ild(u"HëS "));
}

TEST(x86ild, test_48893424) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rsp], rsi
  */
  EXPECT_EQ(4, ild(u"Hë4$"));
}

TEST(x86ild, test_488917) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rdi], rdx
  */
  EXPECT_EQ(3, ild(u"Hë↨"));
}

TEST(x86ild, test_488913) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rbx], rdx
  */
  EXPECT_EQ(3, ild(u"Hë‼"));
}

TEST(x86ild, test_488907) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rdi], rax
  */
  EXPECT_EQ(3, ild(u"Hë•"));
}

TEST(x86ild, test_488906) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rsi], rax
  */
  EXPECT_EQ(3, ild(u"Hë♠"));
}

TEST(x86ild, test_4863F8) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_GPR32
    ISA_SET: LONGMODE
    SHORT: movsxd rdi, eax
  */
  EXPECT_EQ(3, ild(u"Hc°"));
}

TEST(x86ild, test_4863D0) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_GPR32
    ISA_SET: LONGMODE
    SHORT: movsxd rdx, eax
  */
  EXPECT_EQ(3, ild(u"Hc╨"));
}

TEST(x86ild, test_4863C8) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_GPR32
    ISA_SET: LONGMODE
    SHORT: movsxd rcx, eax
  */
  EXPECT_EQ(3, ild(u"Hc╚"));
}

TEST(x86ild, test_4863C7) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_GPR32
    ISA_SET: LONGMODE
    SHORT: movsxd rax, edi
  */
  EXPECT_EQ(3, ild(u"Hc╟"));
}

TEST(x86ild, test_486312) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_MEMd
    ISA_SET: LONGMODE
    SHORT: movsxd rdx, dword ptr [rdx]
  */
  EXPECT_EQ(3, ild(u"Hc↕"));
}

TEST(x86ild, test_47882C0E) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [r14+r9*1], r13b
  */
  EXPECT_EQ(4, ild(u"Gê,♫"));
}

TEST(x86ild, test_458B9000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r10d, dword ptr [r8]
  */
  EXPECT_EQ(7, ild(u"EïÉ    "));
}

TEST(x86ild, test_4589D8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r8d, r11d
  */
  EXPECT_EQ(3, ild(u"Eë╪"));
}

TEST(x86ild, test_4589CB) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r11d, r9d
  */
  EXPECT_EQ(3, ild(u"Eë╦"));
}

TEST(x86ild, test_4589C1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r9d, r8d
  */
  EXPECT_EQ(3, ild(u"Eë┴"));
}

TEST(x86ild, test_4588F8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r8b, r15b
  */
  EXPECT_EQ(3, ild(u"Eê°"));
}

TEST(x86ild, test_4588D3) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r11b, r10b
  */
  EXPECT_EQ(3, ild(u"Eê╙"));
}

TEST(x86ild, test_4588D1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r9b, r10b
  */
  EXPECT_EQ(3, ild(u"Eê╤"));
}

TEST(x86ild, test_4588C7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r15b, r8b
  */
  EXPECT_EQ(3, ild(u"Eê╟"));
}

TEST(x86ild, test_45883424) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [r12], r14b
  */
  EXPECT_EQ(4, ild(u"Eê4$"));
}

TEST(x86ild, test_448B8100000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r8d, dword ptr [rcx]
  */
  EXPECT_EQ(7, ild(u"Dïü    "));
}

TEST(x86ild, test_448B7D00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r15d, dword ptr [rbp]
  */
  EXPECT_EQ(4, ild(u"Dï} "));
}

TEST(x86ild, test_448B7700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r14d, dword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"Dïw "));
}

TEST(x86ild, test_448B6700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r12d, dword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"Dïg "));
}

TEST(x86ild, test_448B4700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r8d, dword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"DïG "));
}

TEST(x86ild, test_448B4500) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r8d, dword ptr [rbp]
  */
  EXPECT_EQ(4, ild(u"DïE "));
}

TEST(x86ild, test_448B0424) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r8d, dword ptr [rsp]
  */
  EXPECT_EQ(4, ild(u"Dï♦$"));
}

TEST(x86ild, test_448A5300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov r10b, byte ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"DèS "));
}

TEST(x86ild, test_448A4E00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov r9b, byte ptr [rsi]
  */
  EXPECT_EQ(4, ild(u"DèN "));
}

TEST(x86ild, test_448A4600) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov r8b, byte ptr [rsi]
  */
  EXPECT_EQ(4, ild(u"DèF "));
}

TEST(x86ild, test_448A01) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov r8b, byte ptr [rcx]
  */
  EXPECT_EQ(3, ild(u"Dè☺"));
}

TEST(x86ild, test_4489E9) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ecx, r13d
  */
  EXPECT_EQ(3, ild(u"DëΘ"));
}

TEST(x86ild, test_4489E8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, r13d
  */
  EXPECT_EQ(3, ild(u"DëΦ"));
}

TEST(x86ild, test_4489CE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov esi, r9d
  */
  EXPECT_EQ(3, ild(u"Dë╬"));
}

TEST(x86ild, test_4489C8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, r9d
  */
  EXPECT_EQ(3, ild(u"Dë╚"));
}

TEST(x86ild, test_4489C2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edx, r8d
  */
  EXPECT_EQ(3, ild(u"Dë┬"));
}

TEST(x86ild, test_4489C0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, r8d
  */
  EXPECT_EQ(3, ild(u"Dë└"));
}

TEST(x86ild, test_44898900000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rcx], r9d
  */
  EXPECT_EQ(7, ild(u"Dëë    "));
}

TEST(x86ild, test_4488E8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov al, r13b
  */
  EXPECT_EQ(3, ild(u"DêΦ"));
}

TEST(x86ild, test_4488D9) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov cl, r11b
  */
  EXPECT_EQ(3, ild(u"Dê┘"));
}

TEST(x86ild, test_4488CE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov sil, r9b
  */
  EXPECT_EQ(3, ild(u"Dê╬"));
}

TEST(x86ild, test_4488CD) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov bpl, r9b
  */
  EXPECT_EQ(3, ild(u"Dê═"));
}

TEST(x86ild, test_4488C5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov bpl, r8b
  */
  EXPECT_EQ(3, ild(u"Dê┼"));
}

TEST(x86ild, test_44886300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], r12b
  */
  EXPECT_EQ(4, ild(u"Dêc "));
}

TEST(x86ild, test_44884B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], r9b
  */
  EXPECT_EQ(4, ild(u"DêK "));
}

TEST(x86ild, test_44884700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rdi], r8b
  */
  EXPECT_EQ(4, ild(u"DêG "));
}

TEST(x86ild, test_44884000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rax], r8b
  */
  EXPECT_EQ(4, ild(u"Dê@ "));
}

TEST(x86ild, test_43C6040E00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [r14+r9*1], 0x0
  */
  EXPECT_EQ(5, ild(u"C╞♦♫ "));
}

TEST(x86ild, test_428A1400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rax+r8*1]
  */
  EXPECT_EQ(4, ild(u"Bè¶ "));
}

TEST(x86ild, test_428A0406) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rsi+r8*1]
  */
  EXPECT_EQ(4, ild(u"Bè♦♠"));
}

TEST(x86ild, test_42880407) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rdi+r8*1], al
  */
  EXPECT_EQ(4, ild(u"Bê♦•"));
}

TEST(x86ild, test_41FF14C4) {
  /*
    ICLASS: CALL_NEAR
    CATEGORY: CALL
    EXTENSION: BASE
    IFORM: CALL_NEAR_MEMv
    ISA_SET: I86
    SHORT: call qword ptr [r12+rax*8]
  */
  EXPECT_EQ(4, ild(u"Aλ¶─"));
}

TEST(x86ild, test_41C6042400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [r12], 0x0
  */
  EXPECT_EQ(5, ild(u"A╞♦$ "));
}

TEST(x86ild, test_41C60000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [r8], 0x0
  */
  EXPECT_EQ(4, ild(u"A╞  "));
}

TEST(x86ild, test_418B8000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [r8]
  */
  EXPECT_EQ(7, ild(u"AïÇ    "));
}

TEST(x86ild, test_418A0C14) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [r12+rdx*1]
  */
  EXPECT_EQ(4, ild(u"Aè♀¶"));
}

TEST(x86ild, test_418A0404) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [r12+rax*1]
  */
  EXPECT_EQ(4, ild(u"Aè♦♦"));
}

TEST(x86ild, test_418A0400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [r8+rax*1]
  */
  EXPECT_EQ(4, ild(u"Aè♦ "));
}

TEST(x86ild, test_4189FB) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r11d, edi
  */
  EXPECT_EQ(3, ild(u"Aë√"));
}

TEST(x86ild, test_4189F8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r8d, edi
  */
  EXPECT_EQ(3, ild(u"Aë°"));
}

TEST(x86ild, test_4189DC) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r12d, ebx
  */
  EXPECT_EQ(3, ild(u"Aë▄"));
}

TEST(x86ild, test_4189D7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r15d, edx
  */
  EXPECT_EQ(3, ild(u"Aë╫"));
}

TEST(x86ild, test_4189D6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r14d, edx
  */
  EXPECT_EQ(3, ild(u"Aë╓"));
}

TEST(x86ild, test_4189D3) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r11d, edx
  */
  EXPECT_EQ(3, ild(u"Aë╙"));
}

TEST(x86ild, test_4189CB) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r11d, ecx
  */
  EXPECT_EQ(3, ild(u"Aë╦"));
}

TEST(x86ild, test_4189C1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r9d, eax
  */
  EXPECT_EQ(3, ild(u"Aë┴"));
}

TEST(x86ild, test_4189C0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r8d, eax
  */
  EXPECT_EQ(3, ild(u"Aë└"));
}

TEST(x86ild, test_41898800000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [r8], ecx
  */
  EXPECT_EQ(7, ild(u"Aëê    "));
}

TEST(x86ild, test_4188F2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r10b, sil
  */
  EXPECT_EQ(3, ild(u"Aê≥"));
}

TEST(x86ild, test_4188CE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r14b, cl
  */
  EXPECT_EQ(3, ild(u"Aê╬"));
}

TEST(x86ild, test_4188C3) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r11b, al
  */
  EXPECT_EQ(3, ild(u"Aê├"));
}

TEST(x86ild, test_4188C2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r10b, al
  */
  EXPECT_EQ(3, ild(u"Aê┬"));
}

TEST(x86ild, test_4188C1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov r9b, al
  */
  EXPECT_EQ(3, ild(u"Aê┴"));
}

TEST(x86ild, test_41885000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [r8], dl
  */
  EXPECT_EQ(4, ild(u"AêP "));
}

TEST(x86ild, test_4158) {
  /*
    ICLASS: POP
    CATEGORY: POP
    EXTENSION: BASE
    IFORM: POP_GPRv_58
    ISA_SET: I86
    SHORT: pop r8
  */
  EXPECT_EQ(2, ild(u"AX"));
}

TEST(x86ild, test_40B600) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_IMMb_B0
    ISA_SET: I86
    SHORT: mov sil, 0x0
  */
  EXPECT_EQ(3, ild(u"@╢ "));
}

TEST(x86ild, test_408A7D00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dil, byte ptr [rbp]
  */
  EXPECT_EQ(4, ild(u"@è} "));
}

TEST(x86ild, test_4088FA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dl, dil
  */
  EXPECT_EQ(3, ild(u"@ê·"));
}

TEST(x86ild, test_4088EA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dl, bpl
  */
  EXPECT_EQ(3, ild(u"@êΩ"));
}

TEST(x86ild, test_4088D7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dil, dl
  */
  EXPECT_EQ(3, ild(u"@ê╫"));
}

TEST(x86ild, test_4088C5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov bpl, al
  */
  EXPECT_EQ(3, ild(u"@ê┼"));
}

TEST(x86ild, test_40887B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], dil
  */
  EXPECT_EQ(4, ild(u"@ê{ "));
}

TEST(x86ild, test_0F8E00000000) {
  /*
    ICLASS: JLE
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JLE_RELBRd
    ISA_SET: I86
    SHORT: jle 0x6
  */
  EXPECT_EQ(6, ild(u"☼Ä    "));
}

TEST(x86ild, test_0F8800000000) {
  /*
    ICLASS: JS
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JS_RELBRd
    ISA_SET: I86
    SHORT: js 0x6
  */
  EXPECT_EQ(6, ild(u"☼ê    "));
}

TEST(x86ild, test_0F8200000000) {
  /*
    ICLASS: JB
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JB_RELBRd
    ISA_SET: I86
    SHORT: jb 0x6
  */
  EXPECT_EQ(6, ild(u"☼é    "));
}
