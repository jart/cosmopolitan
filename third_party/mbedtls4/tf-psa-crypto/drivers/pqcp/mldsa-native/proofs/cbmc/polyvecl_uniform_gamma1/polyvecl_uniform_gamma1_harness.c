// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "polyvec.h"

void harness(void)
{
  mld_polyvecl *v;
  const uint8_t *seed;
  uint16_t nonce;

  mld_polyvecl_uniform_gamma1(v, seed, nonce);
}
