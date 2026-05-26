/* PSA driver for ML-DSA using mldsa-native */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "tf_psa_crypto_common.h"

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(TF_PSA_CRYPTO_PQCP_MLDSA_ENABLED)

#include "wrap_mldsa_native.h"

int tf_psa_crypto_pqcp_driver_exists = 1;

#endif /* MBEDTLS_PSA_CRYPTO_C && TF_PSA_CRYPTO_PQCP_MLDSA_ENABLED */
