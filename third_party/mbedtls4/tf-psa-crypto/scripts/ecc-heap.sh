#!/bin/sh

# Measure heap usage (and performance) of ECC operations with various values of
# the relevant tunable compile-time parameters.
#
# Usage (preferably on a 32-bit platform):
# cmake -D CMAKE_BUILD_TYPE=Release .
# scripts/ecc-heap.sh | tee ecc-heap.log
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

set -eu

CONFIG_H='include/psa/crypto_config.h'

if [ -r $CONFIG_H ]; then :; else
    echo "$CONFIG_H not found" >&2
    exit 1
fi

if cmake --version >/dev/null  ; then :; else
    echo "Needs Cmake" >&2
    exit 1
fi

if git status | grep -F $CONFIG_H >/dev/null 2>&1; then
    echo "$CONFIG_H not clean" >&2
    exit 1
fi

CONFIG_BAK=${CONFIG_H}.bak
cp $CONFIG_H $CONFIG_BAK

cat << EOF >$CONFIG_H
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C
#define MBEDTLS_MEMORY_DEBUG
#define MBEDTLS_TIMING_C
#define MBEDTLS_HAVE_TIME

#define MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_PSA_BUILTIN_GET_ENTROPY

#define PSA_WANT_KEY_TYPE_AES                   1
#define PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY        1
#define PSA_WANT_ALG_ECDH                       1
#define PSA_WANT_ALG_ECDSA                      1
#define PSA_WANT_ALG_SHA_256                    1

// NIST curves >= 256 bits
#define PSA_WANT_ECC_SECP_R1_256                1
#define PSA_WANT_ECC_SECP_R1_384                1
#define PSA_WANT_ECC_SECP_R1_521                1
// SECP "koblitz-like" curve >= 256 bits
#define PSA_WANT_ECC_SECP_K1_256                1
// Brainpool curves (no specialised "mod p" routine)
#define PSA_WANT_ECC_BRAINPOOL_P_R1_256         1
#define PSA_WANT_ECC_BRAINPOOL_P_R1_384         1
#define PSA_WANT_ECC_BRAINPOOL_P_R1_512         1
// Montgomery curves
#define PSA_WANT_ECC_MONTGOMERY_255             1
#define PSA_WANT_ECC_MONTGOMERY_448             1

#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C

#define MBEDTLS_HAVE_ASM // just make things a bit faster
#define MBEDTLS_ECP_NIST_OPTIM // faster and less allocations
//#define MBEDTLS_ECP_WINDOW_SIZE            4
//#define MBEDTLS_ECP_FIXED_POINT_OPTIM      1
EOF

for F in 0 1; do
    for W in 2 3 4; do
        scripts/config.py set MBEDTLS_ECP_WINDOW_SIZE $W
        scripts/config.py set MBEDTLS_ECP_FIXED_POINT_OPTIM $F
        cmake . >/dev/null 2>&1
        make benchmark >/dev/null 2>&1
        echo "fixed point optim = $F, max window size = $W"
        echo "--------------------------------------------"
        programs/test/benchmark ecdh ecdsa
    done
done

# cleanup

mv $CONFIG_BAK $CONFIG_H
make clean
