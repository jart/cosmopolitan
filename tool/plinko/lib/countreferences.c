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
#include "tool/plinko/lib/index.h"
#include "tool/plinko/lib/plinko.h"

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
  DCHECK_GT(v, 0);
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
  DCHECK_GE(r, 0);
  return r;
}
