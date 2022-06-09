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
#include "libc/calls/struct/sigset.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sig.h"

STATIC_YOINK("strerror");

int main(int argc, char *argv[]) {
  sigset_t ss;
  if (argc < 2) {
    fputs("USAGE: EXEC.COM PROG [ARGV₀...]\n", stderr);
    return 1;
  }

  // block arbitrary signal so __printargs() looks cooler
  sigemptyset(&ss);
  sigaddset(&ss, SIGPWR);
  sigprocmask(SIG_BLOCK, &ss, 0);

  execv(argv[1], argv + 2);
  return 127;
}
