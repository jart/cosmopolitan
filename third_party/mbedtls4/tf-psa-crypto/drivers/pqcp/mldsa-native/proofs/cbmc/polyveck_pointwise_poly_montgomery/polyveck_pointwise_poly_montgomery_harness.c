// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "polyvec.h"

void harness(void)
{
  mld_polyveck *a, *b;
  mld_poly *c;
  mld_polyveck_pointwise_poly_montgomery(a, c, b);
}
