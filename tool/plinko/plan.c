#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "cons.h"
#include "error.h"
#include "index.h"
#include "plinko.h"
#include "printf.h"
#include "stack.h"
#include "types.h"

int CountSimpleParameters(int x) {
  int i;
  for (i = 0; x; ++i, x = Cdr(x)) {
    if (x > 0)
      return -1;  // variadic args aren't simple
    if (!Car(x))
      return -1;  // nil parameters aren't simple
    if (Car(x) < 0)
      return -1;  // destructured parameters aren't simple
  }
  return i;
}

int CountSimpleArguments(int x) {
  int i;
  for (i = 0; x; ++i, x = Cdr(x)) {
    if (x > 0)
      return -1;  // apply isn't simple
  }
  return i;
}

static dword PlanQuote(int e, int a, int s) {
  if (Cdr(e) >= 0)
    React(e, e, kQuote);                    // one normal parameter required
  return MAKE(DF(DispatchQuote), Cadr(e));  // >1 prms is sectorlisp comment
}

static dword PlanCar(int e, int a, int s) {
  if (!Cdr(e))
    return DF(DispatchNil);  // (⍅) ⟺ (⍅ ⊥)
  if (Cddr(e))
    React(e, e, kCar);  // too many args
  if (!Cadr(e))
    return DF(DispatchNil);
  return MAKE(DF(DispatchCar), Cadr(e));
}

static dword PlanCdr(int e, int a, int s) {
  if (!Cdr(e))
    return DF(DispatchNil);  // (⍆) ⟺ (⍆ ⊥)
  if (Cddr(e))
    React(e, e, kCdr);  // too many args
  if (!ARG1(e))
    return DF(DispatchNil);
  return MAKE(DF(DispatchCdr), Cadr(e));
}

static dword PlanAtom(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) != 1)
    React(e, e, kAtom);
  return MAKE(DF(DispatchAtom), Cadr(e));
}

static dword PlanEq(int e, int a, int s) {
  int n = CountSimpleArguments(Cdr(e));
  if (n != 2 && n != 1)
    React(e, e, kAtom);  // (≡ 𝑥) is our (null 𝑥)
  return MAKE(DF(DispatchEq), Caddr(e));
}

static dword PlanCmp(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) != 2)
    React(e, e, kCmp);
  return MAKE(DF(DispatchCmp), Caddr(e));
}

static dword PlanOrder(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) != 2)
    React(e, e, kOrder);
  return MAKE(DF(DispatchOrder), Caddr(e));
}

static dword PlanCons(int e, int a, int s) {
  int p = CountSimpleArguments(Cdr(e));
  if (p == -1)
    Error(L"cons dot arg");
  if (p > 2)
    Error(L"too many args");
  return MAKE(DF(DispatchCons), Caddr(e));
}

static dword PlanLambda(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) == -1)
    Error(L"bad lambda: %S", e);
  return DF(DispatchLambda);
}

static dword PlanCond(int e, int a, int s) {
  int x;
  if (!Cdr(e))
    return DF(DispatchNil);  // (ζ) ⟺ ⊥
  for (x = e; (x = Cdr(x));) {
    if (x > 0)
      React(e, e, kCond);  // (ζ . 𝑣) not allowed
    if (Car(x) >= 0)
      React(e, e, kCond);  // (ζ 𝑣) not allowed
    if (Cdr(Car(x)) > 0)
      React(e, e, kCond);  // (ζ (𝑥 . 𝑣)) not allowed
  }
  return MAKE(DF(DispatchCond), Cdr(e));
}

static dword PlanProgn(int e, int a, int s) {
  if (!Cdr(e))
    return DF(DispatchNil);  // (progn) ⟺ ⊥
  if (CountSimpleArguments(Cdr(e)) == -1)
    React(e, e, kProgn);
  return MAKE(DF(DispatchProgn), Cdr(e));
}

