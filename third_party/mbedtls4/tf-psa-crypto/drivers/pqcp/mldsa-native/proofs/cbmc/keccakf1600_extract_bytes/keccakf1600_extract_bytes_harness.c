// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "fips202/fips202.h"

#include <keccakf1600.h>

void harness(void)
{
  uint64_t *state;
  unsigned char *data;
  unsigned offset;
  unsigned length;
  mld_keccakf1600_extract_bytes(state, data, offset, length);
}
