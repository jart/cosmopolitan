// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly_kl.h"

// Prototype for the function under test
uint32_t mld_poly_chknorm_c(mld_poly *a, int32_t B);

void harness(void)
{
  mld_poly *a;
  uint32_t r;
  int32_t B;
  r = mld_poly_chknorm_c(a, B);
}
