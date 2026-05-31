// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"

void harness(void)
{
  const uint8_t *sig;
  size_t siglen;
  const uint8_t *m;
  size_t mlen;
  const uint8_t *ctx;
  size_t ctxlen;
  const uint8_t *pk;

  int r;

  r = mld_sign_verify(sig, siglen, m, mlen, ctx, ctxlen, pk,
                      NULL /* context will be dropped by preprocessor */);
}
