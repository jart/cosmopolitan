#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/math.h"
#include "libc/stdio/stdio.h"

int main(int argc, char *argv[]) {
  volatile long double x = -.5;
  volatile long double y = 1.5;
  printf("Hello World! %.19Lg\n", atan2l(x, y));
  return 0;
}
