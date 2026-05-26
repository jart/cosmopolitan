// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include <stdint.h>
#include "params.h"
#include "polyvec.h"

void harness(void)
{
  mld_polyveck *v;
  mld_polyveck_invntt_tomont(v);
}
