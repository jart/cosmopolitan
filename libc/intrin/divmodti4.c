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
 * Divides 128-bit signed integers w/ remainder.
 *
 * @param a is numerator
 * @param b is denominator
 * @param opt_out_rem receives euclidean division remainder if not null
 * @return quotient or result of division
 * @note rounds towards zero
 */
ti_int __divmodti4(ti_int a, ti_int b, tu_int *opt_out_rem) {
  int k;
  tu_int r;
  ti_int sa, sb, sq, sr, x, y, q;
  k = sizeof(ti_int) * CHAR_BIT - 1;
  if (b == -1 && a == ((ti_int)1 << k)) {
    volatile int x = 0;
    x = 1 / x;  // raise(SIGFPE)
  }
  sa = a >> k;                 // sa = a < 0 ? -1 : 0
  sb = b >> k;                 // sb = b < 0 ? -1 : 0
  x = (tu_int)(a ^ sa) - sa;   // negate if sa == -1
  y = (tu_int)(b ^ sb) - sb;   // negate if sb == -1
  sq = sa ^ sb;                // sign of quotient
  sr = sa;                     // sign of remainder
  q = __udivmodti4(x, y, &r);  // unsigned divide
  q = (tu_int)(q ^ sq) - sq;   // fix quotient sign
  r = (tu_int)(r ^ sr) - sr;   // fix remainder sign
  if (opt_out_rem) *opt_out_rem = r;
  return q;
}

/*

  Intel Kabylake i9-9900 @ 3.10GHz Client Grade

    idiv32                         l:         27𝑐          9𝑛𝑠
    idiv64                         l:         27𝑐          9𝑛𝑠
    divmodti4 small / small        l:         42𝑐         14𝑛𝑠
    divmodti4 small / large        l:         14𝑐          5𝑛𝑠
    divmodti4 large / small        l:         92𝑐         30𝑛𝑠
    divmodti4 large / large        l:        209𝑐         68𝑛𝑠

  Intel Kabylake i3-8100 @ 3.60GHz Client Grade

    idiv32                         l:         51𝑐         14𝑛𝑠
    idiv64                         l:         51𝑐         14𝑛𝑠
    divmodti4 small / small        l:         83𝑐         23𝑛𝑠
    divmodti4 small / large        l:         26𝑐          7𝑛𝑠
    divmodti4 large / small        l:        175𝑐         48𝑛𝑠
    divmodti4 large / large        l:        389𝑐        107𝑛𝑠

*/
