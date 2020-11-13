#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/stdio/stdio.h"

int main() {
  /*
   * Cosmopolitan's "Hello World" demonstrates a best practice.
   *
   * Since we conditionally link heavyweight features based on which
   * characters are present in the format string, it's a good idea to
   * have that string consist solely of directives.
   */
  printf("%s\n", "hello world");
  return 0;
}
