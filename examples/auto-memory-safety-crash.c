#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/intrin/bits.h"
#include "libc/dce.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"

/**
 * ASAN static memory safety crash example.
 *
 *     make -j8 MODE=dbg o/dbg/examples/auto-memory-safety-crash.com
 *     o/dbg/examples/auto-memory-safety-crash.com
 *
 * You should see:
 *
 *     global redzone 1-byte store at 0x42700d shadow 0x8007ce01
 *     ./o/dbg/examples/auto-memory-safety-crash.com
 *                                             x
 *     ........................................GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
 *        |0      |0      |0      |0      |5      |-18    |-18    |-18    |-18
 *      f.☼▼ä     f.☼▼ä     f☼▼D  hello                                                
 *     000000400000-000000427000 .text
 *     000000427000-000000429000 .data ←address
 *     00007fff0000-00008000ffff
 *     000080070000-00008008ffff ←shadow
 *     0e007fff0000-0e008000ffff
 *     100047d20000-100047d3ffff
 *     6ffffffe0000-6fffffffffff
 *     the memory in question belongs to the symbols
 *         buffer [0x427000,0x42700c] size 13
 *     the crash was caused by
 *     0x00000000004046f3: __die at libc/log/die.c:40
 *     0x0000000000404aed: __asan_report_store at libc/intrin/asan.c:1183
 *     0x0000000000402552: main at examples/auto-memory-safety-crash.c:27
 *     0x000000000040268d: cosmo at libc/runtime/cosmo.S:64
 *     0x00000000004021ae: _start at libc/crt/crt.S:77
 *
 * @see libc/intrin/asancodes.h for meaning of G, etc. and negative numbers
 * @see libc/nexgen32e/kcp437.S for meaning of symbols
 */

char buffer[13] = "hello";

int main(int argc, char *argv[]) {
  if (!IsAsan()) {
    printf("this example is intended for MODE=asan or MODE=dbg\n");
    exit(1);
  }
  ShowCrashReports(); /* not needed but yoinks appropriate symbols */
  int i = 13;
  asm("" : "+r"(i)); /* prevent compiler being smart */
  buffer[i] = 1;
  return 0;
}
