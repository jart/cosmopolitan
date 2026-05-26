// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly_kl.h"

// Prototype for the function under test
#define mld_poly_use_hint_c MLD_ADD_PARAM_SET(mld_poly_use_hint_c)
void mld_poly_use_hint_c(mld_poly *b, mld_poly *a, mld_poly *h);


void harness(void)
{
  mld_poly *a, *b, *h;
  mld_poly_use_hint_c(b, a, h);
}
