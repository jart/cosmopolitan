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
#include "stack.h"

struct T DispatchRecur(dword ea, dword tm, dword r, dword p1, dword p2,
                       dword d) {
  struct Binding bz;
  bz = bind_(Car(Car(HI(d))), Cdr(LO(ea)), HI(ea), Cdr(HI(d)), p1, p2);
  return TailCall(MAKE(Cdr(Car(HI(d))), bz.u), tm, r, bz.p1, 0);
}

struct T DispatchRecur1(dword ea, dword tm, dword r, dword p1, dword p2,
                        dword d) {
  return TailCall(
      MAKE(Car(HI(d) + 5), Cdr(HI(d) + 1)), tm, r,
      MAKE(Car(HI(d) + 4), FasterRecurse(Car(Cdr(LO(ea))), HI(ea), p1, p2)), 0);
}

struct T DispatchRecur2(dword ea, dword tm, dword r, dword p1, dword p2,
                        dword d) {
  return TailCall(
      MAKE(Car(HI(d) + 6), Cdr(HI(d))), tm, r,
      MAKE(Car(HI(d) + 4), FasterRecurse(Car(Cdr(LO(ea))), HI(ea), p1, p2)),
      MAKE(Car(HI(d) + 5),
           FasterRecurse(Car(Cdr(Cdr(LO(ea)))), HI(ea), p1, p2)));
}

struct T DispatchYcombine(dword ea, dword tm, dword r, dword p1, dword p2,
                          dword d) {
  int ycomb, p, name, lambda, closure;
  SetFrame(r, LO(ea));
  r |= NEED_GC;
  ycomb = recurse(MAKE(Car(LO(ea)), HI(ea)), p1, p2);
  assert(IsYcombinator(ycomb));
  ycomb = Cadr(ycomb);
  lambda = recurse(MAKE(Cadr(ea), HI(ea)), p1, p2);
  closure =
      recurse(MAKE(Caddr(ycomb), Alist(Car(Cadr(ycomb)), lambda, 0)), 0, 0);
  if (Car(lambda) == kClosure)
    lambda = Car(Cdr(lambda));
  assert(kClosure == Car(closure));
  assert(kLambda == Car(lambda));
  assert(kLambda == Car(Car(Cdr(closure))));
  name = Car(Cadr(lambda));
  lambda = Cadr(closure);
  closure = Enclose(lambda, Cddr(closure));
  closure = Preplan(closure, Cddr(closure), 0);
  lambda = Cadr(closure);
  if ((p = CountSimpleParameters(Cadr(lambda))) == 1 || p == 2) {
    if (p == 1) {
      PlanFuncalls(name, MAKE(DF(DispatchRecur1), closure), Caddr(lambda));
    } else {
      PlanFuncalls(name, MAKE(DF(DispatchRecur2), closure), Caddr(lambda));
    }
  } else {
    PlanFuncalls(name,
                 MAKE(DF(DispatchRecur),
                      Cons(Cons(Cadr(Cadr(closure)), Caddr(Cadr(closure))),
                           Cddr(closure))),
                 Caddr(lambda));
  }
  return Ret(MAKE(closure, 0), tm, r);
}
