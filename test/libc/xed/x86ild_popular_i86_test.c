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

TEST(x86ild, test_E800000000) {
  /*
    ICLASS: CALL_NEAR
    CATEGORY: CALL
    EXTENSION: BASE
    IFORM: CALL_NEAR_RELBRd
    ISA_SET: I86
    SHORT: call 0x5
  */
  EXPECT_EQ(5, ild(u"Φ    "));
  EXPECT_EQ(5, ildlegacy(u"Φ    "));
  EXPECT_EQ(3, ildreal(u"Φ  "));
}

TEST(x86ild, test_C3) {
  /*
    ICLASS: RET_NEAR
    CATEGORY: RET
    EXTENSION: BASE
    IFORM: RET_NEAR
    ISA_SET: I86
    SHORT: ret
  */
  EXPECT_EQ(1, ild(u"├"));
}

TEST(x86ild, test_5A) {
  /*
    ICLASS: POP
    CATEGORY: POP
    EXTENSION: BASE
    IFORM: POP_GPRv_58
    ISA_SET: I86
    SHORT: pop rdx
  */
  EXPECT_EQ(1, ild(u"Z"));
}

TEST(x86ild, test_4889DF) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdi, rbx
  */
  EXPECT_EQ(3, ild(u"Hë▀"));
}

TEST(x86ild, test_BA00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov edx, 0x0
  */
  EXPECT_EQ(5, ild(u"║    "));
}

TEST(x86ild, test_7500) {
  /*
    ICLASS: JNZ
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNZ_RELBRb
    ISA_SET: I86
    SHORT: jnz 0x2
  */
  EXPECT_EQ(2, ild(u"u "));
}

TEST(x86ild, test_B800000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov eax, 0x0
  */
  EXPECT_EQ(5, ild(u"╕    "));
}

TEST(x86ild, test_7400) {
  /*
    ICLASS: JZ
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JZ_RELBRb
    ISA_SET: I86
    SHORT: jz 0x2
  */
  EXPECT_EQ(2, ild(u"t "));
}

TEST(x86ild, test_BE00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov esi, 0x0
  */
  EXPECT_EQ(5, ild(u"╛    "));
}

TEST(x86ild, test_4889C1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rcx, rax
  */
  EXPECT_EQ(3, ild(u"Hë┴"));
}

TEST(x86ild, test_EB00) {
  /*
    ICLASS: JMP
    CATEGORY: UNCOND_BR
    EXTENSION: BASE
    IFORM: JMP_RELBRb
    ISA_SET: I86
    SHORT: jmp 0x2
  */
  EXPECT_EQ(2, ild(u"δ "));
}

TEST(x86ild, test_8B34D500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov esi, dword ptr [rdx*8]
  */
  EXPECT_EQ(7, ild(u"ï4╒    "));
}

TEST(x86ild, test_E900000000) {
  /*
    ICLASS: JMP
    CATEGORY: UNCOND_BR
    EXTENSION: BASE
    IFORM: JMP_RELBRd
    ISA_SET: I86
    SHORT: jmp 0x5
  */
  EXPECT_EQ(5, ild(u"Θ    "));
}

TEST(x86ild, test_8B7300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov esi, dword ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"ïs "));
}

TEST(x86ild, test_8B04D500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rdx*8]
  */
  EXPECT_EQ(7, ild(u"ï♦╒    "));
}

TEST(x86ild, test_89D0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, edx
  */
  EXPECT_EQ(2, ild(u"ë╨"));
}

TEST(x86ild, test_0F8400000000) {
  /*
    ICLASS: JZ
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JZ_RELBRd
    ISA_SET: I86
    SHORT: jz 0x6
  */
  EXPECT_EQ(6, ild(u"☼ä    "));
}

TEST(x86ild, test_8B14CD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rcx*8]
  */
  EXPECT_EQ(7, ild(u"ï¶═    "));
}

TEST(x86ild, test_8B34CD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov esi, dword ptr [rcx*8]
  */
  EXPECT_EQ(7, ild(u"ï4═    "));
}

TEST(x86ild, test_8A4300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"èC "));
}

TEST(x86ild, test_8B4700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"ïG "));
}

TEST(x86ild, test_5B) {
  /*
    ICLASS: POP
    CATEGORY: POP
    EXTENSION: BASE
    IFORM: POP_GPRv_58
    ISA_SET: I86
    SHORT: pop rbx
  */
  EXPECT_EQ(1, ild(u"["));
}

TEST(x86ild, test_66C705000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov word ptr [rip], 0x0
  */
  EXPECT_EQ(9, ild(u"f╟♣      "));
}

TEST(x86ild, test_8A4700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"èG "));
}

TEST(x86ild, test_FFD0) {
  /*
    ICLASS: CALL_NEAR
    CATEGORY: CALL
    EXTENSION: BASE
    IFORM: CALL_NEAR_GPRv
    ISA_SET: I86
    SHORT: call rax
  */
  EXPECT_EQ(2, ild(u"λ╨"));
}

TEST(x86ild, test_C7050000000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov dword ptr [rip], 0x0
  */
  EXPECT_EQ(10, ild(u"╟♣        "));
}

TEST(x86ild, test_53) {
  /*
    ICLASS: PUSH
    CATEGORY: PUSH
    EXTENSION: BASE
    IFORM: PUSH_GPRv_50
    ISA_SET: I86
    SHORT: push rbx
  */
  EXPECT_EQ(1, ild(u"S"));
}

TEST(x86ild, test_0F8500000000) {
  /*
    ICLASS: JNZ
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNZ_RELBRd
    ISA_SET: I86
    SHORT: jnz 0x6
  */
  EXPECT_EQ(6, ild(u"☼à    "));
}

TEST(x86ild, test_C6050000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rip], 0x0
  */
  EXPECT_EQ(7, ild(u"╞♣     "));
}

TEST(x86ild, test_668B7300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov si, word ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"fïs "));
}

TEST(x86ild, test_488B4300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"HïC "));
}

TEST(x86ild, test_BF00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov edi, 0x0
  */
  EXPECT_EQ(5, ild(u"┐    "));
}

