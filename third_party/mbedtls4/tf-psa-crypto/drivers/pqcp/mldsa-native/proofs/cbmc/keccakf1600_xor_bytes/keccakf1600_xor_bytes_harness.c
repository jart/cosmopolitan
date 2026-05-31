// Copyright (c) The mldsa-native project authors
// Copyright (c) The mlkem-native project authors
// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include <keccakf1600.h>

void harness(void)
{
  uint64_t *state;
  const unsigned char *data;
  unsigned offset;
  unsigned length;
  mld_keccakf1600_xor_bytes(state, data, offset, length);
}
