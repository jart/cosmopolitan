// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly.h"

int32_t mld_fqmul(int32_t a, int32_t b);
void harness(void)
{
  int32_t a, b, r;
  r = mld_fqmul(a, b);
}
