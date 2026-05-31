// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "polyvec.h"


// Prototype for the function under test
#define mld_polyvecl_pointwise_acc_montgomery_c \
  MLD_ADD_PARAM_SET(mld_polyvecl_pointwise_acc_montgomery_c)
void mld_polyvecl_pointwise_acc_montgomery_c(mld_poly *a, mld_polyvecl *b,
                                             mld_polyvecl *c);

void harness(void)
{
  mld_poly *a;
  mld_polyvecl *b, *c;
  mld_polyvecl_pointwise_acc_montgomery_c(a, b, c);
}
