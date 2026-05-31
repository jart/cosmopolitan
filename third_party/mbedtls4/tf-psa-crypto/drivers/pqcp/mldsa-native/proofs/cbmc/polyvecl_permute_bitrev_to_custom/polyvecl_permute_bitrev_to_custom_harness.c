// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "polyvec.h"

void mld_polyvecl_permute_bitrev_to_custom(mld_polyvecl *v);

void harness(void)
{
  mld_polyvecl *v;
  mld_polyvecl_permute_bitrev_to_custom(v);
}
