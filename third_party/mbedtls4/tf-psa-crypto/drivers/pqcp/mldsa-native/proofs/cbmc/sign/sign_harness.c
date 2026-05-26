// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"

void harness(void)
{
  uint8_t *sm;
  size_t *smlen;
  uint8_t *m;
  size_t mlen;
  uint8_t *ctx;
  size_t ctxlen;
  uint8_t *sk;
  int r;

  r = mld_sign(sm, smlen, m, mlen, ctx, ctxlen, sk,
               NULL /* context will be dropped by preprocessor */);
}
