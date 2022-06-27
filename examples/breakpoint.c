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
#include "libc/log/backtrace.internal.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"

int main(int argc, char *argv[]) {
  ShowCrashReports();

  if (IsDebuggerPresent(false)) {
    kprintf("debugger found!%n");
  } else {
    kprintf("try running: gdb %s%n", argv[0]);
    kprintf("try running: o//tool/build/strace.com %s%n", argv[0]);
  }

  asm volatile("mov\t%4,%%r10\n\t"
               "mov\t%5,%%r8\n\t"
               "mov\t%6,%%r9\n\t"
               "int3"
               : /* no outputs */
               : "a"(0), "D"(1), "S"(2), "d"(3), "g"(4), "g"(5), "g"(6)
               : "r8", "r9", "r10");

  printf("recovered from SIGTRAP without handler\r\n");
  return 0;
}
