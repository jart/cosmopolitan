/*
 * Test dynamic loading of libtfpsacrypto
 *
 * Copyright The Mbed TLS Contributors
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "tf-psa-crypto/build_info.h"
#include "psa/crypto.h"
#include "mbedtls/platform.h"

#if defined(__APPLE__)
#define SO_SUFFIX ".dylib"
#else
#define SO_SUFFIX ".so"
#endif

#define CRYPTO_SO_FILENAME "libtfpsacrypto" SO_SUFFIX
#include <stdlib.h>
#include <dlfcn.h>

#define CHECK_DLERROR(function, argument)                              \
    do                                                                 \
    {                                                                  \
        char *CHECK_DLERROR_error = dlerror();                         \
        if (CHECK_DLERROR_error != NULL) {                             \
            fprintf(stderr, "Dynamic loading error for %s(%s): %s\n",  \
                    function, argument, CHECK_DLERROR_error);          \
            mbedtls_exit(MBEDTLS_EXIT_FAILURE);            \
        }                                                              \
    }                                                                  \
    while (0)

int main(void)
{
    void *crypto_so = dlopen(CRYPTO_SO_FILENAME, RTLD_NOW);
    CHECK_DLERROR("dlopen", CRYPTO_SO_FILENAME);

#pragma GCC diagnostic push
    /* dlsym() returns an object pointer which is meant to be used as a
     * function pointer. This has undefined behavior in standard C, so
     * "gcc -std=c99 -pedantic" complains about it, but it is perfectly
     * fine on platforms that have dlsym(). */
#pragma GCC diagnostic ignored "-Wpedantic"
    psa_status_t (*psa_crypto_init_ptr)(void) =
        dlsym(crypto_so, "psa_crypto_init");
    psa_status_t (*psa_hash_compute_ptr)(psa_algorithm_t, const uint8_t *, size_t,
                                         uint8_t *, size_t, size_t *) =
        dlsym(crypto_so, "psa_hash_compute");

#pragma GCC diagnostic pop
    CHECK_DLERROR("dlsym", "psa_crypto_init");
    CHECK_DLERROR("dlsym", "psa_hash_compute");

    psa_status_t status = psa_crypto_init_ptr();
    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "psa_crypto_init failed: %d\n", (int) status);
        mbedtls_exit(MBEDTLS_EXIT_FAILURE);
    }

    const uint8_t input[] = "hello world";
    uint8_t hash[32]; // Buffer to hold the output hash
    size_t hash_len = 0;

    status = psa_hash_compute_ptr(PSA_ALG_SHA_256,
                                  input, sizeof(input) - 1,
                                  hash, sizeof(hash),
                                  &hash_len);
    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "psa_hash_compute failed: %d\n", (int) status);
        mbedtls_exit(MBEDTLS_EXIT_FAILURE);
    }

    mbedtls_printf("dlopen(%s): psa_hash_compute succeeded. SHA-256 output length: %zu\n",
                   CRYPTO_SO_FILENAME, hash_len);

    dlclose(crypto_so);
    CHECK_DLERROR("dlclose", CRYPTO_SO_FILENAME);
    return 0;
}
