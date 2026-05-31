/** Support for memory poisoning wrappers for PSA functions.
 *
 *  The wrappers poison the input and output buffers of each function
 *  before calling it, to ensure that it does not access the buffers
 *  except by calling the approved buffer-copying functions.
 *
 * This header declares support functions. The wrappers themselves are
 * decalred in the automatically generated file `test/psa_test_wrappers.h`.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_MEMORY_POISONING_WRAPPERS_H
#define PSA_MEMORY_POISONING_WRAPPERS_H

#include "build_info.h"

#include "psa/crypto.h"

#include "test/memory.h"

#if defined(MBEDTLS_TEST_HOOKS) && defined(MBEDTLS_TEST_MEMORY_CAN_POISON)

/**
 * \brief         Setup the memory poisoning test hooks used by
 *                psa_crypto_copy_input() and psa_crypto_copy_output() for
 *                memory poisoning.
 */
void mbedtls_poison_test_hooks_setup(void);

/**
 * \brief         Teardown the memory poisoning test hooks used by
 *                psa_crypto_copy_input() and psa_crypto_copy_output() for
 *                memory poisoning.
 */
void mbedtls_poison_test_hooks_teardown(void);

#endif /* MBEDTLS_TEST_HOOKS && MBEDTLS_TEST_MEMORY_CAN_POISON */

#endif /* PSA_MEMORY_POISONING_WRAPPERS_H */
