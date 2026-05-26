// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"

void harness(void)
{
  const uint8_t *sig;
  size_t siglen;
  const uint8_t *ph;
  size_t phlen;
  const uint8_t *ctx;
  size_t ctxlen;
  const uint8_t *pk;
  int hashalg;
  int r;
  r = mld_sign_verify_pre_hash_internal(
      sig, siglen, ph, phlen, ctx, ctxlen, pk, hashalg,
      NULL /* context will be dropped by preprocessor */);
}
