#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"

/**
 * Matches
 *
 *     (⍆ X)
 *
 * @return X on success, or 0 on mismatch
 * @note ⍆ means CDR
 */
int IsCdr(int x_) {
  dword w_;
  if (x_ >= 0)
    return 0;
  w_ = Get(x_);  // (⍆ X)
  int ax_ = LO(w_);
  int dx_ = HI(w_);
  if (ax_ != kCdr)
    return 0;
  if (dx_ >= 0)
    return 0;
  w_ = Get(dx_);  // (X)
  int adx_ = LO(w_);
  int ddx_ = HI(w_);
  int X = adx_;
  if (ddx_)
    return 0;
  return X;
}
