#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"

/**
 * @fileoverview Cosmopolitan Command Interpreter Demo
 * Yes this works on Windows.
 */

__static_yoink("_tr");
__static_yoink("_sed");

int main(int argc, char *argv[]) {
  system("x=world\n"
         "echo hello $x |\n"
         "  tr a-z A-Z |\n"
         "  sed 's/\\(.\\)/\\1 /g'");
}
