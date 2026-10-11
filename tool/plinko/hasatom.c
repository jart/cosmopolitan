#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"

bool HasAtom(int v, int x) {
  if (!x)
    return false;
  if (x > 0)
    return v == x;
  return HasAtom(v, Car(x)) || HasAtom(v, Cdr(x));
}
