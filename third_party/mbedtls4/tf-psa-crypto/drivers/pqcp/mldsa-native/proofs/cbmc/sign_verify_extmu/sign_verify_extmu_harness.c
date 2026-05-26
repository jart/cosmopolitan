// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"



void harness(void)
{
  uint8_t *sig;
  size_t siglen;
  uint8_t *mu;
  uint8_t *pk;

  int r;

  r = mld_sign_verify_extmu(sig, siglen, mu, pk,
                            NULL /* context will be dropped by preprocessor */);
}
