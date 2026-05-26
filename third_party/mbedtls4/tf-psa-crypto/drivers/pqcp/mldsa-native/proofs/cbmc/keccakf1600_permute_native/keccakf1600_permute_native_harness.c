// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include <keccakf1600.h>

void harness(void)
{
  uint64_t *a;
  mld_keccakf1600_permute(a);
}