TEST(x86ild, test_4889FB) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rbx, rdi
  */
  EXPECT_EQ(3, ild(u"Hë√"));
}

TEST(x86ild, test_48B80000000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov rax, 0x0
  */
  EXPECT_EQ(10, ild(u"H╕        "));
}

TEST(x86ild, test_48BA0000000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov rdx, 0x0
  */
  EXPECT_EQ(10, ild(u"H║        "));
}

TEST(x86ild, test_488B5300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"HïS "));
}

TEST(x86ild, test_66898200000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rdx], ax
  */
  EXPECT_EQ(7, ild(u"fëé    "));
}

TEST(x86ild, test_66894700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rdi], ax
  */
  EXPECT_EQ(4, ild(u"fëG "));
}

TEST(x86ild, test_7700) {
  /*
    ICLASS: JNBE
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNBE_RELBRb
    ISA_SET: I86
    SHORT: jnbe 0x2
  */
  EXPECT_EQ(2, ild(u"w "));
}

TEST(x86ild, test_894700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdi], eax
  */
  EXPECT_EQ(3, ild(u"ëG "));
}

TEST(x86ild, test_66899000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rax], dx
  */
  EXPECT_EQ(7, ild(u"fëÉ    "));
}

TEST(x86ild, test_4889D0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rax, rdx
  */
  EXPECT_EQ(3, ild(u"Hë╨"));
}

TEST(x86ild, test_668B4700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ax, word ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"fïG "));
}

TEST(x86ild, test_89C2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edx, eax
  */
  EXPECT_EQ(2, ild(u"ë┬"));
}

TEST(x86ild, test_668B8000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ax, word ptr [rax]
  */
  EXPECT_EQ(7, ild(u"fïÇ    "));
}

TEST(x86ild, test_668B9500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov dx, word ptr [rbp]
  */
  EXPECT_EQ(7, ild(u"fïò    "));
}

TEST(x86ild, test_5D) {
  /*
    ICLASS: POP
    CATEGORY: POP
    EXTENSION: BASE
    IFORM: POP_GPRv_58
    ISA_SET: I86
    SHORT: pop rbp
  */
  EXPECT_EQ(1, ild(u"]"));
}

TEST(x86ild, test_55) {
  /*
    ICLASS: PUSH
    CATEGORY: PUSH
    EXTENSION: BASE
    IFORM: PUSH_GPRv_50
    ISA_SET: I86
    SHORT: push rbp
  */
  EXPECT_EQ(1, ild(u"U"));
}

TEST(x86ild, test_89C8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, ecx
  */
  EXPECT_EQ(2, ild(u"ë╚"));
}

TEST(x86ild, test_8B0CD500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ecx, dword ptr [rdx*8]
  */
  EXPECT_EQ(7, ild(u"ï♀╒    "));
}

TEST(x86ild, test_8A4F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"èO "));
}

TEST(x86ild, test_8B148500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rax*4]
  */
  EXPECT_EQ(7, ild(u"ï¶à    "));
}

TEST(x86ild, test_8A5300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"èS "));
}

TEST(x86ild, test_884F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rdi], cl
  */
  EXPECT_EQ(3, ild(u"êO "));
}

TEST(x86ild, test_8B8000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rax]
  */
  EXPECT_EQ(6, ild(u"ïÇ    "));
}

TEST(x86ild, test_8B9500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rbp]
  */
  EXPECT_EQ(6, ild(u"ïò    "));
}

TEST(x86ild, test_8A5700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"èW "));
}

TEST(x86ild, test_884300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], al
  */
  EXPECT_EQ(3, ild(u"êC "));
}

TEST(x86ild, test_884700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rdi], al
  */
  EXPECT_EQ(3, ild(u"êG "));
}

TEST(x86ild, test_8A4800) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rax]
  */
  EXPECT_EQ(3, ild(u"èH "));
}

TEST(x86ild, test_66C780000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov word ptr [rax], 0x0
  */
  EXPECT_EQ(9, ild(u"f╟Ç      "));
}

TEST(x86ild, test_885700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rdi], dl
  */
  EXPECT_EQ(3, ild(u"êW "));
}

TEST(x86ild, test_8B5700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"ïW "));
}

TEST(x86ild, test_48C7050000000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov qword ptr [rip], 0x0
  */
  EXPECT_EQ(11, ild(u"H╟♣        "));
}

TEST(x86ild, test_8B4300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"ïC "));
}

TEST(x86ild, test_E200) {
  /*
    ICLASS: LOOP
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: LOOP_RELBRb
    ISA_SET: I86
    SHORT: loop 0x2
  */
  EXPECT_EQ(2, ild(u"Γ "));
}

TEST(x86ild, test_8B049500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rdx*4]
  */
  EXPECT_EQ(7, ild(u"ï♦ò    "));
}

TEST(x86ild, test_89F0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, esi
  */
  EXPECT_EQ(2, ild(u"ë≡"));
}

TEST(x86ild, test_668B5300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov dx, word ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"fïS "));
}

TEST(x86ild, test_8B7700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov esi, dword ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"ïw "));
}

TEST(x86ild, test_66897700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rdi], si
  */
  EXPECT_EQ(4, ild(u"fëw "));
}

TEST(x86ild, test_66894300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rbx], ax
  */
  EXPECT_EQ(4, ild(u"fëC "));
}

TEST(x86ild, test_8902) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdx], eax
  */
  EXPECT_EQ(2, ild(u"ë☻"));
}

TEST(x86ild, test_408A7000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov sil, byte ptr [rax]
  */
  EXPECT_EQ(4, ild(u"@èp "));
}

TEST(x86ild, test_88C2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dl, al
  */
  EXPECT_EQ(2, ild(u"ê┬"));
}

TEST(x86ild, test_668B14AD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov dx, word ptr [rbp*4]
  */
  EXPECT_EQ(8, ild(u"fï¶¡    "));
}

TEST(x86ild, test_668B4300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ax, word ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"fïC "));
}

TEST(x86ild, test_4088CE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov sil, cl
  */
  EXPECT_EQ(3, ild(u"@ê╬"));
}

