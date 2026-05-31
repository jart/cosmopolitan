/* Ad hoc report on included headers. */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <psa/crypto.h>
#include <mbedtls/platform.h>

int main(void)
{

    /* Which PSA platform header? */
#if defined(PSA_CRYPTO_PLATFORM_H)
    mbedtls_printf("PSA_CRYPTO_PLATFORM_H\n");
#endif
#if defined(PSA_CRYPTO_PLATFORM_ALT_H)
    mbedtls_printf("PSA_CRYPTO_PLATFORM_ALT_H\n");
#endif

    /* Which PSA struct header? */
#if defined(PSA_CRYPTO_STRUCT_H)
    mbedtls_printf("PSA_CRYPTO_STRUCT_H\n");
#endif
#if defined(PSA_CRYPTO_STRUCT_ALT_H)
    mbedtls_printf("PSA_CRYPTO_STRUCT_ALT_H\n");
#endif

}
