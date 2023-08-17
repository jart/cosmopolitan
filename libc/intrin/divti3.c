#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to division,     │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "third_party/compiler_rt/int_lib.h"

/**
 * Divides 128-bit signed integers.
 *
 * @param a is numerator
 * @param b is denominator
 * @return quotient or result of division
 * @note rounds towards zero
 */
ti_int __divti3(ti_int a, ti_int b) {
  return __divmodti4(a, b, NULL);
}
