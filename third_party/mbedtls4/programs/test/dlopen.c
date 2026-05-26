/*
 *  Test dynamic loading of libmbed*
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "mbedtls/build_info.h"

#include "mbedtls/platform.h"

#if defined(MBEDTLS_X509_CRT_PARSE_C)
#include "mbedtls/x509_crt.h"
#endif

#if defined(__APPLE__)
#define SO_SUFFIX ".dylib"
#else
#define SO_SUFFIX ".so"
#endif

#define MBEDCRYPTO_SO_FILENAME "libmbedcrypto" SO_SUFFIX
#define TFPSACRYPTO_SO_FILENAME "libtfpsacrypto" SO_SUFFIX
#define X509_SO_FILENAME "libmbedx509" SO_SUFFIX
#define TLS_SO_FILENAME "libmbedtls" SO_SUFFIX

#include <dlfcn.h>

#define CHECK_DLERROR(function, argument)                             \
    do                                                                  \
    {                                                                   \
        char *CHECK_DLERROR_error = dlerror();                        \
        if (CHECK_DLERROR_error != NULL)                               \
        {                                                               \
            fprintf(stderr, "Dynamic loading error for %s(%s): %s\n",  \
                    function, argument, CHECK_DLERROR_error);         \
            mbedtls_exit(MBEDTLS_EXIT_FAILURE);                       \
        }                                                               \
    }                                                                   \
    while (0)

int main(void)
{
#if defined(MBEDTLS_MD_C) || defined(MBEDTLS_SSL_TLS_C)
    unsigned n;
#endif

#if defined(MBEDTLS_SSL_TLS_C)
    void *tls_so = dlopen(TLS_SO_FILENAME, RTLD_NOW);
    CHECK_DLERROR("dlopen", TLS_SO_FILENAME);
#pragma GCC diagnostic push
    /* dlsym() returns an object pointer which is meant to be used as a
     * function pointer. This has undefined behavior in standard C, so
     * "gcc -std=c99 -pedantic" complains about it, but it is perfectly
     * fine on platforms that have dlsym(). */
#pragma GCC diagnostic ignored "-Wpedantic"
    const int *(*ssl_list_ciphersuites)(void) =
        dlsym(tls_so, "mbedtls_ssl_list_ciphersuites");
#pragma GCC diagnostic pop
    CHECK_DLERROR("dlsym", "mbedtls_ssl_list_ciphersuites");
    const int *ciphersuites = ssl_list_ciphersuites();
    for (n = 0; ciphersuites[n] != 0; n++) {/* nothing to do, we're just counting */
        ;
    }
    mbedtls_printf("dlopen(%s): %u ciphersuites\n",
                   TLS_SO_FILENAME, n);
    dlclose(tls_so);
    CHECK_DLERROR("dlclose", TLS_SO_FILENAME);
#endif  /* MBEDTLS_SSL_TLS_C */

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    void *x509_so = dlopen(X509_SO_FILENAME, RTLD_NOW);
    CHECK_DLERROR("dlopen", X509_SO_FILENAME);
    const mbedtls_x509_crt_profile *profile =
        dlsym(x509_so, "mbedtls_x509_crt_profile_default");
    CHECK_DLERROR("dlsym", "mbedtls_x509_crt_profile_default");
    mbedtls_printf("dlopen(%s): Allowed md mask: %08x\n",
                   X509_SO_FILENAME, (unsigned) profile->allowed_mds);
    dlclose(x509_so);
    CHECK_DLERROR("dlclose", X509_SO_FILENAME);
#endif  /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_MD_C)
    const char *crypto_so_filename = NULL;
    void *crypto_so = dlopen(TFPSACRYPTO_SO_FILENAME, RTLD_NOW);
    if (dlerror() == NULL) {
        crypto_so_filename = TFPSACRYPTO_SO_FILENAME;
    } else {
        crypto_so = dlopen(MBEDCRYPTO_SO_FILENAME, RTLD_NOW);
        CHECK_DLERROR("dlopen", MBEDCRYPTO_SO_FILENAME);
        crypto_so_filename = MBEDCRYPTO_SO_FILENAME;
    }
#pragma GCC diagnostic push
    /* dlsym() returns an object pointer which is meant to be used as a
     * function pointer. This has undefined behavior in standard C, so
     * "gcc -std=c99 -pedantic" complains about it, but it is perfectly
     * fine on platforms that have dlsym(). */
#pragma GCC diagnostic ignored "-Wpedantic"
    psa_status_t (*dyn_psa_crypto_init)(void) =
        dlsym(crypto_so, "psa_crypto_init");
    psa_status_t (*dyn_psa_hash_compute)(psa_algorithm_t, const uint8_t *, size_t, uint8_t *,
                                         size_t, size_t *) =
        dlsym(crypto_so, "psa_hash_compute");

#pragma GCC diagnostic pop
    /* Demonstrate hashing a message with PSA Crypto */

    CHECK_DLERROR("dlsym", "psa_crypto_init");
    CHECK_DLERROR("dlsym", "psa_hash_compute");

    psa_status_t status = dyn_psa_crypto_init();
    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "psa_crypto_init failed: %d\n", (int) status);
        mbedtls_exit(MBEDTLS_EXIT_FAILURE);
    }

    const uint8_t input[] = "hello world";
    uint8_t hash[32]; // Buffer to hold the output hash
    size_t hash_len = 0;

    status = dyn_psa_hash_compute(PSA_ALG_SHA_256,
                                  input, sizeof(input) - 1,
                                  hash, sizeof(hash),
                                  &hash_len);
    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "psa_hash_compute failed: %d\n", (int) status);
        mbedtls_exit(MBEDTLS_EXIT_FAILURE);
    }

    mbedtls_printf("dlopen(%s): psa_hash_compute succeeded. SHA-256 output length: %zu\n",
                   crypto_so_filename, hash_len);


    dlclose(crypto_so);
    CHECK_DLERROR("dlclose", crypto_so_filename);
#endif  /* MBEDTLS_MD_C */

    return 0;
}
