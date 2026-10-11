#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "cons.h"
#include "plinko.h"

struct Binding Bind(int x, int y, int a, int u, dword p1, dword p2) {
  int k, v;
  dword a1 = 0;
  while (x) {
    if (x < 0) {
      if (y <= 0) {
        k = Car(x), x = Cdr(x);
        v = Car(y), y = Cdr(y);
        if (k) {
          if (k > 0) {
            if (!a1) {
              a1 = MAKE(k, FasterRecurse(v, a, p1, p2));
            } else {
              u = Alist(k, FasterRecurse(v, a, p1, p2), u);
            }
          } else {
            u = pairlis(k, FasterRecurse(v, a, p1, p2), u);
          }
        }
      } else {
        u = pairlis(x, FasterRecurse(y, a, p1, p2), u);
        y = 0;
        break;
      }
    } else {
      u = Alist(x, evlis(y, a, p1, p2), u);
      y = 0;
      break;
    }
  }
  if (y < 0) {
    Error(L"bind: too many arguments x=%S y=%S", x, y);
  }
  return (struct Binding){u, a1};
}
