#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "cons.h"
#include "plinko.h"

int Evlis(int x, int a, dword p1, dword p2) {
  if (!x)
    return x;
  if (x > 0)
    return FasterRecurse(x, a, p1, p2);
  int y = FasterRecurse(Car(x), a, p1, p2);
  return Cons(y, Evlis(Cdr(x), a, p1, p2));
}
