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
#include "libc/testlib/ezbench.h"

/**
 * @fileoverview Fast Modulus Using Multiplication Tutorial
 *
 * Expected program output:
 *
 *     23 / 3 = 7
 *     0x5555555555555556 1 1
 *     modulus   l: 15𝑐  5𝑛𝑠
 *     fastmod   l:  4𝑐  1𝑛𝑠
 *     precomp   l: 18𝑐  6𝑛𝑠
 */

struct Modulus {
  uint64_t c;
  uint64_t d;
};

struct Modulus GetModulus(uint64_t d) {
  return (struct Modulus){0xFFFFFFFFFFFFFFFFull / d + 1, d};
}

uint64_t Modulus(uint64_t x, struct Modulus m) {
  return ((uint128_t)(m.c * x) * m.d) >> 64;
}

int main(int argc, char *argv[]) {
  printf("%#lx %% %d = %d\n", 0x23, 3, Modulus(23, GetModulus(3)));
  printf("%#lx %% %d = %d\n", 0x123, 17, Modulus(0x123, GetModulus(17)));
  volatile struct Modulus v = GetModulus(3);
  volatile uint64_t x = 23, y = 3, z;
  EZBENCH2("modulus", donothing, z = x % y);
  EZBENCH2("fastmod", donothing, z = Modulus(x, v));
  EZBENCH2("precomp", donothing, v = GetModulus(y));
  return 0;
}
