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

TEST(x86ild, test_488D7300) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rbx]
  */
  ASSERT_EQ(4, ild(u"Hìs "));
}

TEST(x86ild, test_488D0480) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rax+rax*4]
  */
  ASSERT_EQ(4, ild(u"Hì♦Ç"));
}

TEST(x86ild, test_488D0440) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rax+rax*2]
  */
  ASSERT_EQ(4, ild(u"Hì♦@"));
}

TEST(x86ild, test_8D148500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rax*4]
  */
  ASSERT_EQ(7, ild(u"ì¶à    "));
}

TEST(x86ild, test_488D5000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rax]
  */
  ASSERT_EQ(4, ild(u"HìP "));
}

TEST(x86ild, test_488D1492) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rdx+rdx*4]
  */
  ASSERT_EQ(4, ild(u"Hì¶Æ"));
}

TEST(x86ild, test_8D348D00000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [rcx*4]
  */
  ASSERT_EQ(7, ild(u"ì4ì    "));
}

TEST(x86ild, test_8D34CD00000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [rcx*8]
  */
  ASSERT_EQ(7, ild(u"ì4═    "));
}

TEST(x86ild, test_8D0C8500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rax*4]
  */
  ASSERT_EQ(7, ild(u"ì♀à    "));
}

TEST(x86ild, test_8D1400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rax+rax*1]
  */
  ASSERT_EQ(3, ild(u"ì¶ "));
}

TEST(x86ild, test_488D1401) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rcx+rax*1]
  */
  ASSERT_EQ(4, ild(u"Hì¶☺"));
}

TEST(x86ild, test_488D14D2) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rdx+rdx*8]
  */
  ASSERT_EQ(4, ild(u"Hì¶╥"));
}

TEST(x86ild, test_488D1452) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rdx+rdx*2]
  */
  ASSERT_EQ(4, ild(u"Hì¶R"));
}

TEST(x86ild, test_488D9000000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rax]
  */
  ASSERT_EQ(7, ild(u"HìÉ    "));
}

TEST(x86ild, test_488D04C0) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rax+rax*8]
  */
  ASSERT_EQ(4, ild(u"Hì♦└"));
}

TEST(x86ild, test_8D0CC500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rax*8]
  */
  ASSERT_EQ(7, ild(u"ì♀┼    "));
}

TEST(x86ild, test_488D8800000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rcx, ptr [rax]
  */
  ASSERT_EQ(7, ild(u"Hìê    "));
}

TEST(x86ild, test_488D0C16) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rcx, ptr [rsi+rdx*1]
  */
  ASSERT_EQ(4, ild(u"Hì♀▬"));
}

TEST(x86ild, test_488D742400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rsp]
  */
  ASSERT_EQ(5, ild(u"Hìt$ "));
}

TEST(x86ild, test_488D7C2400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdi, ptr [rsp]
  */
  ASSERT_EQ(5, ild(u"Hì|$ "));
}

TEST(x86ild, test_8D540200) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rdx+rax*1]
  */
  ASSERT_EQ(4, ild(u"ìT☻ "));
}

TEST(x86ild, test_488D0452) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rdx+rdx*2]
  */
  ASSERT_EQ(4, ild(u"Hì♦R"));
}

TEST(x86ild, test_8D54D000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rax+rdx*8]
  */
  ASSERT_EQ(4, ild(u"ìT╨ "));
}

TEST(x86ild, test_8D14C500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rax*8]
  */
  ASSERT_EQ(7, ild(u"ì¶┼    "));
}

TEST(x86ild, test_488D4800) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rcx, ptr [rax]
  */
  ASSERT_EQ(4, ild(u"HìH "));
}

TEST(x86ild, test_488D0C10) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rcx, ptr [rax+rdx*1]
  */
  ASSERT_EQ(4, ild(u"Hì♀►"));
}

TEST(x86ild, test_8D14D0) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rax+rdx*8]
  */
  ASSERT_EQ(3, ild(u"ì¶╨"));
}