static dword PlanQuiet(int e, int a, int s) {
  if (Cdr(e) > 0)
    React(e, e, kQuiet);  // apply not allowed
  if (!Cdr(e))
    React(e, e, kQuiet);  // zero args not allowed
  if (Cdr(Cdr(e)))
    React(e, e, kQuiet);  // >1 args not allowed
  return DF(DispatchQuiet);
}

static dword PlanTrace(int e, int a, int s) {
  if (Cdr(e) > 0)
    React(e, e, kTrace);  // apply not allowed
  if (!Cdr(e))
    React(e, e, kTrace);  // zero args not allowed
  if (Cdr(Cdr(e)))
    React(e, e, kTrace);  // >1 args not allowed
  return DF(DispatchTrace);
}

static dword PlanFtrace(int e, int a, int s) {
  if (Cdr(e) > 0)
    React(e, e, kFtrace);  // apply not allowed
  if (!Cdr(e))
    React(e, e, kFtrace);  // zero args not allowed
  if (Cdr(Cdr(e)))
    React(e, e, kFtrace);  // >1 args not allowed
  return DF(DispatchFtrace);
}

static dword PlanFunction(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) != 1)
    Raise(kFunction);
  return MAKE(DF(DispatchFunction), Cadr(e));
}

static dword PlanBeta(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) != 1)
    Raise(kBeta);
  return MAKE(DF(DispatchBeta), Cadr(e));
}

static dword PlanIgnore(int e, int a, int s) {
  if (!Cdr(e))
    return DF(DispatchIgnore0);
  if (Cdr(e) > 0)
    React(e, e, kIgnore);  // apply not allowed
  if (!Cdr(e))
    React(e, e, kIgnore);  // zero args not allowed
  if (Cdr(Cdr(e)))
    React(e, e, kIgnore);  // >1 args not allowed
  return DF(DispatchIgnore1);
}

static dword PlanExpand(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) != 1)
    React(e, e, kExpand);
  return MAKE(DF(DispatchExpand), Cadr(e));
}

static dword PlanPrint(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) == -1)
    React(e, e, kPrint);
  return DF(DispatchPrint);
}

static dword PlanGensym(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)))
    React(e, e, kGensym);
  return DF(DispatchGensym);
}

static dword PlanPprint(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) == -1)
    React(e, e, kPprint);
  return DF(DispatchPprint);
}

static dword PlanPrintheap(int e, int a, int s) {
  int p = CountSimpleArguments(Cdr(e));
  if (p != 0 && p != 1)
    React(e, e, kPrintheap);
  return DF(DispatchPrintheap);
}

static dword PlanGc(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) != 1)
    React(e, e, kGc);
  return MAKE(DF(DispatchGc), Cadr(e));
}

static dword PlanPrinc(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) == -1)
    React(e, e, kPrinc);
  return DF(DispatchPrinc);
}

static dword PlanFlush(int e, int a, int s) {
  if (CountSimpleArguments(Cdr(e)) == -1)
    React(e, e, kFlush);
  return DF(DispatchFlush);
}

static dword PlanError(int e, int a, int s) {
  return DF(DispatchError);
}

static dword PlanExit(int e, int a, int s) {
  if (Cdr(e))
    React(e, e, kExit);
  return DF(DispatchExit);
}

static dword PlanRead(int e, int a, int s) {
  if (Cdr(e))
    React(e, e, kRead);
  return DF(DispatchRead);
}

static dword PlanDefine(int e, int a, int s) {
  return DF(DispatchIdentity);
}

static dword PlanClosure(int e, int a, int s) {
  return DF(DispatchIdentity);
}

