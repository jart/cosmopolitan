/* TF_PSA_CRYPTO_USER_CONFIG_FILE for testing.
 * Only used for a few test configurations.
 *
 * Typical usage (note multiple levels of quoting):
 *     cmake -DTF_PSA_CRYPTO_USER_CONFIG_FILE="./tests/configs/user-config-for-test.h\"
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#if defined(MBEDTLS_PSA_INJECT_ENTROPY)
/* The #MBEDTLS_PSA_INJECT_ENTROPY feature requires two extra platform
 * functions, which must be configured as #MBEDTLS_PLATFORM_NV_SEED_READ_MACRO
 * and #MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO. The job of these functions
 * is to read and write from the entropy seed file, which is located
 * in the PSA ITS file whose uid is #PSA_CRYPTO_ITS_RANDOM_SEED_UID.
 * (These could have been provided as library functions, but for historical
 * reasons, they weren't, and so each integrator has to provide a copy
 * of these functions.)
 *
 * Provide implementations of these functions for testing. */
#include <stddef.h>
int mbedtls_test_inject_entropy_seed_read(unsigned char *buf, size_t len);
int mbedtls_test_inject_entropy_seed_write(unsigned char *buf, size_t len);
#define MBEDTLS_PLATFORM_NV_SEED_READ_MACRO mbedtls_test_inject_entropy_seed_read
#define MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO mbedtls_test_inject_entropy_seed_write
#endif /* MBEDTLS_PSA_INJECT_ENTROPY */
