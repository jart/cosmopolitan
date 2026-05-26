// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"

void harness(void)
{
  uint8_t *sig;
  size_t *siglen;
  uint8_t *m;
  size_t mlen;
  uint8_t *pre;
  size_t prelen;
  uint8_t *rnd;
  uint8_t *sk;
  int externalmu;
  int r;
  r = mld_sign_signature_internal(
      sig, siglen, m, mlen, pre, prelen, rnd, sk, externalmu,
      NULL /* context will be dropped by preprocessor */);
}
