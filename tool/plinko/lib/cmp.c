/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "tool/plinko/lib/plinko.h"

/**
 * Compares LISP data structures.
 *
 *     (cmp 𝑥 𝑥) ⟹ eq                  everything's equal to itself
 *     (≡ 𝑥 𝑦) ⟶ (≡ (cmp 𝑥 𝑦) 'eq)     (eq) and (cmp) agree if (eq) returns t
 *     (≡ (cmp 𝑥 𝑦) 'eq) ⟺ (equal 𝑥 𝑦) (cmp) returns eq iff (equal) returns t
 *     (cmp (ℶ x 𝑦) (ℶ x 𝑦)) ⟹ eq      i.e. this does deep comparisons
 *     (cmp ⊥ 𝑥) ⟹ lt                  nil is less than everything non-nil
 *     (cmp 𝑥 ⊥) ⟹ t                   comparisons are always symmetric
 *     (cmp 𝑖 𝑗) ⟹ lt                  atom vs. atom compares unicodes
 *     (cmp 𝑖𝑗 𝑘𝑙) ⟺ (cmp (𝑖 𝑗) (𝑘 𝑙)) atom characters treated like lists
 *     (cmp 𝑖 (x . 𝑦)) ⟹ lt            atom vs. cons is always less than
 *     (cmp (x . 𝑦) (x . 𝑦)) ⟹ eq      cons vs. cons just recurses
 *     (cmp (𝑥) (⊥ 𝑦)) ⟹ t             e.g. cmp returns gt because 𝑥 > ⊥
 *     (cmp (𝑥) (𝑧 𝑦)) ⟹ lt            e.g. cmp returns lt because ⊥ < (𝑦)
 *     (cmp (x . 𝑦) (x 𝑦)) ⟹ lt        e.g. cmp returns lt because 𝑦 < (𝑦)
 *
 * @return -1, 0, +1
 */
int Cmp(int x, int y) {
  int c;
  dword t, u;
  if (x == y) return 0;
  if (x > 1 && y > 1) {
    if (LO(Get(x)) < LO(Get(x))) return -1;
    if (LO(Get(x)) > LO(Get(x))) return +1;
  }
  for (;; x = Cdr(x), y = Cdr(y)) {
    if (x == y) return 0;
    if (!x) return -1;
    if (!y) return +1;
    if (x < 0) {
      if (y >= 0) return +1;
      if ((c = Cmp(Car(x), Car(y)))) return c;
    } else {
      if (y < 0) return -1;
      for (;;) {
        t = x != 1 ? Get(x) : MAKE(L'T', TERM);
        u = y != 1 ? Get(y) : MAKE(L'T', TERM);
        if (LO(t) != LO(u)) {
          return LO(t) < LO(u) ? -1 : +1;
        }
        x = HI(t);
        y = HI(u);
        if (x == y) return 0;
        if (x == TERM) return -1;
        if (y == TERM) return +1;
      }
      if (Car(x) != Car(y)) {
        return Car(x) < Car(y) ? -1 : +1;
      }
    }
  }
}
