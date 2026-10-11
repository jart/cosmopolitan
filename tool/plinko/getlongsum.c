#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include <limits.h>
#include <stdckdint.h>
#include "histo.h"

long GetLongSum(const long *h, size_t n) {
  long t;
  size_t i;
  for (t = i = 0; i < n; ++i) {
    if (ckd_add(&t, t, h[i])) {
      t = LONG_MAX;
      break;
    }
  }
  return t;
}