TEST(x86ild, test_89C1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ecx, eax
  */
  EXPECT_EQ(2, ild(u"ë┴"));
}

TEST(x86ild, test_8B14D500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rdx*8]
  */
  EXPECT_EQ(7, ild(u"ï¶╒    "));
}

TEST(x86ild, test_8B149500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rdx*4]
  */
  EXPECT_EQ(7, ild(u"ï¶ò    "));
}

TEST(x86ild, test_884B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], cl
  */
  EXPECT_EQ(3, ild(u"êK "));
}

TEST(x86ild, test_488B4700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"HïG "));
}

TEST(x86ild, test_8B4F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ecx, dword ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"ïO "));
}

TEST(x86ild, test_8A08) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rax]
  */
  EXPECT_EQ(2, ild(u"è◘"));
}

TEST(x86ild, test_668B5700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov dx, word ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"fïW "));
}

TEST(x86ild, test_C6470000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rdi], 0x0
  */
  EXPECT_EQ(4, ild(u"╞G  "));
}

TEST(x86ild, test_66895700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rdi], dx
  */
  EXPECT_EQ(4, ild(u"fëW "));
}

TEST(x86ild, test_FF24D500000000) {
  /*
    ICLASS: JMP
    CATEGORY: UNCOND_BR
    EXTENSION: BASE
    IFORM: JMP_MEMv
    ISA_SET: I86
    SHORT: jmp qword ptr [rdx*8]
  */
  EXPECT_EQ(7, ild(u"λ$╒    "));
}

TEST(x86ild, test_8A4B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"èK "));
}

TEST(x86ild, test_7200) {
  /*
    ICLASS: JB
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JB_RELBRb
    ISA_SET: I86
    SHORT: jb 0x2
  */
  EXPECT_EQ(2, ild(u"r "));
}

TEST(x86ild, test_88C1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov cl, al
  */
  EXPECT_EQ(2, ild(u"ê┴"));
}

TEST(x86ild, test_668B14D500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov dx, word ptr [rdx*8]
  */
  EXPECT_EQ(8, ild(u"fï¶╒    "));
}

TEST(x86ild, test_8A4000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rax]
  */
  EXPECT_EQ(3, ild(u"è@ "));
}

TEST(x86ild, test_C6430000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], 0x0
  */
  EXPECT_EQ(4, ild(u"╞C  "));
}

TEST(x86ild, test_8B14ED00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rbp*8]
  */
  EXPECT_EQ(7, ild(u"ï¶φ    "));
}

TEST(x86ild, test_668B14ED00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov dx, word ptr [rbp*8]
  */
  EXPECT_EQ(8, ild(u"fï¶φ    "));
}

TEST(x86ild, test_408A7300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov sil, byte ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"@ès "));
}

TEST(x86ild, test_7800) {
  /*
    ICLASS: JS
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JS_RELBRb
    ISA_SET: I86
    SHORT: js 0x2
  */
  EXPECT_EQ(2, ild(u"x "));
}

TEST(x86ild, test_40887300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], sil
  */
  EXPECT_EQ(4, ild(u"@ês "));
}

TEST(x86ild, test_48890500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rip], rax
  */
  EXPECT_EQ(7, ild(u"Hë♣    "));
}

TEST(x86ild, test_B900000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov ecx, 0x0
  */
  EXPECT_EQ(5, ild(u"╣    "));
}

TEST(x86ild, test_8B742400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov esi, dword ptr [rsp]
  */
  EXPECT_EQ(4, ild(u"ït$ "));
}

TEST(x86ild, test_89F2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edx, esi
  */
  EXPECT_EQ(2, ild(u"ë≥"));
}

TEST(x86ild, test_4088F7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dil, sil
  */
  EXPECT_EQ(3, ild(u"@ê≈"));
}

TEST(x86ild, test_668B942D00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov dx, word ptr [rbp+rbp*1]
  */
  EXPECT_EQ(8, ild(u"fïö-    "));
}

TEST(x86ild, test_408A7700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov sil, byte ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"@èw "));
}

TEST(x86ild, test_89742400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rsp], esi
  */
  EXPECT_EQ(4, ild(u"ët$ "));
}

TEST(x86ild, test_4889EF) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdi, rbp
  */
  EXPECT_EQ(3, ild(u"Hë∩"));
}

TEST(x86ild, test_7900) {
  /*
    ICLASS: JNS
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNS_RELBRb
    ISA_SET: I86
    SHORT: jns 0x2
  */
  EXPECT_EQ(2, ild(u"y "));
}

TEST(x86ild, test_89FF) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edi, edi
  */
  EXPECT_EQ(2, ild(u"ëλ"));
}

TEST(x86ild, test_89CE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov esi, ecx
  */
  EXPECT_EQ(2, ild(u"ë╬"));
}

TEST(x86ild, test_7600) {
  /*
    ICLASS: JBE
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JBE_RELBRb
    ISA_SET: I86
    SHORT: jbe 0x2
  */
  EXPECT_EQ(2, ild(u"v "));
}

TEST(x86ild, test_4889C6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rsi, rax
  */
  EXPECT_EQ(3, ild(u"Hë╞"));
}

TEST(x86ild, test_8B048500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rax*4]
  */
  EXPECT_EQ(7, ild(u"ï♦à    "));
}

TEST(x86ild, test_415C) {
  /*
    ICLASS: POP
    CATEGORY: POP
    EXTENSION: BASE
    IFORM: POP_GPRv_58
    ISA_SET: I86
    SHORT: pop r12
  */
  EXPECT_EQ(2, ild(u"A\\"));
}

TEST(x86ild, test_488B04C500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rax*8]
  */
  EXPECT_EQ(8, ild(u"Hï♦┼    "));
}

TEST(x86ild, test_885300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], dl
  */
  EXPECT_EQ(3, ild(u"êS "));
}

TEST(x86ild, test_4889FD) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rbp, rdi
  */
  EXPECT_EQ(3, ild(u"Hë²"));
}

