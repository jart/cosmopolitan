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

TEST(x86ild, test_C0E800) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr al, 0x0
  */
  ASSERT_EQ(3, ild(u"└Φ "));
}

TEST(x86ild, test_C1EE00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr esi, 0x0
  */
  ASSERT_EQ(3, ild(u"┴ε "));
}

TEST(x86ild, test_48C1E000) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl rax, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴α "));
}

TEST(x86ild, test_66C1EE00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr si, 0x0
  */
  ASSERT_EQ(4, ild(u"f┴ε "));
}

TEST(x86ild, test_486BC000) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv_IMMb
    ISA_SET: I186
    SHORT: imul rax, rax, 0x0
  */
  ASSERT_EQ(4, ild(u"Hk└ "));
}

TEST(x86ild, test_C1E000) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl eax, 0x0
  */
  ASSERT_EQ(3, ild(u"┴α "));
}

TEST(x86ild, test_486BED00) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv_IMMb
    ISA_SET: I186
    SHORT: imul rbp, rbp, 0x0
  */
  ASSERT_EQ(4, ild(u"Hkφ "));
}

TEST(x86ild, test_69D000000000) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv_IMMz
    ISA_SET: I186
    SHORT: imul edx, eax, 0x0
  */
  ASSERT_EQ(6, ild(u"i╨    "));
}

TEST(x86ild, test_48C1EA00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr rdx, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴Ω "));
}

TEST(x86ild, test_C0EA00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr dl, 0x0
  */
  ASSERT_EQ(3, ild(u"└Ω "));
}

TEST(x86ild, test_C1E200) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl edx, 0x0
  */
  ASSERT_EQ(3, ild(u"┴Γ "));
}

TEST(x86ild, test_C1E600) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl esi, 0x0
  */
  ASSERT_EQ(3, ild(u"┴μ "));
}

TEST(x86ild, test_66C1EA00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr dx, 0x0
  */
  ASSERT_EQ(4, ild(u"f┴Ω "));
}

TEST(x86ild, test_486BD200) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv_IMMb
    ISA_SET: I186
    SHORT: imul rdx, rdx, 0x0
  */
  ASSERT_EQ(4, ild(u"Hk╥ "));
}

TEST(x86ild, test_C0EA83) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr dl, 0x83
  */
  ASSERT_EQ(3, ild(u"└Ωâ"));
}

TEST(x86ild, test_C1E800) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr eax, 0x0
  */
  ASSERT_EQ(3, ild(u"┴Φ "));
}

TEST(x86ild, test_C1EA00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr edx, 0x0
  */
  ASSERT_EQ(3, ild(u"┴Ω "));
}

TEST(x86ild, test_48C1E500) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl rbp, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴σ "));
}

TEST(x86ild, test_66C1E800) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr ax, 0x0
  */
  ASSERT_EQ(4, ild(u"f┴Φ "));
}

TEST(x86ild, test_C1E700) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl edi, 0x0
  */
  ASSERT_EQ(3, ild(u"┴τ "));
}

TEST(x86ild, test_C0E900) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr cl, 0x0
  */
  ASSERT_EQ(3, ild(u"└Θ "));
}

TEST(x86ild, test_C1E100) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl ecx, 0x0
  */
  ASSERT_EQ(3, ild(u"┴ß "));
}

TEST(x86ild, test_48C1E200) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl rdx, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴Γ "));
}

TEST(x86ild, test_40C0EE00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr sil, 0x0
  */
  ASSERT_EQ(4, ild(u"@└ε "));
}

TEST(x86ild, test_69C000000000) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv_IMMz
    ISA_SET: I186
    SHORT: imul eax, eax, 0x0
  */
  ASSERT_EQ(6, ild(u"i└    "));
}

TEST(x86ild, test_48C1E800) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr rax, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴Φ "));
}

TEST(x86ild, test_C0E883) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr al, 0x83
  */
  ASSERT_EQ(3, ild(u"└Φâ"));
}

TEST(x86ild, test_48C1E100) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl rcx, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴ß "));
}

TEST(x86ild, test_4869C000000000) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv_IMMz
    ISA_SET: I186
    SHORT: imul rax, rax, 0x0
  */
  ASSERT_EQ(7, ild(u"Hi└    "));
}

TEST(x86ild, test_48C1E700) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl rdi, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴τ "));
}

TEST(x86ild, test_41C0E800) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr r8b, 0x0
  */
  ASSERT_EQ(4, ild(u"A└Φ "));
}

TEST(x86ild, test_C0E8C0) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr al, 0xc0
  */
  ASSERT_EQ(3, ild(u"└Φ└"));
}

TEST(x86ild, test_C0EA66) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr dl, 0x66
  */
  ASSERT_EQ(3, ild(u"└Ωf"));
}

TEST(x86ild, test_48C1E600) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl rsi, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴μ "));
}

TEST(x86ild, test_C0E848) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr al, 0x48
  */
  ASSERT_EQ(3, ild(u"└ΦH"));
}

TEST(x86ild, test_4869D200000000) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv_IMMz
    ISA_SET: I186
    SHORT: imul rdx, rdx, 0x0
  */
  ASSERT_EQ(7, ild(u"Hi╥    "));
}

TEST(x86ild, test_C1E900) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr ecx, 0x0
  */
  ASSERT_EQ(3, ild(u"┴Θ "));
}

