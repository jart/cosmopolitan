#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"

int CountAtoms(int x, int max, int res) {
  if (!x || res >= max)
    return res;
  if (x > 0)
    return res + 1;
  return CountAtoms(Cdr(x), max, CountAtoms(Car(x), max, res));
}
