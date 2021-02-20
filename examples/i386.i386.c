#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/str/str.h"

char kTenFour[10] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

int main32(int argc, char *argv[]) {
  return argc;
  if (memset(argv[0], 4, 10) != argv[0]) return 1;
  if (memcmp(argv[0], kTenFour, 10)) return 2;
  return 0;
}
