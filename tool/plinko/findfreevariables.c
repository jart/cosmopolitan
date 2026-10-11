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

/**
 * Finds free variables in expression.
 *
 * @param e is expression
 * @param r is atom list result accumulator
 * @param s is atom tree of shadowed bindings
 * @return atom list of crash bindings without duplicates
 * @note this produces garbage when recursing into let
 */
int FindFreeVariables(int e, int r, int s) {
  int f, t;
  if (e >= 0) {
    if (!IsConstant(e) && !HasAtom(e, s) && !HasAtom(e, r)) {
      t = GetCommonCons(e, r);
      r = t ? t : Cons(e, r);
    }
  } else {
    f = Car(e);
    if (f != kQuote && f != kClosure) {
      if (f == kLambda || f == kMacro) {
        r = FindFreeVariables(Caddr(e), r, Shadow(Cadr(e), s));
      } else if (f == kCond) {
        while ((e = Cdr(e)) < 0) {
          if ((f = Car(e)) < 0) {
            r = FindFreeVariables(Car(f), r, s);
            if ((f = Cdr(f)) < 0) {
              r = FindFreeVariables(Car(f), r, s);
            }
          }
        }
      } else {
        while (e) {
          if (e < 0) {
            f = Car(e);
            e = Cdr(e);
          } else {
            f = e;
            e = 0;
          }
          r = FindFreeVariables(f, r, s);
        }
      }
    }
  }
  return r;
}
