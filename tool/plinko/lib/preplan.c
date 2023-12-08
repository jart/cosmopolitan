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
#include "libc/log/countbranch.h"
#include "libc/runtime/runtime.h"
#include "tool/plinko/lib/cons.h"
#include "tool/plinko/lib/index.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/printf.h"

static int CopyTree(int x) {
  int a, b;
  if (x >= 0) return x;
  b = CopyTree(Cdr(x));
  a = CopyTree(Car(x));
  return Cons(a, b);
}

static int PreplanCond(int e, int a, int s) {
  int f, g;
  if (!(e = Cdr(e))) return 0;
  if ((f = Car(e)) < 0) {
    if ((g = Cdr(f)) < 0) {
      f = List(Preplan(Car(f), a, s), Preplan(Car(g), a, s));
    } else {
      f = Cons(Preplan(Car(f), a, s), 0);
    }
  }
  return Cons(f, PreplanCond(e, a, s));
}

static int PreplanList(int e, int a, int s) {
  if (e >= 0) return e;
  return Cons(Preplan(Car(e), a, s), PreplanList(Cdr(e), a, s));
}

int Preplan(int e, int a, int s) {
  int f, x;
  struct qword q;
  if (e >= 0) return e;
  f = Car(e);
  if (f != kQuote) {
    if (f == kClosure) {
      /*
       * (CLOSURE (LAMBDA (X Y) Z) . A)
       * -1 = (   Z, -0) c[6]
       * -2 = (   Y, -0) c[5]
       * -3 = (   X, -2) c[4]
       * -4 = (  -3, -1) c[3]
       * -5 = (LAMB, -4) c[2]
       * -6 = (  -5,  A) c[1]
       * -7 = (CLOS, -5) c[0]
       */
      e = Cons(kClosure, Cons(Preplan(Cadr(e), Cddr(e), 0), Cddr(e)));
    } else if (f == kCond) {
      e = Cons(kCond, PreplanCond(e, a, s));
    } else if (f == kLambda || f == kMacro) {
      /*
       * (LAMBDA (X Y) Z)
       * -1 = (   Z, -0) l[4]
       * -2 = (   Y, -0) l[3]
       * -3 = (   X, -2) l[2]
       * -4 = (  -3, -1) l[1]
       * -5 = (LAMB, -4) l[0]
       */
      x = Preplan(Caddr(e), a, Shadow(Cadr(e), s));
      x = Cons(x, 0);
      x = Cons(CopyTree(Cadr(e)), x);
      e = Cons(f, x);
    } else {
      e = PreplanList(e, a, s);
    }
  }
  if (LO(GetShadow(e)) == EncodeDispatchFn(DispatchPlan)) {
    if ((q = IsIf(e)).ax) {
      /* x = Cons(LO(q.ax), Cons(HI(q.ax), LO(q.dx))); */
      /*
       * guarantees this order
       * -1 = (   Z, -0) if[5]
       * -2 = (   Y, -0) if[4]
       * -3 = (   X, -2) if[3]
       * -4 = (  -1, -0) if[2]
       * -5 = (  -3, -4) if[1]
       * -6 = (COND, -5) if[0]
       */
      e = Cons(LO(q.dx), 0);
      e = List3(kCond, List(LO(q.ax), HI(q.ax)), e);
      SetShadow(e, MAKE(DF(DispatchIf), 0));
    } else {
      SetShadow(e, Plan(e, a, s));
    }
  }
  return e;
}
