// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include <stdint.h>
#include "params.h"
#include "poly.h"

// Prototype for the function under test
void mld_poly_invntt_tomont_c(mld_poly *p);


void harness(void)
{
  mld_poly *a;
  mld_poly_invntt_tomont_c(a);
}
