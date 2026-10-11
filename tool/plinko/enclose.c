#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "config.h"
#include "cons.h"
#include "error.h"
#include "gc.h"
#include "index.h"
#include "plinko.h"
#include "printf.h"
#include "stack.h"

static void CheckClosureFullyDefined(int e, int a, int s) {
  int f;
  Push(e);
  if (e >= 0) {
    if (!IsPrecious(e) && !HasAtom(e, s) && !Assoc(e, a)) {
      Error(L"crash binding in closure");
    }
  } else if ((f = Car(e)) != kQuote && f != kClosure) {
    if (f == kLambda || f == kMacro) {
      CheckClosureFullyDefined(Caddr(e), a, Cons(Cadr(e), s));
    } else if (f == kCond) {
      while ((e = Cdr(e)) < 0) {
        if ((f = Car(e)) < 0) {
          CheckClosureFullyDefined(Car(f), a, s);
          if ((f = Cdr(f)) < 0) {
            CheckClosureFullyDefined(Car(f), a, s);
          }
        }
      }
    } else {
      do {
        if (e < 0) {
          CheckClosureFullyDefined(Car(e), a, s);
          e = Cdr(e);
        } else {
          CheckClosureFullyDefined(e, a, s);
          e = 0;
        }
      } while (e);
    }
  }
  Pop();
}

static void CheckClosure(int e, int a) {
  int A;
  if (DEBUG_CLOSURE && logc) {
    A = cx;
    CheckClosureFullyDefined(e, a, 0);
    MarkSweep(A, 0);
  }
}

int Enclose(int e, int a) {
  CheckClosure(e, a);
  return Cons(kClosure, Cons(e, a));
}