TEST(x86ild, test_4088F0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov al, sil
  */
  EXPECT_EQ(3, ild(u"@ê≡"));
}

TEST(x86ild, test_89442400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rsp], eax
  */
  EXPECT_EQ(4, ild(u"ëD$ "));
}

TEST(x86ild, test_894300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rbx], eax
  */
  EXPECT_EQ(3, ild(u"ëC "));
}

TEST(x86ild, test_88D0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov al, dl
  */
  EXPECT_EQ(2, ild(u"ê╨"));
}

TEST(x86ild, test_4154) {
  /*
    ICLASS: PUSH
    CATEGORY: PUSH
    EXTENSION: BASE
    IFORM: PUSH_GPRv_50
    ISA_SET: I86
    SHORT: push r12
  */
  EXPECT_EQ(2, ild(u"AT"));
}

TEST(x86ild, test_7300) {
  /*
    ICLASS: JNB
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNB_RELBRb
    ISA_SET: I86
    SHORT: jnb 0x2
  */
  EXPECT_EQ(2, ild(u"s "));
}

TEST(x86ild, test_448A4700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov r8b, byte ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"DèG "));
}

TEST(x86ild, test_89F7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edi, esi
  */
  EXPECT_EQ(2, ild(u"ë≈"));
}

TEST(x86ild, test_89C0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, eax
  */
  EXPECT_EQ(2, ild(u"ë└"));
}

TEST(x86ild, test_668B4F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov cx, word ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"fïO "));
}

TEST(x86ild, test_BB00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov ebx, 0x0
  */
  EXPECT_EQ(5, ild(u"╗    "));
}

TEST(x86ild, test_88C8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov al, cl
  */
  EXPECT_EQ(2, ild(u"ê╚"));
}

TEST(x86ild, test_66894F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rdi], cx
  */
  EXPECT_EQ(4, ild(u"fëO "));
}

TEST(x86ild, test_415D) {
  /*
    ICLASS: POP
    CATEGORY: POP
    EXTENSION: BASE
    IFORM: POP_GPRv_58
    ISA_SET: I86
    SHORT: pop r13
  */
  EXPECT_EQ(2, ild(u"A]"));
}

TEST(x86ild, test_FFE0) {
  /*
    ICLASS: JMP
    CATEGORY: UNCOND_BR
    EXTENSION: BASE
    IFORM: JMP_GPRv
    ISA_SET: I86
    SHORT: jmp rax
  */
  EXPECT_EQ(2, ild(u"λα"));
}

TEST(x86ild, test_FFD2) {
  /*
    ICLASS: CALL_NEAR
    CATEGORY: CALL
    EXTENSION: BASE
    IFORM: CALL_NEAR_GPRv
    ISA_SET: I86
    SHORT: call rdx
  */
  EXPECT_EQ(2, ild(u"λ╥"));
}

TEST(x86ild, test_FF24C500000000) {
  /*
    ICLASS: JMP
    CATEGORY: UNCOND_BR
    EXTENSION: BASE
    IFORM: JMP_MEMv
    ISA_SET: I86
    SHORT: jmp qword ptr [rax*8]
  */
  EXPECT_EQ(7, ild(u"λ$┼    "));
}

TEST(x86ild, test_E000) {
  /*
    ICLASS: LOOPNE
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: LOOPNE_RELBRb
    ISA_SET: I86
    SHORT: loopne 0x2
  */
  EXPECT_EQ(2, ild(u"α "));
}

TEST(x86ild, test_8B4000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rax]
  */
  EXPECT_EQ(3, ild(u"ï@ "));
}

TEST(x86ild, test_89D1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ecx, edx
  */
  EXPECT_EQ(2, ild(u"ë╤"));
}

TEST(x86ild, test_8B14AD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rbp*4]
  */
  EXPECT_EQ(7, ild(u"ï¶¡    "));
}

TEST(x86ild, test_895700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdi], edx
  */
  EXPECT_EQ(3, ild(u"ëW "));
}

TEST(x86ild, test_488B5700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"HïW "));
}

TEST(x86ild, test_4155) {
  /*
    ICLASS: PUSH
    CATEGORY: PUSH
    EXTENSION: BASE
    IFORM: PUSH_GPRv_50
    ISA_SET: I86
    SHORT: push r13
  */
  EXPECT_EQ(2, ild(u"AU"));
}

TEST(x86ild, test_668B840000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ax, word ptr [rax+rax*1]
  */
  EXPECT_EQ(8, ild(u"fïä     "));
}

TEST(x86ild, test_488B8000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rax]
  */
  EXPECT_EQ(7, ild(u"HïÇ    "));
}

TEST(x86ild, test_488B4B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rcx, qword ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"HïK "));
}

TEST(x86ild, test_0F8700000000) {
  /*
    ICLASS: JNBE
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNBE_RELBRd
    ISA_SET: I86
    SHORT: jnbe 0x6
  */
  EXPECT_EQ(6, ild(u"☼ç    "));
}

TEST(x86ild, test_8B5300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"ïS "));
}

TEST(x86ild, test_668B048500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ax, word ptr [rax*4]
  */
  EXPECT_EQ(8, ild(u"fï♦à    "));
}

TEST(x86ild, test_66898100000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rcx], ax
  */
  EXPECT_EQ(7, ild(u"fëü    "));
}

TEST(x86ild, test_488B17) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
g    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rdi]
  */
  EXPECT_EQ(3, ild(u"Hï↨"));
}

TEST(x86ild, test_4889C2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdx, rax
  */
  EXPECT_EQ(3, ild(u"Hë┬"));
}

TEST(x86ild, test_C7400000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov dword ptr [rax], 0x0
  */
  EXPECT_EQ(7, ild(u"╟@     "));
}

TEST(x86ild, test_4C89E7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdi, r12
  */
  EXPECT_EQ(3, ild(u"Lëτ"));
}

TEST(x86ild, test_488B3B) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdi, qword ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"Hï;"));
}

TEST(x86ild, test_4889EE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rsi, rbp
  */
  EXPECT_EQ(3, ild(u"Hëε"));
}

