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
#include "libc/macros.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"

/**
 * @fileoverview Fast Division Using Multiplication Tutorial
 *
 * Expected program output:
 *
 *     23 / 3 = 7
 *     0x5555555555555556 1 1
 *     division   l: 16𝑐  5𝑛𝑠
 *     fast div   l:  5𝑐  2𝑛𝑠
 *     precomps   l: 70𝑐 23𝑛𝑠
 */

struct Divisor {
  uint64_t m;
  uint8_t s;
  uint8_t t;
};

struct Divisor GetDivisor(uint64_t d) {
  int b;
  uint128_t x;
  if (!d) raise(SIGFPE);
  b = __builtin_clzll(d) ^ 63;
  x = -d & (((1ull << b) - 1) | (1ull << b));
  return (struct Divisor){(x << 64) / d + 1, MIN(1, b + 1), MAX(0, b)};
}

uint64_t Divide(uint64_t x, struct Divisor d) {
  uint128_t t;
  uint64_t l, h;
  t = d.m;
  t *= x;
  l = t;
  h = t >> 64;
  l = (x - h) >> d.s;
  return (h + l) >> d.t;
}

int main(int argc, char *argv[]) {
  printf("23 / 3 = %ld\n", Divide(23, GetDivisor(3)));
  volatile struct Divisor v = GetDivisor(3);
  volatile uint64_t x = 23, y = 3, z;
  EZBENCH2("division", donothing, z = x / y);
  EZBENCH2("fast div", donothing, z = Divide(x, v));
  EZBENCH2("precomp ", donothing, v = GetDivisor(y));
  return 0;
}
