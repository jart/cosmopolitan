// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly.h"

int32_t mld_fqscale(int32_t a);
void harness(void)
{
  int32_t a, r;
  r = mld_fqscale(a);
}
