/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "tool/plinko/lib/plinko.h"

/**
 * Matches
 *
 *     (⅄ (λ (N) ((λ (W) (W W)) (λ (V) (N (λ M ((V V) . M)))))) . Q)
 *
 * @return true if matches, otherwise false
 * @note M must be a non-nil atom
 * @note N must be a non-nil atom
 * @note Q is ignored
 * @note V must be a non-nil atom
 * @note W must be a non-nil atom
 * @note λ means LAMBDA
 * @note ⅄ means CLOSURE
 */
bool IsYcombinator(int x_) {
  dword w_;
  if (x_ >= 0) return false;
  w_ = Get(x_);
  int ax_ = LO(w_);
  int dx_ = HI(w_);
  if (ax_ != kClosure) return false;
  if (dx_ >= 0) return false;
  w_ = Get(dx_);  // ((λ (N) ((λ (W) (W W)) (λ (V) (N (λ M ((V V) . M)))))) . Q)
  int adx_ = LO(w_);
  if (adx_ >= 0) return false;
  w_ = Get(adx_);  // (λ (N) ((λ (W) (W W)) (λ (V) (N (λ M ((V V) . M))))))
  int aadx_ = LO(w_);
  int dadx_ = HI(w_);
  if (aadx_ != kLambda) return false;
  if (dadx_ >= 0) return false;
  w_ = Get(dadx_);  // ((N) ((λ (W) (W W)) (λ (V) (N (λ M ((V V) . M))))))
  int adadx_ = LO(w_);
  int ddadx_ = HI(w_);
  if (adadx_ >= 0) return false;
  w_ = Get(adadx_);  // (N)
  int aadadx_ = LO(w_);
  int dadadx_ = HI(w_);
  if (ddadx_ >= 0) return false;
  w_ = Get(ddadx_);  // (((λ (W) (W W)) (λ (V) (N (λ M ((V V) . M))))))
  int addadx_ = LO(w_);
  int dddadx_ = HI(w_);
  int N = aadadx_;
  if (N <= 0) return false;
  if (addadx_ >= 0) return false;
  w_ = Get(addadx_);  // ((λ (W) (W W)) (λ (V) (N (λ M ((V V) . M)))))
  int aaddadx_ = LO(w_);
  int daddadx_ = HI(w_);
  if (dadadx_) return false;
  if (dddadx_) return false;
  if (aaddadx_ >= 0) return false;
  w_ = Get(aaddadx_);  // (λ (W) (W W))
  int aaaddadx_ = LO(w_);
  int daaddadx_ = HI(w_);
  if (daddadx_ >= 0) return false;
  w_ = Get(daddadx_);  // ((λ (V) (N (λ M ((V V) . M)))))
  int adaddadx_ = LO(w_);
  int ddaddadx_ = HI(w_);
  if (aaaddadx_ != kLambda) return false;
  if (adaddadx_ >= 0) return false;
  w_ = Get(adaddadx_);  // (λ (V) (N (λ M ((V V) . M))))
  int aadaddadx_ = LO(w_);
  int dadaddadx_ = HI(w_);
  if (daaddadx_ >= 0) return false;
  w_ = Get(daaddadx_);  // ((W) (W W))
  int adaaddadx_ = LO(w_);
  int ddaaddadx_ = HI(w_);
  if (ddaddadx_) return false;
  if (adaaddadx_ >= 0) return false;
  w_ = Get(adaaddadx_);  // (W)
  int aadaaddadx_ = LO(w_);
  int dadaaddadx_ = HI(w_);
  if (aadaddadx_ != kLambda) return false;
  if (ddaaddadx_ >= 0) return false;
  w_ = Get(ddaaddadx_);  // ((W W))
  int addaaddadx_ = LO(w_);
  int dddaaddadx_ = HI(w_);
  if (dadaddadx_ >= 0) return false;
  w_ = Get(dadaddadx_);  // ((V) (N (λ M ((V V) . M))))
  int adadaddadx_ = LO(w_);
  int ddadaddadx_ = HI(w_);
  int W = aadaaddadx_;
  if (W <= 0) return false;
  if (adadaddadx_ >= 0) return false;
  w_ = Get(adadaddadx_);  // (V)
  int aadadaddadx_ = LO(w_);
  int dadadaddadx_ = HI(w_);
  if (addaaddadx_ >= 0) return false;
  w_ = Get(addaaddadx_);  // (W W)
  int aaddaaddadx_ = LO(w_);
  int daddaaddadx_ = HI(w_);
  if (ddadaddadx_ >= 0) return false;
  w_ = Get(ddadaddadx_);  // ((N (λ M ((V V) . M))))
  int addadaddadx_ = LO(w_);
  int dddadaddadx_ = HI(w_);
  if (dadaaddadx_) return false;
  int V = aadadaddadx_;
  if (V <= 0) return false;
  if (dddaaddadx_) return false;
  if (addadaddadx_ >= 0) return false;
  w_ = Get(addadaddadx_);  // (N (λ M ((V V) . M)))
  int aaddadaddadx_ = LO(w_);
  int daddadaddadx_ = HI(w_);
  if (aaddaaddadx_ != W) return false;
  if (dadadaddadx_) return false;
  if (daddaaddadx_ >= 0) return false;
  w_ = Get(daddaaddadx_);  // (W)
  int adaddaaddadx_ = LO(w_);
  int ddaddaaddadx_ = HI(w_);
  if (dddadaddadx_) return false;
  if (adaddaaddadx_ != W) return false;
  if (aaddadaddadx_ != N) return false;
  if (ddaddaaddadx_) return false;
  if (daddadaddadx_ >= 0) return false;
  w_ = Get(daddadaddadx_);  // ((λ M ((V V) . M)))
  int adaddadaddadx_ = LO(w_);
  int ddaddadaddadx_ = HI(w_);
  if (adaddadaddadx_ >= 0) return false;
  w_ = Get(adaddadaddadx_);  // (λ M ((V V) . M))
  int aadaddadaddadx_ = LO(w_);
  int dadaddadaddadx_ = HI(w_);
  if (ddaddadaddadx_) return false;
  if (aadaddadaddadx_ != kLambda) return false;
  if (dadaddadaddadx_ >= 0) return false;
  w_ = Get(dadaddadaddadx_);  // (M ((V V) . M))
  int adadaddadaddadx_ = LO(w_);
  int ddadaddadaddadx_ = HI(w_);
  int M = adadaddadaddadx_;
  if (M <= 0) return false;
  if (ddadaddadaddadx_ >= 0) return false;
  w_ = Get(ddadaddadaddadx_);  // (((V V) . M))
  int addadaddadaddadx_ = LO(w_);
  int dddadaddadaddadx_ = HI(w_);
  if (addadaddadaddadx_ >= 0) return false;
  w_ = Get(addadaddadaddadx_);  // ((V V) . M)
  int aaddadaddadaddadx_ = LO(w_);
  int daddadaddadaddadx_ = HI(w_);
  if (dddadaddadaddadx_) return false;
  if (aaddadaddadaddadx_ >= 0) return false;
  w_ = Get(aaddadaddadaddadx_);  // (V V)
  int aaaddadaddadaddadx_ = LO(w_);
  int daaddadaddadaddadx_ = HI(w_);
  if (daddadaddadaddadx_ != M) return false;
  if (aaaddadaddadaddadx_ != V) return false;
  if (daaddadaddadaddadx_ >= 0) return false;
  w_ = Get(daaddadaddadaddadx_);  // (V)
  int adaaddadaddadaddadx_ = LO(w_);
  int ddaaddadaddadaddadx_ = HI(w_);
  if (adaaddadaddadaddadx_ != V) return false;
  if (ddaaddadaddadaddadx_) return false;
  return true;
}
