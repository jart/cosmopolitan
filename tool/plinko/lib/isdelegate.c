/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
 *     (λ V (F . V) . Q)
 *
 * @return F on success, or 0 on mismatch
 * @note Q is ignored
 * @note V must be a non-nil atom
 * @note λ means LAMBDA
 */
int IsDelegate(int x_) {
  dword w_;
  if (x_ >= 0) return 0;
  w_ = Get(x_);  // (λ V (F . V) . Q)
  int ax_ = LO(w_);
  int dx_ = HI(w_);
  if (ax_ != kLambda) return 0;
  if (dx_ >= 0) return 0;
  w_ = Get(dx_);  // (V (F . V) . Q)
  int adx_ = LO(w_);
  int ddx_ = HI(w_);
  int V = adx_;
  if (V <= 0) return 0;
  if (ddx_ >= 0) return 0;
  w_ = Get(ddx_);  // ((F . V) . Q)
  int addx_ = LO(w_);
  if (addx_ >= 0) return 0;
  w_ = Get(addx_);  // (F . V)
  int aaddx_ = LO(w_);
  int daddx_ = HI(w_);
  int F = aaddx_;
  if (daddx_ != V) return 0;
  return F;
}
