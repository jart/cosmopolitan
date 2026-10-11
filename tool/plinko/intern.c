#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "cons.h"
#include "plinko.h"

#define MAX_(X, Y) ((Y) < (X) ? (X) : (Y))

static inline int Probe(unsigned h, unsigned p) {
  return (h + p * p) & MASK(TERM);
}

static inline int Hash(unsigned h, unsigned x) {
  return MAX_(2, ((h * 0xdeaadead) ^ x) & MASK(TERM));
}

static int Interner(dword t, int h, int p) {
  dword u;
  if ((u = Get(h))) {
    if (u != t) {
      h = Interner(t, Probe(h, p), p + 1);
    }
    return h;
  } else if (++cAtoms < TERM / 2) {
    Set(h, t);
    SetShadow(h, DF(DispatchLookup));
    return h;
  } else {
    Error(L"too many atoms");
  }
}

int Intern(int x, int y) {
  return Interner(MAKE(x, y), (ax = Hash(x, ax)), 1);
}
