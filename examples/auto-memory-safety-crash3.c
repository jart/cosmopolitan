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
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * ASAN use-after-free memory safety crash example.
 *
 *     make -j8 MODE=dbg o/dbg/examples/auto-memory-safety-crash3.com
 *     o/dbg/examples/auto-memory-safety-crash3.com
 *
 */

int main(int argc, char *argv[]) {
  if (!IsAsan()) {
    printf("this example is intended for MODE=asan or MODE=dbg\n");
    exit(1);
  }
  char *buffer;
  ShowCrashReports(); /* not needed but yoinks appropriate symbols */
  buffer = malloc(13);
  strcpy(buffer, "hello");
  free(buffer);
  asm("" : "+r"(buffer)); /* prevent compiler being smart */
  buffer[0] = 1;
  return 0;
}
