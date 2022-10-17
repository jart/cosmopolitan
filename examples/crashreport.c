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
#include "libc/log/log.h"
#include "libc/runtime/symbols.internal.h"

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

noubsan int main(int argc, char *argv[]) {

  kprintf("----------------\n");
  kprintf(" THIS IS A TEST \n");
  kprintf("SIMULATING CRASH\n");
  kprintf("----------------\n");

  volatile int64_t x;
  ShowCrashReports();
  return 1 / (x = 0);
}
