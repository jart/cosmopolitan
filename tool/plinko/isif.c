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
 *     (ζ (X Y) (Z))
 *
 * @return MAKE4(X,Y,Z,0) on success, or ZERO4 on mismatch
 * @note ζ means COND
 */
struct qword IsIf(int x_) {
  dword w_;
  if (x_ >= 0)
    return ZERO4;
  w_ = Get(x_);  // (ζ (X Y) (Z))
  int ax_ = LO(w_);
  int dx_ = HI(w_);
  if (ax_ != kCond)
    return ZERO4;
  if (dx_ >= 0)
    return ZERO4;
  w_ = Get(dx_);  // ((X Y) (Z))
  int adx_ = LO(w_);
  int ddx_ = HI(w_);
  if (adx_ >= 0)
    return ZERO4;
  w_ = Get(adx_);  // (X Y)
  int aadx_ = LO(w_);
  int dadx_ = HI(w_);
  if (ddx_ >= 0)
    return ZERO4;
  w_ = Get(ddx_);  // ((Z))
  int addx_ = LO(w_);
  int dddx_ = HI(w_);
  int X = aadx_;
  if (addx_ >= 0)
    return ZERO4;
  w_ = Get(addx_);  // (Z)
  int aaddx_ = LO(w_);
  int daddx_ = HI(w_);
  if (dadx_ >= 0)
    return ZERO4;
  w_ = Get(dadx_);  // (Y)
  int adadx_ = LO(w_);
  int ddadx_ = HI(w_);
  if (dddx_)
    return ZERO4;
  int Y = adadx_;
  int Z = aaddx_;
  if (ddadx_)
    return ZERO4;
  if (daddx_)
    return ZERO4;
  return MAKE4(X, Y, Z, 0);
}
