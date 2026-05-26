// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly_kl.h"


// Prototype for the function under test
#define mld_polyz_unpack_c MLD_ADD_PARAM_SET(mld_polyz_unpack_c)
void mld_polyz_unpack_c(mld_poly *a, uint8_t *b);


void harness(void)
{
  mld_poly *a;
  uint8_t *b;
  mld_polyz_unpack_c(a, b);
}
