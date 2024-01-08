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
#include "libc/mem/gc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "test/libc/xed/lib.h"
#include "third_party/xed/x86.h"

/**
 * @fileoverview Instruction Length Decoding Tests.
 *
 * It is demonstrated that our 3.5kb x86 parser supports all legal x86
 * instruction set architectures and addressing modes since the 1970's,
 * including the really sophisticated ones, e.g. avx512, as well as the
 * less popular ones, e.g. 3dnow.
 */

TEST(x86ild, testSomeThingsNeverChange) {
  ASSERT_EQ(3, ildreal(u"â└↨"));   /* add $23,%ax  */
  ASSERT_EQ(3, ildlegacy(u"â└↨")); /* add $23,%eax */
  ASSERT_EQ(3, ild(u"â└↨"));       /* add $23,%eax */
}

TEST(x86ild, testSomeThingsDoChange) {
  ASSERT_EQ(3, ildreal(u"♣7‼ÉÉ"));   /* add $0x1337,%ax */
  ASSERT_EQ(5, ildlegacy(u"♣7‼ÉÉ")); /* add $0x90901337,%eax */
  ASSERT_EQ(5, ild(u"♣7‼ÉÉ"));       /* add $0x90901337,%eax */
  ASSERT_EQ(1, ildreal(u"@É"));      /* inc %ax */
  ASSERT_EQ(1, ildlegacy(u"@É"));    /* inc %eax */
  ASSERT_EQ(2, ild(u"@É"));          /* rex xchg %eax,%eax */
}

TEST(x86ild, testHugeInstructions) {
  ASSERT_EQ(10, ild(u"H║       Ç"));   /* movabs $0x8000000000000000,%rdx */
  ASSERT_EQ(11, ild(u"H╟♣8l   É  "));  /* movq $0x209000,0x6c38(%rip) */
  ASSERT_EQ(12, ild(u"H╟ä$á       ")); /* movq $0x0,0xa0(%rsp) */
}

TEST(x86ild, testLaughOutLoudLargeCanonicalInstructions) {
  ASSERT_EQ(15, /* lock addl $0x12331337,%fs:-0x1337(%ebx,%esi,1) */
            ildreal(u"≡dfgüä3╔∞λλ7‼3↕"));
}

TEST(x86ild, testEncodingDisagreements) {
  ASSERT_EQ(13, /* lock addw $0x1337,%fs:-0x1337(%ebx,%esi,1) */
            ild(u"fg≡düä3╔∞λλ7‼")); /* ← xed encoding */
  ASSERT_EQ(13, /* lock addw $0x1337,%fs:-0x1337(%ebx,%esi,1) */
            ild(u"dgf≡üä3╔∞λλ7‼")); /* ← gas encoding */
}

TEST(x86ild, testOverlongInstructions) {
  ASSERT_EQ(3, ild(u"≤≤É")); /* rep pause */
  ASSERT_EQ(-XED_ERROR_BUFFER_TOO_SHORT /* suboptimal error code */,
            ildreal(u"≡≡≡≡dfgüä3╔∞λλ7‼3↕"));
}

TEST(x86ild, testAvx512_inRealMode_throwsError) {
  ASSERT_EQ(-XED_ERROR_INVALID_MODE, ildreal(u"bßTXYö∟¶♦  "));
}

TEST(x86ild, testAvx512) {
  ASSERT_EQ(6, /* vaddps %zmm17,%zmm16,%zmm16 */
            ild(u"b¡|@X┴"));
  ASSERT_EQ(11, /* vmulps 0x414(%rsp,%rbx,1){1to16},%zmm5,%zmm18 */
            ild(u"bßTXYö∟¶♦  "));
}

TEST(x86ild, testCascadeLake_advancedNeuralNetworkInstructions) {
  ASSERT_EQ(6, ild(u"b≥m◘P╦")); /* vpdpbusd %xmm3,%xmm2,%xmm1 */
  ASSERT_EQ(11 /* vpdpbusd 0x10000000(%rcx,%r14,8),%xmm2,%xmm1 */,
            ild(u"b▓m◘Pî±   ►"));
}

TEST(x86ild, testAmd3dnow) {
  ASSERT_EQ(4, ild(u"☼☼╚ª")); /* pfrcpit1 %mm0,%mm1 */
  struct XedDecodedInst xedd;
  ASSERT_EQ(
      0, xed_instruction_length_decode(
             xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LEGACY_32),
             gc(unbingx86op(u"☼☼╚ª")), 4));
  ASSERT_EQ(true, (int)xedd.op.amd3dnow);
  ASSERT_EQ(0xa6, xedd.op.opcode);
}

TEST(x86ild, testPopToMemory) {
  ASSERT_EQ(3, ild(u"ÅF◘")); /* 8f 46 08 */
}

TEST(x86ild, testFinit) {
  ASSERT_EQ(1, ild(u"¢█π")); /* 9B DB E3: fwait */
  ASSERT_EQ(2, ild(u"█π"));  /* DB E3:    fninit */
}

BENCH(x86ild, bench) {
  uint8_t *x = gc(unbingx86op(u"b▓m◘Pî±   ►"));
  struct XedDecodedInst xedd;
  EZBENCH2("x86ild", donothing,
           xed_instruction_length_decode(
               xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64),
               x, XED_MAX_INSTRUCTION_BYTES));
}
