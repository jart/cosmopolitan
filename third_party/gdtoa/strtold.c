#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "third_party/gdtoa/gdtoa.h"

/**
 * Converts string to long double.
 */
long double strtold(const char *s, char **endptr) {
  long double x;
  strtorx(s, endptr, FPI_Round_near, &x);
  return x;
}