static dword PlanLet(int e, int a, int s) {
  int n;
  if ((n = CountSimpleArguments(Cdr(e))) == -1)
    return DF(DispatchFuncall);
  if (CountSimpleArguments(Car(e)) < 3)
    React(e, e, kLambda);  // need (λ 𝑥 𝑦)
  switch (CountSimpleParameters(Cadr(Car(e)))) {
    case -1:
      return DF(DispatchFuncall);
    case 0:
      if (n != 0)
        Error(L"let argument count mismatch: %S", e);
      return MAKE(DF(DispatchShortcut), Caddr(Car(e)));  // ((λ ⊥ 𝑦)) becomes 𝑦
    case 1:
      if (n != 1)
        Error(L"let argument count mismatch: %S", e);
      return MAKE(DF(DispatchLet1), Cdar(e));
    default:
      return MAKE(DF(DispatchFuncall), 0);
  }
}

static dword PlanPrecious(int e, int a, int s, int f) {
  assert(f > 0);
  if (f == kCar)
    return PlanCar(e, a, s);
  if (f == kCdr)
    return PlanCdr(e, a, s);
  if (f == kGc)
    return PlanGc(e, a, s);
  if (f == kEq)
    return PlanEq(e, a, s);
  if (f == kCmp)
    return PlanCmp(e, a, s);
  if (f == kBeta)
    return PlanBeta(e, a, s);
  if (f == kCond)
    return PlanCond(e, a, s);
  if (f == kAtom)
    return PlanAtom(e, a, s);
  if (f == kCons)
    return PlanCons(e, a, s);
  if (f == kExit)
    return PlanExit(e, a, s);
  if (f == kRead)
    return PlanRead(e, a, s);
  if (f == kOrder)
    return PlanOrder(e, a, s);
  if (f == kQuote)
    return PlanQuote(e, a, s);
  if (f == kProgn)
    return PlanProgn(e, a, s);
  if (f == kQuiet)
    return PlanQuiet(e, a, s);
  if (f == kTrace)
    return PlanTrace(e, a, s);
  if (f == kPrint)
    return PlanPrint(e, a, s);
  if (f == kPrinc)
    return PlanPrinc(e, a, s);
  if (f == kFlush)
    return PlanFlush(e, a, s);
  if (f == kError)
    return PlanError(e, a, s);
  if (f == kMacro)
    return PlanLambda(e, a, s);
  if (f == kFtrace)
    return PlanFtrace(e, a, s);
  if (f == kLambda)
    return PlanLambda(e, a, s);
  if (f == kGensym)
    return PlanGensym(e, a, s);
  if (f == kPprint)
    return PlanPprint(e, a, s);
  if (f == kIgnore)
    return PlanIgnore(e, a, s);
  if (f == kExpand)
    return PlanExpand(e, a, s);
  if (f == kDefine)
    return PlanDefine(e, a, s);
  if (f == kClosure)
    return PlanClosure(e, a, s);
  if (f == kFunction)
    return PlanFunction(e, a, s);
  if (f == kPrintheap)
    return PlanPrintheap(e, a, s);
  if (!a) {
    Push(e);
    Push(f);
    Raise(kFunction);
  }
  return DF(DispatchFuncall);
}

