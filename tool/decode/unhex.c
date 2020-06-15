/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * @fileoverview Hex to binary converter program.
 * Non-hex bytes are ignored. If you've got imposter syndrome you could
 * call this a compiler and start coding in hex.
 */

int main() {
  int o, t = -1;
  while (0 <= (o = getchar()) && o <= 255) {
    if (!isxdigit(o)) continue;
    int h = hextoint(o);
    if (t != -1) putchar(t * 16 + h), h = -1;
    t = h;
  }
  if (ferror(stdout)) return 1;
  if (t != -1) return 2;
  return 0;
}
