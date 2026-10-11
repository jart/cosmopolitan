#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "cons.h"
#include "index.h"
#include "plinko.h"

void PlanFuncalls(int n, dword p, int x) {
  int h;
  if (x < 0 && (h = Car(x)) != kQuote && h != kClosure && h != kMacro) {
    if (h == kLambda) {
      if (!HasAtom(n, Cadr(x))) {
        PlanFuncalls(n, p, Caddr(x));
      }
    } else if (h == kCond) {
      while ((x = Cdr(x)) < 0) {
        if ((h = Car(x)) < 0) {
          PlanFuncalls(n, p, Car(h));
          if ((h = Cdr(h)) < 0) {
            PlanFuncalls(n, p, Car(h));
          }
        }
      }
    } else {
      if (h == n) {
        SetShadow(x, p);
      }
      while (x) {
        if (x < 0) {
          h = Car(x);
          x = Cdr(x);
        } else {
          h = x;
          x = 0;
        }
        PlanFuncalls(n, p, h);
      }
    }
  }
}
