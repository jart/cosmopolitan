#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/intrin/kprintf.h"
#include "libc/math.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"

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

dontubsan int main(int argc, char *argv[]) {
  kprintf("----------------\n");
  kprintf(" THIS IS A TEST \n");
  kprintf("SIMULATING CRASH\n");
  kprintf("----------------\n");

  ShowCrashReports();

  volatile double a = 0;
  volatile double b = 23;
  volatile double c = exp(b) / a;
  (void)c;

  volatile int x = 0;
  volatile int y = 1 / x;
  return y;
}
