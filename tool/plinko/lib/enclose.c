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
#include "tool/plinko/lib/config.h"
#include "tool/plinko/lib/cons.h"
#include "tool/plinko/lib/error.h"
#include "tool/plinko/lib/gc.h"
#include "tool/plinko/lib/index.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/printf.h"
#include "tool/plinko/lib/stack.h"

static void CheckClosureFullyDefined(int e, int a, int s) {
  int f;
  Push(e);
  if (e >= 0) {
    if (!IsPrecious(e) && !HasAtom(e, s) && !Assoc(e, a)) {
      Error("crash binding in closure");
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
