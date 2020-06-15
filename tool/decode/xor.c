/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/x/x.h"

/**
 * @fileoverview Hex to binary converter program.
 * Non-hex bytes are ignored. If you've got imposter syndrome you could
 * call this a compiler and start coding in hex.
 */

int main(int argc, char *argv[]) {
  size_t i, j, l;
  uint8_t *buf;
  if (argc == 1) return 1;
  buf = gc(xmalloc((l = strlen(argv[1]) / 2)));
  for (j = 0; j < l; ++j) {
    buf[j] = 0;
  }
  for (i = 1; i < argc; ++i) {
    for (j = 0; j < l; ++j) {
      buf[j] ^= hextoint(argv[i][j + 0]) << 4 | hextoint(argv[i][j + 1]);
    }
  }
  for (j = 0; j < l; ++j) {
    putchar("0123456789abcdef"[(buf[j] >> 4) & 0xf]);
    putchar("0123456789abcdef"[(buf[j] >> 0) & 0xf]);
  }
  putchar('\n');
  return 0;
}
