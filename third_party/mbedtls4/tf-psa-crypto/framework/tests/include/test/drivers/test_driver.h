/*
 * Umbrella include for all of the test driver functionality
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_CRYPTO_TEST_DRIVER_H
#define PSA_CRYPTO_TEST_DRIVER_H

#if defined(PSA_CRYPTO_DRIVER_TEST)
#ifndef PSA_CRYPTO_DRIVER_PRESENT
#define PSA_CRYPTO_DRIVER_PRESENT
#endif
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif

#define PSA_CRYPTO_TEST_DRIVER_LOCATION 0x7fffff

#include "test/drivers/aead.h"
#include "test/drivers/cipher.h"
#include "test/drivers/hash.h"
#include "test/drivers/mac.h"
#include "test/drivers/key_management.h"
#include "test/drivers/signature.h"
#include "test/drivers/asymmetric_encryption.h"
#include "test/drivers/key_agreement.h"
#include "test/drivers/pake.h"
#include "test/drivers/xof.h"

#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_TEST_DRIVER_H */
