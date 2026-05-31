// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include <stdint.h>
#include "cbmc.h"
#include "params.h"


int mld_ntt_native(int32_t data[MLDSA_N]);

void harness(void)
{
  int32_t *r;
  int t;
  t = mld_ntt_native(r);
}
