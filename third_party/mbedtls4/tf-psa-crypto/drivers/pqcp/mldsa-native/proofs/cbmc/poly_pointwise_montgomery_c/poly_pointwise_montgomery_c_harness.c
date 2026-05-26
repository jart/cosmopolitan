// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly.h"

// Prototype for the function under test
void mld_poly_pointwise_montgomery_c(mld_poly *c, mld_poly *a, mld_poly *b);


void harness(void)
{
  mld_poly *a, *b, *c;
  mld_poly_pointwise_montgomery_c(c, a, b);
}
