#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "cons.h"
#include "error.h"
#include "plinko.h"

int Pairlis(int x, int y, int a) {
  if (!x)
    return a;
  if (x > 0)
    return Alist(x, y, a);
  if (y <= 0) {
    a = pairlis(Cdr(x), Cdr(y), a);
    return Car(x) ? pairlis(Car(x), Car(y), a) : a;
  } else {
    Error(L"argument structure%n"
          L" want: %S%n"
          L"  got: %S",
          x, y);
  }
}
