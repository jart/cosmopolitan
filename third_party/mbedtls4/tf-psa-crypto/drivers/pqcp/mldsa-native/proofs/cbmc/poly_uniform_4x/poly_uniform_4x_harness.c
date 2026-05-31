// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly.h"

void harness(void)
{
  mld_poly *r0;
  mld_poly *r1;
  mld_poly *r2;
  mld_poly *r3;
  uint8_t(*seed)[MLD_ALIGN_UP(MLDSA_SEEDBYTES + 2)];

  mld_poly_uniform_4x(r0, r1, r2, r3, seed);
}
