// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include <stdint.h>
#include "params.h"

void mld_ntt_butterfly_block(int32_t r[MLDSA_N], int32_t zeta, unsigned start,
                             unsigned len, unsigned bound);

void harness(void)
{
  int32_t *r, zeta;
  unsigned start, len;
  unsigned bound;
  mld_ntt_butterfly_block(r, zeta, start, len, bound);
}
