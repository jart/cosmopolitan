#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/log/log.h"

/**
 * Automatically launchs GDB Debugger TUI during crash.
 *
 * Run the following inside a terminal:
 *
 *     sudo apt install gdb
 *     make -j12 o//examples/auto-launch-gdb-on-crash.com
 *     o//examples/auto-launch-gdb-on-crash.com
 *
 * Backtrace is logged instead if run outside interactive terminal.
 * Environmental factors such as GDB, MAKEFLAGS, ADDR2LINE, TERM, and
 * isatty(STDERR_FILENO) should also be taken into consideration:
 *
 *     $ export GDB=eoatuhshtuone
 *     $ o//examples/auto-launch-gdb-on-crash.com
 *     error: Uncaught SIGTRAP
 *     etc. etc.
 *
 * @see https://www.felixcloutier.com/x86/intn:into:int3:int1
 * @see https://en.wikipedia.org/wiki/INT_(x86_instruction)#INT3
 * @see examples and reference material on using the asm() keyword
 *     - libc/nexgen32e/bsf.h
 *     - libc/nexgen32e/tzcnt.h
 *     - libc/nexgen32e/cpuid4.internal.h
 *     - https://gist.github.com/jart/fe8d104ef93149b5ba9b72912820282c
 */

int main(int argc, char *argv[]) {
  showcrashreports();
  asm("int3"); /* cf. __die(), perror("msg"), abort(), exit(1), _Exit(1) */
  return 0;
}
