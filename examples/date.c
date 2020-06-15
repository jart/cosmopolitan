#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/x/x.h"

/**
 * @fileoverview ISO-8601 international high-precision timestamp printer.
 */

int main(int argc, char *argv[]) {
  puts(gc(xiso8601ts(NULL)));
  return 0;
}
