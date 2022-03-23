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
#include "libc/log/check.h"
#include "libc/stdio/stdio.h"
#include "libc/time/time.h"

int main(int argc, char *argv[]) {
  double x[3];
  CHECK_NE(-1, getloadavg(x, 3));
  printf("%g %g %g\n", x[0], x[1], x[2]);
  return 0;
}