dword Plan(int e, int a, int s) {
  int c, f, p, x1, x2, x3, x4;
  assert(e < 0);

  if (!unplanned && (x1 = IsCar(e))) {
    if ((x2 = IsCar(x1))) {
      if ((x3 = IsCar(x2))) {
        if ((x4 = IsCar(x3)))
          return MAKE(DF(DispatchCaaaar), x4);
        if ((x4 = IsCdr(x3)))
          return MAKE(DF(DispatchCaaadr), x4);
        return MAKE(DF(DispatchCaaar), x3);
      }
      if ((x3 = IsCdr(x2))) {
        if ((x4 = IsCar(x3)))
          return MAKE(DF(DispatchCaadar), x4);
        if ((x4 = IsCdr(x3)))
          return MAKE(DF(DispatchCaaddr), x4);
        return MAKE(DF(DispatchCaadr), x3);
      }
      return MAKE(DF(DispatchCaar), x2);
    }
    if ((x2 = IsCdr(x1))) {
      if ((x3 = IsCar(x2))) {
        if ((x4 = IsCar(x3)))
          return MAKE(DF(DispatchCadaar), x4);
        if ((x4 = IsCdr(x3)))
          return MAKE(DF(DispatchCadadr), x4);
        return MAKE(DF(DispatchCadar), x3);
      }
      if ((x3 = IsCdr(x2))) {
        if ((x4 = IsCar(x3)))
          return MAKE(DF(DispatchCaddar), x4);
        if ((x4 = IsCdr(x3)))
          return MAKE(DF(DispatchCadddr), x4);
        return MAKE(DF(DispatchCaddr), x3);
      }
      return MAKE(DF(DispatchCadr), x2);
    }
    return MAKE(DF(DispatchCar), x1);
  }

  if (!unplanned && (x1 = IsCdr(e))) {
    if ((x2 = IsCar(x1))) {
      if ((x3 = IsCar(x2))) {
        if ((x4 = IsCar(x3)))
          return MAKE(DF(DispatchCdaaar), x4);
        if ((x4 = IsCdr(x3)))
          return MAKE(DF(DispatchCdaadr), x4);
        return MAKE(DF(DispatchCdaar), x3);
      }
      if ((x3 = IsCdr(x2))) {
        if ((x4 = IsCar(x3)))
          return MAKE(DF(DispatchCdadar), x4);
        if ((x4 = IsCdr(x3)))
          return MAKE(DF(DispatchCdaddr), x4);
        return MAKE(DF(DispatchCdadr), x3);
      }
      return MAKE(DF(DispatchCdar), x2);
    }
    if ((x2 = IsCdr(x1))) {
      if ((x3 = IsCar(x2))) {
        if ((x4 = IsCar(x3)))
          return MAKE(DF(DispatchCddaar), x4);
        if ((x4 = IsCdr(x3)))
          return MAKE(DF(DispatchCddadr), x4);
        return MAKE(DF(DispatchCddar), x3);
      }
      if ((x3 = IsCdr(x2))) {
        if ((x4 = IsCar(x3)))
          return MAKE(DF(DispatchCdddar), x4);
        if ((x4 = IsCdr(x3)))
          return MAKE(DF(DispatchCddddr), x4);
        return MAKE(DF(DispatchCdddr), x3);
      }
      return MAKE(DF(DispatchCddr), x2);
    }
    return MAKE(DF(DispatchCdr), x1);
  }

  if ((f = Car(e)) > 0) {
    if (LO(GetShadow(f)) == EncodeDispatchFn(DispatchPrecious)) {
      return PlanPrecious(e, a, s, f);
    }
    // only a global definition is guaranteed to be what this name means
    // every time this expression is evaluated
    if (!unplanned && !HasAtom(f, s) && (f = AssocGlobal(f, a))) {
      f = Cdr(f);
      if (IsYcombinator(f)) {
        return DF(DispatchYcombine);
      } else if (f < 0 && Car(f) == kClosure && f > e) {
        if (Car(Cadr(f)) == kLambda) {
          c = CountSimpleArguments(Cdr(e));
          p = CountSimpleParameters(Cadr(Cadr(f)));
          if (c == 1 && p == 1) {
            return MAKE(DF(DispatchCall1), f);
          } else if (c == 2 && p == 2) {
            return MAKE(DF(DispatchCall2), f);
          }
        }
        return MAKE(DF(DispatchFuncall), f);
      }
    }
  } else if (Car(f) == kLambda && !unplanned) {
    return PlanLet(e, a, s);
  }

  return DF(DispatchFuncall);
}

struct T DispatchPlan(dword ea, dword tm, dword r, dword p1, dword p2,
                      dword d) {
  int s = 0;
  // p1 and p2 hold local bindings that aren't in the association list
  if (LO(p1))
    s = Cons(LO(p1), s);
  if (LO(p2))
    s = Cons(LO(p2), s);
  SetShadow(LO(ea), (d = Plan(LO(ea), HI(ea), s)));
  return DecodeDispatchFn(d)(ea, tm, r, p1, p2, d);
}
