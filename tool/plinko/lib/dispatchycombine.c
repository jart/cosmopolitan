/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "tool/plinko/lib/cons.h"
#include "tool/plinko/lib/index.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/stack.h"

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
  DCHECK(IsYcombinator(ycomb));
  ycomb = Cadr(ycomb);
  lambda = recurse(MAKE(Cadr(ea), HI(ea)), p1, p2);
  closure =
      recurse(MAKE(Caddr(ycomb), Alist(Car(Cadr(ycomb)), lambda, 0)), 0, 0);
  if (Car(lambda) == kClosure) lambda = Car(Cdr(lambda));
  DCHECK_EQ(kClosure, Car(closure));
  DCHECK_EQ(kLambda, Car(lambda));
  DCHECK_EQ(kLambda, Car(Car(Cdr(closure))));
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
