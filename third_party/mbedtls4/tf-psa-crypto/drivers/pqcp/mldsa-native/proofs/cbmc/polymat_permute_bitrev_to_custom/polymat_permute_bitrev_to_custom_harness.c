// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "polyvec.h"

void mld_polymat_permute_bitrev_to_custom(mld_polymat *mat);

void harness(void)
{
  mld_polymat *mat;
  mld_polymat_permute_bitrev_to_custom(mat);
}
