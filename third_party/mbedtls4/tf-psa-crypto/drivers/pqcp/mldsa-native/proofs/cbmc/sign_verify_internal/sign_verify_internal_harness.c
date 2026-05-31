// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"

void harness(void)
{
  const uint8_t *sig;
  size_t siglen;
  const uint8_t *m;
  size_t mlen;
  const uint8_t *pre;
  size_t prelen;
  const uint8_t *pk;
  int externalmu;

  mld_sign_verify_internal(sig, siglen, m, mlen, pre, prelen, pk, externalmu,
                           NULL /* context will be dropped by preprocessor */);
}