TEST(x86ild, test_4488C0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov al, r8b
  */
  EXPECT_EQ(3, ild(u"Dê└"));
}

TEST(x86ild, test_415E) {
  /*
    ICLASS: POP
    CATEGORY: POP
    EXTENSION: BASE
    IFORM: POP_GPRv_58
    ISA_SET: I86
    SHORT: pop r14
  */
  EXPECT_EQ(2, ild(u"A^"));
}

TEST(x86ild, test_4156) {
  /*
    ICLASS: PUSH
    CATEGORY: PUSH
    EXTENSION: BASE
    IFORM: PUSH_GPRv_50
    ISA_SET: I86
    SHORT: push r14
  */
  EXPECT_EQ(2, ild(u"AV"));
}

TEST(x86ild, test_8B542400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rsp]
  */
  EXPECT_EQ(4, ild(u"ïT$ "));
}

TEST(x86ild, test_89C7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edi, eax
  */
  EXPECT_EQ(2, ild(u"ë╟"));
}

TEST(x86ild, test_488B8700000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rdi]
  */
  EXPECT_EQ(7, ild(u"Hïç    "));
}

TEST(x86ild, test_448A4F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov r9b, byte ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"DèO "));
}

TEST(x86ild, test_408A30) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov sil, byte ptr [rax]
  */
  EXPECT_EQ(3, ild(u"@è0"));
}

TEST(x86ild, test_4189C6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r14d, eax
  */
  EXPECT_EQ(3, ild(u"Aë╞"));
}

TEST(x86ild, test_897300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rbx], esi
  */
  EXPECT_EQ(3, ild(u"ës "));
}

TEST(x86ild, test_894F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdi], ecx
  */
  EXPECT_EQ(3, ild(u"ëO "));
}

TEST(x86ild, test_4489E6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov esi, r12d
  */
  EXPECT_EQ(3, ild(u"Dëμ"));
}

TEST(x86ild, test_4488C8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov al, r9b
  */
  EXPECT_EQ(3, ild(u"Dê╚"));
}

TEST(x86ild, test_8A10) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rax]
  */
  EXPECT_EQ(2, ild(u"è►"));
}

TEST(x86ild, test_894200) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdx], eax
  */
  EXPECT_EQ(3, ild(u"ëB "));
}

TEST(x86ild, test_88D1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov cl, dl
  */
  EXPECT_EQ(2, ild(u"ê╤"));
}

TEST(x86ild, test_58) {
  /*
    ICLASS: POP
    CATEGORY: POP
    EXTENSION: BASE
    IFORM: POP_GPRv_58
    ISA_SET: I86
    SHORT: pop rax
  */
  EXPECT_EQ(1, ild(u"X"));
}

TEST(x86ild, test_48B90000000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov rcx, 0x0
  */
  EXPECT_EQ(10, ild(u"H╣        "));
}

TEST(x86ild, test_488B14D500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rdx*8]
  */
  EXPECT_EQ(8, ild(u"Hï¶╒    "));
}

TEST(x86ild, test_4489F2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edx, r14d
  */
  EXPECT_EQ(3, ild(u"Dë≥"));
}

TEST(x86ild, test_4489EA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edx, r13d
  */
  EXPECT_EQ(3, ild(u"DëΩ"));
}

TEST(x86ild, test_8B04BD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rdi*4]
  */
  EXPECT_EQ(7, ild(u"ï♦╜    "));
}

TEST(x86ild, test_89F1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ecx, esi
  */
  EXPECT_EQ(2, ild(u"ë±"));
}

TEST(x86ild, test_488B7F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdi, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"Hï⌂ "));
}

TEST(x86ild, test_4488C2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dl, r8b
  */
  EXPECT_EQ(3, ild(u"Dê┬"));
}

TEST(x86ild, test_4189C4) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r12d, eax
  */
  EXPECT_EQ(3, ild(u"Aë─"));
}

TEST(x86ild, test_8A049500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rdx*4]
  */
  EXPECT_EQ(7, ild(u"è♦ò    "));
}

TEST(x86ild, test_89F6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov esi, esi
  */
  EXPECT_EQ(2, ild(u"ë÷"));
}

TEST(x86ild, test_89EE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov esi, ebp
  */
  EXPECT_EQ(2, ild(u"ëε"));
}

TEST(x86ild, test_89DE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov esi, ebx
  */
  EXPECT_EQ(2, ild(u"ë▐"));
}

TEST(x86ild, test_89DA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edx, ebx
  */
  EXPECT_EQ(2, ild(u"ë┌"));
}

TEST(x86ild, test_89C3) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ebx, eax
  */
  EXPECT_EQ(2, ild(u"ë├"));
}

TEST(x86ild, test_7E00) {
  /*
    ICLASS: JLE
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JLE_RELBRb
    ISA_SET: I86
    SHORT: jle 0x2
  */
  EXPECT_EQ(2, ild(u"~ "));
}

TEST(x86ild, test_66897300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov word ptr [rbx], si
  */
  EXPECT_EQ(4, ild(u"fës "));
}

TEST(x86ild, test_4889F3) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rbx, rsi
  */
  EXPECT_EQ(3, ild(u"Hë≤"));
}

TEST(x86ild, test_4489C1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ecx, r8d
  */
  EXPECT_EQ(3, ild(u"Dë┴"));
}

TEST(x86ild, test_40886B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rbx], bpl
  */
  EXPECT_EQ(4, ild(u"@êk "));
}

TEST(x86ild, test_C7800000000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov dword ptr [rax], 0x0
  */
  EXPECT_EQ(10, ild(u"╟Ç        "));
}

TEST(x86ild, test_C7420000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov dword ptr [rdx], 0x0
  */
  EXPECT_EQ(7, ild(u"╟B     "));
}

TEST(x86ild, test_899000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rax], edx
  */
  EXPECT_EQ(6, ild(u"ëÉ    "));
}

TEST(x86ild, test_59) {
  /*
    ICLASS: POP
    CATEGORY: POP
    EXTENSION: BASE
    IFORM: POP_GPRv_58
    ISA_SET: I86
    SHORT: pop rcx
  */
  EXPECT_EQ(1, ild(u"Y"));
}

