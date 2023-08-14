/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"

/**
 * @fileoverview Hex to binary converter program.
 * Non-hex bytes are ignored. If you've got imposter syndrome you could
 * call this a compiler and start coding in hex.
 */

int main() {
  int h, o, t = -1;
  while ((o = getchar()) != -1) {
    if ((h = kHexToInt[o & 255]) != -1) {
      if (t != -1) {
        putchar(t * 16 + h);
        h = -1;
      }
      t = h;
    }
  }
}
