#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include <assert.h>
#include "print.h"
#include "types.h"

int PrintInt(int fd, long x, int cols, char quot, char zero, int base,
             bool issigned) {
  dword y;
  char z[32];
  int i, j, k, n;
  assert(base <= 36);
  i = j = 0;
  y = x < 0 && issigned ? -x : x;
  do {
    if (quot && j == 3)
      z[i++ & 31] = quot, j = 0;
    z[i++ & 31] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[y % base];
  } while (++j, (y /= base));
  k = i + (x < 0 && issigned);
  if (zero) {
    n = PrintZeroes(fd, +cols - k);
  } else {
    n = PrintIndent(fd, +cols - k);
  }
  if (x < 0 && issigned)
    n += PrintChar(fd, L'-');
  while (i)
    n += PrintChar(fd, z[--i & 31]);
  PrintIndent(fd, -cols - n);
  return n;
}
