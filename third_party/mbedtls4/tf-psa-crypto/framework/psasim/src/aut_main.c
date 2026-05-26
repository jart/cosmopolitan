/**
 * This is the base AUT that exectues all other AUTs meant to test PSA APIs
 * through PSASIM.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/* First include Mbed TLS headers to get the Mbed TLS configuration and
 * platform definitions that we'll use in this program. Also include
 * standard C headers for functions we'll use here. */
#include "mbedtls/build_info.h"

#include "psa/crypto.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int psa_hash_compute_main(void);
int psa_hash_main(void);
int psa_aead_encrypt_main(char *cipher_name);
int psa_aead_encrypt_decrypt_main(void);
int psa_cipher_encrypt_decrypt_main(void);
int psa_asymmetric_encrypt_decrypt_main(void);
int psa_random_main(void);
int psa_mac_main(void);
int psa_key_agreement_main(void);
int psa_sign_verify_main(void);
int psa_hkdf_main(void);

#define TEST_MODULE(main_func) \
    do {    \
        char title[128] = { 0 }; \
        char separator[128] = { 0 }; \
        int title_len = snprintf(title, sizeof(title), "=== Test: %s ===", #main_func); \
        memset(separator, '=', title_len); \
        printf("%s\n%s\n%s\n", separator, title, separator); \
        ret = main_func; \
        if (ret != 0) { \
            goto exit; \
        } \
    } while (0)

int main()
{
    int ret;

    TEST_MODULE(psa_hash_compute_main());
    TEST_MODULE(psa_hash_main());

    TEST_MODULE(psa_aead_encrypt_main("aes128-gcm"));
    TEST_MODULE(psa_aead_encrypt_main("aes256-gcm"));
    TEST_MODULE(psa_aead_encrypt_main("aes128-gcm_8"));
    TEST_MODULE(psa_aead_encrypt_main("chachapoly"));
    TEST_MODULE(psa_aead_encrypt_decrypt_main());
    TEST_MODULE(psa_cipher_encrypt_decrypt_main());
    TEST_MODULE(psa_asymmetric_encrypt_decrypt_main());

    TEST_MODULE(psa_random_main());

    TEST_MODULE(psa_mac_main());
    TEST_MODULE(psa_key_agreement_main());
    TEST_MODULE(psa_sign_verify_main());
    TEST_MODULE(psa_hkdf_main());

exit:
    return (ret != 0) ? 1 : 0;
}