TEST(x86ild, test_498D3408) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [r8+rcx*1]
  */
  ASSERT_EQ(4, ild(u"Iì4◘"));
}

TEST(x86ild, test_488D340A) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rdx+rcx*1]
  */
  ASSERT_EQ(4, ild(u"Hì4◙"));
}

TEST(x86ild, test_8D5500) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rbp]
  */
  ASSERT_EQ(3, ild(u"ìU "));
}

TEST(x86ild, test_8D349500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [rdx*4]
  */
  ASSERT_EQ(7, ild(u"ì4ò    "));
}

TEST(x86ild, test_488DB000000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rax]
  */
  ASSERT_EQ(7, ild(u"Hì░    "));
}

TEST(x86ild, test_8D541000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rax+rdx*1]
  */
  ASSERT_EQ(4, ild(u"ìT► "));
}

TEST(x86ild, test_8D4200) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rdx]
  */
  ASSERT_EQ(3, ild(u"ìB "));
}

TEST(x86ild, test_8D3CF500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edi, ptr [rsi*8]
  */
  ASSERT_EQ(7, ild(u"ì<⌡    "));
}

TEST(x86ild, test_8D3409) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [rcx+rcx*1]
  */
  ASSERT_EQ(3, ild(u"ì4○"));
}

TEST(x86ild, test_8D0C00) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rax+rax*1]
  */
  ASSERT_EQ(3, ild(u"ì♀ "));
}

TEST(x86ild, test_8D0448) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rax+rcx*2]
  */
  ASSERT_EQ(3, ild(u"ì♦H"));
}

TEST(x86ild, test_488D542400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rsp]
  */
  ASSERT_EQ(5, ild(u"HìT$ "));
}

TEST(x86ild, test_8D5000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rax]
  */
  ASSERT_EQ(3, ild(u"ìP "));
}

TEST(x86ild, test_8D1450) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rax+rdx*2]
  */
  ASSERT_EQ(3, ild(u"ì¶P"));
}

TEST(x86ild, test_8D0490) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rax+rdx*4]
  */
  ASSERT_EQ(3, ild(u"ì♦É"));
}

TEST(x86ild, test_4C8D0431) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r8, ptr [rcx+rsi*1]
  */
  ASSERT_EQ(4, ild(u"Lì♦1"));
}

TEST(x86ild, test_488DB100000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rcx]
  */
  ASSERT_EQ(7, ild(u"Hì▒    "));
}

TEST(x86ild, test_8D4E00) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rsi]
  */
  ASSERT_EQ(3, ild(u"ìN "));
}

TEST(x86ild, test_8D440200) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rdx+rax*1]
  */
  ASSERT_EQ(4, ild(u"ìD☻ "));
}

TEST(x86ild, test_8D1490) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rax+rdx*4]
  */
  ASSERT_EQ(3, ild(u"ì¶É"));
}

TEST(x86ild, test_8D1482) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rdx+rax*4]
  */
  ASSERT_EQ(3, ild(u"ì¶é"));
}

TEST(x86ild, test_8D1401) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rcx+rax*1]
  */
  ASSERT_EQ(3, ild(u"ì¶☺"));
}

TEST(x86ild, test_8D0412) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rdx+rdx*1]
  */
  ASSERT_EQ(3, ild(u"ì♦↕"));
}

TEST(x86ild, test_4C8D0416) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r8, ptr [rsi+rdx*1]
  */
  ASSERT_EQ(4, ild(u"Lì♦▬"));
}

TEST(x86ild, test_498D1430) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [r8+rsi*1]
  */
  ASSERT_EQ(4, ild(u"Iì¶0"));
}

TEST(x86ild, test_488D8A00000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rcx, ptr [rdx]
  */
  ASSERT_EQ(7, ild(u"Hìè    "));
}

TEST(x86ild, test_488D4700) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"HìG "));
}

TEST(x86ild, test_488D340E) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rsi+rcx*1]
  */
  ASSERT_EQ(4, ild(u"Hì4♫"));
}