TEST(x86ild, test_48D3E2) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_CL_D3r4
    ISA_SET: I86
    SHORT: shl rdx, cl
  */
  EXPECT_EQ(3, ild(u"H╙Γ"));
}

TEST(x86ild, test_48C7470000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov qword ptr [rdi], 0x0
  */
  EXPECT_EQ(8, ild(u"H╟G     "));
}

TEST(x86ild, test_488B4F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rcx, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"HïO "));
}

TEST(x86ild, test_4863F6) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_GPR32
    ISA_SET: LONGMODE
    SHORT: movsxd rsi, esi
  */
  EXPECT_EQ(3, ild(u"Hc÷"));
}

TEST(x86ild, test_4489E2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edx, r12d
  */
  EXPECT_EQ(3, ild(u"DëΓ"));
}

TEST(x86ild, test_415F) {
  /*
    ICLASS: POP
    CATEGORY: POP
    EXTENSION: BASE
    IFORM: POP_GPRv_58
    ISA_SET: I86
    SHORT: pop r15
  */
  EXPECT_EQ(2, ild(u"A_"));
}

TEST(x86ild, test_4157) {
  /*
    ICLASS: PUSH
    CATEGORY: PUSH
    EXTENSION: BASE
    IFORM: PUSH_GPRv_50
    ISA_SET: I86
    SHORT: push r15
  */
  EXPECT_EQ(2, ild(u"AW"));
}

TEST(x86ild, test_408A6B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov bpl, byte ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"@èk "));
}

TEST(x86ild, test_4088C6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov sil, al
  */
  EXPECT_EQ(3, ild(u"@ê╞"));
}

TEST(x86ild, test_8A4600) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rsi]
  */
  EXPECT_EQ(3, ild(u"èF "));
}

TEST(x86ild, test_8A04D500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rdx*8]
  */
  EXPECT_EQ(7, ild(u"è♦╒    "));
}

TEST(x86ild, test_89F8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, edi
  */
  EXPECT_EQ(2, ild(u"ë°"));
}

TEST(x86ild, test_89C6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov esi, eax
  */
  EXPECT_EQ(2, ild(u"ë╞"));
}

TEST(x86ild, test_89542400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rsp], edx
  */
  EXPECT_EQ(4, ild(u"ëT$ "));
}

TEST(x86ild, test_88CA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dl, cl
  */
  EXPECT_EQ(2, ild(u"ê╩"));
}

TEST(x86ild, test_4C89EF) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdi, r13
  */
  EXPECT_EQ(3, ild(u"Lë∩"));
}

TEST(x86ild, test_48C70700000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov qword ptr [rdi], 0x0
  */
  EXPECT_EQ(7, ild(u"H╟•    "));
}

TEST(x86ild, test_48897700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rdi], rsi
  */
  EXPECT_EQ(4, ild(u"Hëw "));
}

TEST(x86ild, test_448B6C2400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r13d, dword ptr [rsp]
  */
  EXPECT_EQ(5, ild(u"Dïl$ "));
}

TEST(x86ild, test_FF24CD00000000) {
  /*
    ICLASS: JMP
    CATEGORY: UNCOND_BR
    EXTENSION: BASE
    IFORM: JMP_MEMv
    ISA_SET: I86
    SHORT: jmp qword ptr [rcx*8]
  */
  EXPECT_EQ(7, ild(u"λ$═    "));
}

TEST(x86ild, test_F2AE) {
  /*
    ICLASS: REPNE_SCASB
    CATEGORY: STRINGOP
    EXTENSION: BASE
    IFORM: REPNE_SCASB
    ISA_SET: I86
    SHORT: repne scasb byte ptr [rdi]
  */
  EXPECT_EQ(2, ild(u"≥«"));
}

TEST(x86ild, test_E90083E000) {
  /*
    ICLASS: JMP
    CATEGORY: UNCOND_BR
    EXTENSION: BASE
    IFORM: JMP_RELBRd
    ISA_SET: I86
    SHORT: jmp 0xe08305
  */
  EXPECT_EQ(5, ild(u"Θ âα "));
}

TEST(x86ild, test_C7040700000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov dword ptr [rdi+rax*1], 0x0
  */
  EXPECT_EQ(7, ild(u"╟♦•    "));
}

TEST(x86ild, test_C644240000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rsp], 0x0
  */
  EXPECT_EQ(5, ild(u"╞D$  "));
}

TEST(x86ild, test_8B9200000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edx, dword ptr [rdx]
  */
  EXPECT_EQ(6, ild(u"ïÆ    "));
}

TEST(x86ild, test_8B7D00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edi, dword ptr [rbp]
  */
  EXPECT_EQ(3, ild(u"ï} "));
}

TEST(x86ild, test_8A8000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rax]
  */
  EXPECT_EQ(6, ild(u"èÇ    "));
}

TEST(x86ild, test_8A5000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rax]
  */
  EXPECT_EQ(3, ild(u"èP "));
}

TEST(x86ild, test_89F5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ebp, esi
  */
  EXPECT_EQ(2, ild(u"ë⌡"));
}

TEST(x86ild, test_89EA) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edx, ebp
  */
  EXPECT_EQ(2, ild(u"ëΩ"));
}

TEST(x86ild, test_89E8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov eax, ebp
  */
  EXPECT_EQ(2, ild(u"ëΦ"));
}

TEST(x86ild, test_668B7B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov di, word ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"fï{ "));
}

TEST(x86ild, test_4989FC) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r12, rdi
  */
  EXPECT_EQ(3, ild(u"Iëⁿ"));
}

TEST(x86ild, test_48D3E0) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_CL_D3r4
    ISA_SET: I86
    SHORT: shl rax, cl
  */
  EXPECT_EQ(3, ild(u"H╙α"));
}

TEST(x86ild, test_48BE0000000000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov rsi, 0x0
  */
  EXPECT_EQ(10, ild(u"H╛        "));
}

TEST(x86ild, test_488B6F00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rbp, qword ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"Hïo "));
}

