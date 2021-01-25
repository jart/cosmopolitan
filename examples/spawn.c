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

int main(int argc, char *argv[]) {
  int pid, wstatus;
  if (argc < 2) return 1;
  pid = fork();
  if (!pid) {
    execv(argv[1], argv + 1);
    abort();
  }
  waitpid(pid, &wstatus, 0);
  return WEXITSTATUS(wstatus);
}
