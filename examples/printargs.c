#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"

int main() {
  int pip[2];
  char buf[PATH_MAX];
  char *args[2] = {0};
  if (strcmp(nulltoempty(getenv("TERM")), "dumb") && isatty(0) && isatty(1) &&
      ((args[0] = commandv("less", buf)) ||
       (args[0] = commandv("more", buf)))) {
    close(0);
    close(2);
    pipe(pip);
    if (!vfork()) {
      close(2);
      execv(args[0], args);
      _Exit(127);
    }
    close(0);
    __printargs("");
    close(2);
    wait(0);
  } else {
    __printargs("");
  }
}
