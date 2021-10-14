#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/runtime/stack.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/prot.h"

/**
 * @fileoverview Stack Overflow Demo
 */

#define N INT_MAX

STATIC_STACK_SIZE(FRAMESIZE);

int A(int f(), int n) {
  if (n < N) {
    return f(f, n + 1) - 1;
  } else {
    return N;
  }
}

int (*Ap)(int (*)(), int) = A;

int main(int argc, char *argv[]) {
  ShowCrashReports();
  return !!Ap(Ap, 0);
}

/*
error: Uncaught SIGSEGV (Stack Overflow) on rhel5 pid 368
  ./o//examples/stackoverflow.com
  EUNKNOWN[No error information][0]
  Linux rhel5 2.6.18-8.el5 #1 SMP Thu Mar 15 19:46:53 EDT 2007

0x0000000000406896: A at examples/stackoverflow.c:24
0x0000000000406898: A at examples/stackoverflow.c:24
0x0000000000406898: A at examples/stackoverflow.c:24
0x0000000000406898: A at examples/stackoverflow.c:24
0x0000000000406898: A at examples/stackoverflow.c:24
0x0000000000406898: A at examples/stackoverflow.c:24
0x0000000000406898: A at examples/stackoverflow.c:24
0x0000000000406898: A at examples/stackoverflow.c:24
0x0000000000406898: A at examples/stackoverflow.c:24
etc. etc.

RAX 0000000000000000 RBX 0000000000000001 RDI 000000000040687e ST(0) 0.0
RCX 0000000000417125 RDX 000000000041cd70 RSI 0000000000000efe ST(1) 0.0
RBP 00006ffffffe1000 RSP 00006ffffffe1000 RIP 0000000000406897 ST(2) 0.0
 R8 0000000000000000  R9 0000000000000022 R10 0000000000000008 ST(3) 0.0
R11 0000000000000293 R12 0000000000000001 R13 00007ffc70b4fc48 ST(4) 0.0
R14 00007ffc70b4fc58 R15 00007ffc70b4fd18 VF IF

XMM0  00000000000000000000000000000000 XMM8  00000000000000000000000000000000
XMM1  ffffffffffffeb030000000000000000 XMM9  00000000000000000000000000000000
XMM2  0000000000000000ffffffffffffffff XMM10 00000000000000000000000000000000
XMM3  00000000000000000000000000000000 XMM11 00000000000000000000000000000000
XMM4  00000000000000000000000000000000 XMM12 00000000000000000000000000000000
XMM5  00000000000000000000000000000000 XMM13 00000000000000000000000000000000
XMM6  00000000000000000000000000000000 XMM14 00000000000000000000000000000000
XMM7  00000000000000000000000000000000 XMM15 00000000000000000000000000000000

100080000000-100080030000 rw-pa--     3x automap
6ffffffe0000-6fffffff0000 rw-paSF     1x stack
# 4 frames mapped w/ 0 frames gapped
*/