TEST(x86ild, test_488D3401) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rcx+rax*1]
  */
  ASSERT_EQ(4, ild(u"Hì4☺"));
}

TEST(x86ild, test_488D1431) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rcx+rsi*1]
  */
  ASSERT_EQ(4, ild(u"Hì¶1"));
}

TEST(x86ild, test_488D0C49) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rcx, ptr [rcx+rcx*2]
  */
  ASSERT_EQ(4, ild(u"Hì♀I"));
}

TEST(x86ild, test_488D048500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rax*4]
  */
  ASSERT_EQ(8, ild(u"Hì♦à    "));
}

TEST(x86ild, test_488D040E) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rsi+rcx*1]
  */
  ASSERT_EQ(4, ild(u"Hì♦♫"));
}

TEST(x86ild, test_8D9300000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rbx]
  */
  ASSERT_EQ(6, ild(u"ìô    "));
}

TEST(x86ild, test_8D7900) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edi, ptr [rcx]
  */
  ASSERT_EQ(3, ild(u"ìy "));
}

TEST(x86ild, test_8D5600) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rsi]
  */
  ASSERT_EQ(3, ild(u"ìV "));
}

TEST(x86ild, test_8D545000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rax+rdx*2]
  */
  ASSERT_EQ(4, ild(u"ìTP "));
}

TEST(x86ild, test_8D4800) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rax]
  */
  ASSERT_EQ(3, ild(u"ìH "));
}

TEST(x86ild, test_8D4700) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rdi]
  */
  ASSERT_EQ(3, ild(u"ìG "));
}

TEST(x86ild, test_8D4600) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rsi]
  */
  ASSERT_EQ(3, ild(u"ìF "));
}

TEST(x86ild, test_8D0CF500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rsi*8]
  */
  ASSERT_EQ(7, ild(u"ì♀⌡    "));
}

TEST(x86ild, test_8D0CB500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rsi*4]
  */
  ASSERT_EQ(7, ild(u"ì♀╡    "));
}

TEST(x86ild, test_8D0C9500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rdx*4]
  */
  ASSERT_EQ(7, ild(u"ì♀ò    "));
}

TEST(x86ild, test_8D04B500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rsi*4]
  */
  ASSERT_EQ(7, ild(u"ì♦╡    "));
}

TEST(x86ild, test_4F8D0C49) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r9, ptr [r9+r9*2]
  */
  ASSERT_EQ(4, ild(u"Oì♀I"));
}

TEST(x86ild, test_498D7C2400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdi, ptr [r12]
  */
  ASSERT_EQ(5, ild(u"Iì|$ "));
}

TEST(x86ild, test_488DB800000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdi, ptr [rax]
  */
  ASSERT_EQ(7, ild(u"Hì╕    "));
}

TEST(x86ild, test_488D4C2400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rcx, ptr [rsp]
  */
  ASSERT_EQ(5, ild(u"HìL$ "));
}

TEST(x86ild, test_488D442400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rsp]
  */
  ASSERT_EQ(5, ild(u"HìD$ "));
}

TEST(x86ild, test_488D3CC500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdi, ptr [rax*8]
  */
  ASSERT_EQ(8, ild(u"Hì<┼    "));
}

TEST(x86ild, test_488D3402) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rdx+rax*1]
  */
  ASSERT_EQ(4, ild(u"Hì4☻"));
}

TEST(x86ild, test_488D040A) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rdx+rcx*1]
  */
  ASSERT_EQ(4, ild(u"Hì♦◙"));
}

TEST(x86ild, test_488D0408) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rax+rcx*1]
  */
  ASSERT_EQ(4, ild(u"Hì♦◘"));
}

TEST(x86ild, test_448D4100) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r8d, ptr [rcx]
  */
  ASSERT_EQ(4, ild(u"DìA "));
}

TEST(x86ild, test_418D442400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [r12]
  */
  ASSERT_EQ(5, ild(u"AìD$ "));
}

