// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"

void harness(void)
{
  uint8_t *pk;
  uint8_t *sk;

  int r;
  r = mld_sign_pk_from_sk(pk, sk,
                          NULL /* context will be dropped by preprocessor */);
}
