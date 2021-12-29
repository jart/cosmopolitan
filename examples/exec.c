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

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fputs("USAGE: EXEC.COM PROG ARGV₀ [ARGV₁...]\n", stderr);
    return 1;
  }
  execv(argv[1], argv + 1);
  return 127;
}