TEST(x86ild, test_8D8A00000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rdx]
  */
  ASSERT_EQ(6, ild(u"ìè    "));
}

TEST(x86ild, test_8D8300000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rbx]
  */
  ASSERT_EQ(6, ild(u"ìâ    "));
}

TEST(x86ild, test_8D7100) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [rcx]
  */
  ASSERT_EQ(3, ild(u"ìq "));
}

TEST(x86ild, test_8D4D00) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rbp]
  */
  ASSERT_EQ(3, ild(u"ìM "));
}

TEST(x86ild, test_8D4500) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rbp]
  */
  ASSERT_EQ(3, ild(u"ìE "));
}

TEST(x86ild, test_8D44F000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rax+rsi*8]
  */
  ASSERT_EQ(4, ild(u"ìD≡ "));
}

TEST(x86ild, test_8D449000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rax+rdx*4]
  */
  ASSERT_EQ(4, ild(u"ìDÉ "));
}

TEST(x86ild, test_8D441500) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rbp+rdx*1]
  */
  ASSERT_EQ(4, ild(u"ìD§ "));
}

TEST(x86ild, test_8D440A00) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rdx+rcx*1]
  */
  ASSERT_EQ(4, ild(u"ìD◙ "));
}

TEST(x86ild, test_8D440100) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rcx+rax*1]
  */
  ASSERT_EQ(4, ild(u"ìD☺ "));
}

TEST(x86ild, test_8D4100) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rcx]
  */
  ASSERT_EQ(3, ild(u"ìA "));
}

TEST(x86ild, test_8D3CB500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edi, ptr [rsi*4]
  */
  ASSERT_EQ(7, ild(u"ì<╡    "));
}

TEST(x86ild, test_8D34D500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [rdx*8]
  */
  ASSERT_EQ(7, ild(u"ì4╒    "));
}

TEST(x86ild, test_8D34C500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [rax*8]
  */
  ASSERT_EQ(7, ild(u"ì4┼    "));
}

TEST(x86ild, test_8D3412) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [rdx+rdx*1]
  */
  ASSERT_EQ(3, ild(u"ì4↕"));
}

TEST(x86ild, test_8D14F500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rsi*8]
  */
  ASSERT_EQ(7, ild(u"ì¶⌡    "));
}

TEST(x86ild, test_8D14BD00000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rdi*4]
  */
  ASSERT_EQ(7, ild(u"ì¶╜    "));
}

TEST(x86ild, test_8D1451) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [rcx+rdx*2]
  */
  ASSERT_EQ(3, ild(u"ì¶Q"));
}

TEST(x86ild, test_8D0CD500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [rdx*8]
  */
  ASSERT_EQ(7, ild(u"ì♀╒    "));
}

TEST(x86ild, test_8D04FD00000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rdi*8]
  */
  ASSERT_EQ(7, ild(u"ì♦²    "));
}

TEST(x86ild, test_8D049500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rdx*4]
  */
  ASSERT_EQ(7, ild(u"ì♦ò    "));
}

TEST(x86ild, test_8D0488) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rax+rcx*4]
  */
  ASSERT_EQ(3, ild(u"ì♦ê"));
}

TEST(x86ild, test_8D0409) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [rcx+rcx*1]
  */
  ASSERT_EQ(3, ild(u"ì♦○"));
}

TEST(x86ild, test_4E8D0C06) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r9, ptr [rsi+r8*1]
  */
  ASSERT_EQ(4, ild(u"Nì♀♠"));
}

TEST(x86ild, test_4D8D6C2400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r13, ptr [r12]
  */
  ASSERT_EQ(5, ild(u"Mìl$ "));
}

TEST(x86ild, test_4D8D0431) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r8, ptr [r9+rsi*1]
  */
  ASSERT_EQ(4, ild(u"Mì♦1"));
}

TEST(x86ild, test_4C8D642400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r12, ptr [rsp]
  */
  ASSERT_EQ(5, ild(u"Lìd$ "));
}

