# components-configuration-tuning.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Configuration Testing - Optimization tuning
################################################################

## Usage:
##   test_sha3_variation NAME THETA PI CHI RHO
## Parameters:
##   NAME: name of the variation, used in messages and in the outcome file
##   THETA, PI, CHI, RHO: 0 to disable unrolling, 1 to enable unrolling
## Inputs:
##   targets: the list of test suites to run (base names only)
test_sha3_variation () {
    declare MBEDTLS_TEST_CONFIGURATION="$MBEDTLS_TEST_CONFIGURATION+$1"

    cd "$OUT_OF_SOURCE_DIR"
    cat <<EOF >"$MBEDTLS_TEST_CONFIGURATION.h"
#define TF_PSA_CRYPTO_CONFIG_CHECK_BYPASS
#define MBEDTLS_PSA_BUILTIN_ALG_SHA3_224 1
#define MBEDTLS_PSA_BUILTIN_ALG_SHA3_256 1
#define MBEDTLS_PSA_BUILTIN_ALG_SHA3_384 1
#define MBEDTLS_PSA_BUILTIN_ALG_SHA3_512 1
#define MBEDTLS_SHA3_THETA_UNROLL $2
#define MBEDTLS_SHA3_PI_UNROLL $3
#define MBEDTLS_SHA3_CHI_UNROLL $4
#define MBEDTLS_SHA3_RHO_UNROLL $5
EOF

    cmake -D CMAKE_BUILD_TYPE:String=Release -D TF_PSA_CRYPTO_CONFIG_FILE="$MBEDTLS_TEST_CONFIGURATION.h" "$TF_PSA_CRYPTO_ROOT_DIR"
    make "${targets[@]}"
    for t in "${targets[@]}"; do
        tests/$t
    done
    make clean
}

component_test_sha3_variations () {
    declare -a targets=(tests/suites/test_suite_sha3*.data)
    targets=("${targets[@]##*/}")
    targets=("${targets[@]%.data}")

    msg "sha3 loop unroll variations: all loops unrolled"
    test_sha3_variation all_unrolled 1 1 1 1

    msg "sha3 loop unroll variations: all loops rolled up"
    test_sha3_variation all_rolled_up 0 0 0 0
}
