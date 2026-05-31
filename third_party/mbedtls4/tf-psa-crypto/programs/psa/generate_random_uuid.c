/*
 *  Generate a random UUID.
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <psa/crypto.h>
#include <mbedtls/platform.h>
#include <mbedtls/platform_util.h>
#include <stdio.h> // for stdout and stderr

#if !defined(MBEDTLS_PSA_CRYPTO_CLIENT)
int main(void)
{
    mbedtls_printf("MBEDTLS_PSA_CRYPTO_CLIENT not defined.\r\n");
    return EXIT_SUCCESS;
}
#else

int main(void)
{
    /* Initialize the PSA subsystem.
     *
     * This takes care of initializing the random generator and
     * seeding it from the configured entropy source(s) and/or
     * a nonvolatile seed (MBEDTLS_PLATFORM_NV_SEED).
     *
     * In client-server builds, psa_crypto_init() connects the client
     * to a server. The server is where the random generator lives.
     */
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "psa_crypto_init() failed (status=%d)\n",
                        status);
        mbedtls_exit(MBEDTLS_EXIT_FAILURE);
    }

    /* Generate random data.
     *
     * This probably won't fail in this toy example, because we've just
     * initialized the random generator. But in the real world,
     * psa_generate_random() can fail if the random generator's security
     * policy requires reseeding and the entropy source fails.
     * So always check the return value of psa_generate_random()!
     */
    unsigned char uuid[16];
    status = psa_generate_random(uuid, sizeof(uuid));
    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "psa_generate_random() failed (status=%d)\n",
                        status);
        mbedtls_exit(MBEDTLS_EXIT_FAILURE);
    }

    /* Force the UUID variant and version bits */
    uuid[8] = (uuid[8] & 0x3f) | 0x80; // RFC 4122
    uuid[6] = (uuid[6] & 0x0f) | 0x40; // v4 (random)

    /* Don't let stdio buffering keep a copy of the UUID.
     *
     * This is useful if the UUID is secret, in case the program is breached
     * later (or the memory is reclaimed by the operating system, and the
     * memory content leaks later before it is wiped and reused).
     */
    mbedtls_setbuf(stdout, NULL);

    /* Print out the UUID in the standard hexadecimal representation */
    mbedtls_printf("%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
                   uuid[0], uuid[1], uuid[2], uuid[3],
                   uuid[4], uuid[5], uuid[6], uuid[7],
                   uuid[8], uuid[9], uuid[10], uuid[11],
                   uuid[12], uuid[13], uuid[14], uuid[15]);

    /* Wipe the copy of the UUID in our memory.
     *
     * This is useful if the UUID is secret, in case the program is breached
     * later (or the memory is reclaimed by the operating system, and the
     * memory content leaks later before it is wiped and reused).
     */
    mbedtls_platform_zeroize(uuid, sizeof(uuid));

    /* Free resources associated with PSA.
     * (This will happen automatically when the program exits anyway.
     * The main reason to call this is if you're checking that your
     * program has no resource leaks.) */
    mbedtls_psa_crypto_free();
}
#endif /* !MBEDTLS_PSA_CRYPTO_CLIENT */