TEST(x86ild, test_4C8D4100) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r8, ptr [rcx]
  */
  ASSERT_EQ(4, ild(u"LìA "));
}

TEST(x86ild, test_4C8D0C19) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r9, ptr [rcx+rbx*1]
  */
  ASSERT_EQ(4, ild(u"Lì♀↓"));
}

TEST(x86ild, test_4C8D0402) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r8, ptr [rdx+rax*1]
  */
  ASSERT_EQ(4, ild(u"Lì♦☻"));
}

TEST(x86ild, test_4A8D0C07) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rcx, ptr [rdi+r8*1]
  */
  ASSERT_EQ(4, ild(u"Jì♀•"));
}

TEST(x86ild, test_488DA800000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rbp, ptr [rax]
  */
  ASSERT_EQ(7, ild(u"Hì¿    "));
}

TEST(x86ild, test_488D5700) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rdi]
  */
  ASSERT_EQ(4, ild(u"HìW "));
}

TEST(x86ild, test_488D4200) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rdx]
  */
  ASSERT_EQ(4, ild(u"HìB "));
}

TEST(x86ild, test_488D3C7F) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdi, ptr [rdi+rdi*2]
  */
  ASSERT_EQ(4, ild(u"Hì<⌂"));
}

TEST(x86ild, test_488D3500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rip]
  */
  ASSERT_EQ(7, ild(u"Hì5    "));
}

TEST(x86ild, test_488D34F7) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rdi+rsi*8]
  */
  ASSERT_EQ(4, ild(u"Hì4≈"));
}

TEST(x86ild, test_488D3476) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rsi+rsi*2]
  */
  ASSERT_EQ(4, ild(u"Hì4v"));
}

TEST(x86ild, test_488D3410) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rax+rdx*1]
  */
  ASSERT_EQ(4, ild(u"Hì4►"));
}

TEST(x86ild, test_488D3408) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rsi, ptr [rax+rcx*1]
  */
  ASSERT_EQ(4, ild(u"Hì4◘"));
}

TEST(x86ild, test_488D2CD500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rbp, ptr [rdx*8]
  */
  ASSERT_EQ(8, ild(u"Hì,╒    "));
}

TEST(x86ild, test_488D14B0) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rax+rsi*4]
  */
  ASSERT_EQ(4, ild(u"Hì¶░"));
}

TEST(x86ild, test_488D1496) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rsi+rdx*4]
  */
  ASSERT_EQ(4, ild(u"Hì¶û"));
}

TEST(x86ild, test_488D1406) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rdx, ptr [rsi+rax*1]
  */
  ASSERT_EQ(4, ild(u"Hì¶♠"));
}

TEST(x86ild, test_488D0C02) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rcx, ptr [rdx+rax*1]
  */
  ASSERT_EQ(4, ild(u"Hì♀☻"));
}

TEST(x86ild, test_488D04FD00000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rdi*8]
  */
  ASSERT_EQ(8, ild(u"Hì♦²    "));
}

TEST(x86ild, test_488D04F500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rsi*8]
  */
  ASSERT_EQ(8, ild(u"Hì♦⌡    "));
}

TEST(x86ild, test_488D04CD00000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rcx*8]
  */
  ASSERT_EQ(8, ild(u"Hì♦═    "));
}

TEST(x86ild, test_488D04B0) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rax+rsi*4]
  */
  ASSERT_EQ(4, ild(u"Hì♦░"));
}

TEST(x86ild, test_488D049500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rdx*4]
  */
  ASSERT_EQ(8, ild(u"Hì♦ò    "));
}

TEST(x86ild, test_488D0486) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rsi+rax*4]
  */
  ASSERT_EQ(4, ild(u"Hì♦å"));
}

TEST(x86ild, test_488D0482) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rdx+rax*4]
  */
  ASSERT_EQ(4, ild(u"Hì♦é"));
}

TEST(x86ild, test_488D0430) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea rax, ptr [rax+rsi*1]
  */
  ASSERT_EQ(4, ild(u"Hì♦0"));
}

