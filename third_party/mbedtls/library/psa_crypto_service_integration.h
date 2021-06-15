/*
 *  Copyright The Mbed TLS Contributors
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
 */

#ifndef PSA_CRYPTO_SERVICE_INTEGRATION_H
#define PSA_CRYPTO_SERVICE_INTEGRATION_H

/*
 * When MBEDTLS_PSA_CRYPTO_SPM is defined, the code is being built for SPM
 * (Secure Partition Manager) integration which separates the code into two
 * parts: NSPE (Non-Secure Processing Environment) and SPE (Secure Processing
 * Environment). When building for the SPE, an additional header file should be
 * included.
 */
#if defined(MBEDTLS_PSA_CRYPTO_SPM)
/*
 * PSA_CRYPTO_SECURE means that the file which included this file is being
 * compiled for SPE. The files crypto_structs.h and crypto_types.h have
 * different implementations for NSPE and SPE and are compiled according to this
 * flag.
 */
#define PSA_CRYPTO_SECURE 1
#include "crypto_spe.h"
#endif // MBEDTLS_PSA_CRYPTO_SPM

#endif // PSA_CRYPTO_SERVICE_INTEGRATION_H
