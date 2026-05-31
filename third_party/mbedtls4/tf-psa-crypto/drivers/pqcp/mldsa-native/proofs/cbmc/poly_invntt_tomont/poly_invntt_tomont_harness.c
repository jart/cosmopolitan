// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include <stdint.h>
#include "params.h"
#include "poly.h"

void harness(void)
{
  mld_poly *a;
  mld_poly_invntt_tomont(a);
}
