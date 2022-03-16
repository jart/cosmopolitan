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
#include "libc/stdio/stdio.h"

int main(int argc, char *argv[]) {
  int pid;
  volatile void *p;
  if (argc < 3) {
    fputs("USAGE: FORKEXECWAIT.COM PROG ARGV₀ [ARGV₁...]\n", stderr);
    return 1;
  }
  if (!fork()) {
    execv(argv[1], argv + 2);
    return 127;
  }
  wait(0);
}
