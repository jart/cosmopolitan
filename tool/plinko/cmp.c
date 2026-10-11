#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"

/**
 * Compares LISP data structures
 *
 *     (≷ 𝑥 𝑦) ⟹ (⊥) | ⊥ | ⊤
 *
 * Where
 *
 *     (⊥) means less than       a.k.a. -1
 *      ⊥  means equal           a.k.a.  0
 *      ⊤  means greater than    a.k.a. +1
 *
 * The comparison is performed as follows:
 *
 *     (≷ 𝑥 𝑥) ⟹ ⊥                   everything's equal to itself
 *     (≡ 𝑥 𝑦) ⟶ (≡ (≷ 𝑥 𝑦) ⊥)       (eq) and (cmp) agree if (eq) returns t
 *     (≡ (≷ 𝑥 𝑦) ⊥) ⟺ (equal 𝑥 𝑦)   (cmp) returns eq iff (equal) returns t
 *     (≷ (ℶ x 𝑦) (ℶ x 𝑦)) ⟹ ⊥       i.e. this does deep comparisons
 *     (≷ ⊥ 𝑥) ⟹ (⊥)                 nil is less than everything non-nil
 *     (≷ 𝑥 ⊥) ⟹ ⊤                   comparisons are always symmetric
 *     (≷ 𝑖 𝑗) ⟹ (⊥)                 atom vs. atom compares unicodes
 *     (≷ 𝑖𝑗 𝑘𝑙) ⟺ (≷ (𝑖 𝑗) (𝑘 𝑙))   atom characters treated like lists
 *     (≷ 𝑖 (x . 𝑦)) ⟹ (⊥)           atom vs. cons is always less than
 *     (≷ (x . 𝑦) (x . 𝑦)) ⟹ ⊥       cons vs. cons just recurses
 *     (≷ (𝑥) (⊥ 𝑦)) ⟹ ⊤             e.g. cmp returns gt because 𝑥 > ⊥
 *     (≷ (𝑥) (𝑧 𝑦)) ⟹ (⊥)           e.g. cmp returns lt because ⊥ < (𝑦)
 *     (≷ (x . 𝑦) (x 𝑦)) ⟹ (⊥)       e.g. cmp returns lt because 𝑦 < (𝑦)
 *
 * @return -1, 0, +1
 */
int Cmp(int x, int y) {
  int c;
  dword t, u;
  if (x == y)
    return 0;
  if (x > 1 && y > 1) {
    if (LO(Get(x)) < LO(Get(x)))
      return -1;
    if (LO(Get(x)) > LO(Get(x)))
      return +1;
  }
  for (;; x = Cdr(x), y = Cdr(y)) {
    if (x == y)
      return 0;
    if (!x)
      return -1;
    if (!y)
      return +1;
    if (x < 0) {
      if (y >= 0)
        return +1;
      if ((c = Cmp(Car(x), Car(y))))
        return c;
    } else {
      if (y < 0)
        return -1;
      for (;;) {
        t = x != 1 ? Get(x) : MAKE(L'T', TERM);
        u = y != 1 ? Get(y) : MAKE(L'T', TERM);
        if (LO(t) != LO(u)) {
          return LO(t) < LO(u) ? -1 : +1;
        }
        x = HI(t);
        y = HI(u);
        if (x == y)
          return 0;
        if (x == TERM)
          return -1;
        if (y == TERM)
          return +1;
      }
      if (Car(x) != Car(y)) {
        return Car(x) < Car(y) ? -1 : +1;
      }
    }
  }
}