TEST(x86ild, test_468D240A) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r12d, ptr [rdx+r9*1]
  */
  ASSERT_EQ(4, ild(u"Fì$◙"));
}

TEST(x86ild, test_468D0CD500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r9d, ptr [r10*8]
  */
  ASSERT_EQ(8, ild(u"Fì♀╒    "));
}

TEST(x86ild, test_468D0CC500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r9d, ptr [r8*8]
  */
  ASSERT_EQ(8, ild(u"Fì♀┼    "));
}

TEST(x86ild, test_468D0C06) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r9d, ptr [rsi+r8*1]
  */
  ASSERT_EQ(4, ild(u"Fì♀♠"));
}

TEST(x86ild, test_458D7E00) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r15d, ptr [r14]
  */
  ASSERT_EQ(4, ild(u"Eì~ "));
}

TEST(x86ild, test_458D3401) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r14d, ptr [r9+rax*1]
  */
  ASSERT_EQ(4, ild(u"Eì4☺"));
}

TEST(x86ild, test_448D7000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r14d, ptr [rax]
  */
  ASSERT_EQ(4, ild(u"Dìp "));
}

TEST(x86ild, test_448D4800) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r9d, ptr [rax]
  */
  ASSERT_EQ(4, ild(u"DìH "));
}

TEST(x86ild, test_448D4600) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r8d, ptr [rsi]
  */
  ASSERT_EQ(4, ild(u"DìF "));
}

TEST(x86ild, test_448D14C500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r10d, ptr [rax*8]
  */
  ASSERT_EQ(8, ild(u"Dì¶┼    "));
}

TEST(x86ild, test_448D148500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r10d, ptr [rax*4]
  */
  ASSERT_EQ(8, ild(u"Dì¶à    "));
}

TEST(x86ild, test_448D04F500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r8d, ptr [rsi*8]
  */
  ASSERT_EQ(8, ild(u"Dì♦⌡    "));
}

TEST(x86ild, test_448D04B500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r8d, ptr [rsi*4]
  */
  ASSERT_EQ(8, ild(u"Dì♦╡    "));
}

TEST(x86ild, test_448D043F) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea r8d, ptr [rdi+rdi*1]
  */
  ASSERT_EQ(4, ild(u"Dì♦⁇"));
}

TEST(x86ild, test_428D34C500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [r8*8]
  */
  ASSERT_EQ(8, ild(u"Bì4┼    "));
}

TEST(x86ild, test_428D348500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [r8*4]
  */
  ASSERT_EQ(8, ild(u"Bì4à    "));
}

TEST(x86ild, test_428D2C30) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ebp, ptr [rax+r14*1]
  */
  ASSERT_EQ(4, ild(u"Bì,0"));
}

TEST(x86ild, test_428D0CB500000000) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [r14*4]
  */
  ASSERT_EQ(8, ild(u"Bì♀╡    "));
}

TEST(x86ild, test_418D741500) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea esi, ptr [r13+rdx*1]
  */
  ASSERT_EQ(5, ild(u"Aìt§ "));
}

TEST(x86ild, test_418D6B00) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ebp, ptr [r11]
  */
  ASSERT_EQ(4, ild(u"Aìk "));
}

TEST(x86ild, test_418D5600) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea edx, ptr [r14]
  */
  ASSERT_EQ(4, ild(u"AìV "));
}

TEST(x86ild, test_418D4C2400) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea ecx, ptr [r12]
  */
  ASSERT_EQ(5, ild(u"AìL$ "));
}

TEST(x86ild, test_418D4700) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [r15]
  */
  ASSERT_EQ(4, ild(u"AìG "));
}

TEST(x86ild, test_418D440C00) {
  /*
    ICLASS: LEA
    CATEGORY: MISC
    EXTENSION: BASE
    IFORM: LEA_GPRv_AGEN
    ISA_SET: I86
    SHORT: lea eax, ptr [r12+rcx*1]
  */
  ASSERT_EQ(5, ild(u"AìD♀ "));
}
