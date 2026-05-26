// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly.h"

static unsigned int mld_rej_eta(int32_t *a, unsigned int target,
                                unsigned int offset, const uint8_t *buf,
                                unsigned int buflen);

void harness(void)
{
  int32_t *a;
  unsigned int target;
  unsigned int offset;
  const uint8_t *buf;
  unsigned int buflen;

  mld_rej_eta(a, target, offset, buf, buflen);
}
