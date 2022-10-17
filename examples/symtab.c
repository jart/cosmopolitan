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

/**
 * @fileoverview example of how to embed symbol table in .com file
 *
 *     # build our binary
 *     make -j16 o//examples/symtab.com
 *
 *     # move binary somewhere else
 *     # so it can't find the .com.dbg binary
 *     cp o//examples/symtab.com /tmp
 *
 *     # run program
 *     # notice that it has a symbolic backtrace
 *     /tmp/symtab.com
 *
 * @see examples/examples.mk
 */

int main(int argc, char *argv[]) {

  // this links all the debugging and zip functionality
  ShowCrashReports();

  kprintf("----------------\n");
  kprintf(" THIS IS A TEST \n");
  kprintf("SIMULATING CRASH\n");
  kprintf("----------------\n");

  volatile int64_t x;
  return 1 / (x = 0);
}
