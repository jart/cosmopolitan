// Copyright (c) The mldsa-native project authors
// SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

#include "poly.h"

#define mld_rej_eta_c MLD_ADD_PARAM_SET(mld_rej_eta_c)
static unsigned int mld_rej_eta_c(int32_t *a, unsigned int target,
                                  unsigned int offset, const uint8_t *buf,
                                  unsigned int buflen);

void harness(void)
{
  int32_t *a;
  unsigned int target;
  unsigned int offset;
  const uint8_t *buf;
  unsigned int buflen;

  mld_rej_eta_c(a, target, offset, buf, buflen);
}
