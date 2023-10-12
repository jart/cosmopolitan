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
  char ibuf[21];
  switch (argc) {
    case 2:
      a = 1;
      b = strtol(argv[1], 0, 0);
      break;
    case 3:
      a = strtol(argv[1], 0, 0);
      b = strtol(argv[2], 0, 0);
      break;
    default:
      tinyprint(2, argv[0] ? argv[0] : "seq", ": missing operand\n", NULL);
      return 1;
  }
  for (i = a; i <= b; ++i) {
    FormatInt64(ibuf, i);
    tinyprint(1, ibuf, "\n", NULL);
  }
}
