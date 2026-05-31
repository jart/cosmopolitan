// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "fips202/fips202.h"
#include "poly.h"

void harness(void)
{
  mld_poly *a;
  const uint8_t *seed;

  mld_poly_uniform(a, seed);
}