TEST(x86ild, test_40C0ED00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr bpl, 0x0
  */
  ASSERT_EQ(4, ild(u"@└φ "));
}

TEST(x86ild, test_C1EF00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr edi, 0x0
  */
  ASSERT_EQ(3, ild(u"┴∩ "));
}

TEST(x86ild, test_66C1EF00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr di, 0x0
  */
  ASSERT_EQ(4, ild(u"f┴∩ "));
}

TEST(x86ild, test_486BCA00) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv_IMMb
    ISA_SET: I186
    SHORT: imul rcx, rdx, 0x0
  */
  ASSERT_EQ(4, ild(u"Hk╩ "));
}

TEST(x86ild, test_41C1EA00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr r10d, 0x0
  */
  ASSERT_EQ(4, ild(u"A┴Ω "));
}

TEST(x86ild, test_41C1E000) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl r8d, 0x0
  */
  ASSERT_EQ(4, ild(u"A┴α "));
}

TEST(x86ild, test_40C0EF00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr dil, 0x0
  */
  ASSERT_EQ(4, ild(u"@└∩ "));
}

TEST(x86ild, test_49C1E000) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl r8, 0x0
  */
  ASSERT_EQ(4, ild(u"I┴α "));
}

TEST(x86ild, test_41C1E800) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr r8d, 0x0
  */
  ASSERT_EQ(4, ild(u"A┴Φ "));
}

TEST(x86ild, test_41C1E200) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl r10d, 0x0
  */
  ASSERT_EQ(4, ild(u"A┴Γ "));
}

TEST(x86ild, test_41C0E900) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr r9b, 0x0
  */
  ASSERT_EQ(4, ild(u"A└Θ "));
}

TEST(x86ild, test_C0EAC0) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPR8_IMMb
    ISA_SET: I186
    SHORT: shr dl, 0xc0
  */
  ASSERT_EQ(3, ild(u"└Ω└"));
}

TEST(x86ild, test_66C1E900) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr cx, 0x0
  */
  ASSERT_EQ(4, ild(u"f┴Θ "));
}

TEST(x86ild, test_48C1F800) {
  /*
    ICLASS: SAR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SAR_GPRv_IMMb
    ISA_SET: I186
    SHORT: sar rax, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴° "));
}

TEST(x86ild, test_41C1EB00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr r11d, 0x0
  */
  ASSERT_EQ(4, ild(u"A┴δ "));
}

TEST(x86ild, test_41C1E300) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl r11d, 0x0
  */
  ASSERT_EQ(4, ild(u"A┴π "));
}

TEST(x86ild, test_C1ED00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr ebp, 0x0
  */
  ASSERT_EQ(3, ild(u"┴φ "));
}

TEST(x86ild, test_C1EB00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr ebx, 0x0
  */
  ASSERT_EQ(3, ild(u"┴δ "));
}

TEST(x86ild, test_C1E500) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl ebp, 0x0
  */
  ASSERT_EQ(3, ild(u"┴σ "));
}

TEST(x86ild, test_69D800000000) {
  /*
    ICLASS: IMUL
    CATEGORY: BINARY
    EXTENSION: BASE
    IFORM: IMUL_GPRv_GPRv_IMMz
    ISA_SET: I186
    SHORT: imul ebx, eax, 0x0
  */
  ASSERT_EQ(6, ild(u"i╪    "));
}

TEST(x86ild, test_6641C1EE00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr r14w, 0x0
  */
  ASSERT_EQ(5, ild(u"fA┴ε "));
}

TEST(x86ild, test_6641C1ED00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr r13w, 0x0
  */
  ASSERT_EQ(5, ild(u"fA┴φ "));
}

TEST(x86ild, test_6641C1E800) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr r8w, 0x0
  */
  ASSERT_EQ(5, ild(u"fA┴Φ "));
}

TEST(x86ild, test_49C1E100) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl r9, 0x0
  */
  ASSERT_EQ(4, ild(u"I┴ß "));
}

TEST(x86ild, test_48C1FE74) {
  /*
    ICLASS: SAR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SAR_GPRv_IMMb
    ISA_SET: I186
    SHORT: sar rsi, 0x74
  */
  ASSERT_EQ(4, ild(u"H┴■t"));
}

TEST(x86ild, test_48C1FE00) {
  /*
    ICLASS: SAR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SAR_GPRv_IMMb
    ISA_SET: I186
    SHORT: sar rsi, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴■ "));
}

TEST(x86ild, test_48C1FB00) {
  /*
    ICLASS: SAR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SAR_GPRv_IMMb
    ISA_SET: I186
    SHORT: sar rbx, 0x0
  */
  ASSERT_EQ(4, ild(u"H┴√ "));
}

TEST(x86ild, test_41C1EE00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr r14d, 0x0
  */
  ASSERT_EQ(4, ild(u"A┴ε "));
}

TEST(x86ild, test_41C1EC00) {
  /*
    ICLASS: SHR
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHR_GPRv_IMMb
    ISA_SET: I186
    SHORT: shr r12d, 0x0
  */
  ASSERT_EQ(4, ild(u"A┴∞ "));
}

TEST(x86ild, test_41C1E700) {
  /*
    ICLASS: SHL
    CATEGORY: SHIFT
    EXTENSION: BASE
    IFORM: SHL_GPRv_IMMb_C1r4
    ISA_SET: I186
    SHORT: shl r15d, 0x0
  */
  ASSERT_EQ(4, ild(u"A┴τ "));
}
