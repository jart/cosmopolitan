/*
 *  A program that prints the current AES implementation, namely one of:
 *  software, AESCE, AESNI assembly, or AESNI intrinsics.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS

#include "tf-psa-crypto/build_info.h"
#include "mbedtls/platform.h"
#include "mbedtls/private/aes.h"

#if !defined(MBEDTLS_AES_C)
int main(void)
{
    mbedtls_printf("MBEDTLS_AES_C not defined.\n");
    return 0;
}
#else
int main(void)
{
    mbedtls_aes_implementation aes_imp = mbedtls_aes_get_implementation();
    switch (aes_imp) {
        case MBEDTLS_AES_IMP_SOFTWARE:
            mbedtls_printf("AES using SOFTWARE implementation\n");
            break;
        case MBEDTLS_AES_IMP_AESCE:
            mbedtls_printf("AES using AESCE implementation\n");
            break;
        case MBEDTLS_AES_IMP_AESNI_ASM:
            mbedtls_printf("AES using AESNI ASSEMBLY implementation\n");
            break;
        case MBEDTLS_AES_IMP_AESNI_INTRINSICS:
            mbedtls_printf("AES using AESNI INTRINSICS implementation\n");
            break;
        default:
            mbedtls_printf("AES IMPLEMENTATION UNKNOWN!\n");
    }
    return 0;
}
#endif /* !MBEDTLS_AES_C */
