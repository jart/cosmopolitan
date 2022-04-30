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
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * @fileoverview Command for updating timestamps on files.
 */

int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i < argc; ++i) {
    if (touch(argv[i], 0666) == -1) {
      fprintf(stderr, "ERROR: %s: %s\n", argv[i], strerror(errno));
      exit(1);
    }
  }
  return 0;
}
