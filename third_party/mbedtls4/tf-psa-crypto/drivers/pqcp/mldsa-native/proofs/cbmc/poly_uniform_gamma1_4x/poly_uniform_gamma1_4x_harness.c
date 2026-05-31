// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly_kl.h"

void harness(void)
{
  mld_poly *r0, *r1, *r2, *r3;
  const uint8_t *seed;
  uint16_t n0, n1, n2, n3;

  mld_poly_uniform_gamma1_4x(r0, r1, r2, r3, seed, n0, n1, n2, n3);
}
