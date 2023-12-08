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
