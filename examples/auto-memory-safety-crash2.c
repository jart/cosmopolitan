#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/dce.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

/**
 * ASAN heap memory safety crash example.
 *
 *     make -j8 MODE=dbg o/dbg/examples/auto-memory-safety-crash2.com
 *     o/dbg/examples/auto-memory-safety-crash2.com
 *
 * You should see:
 *
 *     heap overrun 1-byte store at 0x10008004002d shadow 0x20090000005
 *     ./o/dbg/examples/auto-memory-safety-crash2.com
 *                                             x
 *     OOOOOOOOOOOUUUUUUUUUUUUUUUU.............OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
 *        |-7     |-6     |-6     |0      |5      |-7     |-7     |-7     |-7
 *        »!@     ÿ▄:Y╩≥= S       hello ∙∙∙∙∙∙∙           ♪     GT◘&@     á+@     »!@  
 *     000000400000-00000042b000 .text
 *     00000042b000-00000042d000 .data
 *     00007fff0000-00008000ffff
 *     000080070000-00008008ffff
 *     02008fff0000-02009000ffff ←shadow
 *     0e007fff0000-0e008000ffff
 *     10003ab90000-10003abaffff
 *     100080000000-10008000ffff ←address
 *     6ffffffe0000-6fffffffffff
 *
 *     the memory was allocated by
 *     0x100080040020 64 bytes [dlmalloc]
 *     0x100080040030 13 bytes [actual]
 *           402608 main
 *           402ba0 cosmo
 *           4021af _start
 *
 *     the crash was caused by
 *     0x0000000000404793: __die at libc/log/die.c:40
 *     0x0000000000404f56: __asan_report_store at libc/intrin/asan.c:1183
 *     0x0000000000402579: main at examples/auto-memory-safety-crash2.c:30
 *     0x000000000040270f: cosmo at libc/runtime/cosmo.S:64
 *     0x00000000004021ae: _start at libc/crt/crt.S:77
 *
 * @see libc/intrin/asancodes.h for meaning of U, O, etc. and negative numbers
 * @see libc/nexgen32e/kcp437.S for meaning of symbols
 */

int main(int argc, char *argv[]) {
  if (!IsAsan()) {
    kprintf("this example is intended for MODE=asan or MODE=dbg\n");
    exit(1);
  }

  kprintf("----------------\n");
  kprintf(" THIS IS A TEST \n");
  kprintf("SIMULATING CRASH\n");
  kprintf("----------------\n");

  char *buffer;
  ShowCrashReports(); /* not needed but yoinks appropriate symbols */
  buffer = malloc(13);
  strcpy(buffer, "hello");
  int i = 13;
  asm("" : "+r"(i)); /* prevent compiler being smart */
  buffer[i] = 1;
  asm("" : "+r"(buffer)); /* prevent compiler being smart */
  return 0;
}
