/*
 *  ECDH with curve-optimized implementation multiplexing
 *
 *  Copyright 2016-2018 INRIA and Microsoft Corporation
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of Mbed TLS (https://tls.mbed.org)
 */

#include "tf_psa_crypto_common.h"
#include "mbedtls/private_access.h"

#if defined(MBEDTLS_PSA_BUILTIN_ALG_ECDH) && defined(MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED)

#if !(defined(__SIZEOF_INT128__) && (__SIZEOF_INT128__ == 16))
#define KRML_VERIFIED_UINT128
#endif

#include <Hacl_Curve25519.h>
#include <mbedtls/platform_util.h>

#include "x25519.h"

void mbedtls_x25519_scalarmult(uint8_t out[MBEDTLS_X25519_KEY_SIZE_BYTES],
                               const uint8_t scalar[MBEDTLS_X25519_KEY_SIZE_BYTES],
                               const uint8_t point[MBEDTLS_X25519_KEY_SIZE_BYTES])
{
    uint8_t cp_scalar[MBEDTLS_X25519_KEY_SIZE_BYTES];
    uint8_t cp_point[MBEDTLS_X25519_KEY_SIZE_BYTES];

    memcpy(cp_scalar, scalar, sizeof(cp_scalar));
    memcpy(cp_point, point, sizeof(cp_point));

    Hacl_Curve25519_crypto_scalarmult(out, cp_scalar, cp_point);

    mbedtls_platform_zeroize(cp_scalar, sizeof(cp_scalar));
    mbedtls_platform_zeroize(cp_point, sizeof(cp_point));
}

#endif /* MBEDTLS_PSA_BUILTIN_ALG_ECDH && MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED */
