#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/bits/pushpop.h"
#include "libc/calls/ucontext.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/nt/enum/errormodeflags.h"
#include "libc/nt/signals.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"

/**
 * @fileoverview Demonstrates SEGFAULT via NULL pointer dereference.
 *
 * In MODE=dbg mode, UBSAN should catch this and display a backtrace.
 * In most other modes, e.g. MODE=balanced, the oncrash.c sig handler
 * will either launch GDB or display a very fancy report.
 *
 *     make -j8 -O MODE=opt o/opt/examples/crashreportdemo.com
 *     o/opt/examples/crashreportdemo.com
 *
 * To prevent the GDB GUI from popping up:
 *
 *     export GDB=""
 *     make -j8 -O MODE=opt o/opt/examples/crashreportdemo.com
 *     o/opt/examples/crashreportdemo.com
 */

int boo;

void boop(void);

int main(int argc, char *argv[]) {
  int64_t res;
  showcrashreports();
  res = 0;

  asm volatile(
      "mov\t$0x1111111111111111,%rax\n\t"
      "mov\t$0x3333333333333333,%rcx\n\t"
      "mov\t$0x4444444444444444,%rdx\n\t"
      "mov\t$0x8888888888888888,%r8\n\t"
      "mov\t$0x9999999999999999,%r9d\n\t"
      "mov\t$0xaaaaaaaaaaaaaaaa,%r10d\n\t"
      "mov\t$0xbbbbbbbbbbbbbbbb,%r11d\n\t"
      "mov\t$0x0000000000000000,%rax\n\t"
      "movd\t%rax,%xmm0\n\t"
      "mov\t$0x1111111111111111,%rax\n\t"
      "movd\t%rax,%xmm1\n\t"
      "mov\t$0x2222222222222222,%rax\n\t"
      "movd\t%rax,%xmm2\n\t"
      "mov\t$0x3333333333333333,%rax\n\t"
      "movd\t%rax,%xmm3\n\t"
      "mov\t$0x4444444444444444,%rax\n\t"
      "movd\t%rax,%xmm4\n\t"
      "mov\t$0x5555555555555555,%rax\n\t"
      "movd\t%rax,%xmm5\n\t"
      "mov\t$0x6666666666666666,%rax\n\t"
      "movd\t%rax,%xmm6\n\t"
      "mov\t$0x7777777777777777,%rax\n\t"
      "movd\t%rax,%xmm7\n\t"
      "mov\t$0x8888888888888888,%rax\n\t"
      "movd\t%rax,%xmm8\n\t"
      "mov\t$0x9999999999999999,%rax\n\t"
      "movd\t%rax,%xmm9\n\t"
      "mov\t$0xaaaaaaaaaaaaaaaa,%rax\n\t"
      "movd\t%rax,%xmm10\n\t"
      "mov\t$0xbbbbbbbbbbbbbbbb,%rax\n\t"
      "movd\t%rax,%xmm11\n\t"
      "mov\t$0xcccccccccccccccc,%rax\n\t"
      "movd\t%rax,%xmm12\n\t"
      "mov\t$0xdddddddddddddddd,%rax\n\t"
      "movd\t%rax,%xmm13\n\t"
      "mov\t$0xeeeeeeeeeeeeeeee,%rax\n\t"
      "movd\t%rax,%xmm14\n\t"
      "mov\t$0xffffffffffffffff,%rax\n\t"
      "movd\t%rax,%xmm15\n\t"
      "fldlg2\n\t");

  res = *(int *)(intptr_t)boo / boo;
  return res;
}
