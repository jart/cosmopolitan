#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"

bool IsConstant(int e) {
  unsigned f = LO(GetShadow(e));
  if (f == EncodeDispatchFn(DispatchNil))
    return true;
  if (f == EncodeDispatchFn(DispatchTrue))
    return true;
  if (f == EncodeDispatchFn(DispatchPrecious))
    return true;
  if (f == EncodeDispatchFn(DispatchQuote))
    return true;
  return false;
}
