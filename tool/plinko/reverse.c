#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "cons.h"
#include "plinko.h"

int Reverse(int x, int y) {
  dword t;
  while (x < 0) {
    t = Get(x);
    x = HI(t);
    y = Cons(LO(t), y);
  }
  return y;
}
