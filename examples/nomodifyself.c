#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "ape/sections.internal.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"

/**
 * @fileoverview Non-Self-Modifying APE Binary Demo
 *
 * See examples/examples.mk for the build config, which uses the
 * alternative APE runtime.
 */

int main(int argc, char *argv[]) {
  if (_base[0] == 'M' && _base[1] == 'Z') {
    printf("success: %s spawned without needing to modify its "
           "executable header",
           argv[0]);
    if (!IsWindows()) {
      printf(", thanks to APE loader!\n");
    } else {
      printf(", because you ran it on Windows :P\n");
    }
    return 0;
  } else {
    printf("error: %s doesn't have an MZ file header!\n", argv[0]);
    return 1;
  }
}
