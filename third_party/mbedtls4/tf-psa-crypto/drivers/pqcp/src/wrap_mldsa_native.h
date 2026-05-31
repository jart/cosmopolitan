/** \brief Simple integration of mldsa-native from PQCP
 *
 * Declare the functions defined in wrap_mldsa_native.c.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef TF_PSA_CRYPTO_WRAP_MLDSA_NATIVE_H
#define TF_PSA_CRYPTO_WRAP_MLDSA_NATIVE_H

#include <tf-psa-crypto/build_info.h>

#if defined(TF_PSA_CRYPTO_PQCP_MLDSA_ENABLED)

/* The mldsa-native config file defines options that apply to all
 * parameter sets. It is included both when building code that uses
 * mldsa-native (via wrap_mldsa_native.h) and when building mldsa-native
 * itself (via wrap_mldsa_native.c). */
#define MLD_CONFIG_FILE "pqcp-config.h"

/* Include the declarations of mldsa-native functions, one parameter set
 * (44, 65 or 87) at a time. The function names have the prefix
 * MLD_CONFIG_NAMESPACE_PREFIX defined in pqcp-config.h, followed by
 * the parameter set (except for functions shared between levels), e.g.
 * tf_psa_crypto_pqcp_mldsa87_keypair_internal().
 * */

#if defined(TF_PSA_CRYPTO_PQCP_MLDSA_87_ENABLED)
#  define MLD_CONFIG_PARAMETER_SET 87
#  include "mldsa_native.h"
#  undef MLD_CONFIG_PARAMETER_SET
#endif

#endif  /* TF_PSA_CRYPTO_PQCP_MLDSA_ENABLED */

#endif /* <wrap_mldsa_native.h> */
