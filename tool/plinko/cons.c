#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "cons.h"
#include "char.h"

int List(int x, int y) {
  return Cons(x, Cons(y, -0));
}

int List3(int x, int y, int z) {
  return Cons(x, List(y, z));
}

int Shadow(int p, int s) {
  int t = GetCommonCons(p, s);
  return t ? t : Cons(p, s);
}

int GetCommonCons(int x, int y) {
  if (!y) {
    if (!x)
      return -1;
    if (x > 0 && cFrost < -1 && IsUpper(LO(Get(x))) && HI(Get(x)) == TERM) {
      return kConsAlphabet[LO(Get(x)) - L'A'];
    }
  }
  return 0;
}

int ShareCons(int x, int y) {
  int i;
  if ((i = GetCommonCons(x, y)))
    return i;
#if 0
  t = MAKE(x, y);
  for (i = cx, n = MIN(0, i + 64); i < n; ++i) {
    if (t == Get(i)) {
      return i;
    }
  }
#endif
  return Cons(x, y);
}

int ShareList(int x, int y) {
  return ShareCons(x, ShareCons(y, -0));
}
