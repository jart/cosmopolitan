#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <cosmo.h>

/**
 * @fileoverview How to print backtraces and cpu state on crash.
 *
 *     make -j8 -O o//examples/crashreport.com
 *     o//examples/crashreport.com
 *
 * To prevent the GDB GUI from popping up:
 *
 *     export GDB=
 *     make -j8 -O o//examples/crashreport.com
 *     o//examples/crashreport.com
 */

int Divide(int x, int y) {
  volatile int z = 0;  // force creation of stack frame
  return x / y + z;
}

int (*pDivide)(int, int) = Divide;

dontubsan int main(int argc, char *argv[]) {
  kprintf("----------------\n");
  kprintf(" THIS IS A TEST \n");
  kprintf("SIMULATING CRASH\n");
  kprintf("----------------\n");

  ShowCrashReports();

  pDivide(1, 0);
  pDivide(2, 0);
  pDivide(3, 0);
}
