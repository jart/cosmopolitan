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
#include "libc/log/check.h"
#include "tool/plinko/lib/config.h"
#include "tool/plinko/lib/cons.h"
#include "tool/plinko/lib/gc.h"
#include "tool/plinko/lib/index.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/trace.h"

int Exlis(int x, int a) {
  int y;
  if (!x) return x;
  if (x > 0) return expand(x, a);
  y = expand(Car(x), a);
  return Keep(x, Cons(y, Exlis(Cdr(x), a)));
}

static int Expander(int e, int a) {
  int f, u, x, y, s;
  for (s = 0;;) {
    DCHECK_LT(e, TERM);
    DCHECK_LE(a, 0);
    if (e >= 0) return e;
    if ((f = Car(e)) > 0) {
      if (f == kQuote) return e;
      if (f == kClosure) return e;
      if (f == kTrace) {
        START_TRACE;
        x = Cadr(e);
        y = expand(x, a);
        e = x == y ? e : List(Car(e), y);
        END_TRACE;
        return e;
      }
      if (HasAtom(f, s)) return e;
      s = Cons(f, s);
    }
    e = Exlis(e, a);
    if (f >= 0) {
      if (!(f = Assoc(f, a))) return e;
      f = Cdr(f);
      if (f >= 0) return e;
    }
    if (Car(f) == kClosure) {
      u = Cddr(f);
      f = Cadr(f);
    } else {
      u = a;
    }
    if (Head(f) != kMacro) return e;
    e = eval(Caddr(f), pairlis(Cadr(f), Cdr(e), u));
  }
}

int Expand(int e, int a) {
  int r, A;
  A = cx;
  Push(List(kExpand, e));
  r = Keep(e, Expander(e, a));
  Pop();
  r = MarkSweep(A, r);
  return r;
}
