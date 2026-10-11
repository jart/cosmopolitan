#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "index.h"

#define INDEXER(NAME, EVAL)                                                 \
  struct T NAME(dword ea, dword tm, dword r, dword p1, dword p2, dword d) { \
    return Ret(MAKE(EVAL(FasterRecurse(HI(d), HI(ea), p1, p2)), 0), tm, r); \
  }

INDEXER(DispatchCaar, Caar)
INDEXER(DispatchCadr, Cadr)
INDEXER(DispatchCdar, Cdar)
INDEXER(DispatchCddr, Cddr)
INDEXER(DispatchCaaar, Caaar)
INDEXER(DispatchCaadr, Caadr)
INDEXER(DispatchCadar, Cadar)
INDEXER(DispatchCaddr, Caddr)
INDEXER(DispatchCdaar, Cdaar)
INDEXER(DispatchCdadr, Cdadr)
INDEXER(DispatchCddar, Cddar)
INDEXER(DispatchCdddr, Cdddr)
INDEXER(DispatchCaaaar, Caaaar)
INDEXER(DispatchCaaadr, Caaadr)
INDEXER(DispatchCaadar, Caadar)
INDEXER(DispatchCaaddr, Caaddr)
INDEXER(DispatchCadaar, Cadaar)
INDEXER(DispatchCadadr, Cadadr)
INDEXER(DispatchCaddar, Caddar)
INDEXER(DispatchCadddr, Cadddr)
INDEXER(DispatchCdaaar, Cdaaar)
INDEXER(DispatchCdaadr, Cdaadr)
INDEXER(DispatchCdadar, Cdadar)
INDEXER(DispatchCdaddr, Cdaddr)
INDEXER(DispatchCddaar, Cddaar)
INDEXER(DispatchCddadr, Cddadr)
INDEXER(DispatchCdddar, Cdddar)
INDEXER(DispatchCddddr, Cddddr)
