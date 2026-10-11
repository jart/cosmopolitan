#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include <inttypes.h>
#include "plinko.h"
#include "printf.h"

static const wchar_t *GetDispatchName(int x) {
  return L"0xDEADBEEF";
}

void PrintHeap(int i) {
  for (; i-- > cx;) {
    Printf(L"%10d (%10d . %10d) %10s[%10d] %p%n", i, LO(Get(i)), HI(Get(i)),
           GetDispatchName(i), HI(GetShadow(i)), i);
  }
}