TEST(x86ild, test_488B5500) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rbp]
  */
  EXPECT_EQ(4, ild(u"HïU "));
}

TEST(x86ild, test_4889E5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rbp, rsp
  */
  EXPECT_EQ(3, ild(u"Hëσ"));
}

TEST(x86ild, test_4889D5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rbp, rdx
  */
  EXPECT_EQ(3, ild(u"Hë╒"));
}

TEST(x86ild, test_448B442400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r8d, dword ptr [rsp]
  */
  EXPECT_EQ(5, ild(u"DïD$ "));
}

TEST(x86ild, test_4488C6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov sil, r8b
  */
  EXPECT_EQ(3, ild(u"Dê╞"));
}

TEST(x86ild, test_4189C5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r13d, eax
  */
  EXPECT_EQ(3, ild(u"Aë┼"));
}

TEST(x86ild, test_0F8D00000000) {
  /*
    ICLASS: JNL
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNL_RELBRd
    ISA_SET: I86
    SHORT: jnl 0x6
  */
  EXPECT_EQ(6, ild(u"☼ì    "));
}

TEST(x86ild, test_C6400000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_IMMb
    ISA_SET: I86
    SHORT: mov byte ptr [rax], 0x0
  */
  EXPECT_EQ(4, ild(u"╞@  "));
}

TEST(x86ild, test_B000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_IMMb_B0
    ISA_SET: I86
    SHORT: mov al, 0x0
  */
  EXPECT_EQ(2, ild(u"░ "));
}

TEST(x86ild, test_8BB900000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov edi, dword ptr [rcx]
  */
  EXPECT_EQ(6, ild(u"ï╣    "));
}

TEST(x86ild, test_8BB000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov esi, dword ptr [rax]
  */
  EXPECT_EQ(6, ild(u"ï░    "));
}

TEST(x86ild, test_8A00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rax]
  */
  EXPECT_EQ(2, ild(u"è "));
}

TEST(x86ild, test_89C5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ebp, eax
  */
  EXPECT_EQ(2, ild(u"ë┼"));
}

TEST(x86ild, test_896A00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdx], ebp
  */
  EXPECT_EQ(3, ild(u"ëj "));
}

TEST(x86ild, test_8802) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rdx], al
  */
  EXPECT_EQ(2, ild(u"ê☻"));
}

TEST(x86ild, test_7F00) {
  /*
    ICLASS: JNLE
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNLE_RELBRb
    ISA_SET: I86
    SHORT: jnle 0x2
  */
  EXPECT_EQ(2, ild(u"⌂ "));
}

TEST(x86ild, test_7D00) {
  /*
    ICLASS: JNL
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JNL_RELBRb
    ISA_SET: I86
    SHORT: jnl 0x2
  */
  EXPECT_EQ(2, ild(u"} "));
}

TEST(x86ild, test_668B7700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov si, word ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"fïw "));
}

TEST(x86ild, test_48D3EA) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_CL
    ISA_SET: I86
    SHORT: shr rdx, cl
  */
  EXPECT_EQ(3, ild(u"H╙Ω"));
}

TEST(x86ild, test_488B3D00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdi, qword ptr [rip]
  */
  EXPECT_EQ(7, ild(u"Hï=    "));
}

TEST(x86ild, test_488B14DD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rbx*8]
  */
  EXPECT_EQ(8, ild(u"Hï¶▌    "));
}

TEST(x86ild, test_488B0500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rax, qword ptr [rip]
  */
  EXPECT_EQ(7, ild(u"Hï♣    "));
}

TEST(x86ild, test_4889FE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rsi, rdi
  */
  EXPECT_EQ(3, ild(u"Hë■"));
}

TEST(x86ild, test_4889D9) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rcx, rbx
  */
  EXPECT_EQ(3, ild(u"Hë┘"));
}

TEST(x86ild, test_4889C8) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rax, rcx
  */
  EXPECT_EQ(3, ild(u"Hë╚"));
}

TEST(x86ild, test_4889C3) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rbx, rax
  */
  EXPECT_EQ(3, ild(u"Hë├"));
}

TEST(x86ild, test_488937) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rdi], rsi
  */
  EXPECT_EQ(3, ild(u"Hë7"));
}

TEST(x86ild, test_4863C9) {
  /*
    ICLASS: MOVSXD
    CATEGORY: DATAXFER
    EXTENSION: LONGMODE
    IFORM: MOVSXD_GPRv_GPR32
    ISA_SET: LONGMODE
    SHORT: movsxd rcx, ecx
  */
  EXPECT_EQ(3, ild(u"Hc╔"));
}

TEST(x86ild, test_448B7C2400) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov r15d, dword ptr [rsp]
  */
  EXPECT_EQ(5, ild(u"Dï|$ "));
}

TEST(x86ild, test_448A5700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov r10b, byte ptr [rdi]
  */
  EXPECT_EQ(4, ild(u"DèW "));
}

TEST(x86ild, test_448A4300) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov r8b, byte ptr [rbx]
  */
  EXPECT_EQ(4, ild(u"DèC "));
}

TEST(x86ild, test_4488D2) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov dl, r10b
  */
  EXPECT_EQ(3, ild(u"Dê╥"));
}

TEST(x86ild, test_4488C9) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_GPR8_88
    ISA_SET: I86
    SHORT: mov cl, r9b
  */
  EXPECT_EQ(3, ild(u"Dê╔"));
}

TEST(x86ild, test_41BC00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov r12d, 0x0
  */
  EXPECT_EQ(6, ild(u"A╝    "));
}

TEST(x86ild, test_4189F5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r13d, esi
  */
  EXPECT_EQ(3, ild(u"Aë⌡"));
}

TEST(x86ild, test_40887700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMb_GPR8
    ISA_SET: I86
    SHORT: mov byte ptr [rdi], sil
  */
  EXPECT_EQ(4, ild(u"@êw "));
}

TEST(x86ild, test_decode) {
  /*
   */
  EXPECT_EQ(2, ild(u"▐═"));
}

