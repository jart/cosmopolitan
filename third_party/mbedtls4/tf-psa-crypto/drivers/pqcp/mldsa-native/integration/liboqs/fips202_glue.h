/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#ifndef MLD_INTEGRATION_LIBOQS_FIPS202_GLUE_H
#define MLD_INTEGRATION_LIBOQS_FIPS202_GLUE_H

/* Include OQS's own FIPS202 header */
#include "fips202.h"

#define mld_shake128ctx shake128incctx
#define mld_shake128_init shake128_inc_init
#define mld_shake128_absorb shake128_inc_absorb
#define mld_shake128_finalize shake128_inc_finalize
#define mld_shake128_squeeze shake128_inc_squeeze
#define mld_shake128_release shake128_inc_ctx_release

#define mld_shake256ctx shake256incctx
#define mld_shake256_init shake256_inc_init
#define mld_shake256_absorb shake256_inc_absorb
#define mld_shake256_finalize shake256_inc_finalize
#define mld_shake256_squeeze shake256_inc_squeeze
#define mld_shake256_release shake256_inc_ctx_release

#define mld_shake256 shake256

#endif /* !MLD_INTEGRATION_LIBOQS_FIPS202_GLUE_H */
