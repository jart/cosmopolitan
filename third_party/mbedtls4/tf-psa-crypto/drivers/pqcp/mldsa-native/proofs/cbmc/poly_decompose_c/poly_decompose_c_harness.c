// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly_kl.h"

// Prototype for the function under test
#define mld_poly_decompose_c MLD_ADD_PARAM_SET(mld_poly_decompose_c)
void mld_poly_decompose_c(mld_poly *a1, mld_poly *a0);

void harness(void)
{
  mld_poly *a0, *a1;
  mld_poly_decompose_c(a1, a0);
}
