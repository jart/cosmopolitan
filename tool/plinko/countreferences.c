#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "index.h"
#include "plinko.h"

/**
 * Counts references to variable.
 *
 * @param v is atom name of variable
 * @param m is count after which we should stop counting
 * @param e is expression
 * @return number of counted references greater than zero
 * @note this produces garbage when recursing into let
 */
int CountReferences(int v, int m, int e) {
  int f, r;
  assert(v > 0);
  if (e >= 0) {
    r = e == v;
  } else {
    f = Car(e);
    if (f == kQuote || f == kClosure) {
      r = 0;
    } else if (f == kLambda || f == kMacro) {
      if (m > 0 && !HasAtom(v, Cadr(e))) {
        r = CountReferences(v, m, Caddr(e));
      } else {
        r = 0;
      }
    } else if (f == kCond) {
      for (r = 0; (e = Cdr(e)) < 0 && r < m;) {
        if ((f = Car(e)) < 0) {
          r += CountReferences(v, m - r, Car(f));
          if ((f = Cdr(f)) < 0) {
            r += CountReferences(v, m - r, Car(f));
          }
        }
      }
    } else {
      for (r = 0; e && r < m;) {
        if (e < 0) {
          f = Car(e);
          e = Cdr(e);
        } else {
          f = e;
          e = 0;
        }
        r += CountReferences(v, m - r, f);
      }
    }
  }
  assert(r >= 0);
  return r;
}
