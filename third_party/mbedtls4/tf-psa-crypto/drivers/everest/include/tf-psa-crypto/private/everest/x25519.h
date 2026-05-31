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

#ifndef TF_PSA_CRYPTO_X25519_H
#define TF_PSA_CRYPTO_X25519_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS)

#define MBEDTLS_X25519_KEY_SIZE_BYTES 32

/*
 * Scalar multiplication: out = scalar * point
 *
 * All buffer must of at least the expected size.
 */
void mbedtls_x25519_scalarmult(uint8_t out[MBEDTLS_X25519_KEY_SIZE_BYTES],
                               const uint8_t scalar[MBEDTLS_X25519_KEY_SIZE_BYTES],
                               const uint8_t point[MBEDTLS_X25519_KEY_SIZE_BYTES]);

#endif /* MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS */

#ifdef __cplusplus
}
#endif

#endif /* TF_PSA_CRYPTO_X25519_H */