TEST(x86ild, test_D3E0) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_CL_D3r4
    ISA_SET: I86
    SHORT: shl eax, cl
  */
  EXPECT_EQ(2, ild(u"╙α"));
}

TEST(x86ild, test_BD00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_IMMv
    ISA_SET: I86
    SHORT: mov ebp, 0x0
  */
  EXPECT_EQ(5, ild(u"╜    "));
}

TEST(x86ild, test_8B4B00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov ecx, dword ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"ïK "));
}

TEST(x86ild, test_8B07) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov eax, dword ptr [rdi]
  */
  EXPECT_EQ(2, ild(u"ï•"));
}

TEST(x86ild, test_8A9000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rax]
  */
  EXPECT_EQ(6, ild(u"èÉ    "));
}

TEST(x86ild, test_8A4A00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rdx]
  */
  EXPECT_EQ(3, ild(u"èJ "));
}

TEST(x86ild, test_8A4500) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov al, byte ptr [rbp]
  */
  EXPECT_EQ(3, ild(u"èE "));
}

TEST(x86ild, test_8A14D500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov dl, byte ptr [rdx*8]
  */
  EXPECT_EQ(7, ild(u"è¶╒    "));
}

TEST(x86ild, test_8A0CD500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rdx*8]
  */
  EXPECT_EQ(7, ild(u"è♀╒    "));
}

TEST(x86ild, test_8A0C08) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPR8_MEMb
    ISA_SET: I86
    SHORT: mov cl, byte ptr [rax+rcx*1]
  */
  EXPECT_EQ(3, ild(u"è♀◘"));
}

TEST(x86ild, test_89D6) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov esi, edx
  */
  EXPECT_EQ(2, ild(u"ë╓"));
}

TEST(x86ild, test_89D5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov ebp, edx
  */
  EXPECT_EQ(2, ild(u"ë╒"));
}

TEST(x86ild, test_897700) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdi], esi
  */
  EXPECT_EQ(3, ild(u"ëw "));
}

TEST(x86ild, test_895A00) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rdx], ebx
  */
  EXPECT_EQ(3, ild(u"ëZ "));
}

TEST(x86ild, test_895000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rax], edx
  */
  EXPECT_EQ(3, ild(u"ëP "));
}

TEST(x86ild, test_8910) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov dword ptr [rax], edx
  */
  EXPECT_EQ(2, ild(u"ë►"));
}

TEST(x86ild, test_7C00) {
  /*
    ICLASS: JL
    CATEGORY: COND_BR
    EXTENSION: BASE
    IFORM: JL_RELBRb
    ISA_SET: I86
    SHORT: jl 0x2
  */
  EXPECT_EQ(2, ild(u"| "));
}

TEST(x86ild, test_4C89C1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rcx, r8
  */
  EXPECT_EQ(3, ild(u"Lë┴"));
}

TEST(x86ild, test_498B5500) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [r13]
  */
  EXPECT_EQ(4, ild(u"IïU "));
}

TEST(x86ild, test_4989D5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov r13, rdx
  */
  EXPECT_EQ(3, ild(u"Ië╒"));
}

TEST(x86ild, test_48D3E8) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_CL
    ISA_SET: I86
    SHORT: shr rax, cl
  */
  EXPECT_EQ(3, ild(u"H╙Φ"));
}

TEST(x86ild, test_48C7430000000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_IMMz
    ISA_SET: I86
    SHORT: mov qword ptr [rbx], 0x0
  */
  EXPECT_EQ(8, ild(u"H╟C     "));
}

TEST(x86ild, test_4898) {
  /*
    ICLASS: CDQE
    CATEGORY: CONVERT
    EXTENSION: LONGMODE
    IFORM: CDQE
    ISA_SET: LONGMODE
    SHORT: cdqe
  */
  EXPECT_EQ(2, ild(u"Hÿ"));
}

TEST(x86ild, test_488B9600000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rdx, qword ptr [rsi]
  */
  EXPECT_EQ(7, ild(u"Hïû    "));
}

TEST(x86ild, test_488B0B) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_MEMv
    ISA_SET: I86
    SHORT: mov rcx, qword ptr [rbx]
  */
  EXPECT_EQ(3, ild(u"Hï♂"));
}

TEST(x86ild, test_4889F7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdi, rsi
  */
  EXPECT_EQ(3, ild(u"Hë≈"));
}

TEST(x86ild, test_4889F5) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rbp, rsi
  */
  EXPECT_EQ(3, ild(u"Hë⌡"));
}

TEST(x86ild, test_4889F0) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rax, rsi
  */
  EXPECT_EQ(3, ild(u"Hë≡"));
}

TEST(x86ild, test_4889DE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rsi, rbx
  */
  EXPECT_EQ(3, ild(u"Hë▐"));
}

TEST(x86ild, test_4889D1) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rcx, rdx
  */
  EXPECT_EQ(3, ild(u"Hë╤"));
}

TEST(x86ild, test_4889CE) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rsi, rcx
  */
  EXPECT_EQ(3, ild(u"Hë╬"));
}

TEST(x86ild, test_4889C7) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov rdi, rax
  */
  EXPECT_EQ(3, ild(u"Hë╟"));
}

TEST(x86ild, test_48893D00000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rip], rdi
  */
  EXPECT_EQ(7, ild(u"Hë=    "));
}

TEST(x86ild, test_48893500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rip], rsi
  */
  EXPECT_EQ(7, ild(u"Hë5    "));
}

TEST(x86ild, test_48891500000000) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rip], rdx
  */
  EXPECT_EQ(7, ild(u"Hë§    "));
}

TEST(x86ild, test_488903) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_MEMv_GPRv
    ISA_SET: I86
    SHORT: mov qword ptr [rbx], rax
  */
  EXPECT_EQ(3, ild(u"Hë♥"));
}

TEST(x86ild, test_4489FF) {
  /*
    ICLASS: MOV
    CATEGORY: DATAXFER
    EXTENSION: BASE
    IFORM: MOV_GPRv_GPRv_89
    ISA_SET: I86
    SHORT: mov edi, r15d
  */
  EXPECT_EQ(3, ild(u"Dëλ"));
}
