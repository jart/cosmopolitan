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
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"

/**
 * @fileoverview Prints sequence of numbers.
 */

int main(int argc, char *argv[]) {
  long a, b, i;
  char buf[21], *p;
  switch (argc) {
    case 2:
      a = 1;
      b = strtol(argv[1], NULL, 0);
      break;
    case 3:
      a = strtol(argv[1], NULL, 0);
      b = strtol(argv[2], NULL, 0);
      break;
    default:
      return 1;
  }
  for (i = a; i <= b; ++i) {
    p = buf;
    p = FormatInt64(p, i);
    *p++ = '\n';
    write(1, buf, p - buf);
  }
}
