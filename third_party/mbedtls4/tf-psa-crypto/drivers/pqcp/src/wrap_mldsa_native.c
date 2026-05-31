/** \brief Simple integration of mldsa-native from PQCP
 *
 * Define the functions declared in wrap_mldsa_native.c.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <tf-psa-crypto/build_info.h>

#if defined(TF_PSA_CRYPTO_PQCP_MLDSA_ENABLED)

//#include "pqcp-config.h"
#include "wrap_mldsa_native.h"

/* If we include multiple levels, tell the first level to include the
 * shared stuff.
 * After including the first level, we'll tell the other levels not to
 * include the shared stuff.
 */
#define MLD_CONFIG_MULTILEVEL_WITH_SHARED
#define MLD_CONFIG_MONOBUILD_KEEP_SHARED_HEADERS

/* Include the definitions of mldsa-native functions, one parameter set
 * (44, 65 or 87) at a time. The function names have the prefix
 * MLD_CONFIG_NAMESPACE_PREFIX defined in pqcp-config.h, followed by
 * the parameter set (except for functions shared between levels), e.g.
 * tf_psa_crypto_pqcp_mldsa87_keypair_internal().
 * */

#if defined(TF_PSA_CRYPTO_PQCP_MLDSA_87_ENABLED)
#  define MLD_CONFIG_PARAMETER_SET 87
#  include "mldsa_native.c"
#  undef MLD_CONFIG_PARAMETER_SET
/* Don't include the shared code in subsequent inclusions of mldsa_native.c */
#  undef MLD_CONFIG_MULTILEVEL_WITH_SHARED
#  define MLD_CONFIG_MULTILEVEL_NO_SHARED
#endif

#endif /* TF_PSA_CRYPTO_PQCP_MLDSA_ENABLED */
