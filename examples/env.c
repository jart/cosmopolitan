#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <stdio.h>
#include <stdlib.h>

/**
 * @fileoverview prints environment variables
 */

int main(int argc, char* argv[]) {
  for (char** p = environ; *p; ++p)
    puts(*p);
  return 0;
}
