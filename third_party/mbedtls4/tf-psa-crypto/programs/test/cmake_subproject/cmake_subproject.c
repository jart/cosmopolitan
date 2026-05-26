/*
 *  Simple program to test that CMake builds with TF-PSA-Crypto as a
 *  subdirectory work correctly.
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <psa/crypto.h>

/* The main reason to build this is for testing the CMake build, so the program
 * doesn't need to do very much. It calls a PSA cryptography API to ensure
 * linkage works, but that is all. */
int main()
{
    psa_crypto_init();

    return 0;
}
