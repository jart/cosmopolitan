// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "sign.h"

void harness(void)
{
  uint8_t *a, *b, *c;
  mld_sign_keypair_internal(a, b, c,
                            NULL /* context will be dropped by preprocessor */);
}
