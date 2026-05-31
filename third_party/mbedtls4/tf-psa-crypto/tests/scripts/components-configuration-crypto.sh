# components-configuration-crypto.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Configuration Testing - Crypto
################################################################

CMAKE_BUILTIN_BUILD_DIR="drivers/builtin/CMakeFiles/builtin.dir/src"

component_test_accel_ecc_all () {
    msg "build: full + all ECC accelerated"

    # Configure
    # ---------

    ./scripts/config.py full
    # Disable all the features that auto-enable ECP_LIGHT (see build_info.h)
    scripts/config.py unset MBEDTLS_PK_PARSE_EC_EXTENDED
    scripts/config.py unset MBEDTLS_PK_PARSE_EC_COMPRESSED
    scripts/config.py unset PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

    # Restartable feature is not yet supported by PSA. Once it will in
    # the future, the following line could be removed (see issues
    # 6061, 6332 and following ones)
    scripts/config.py unset MBEDTLS_ECP_RESTARTABLE

    # Build
    # -----

    cd $OUT_OF_SOURCE_DIR
    cmake -DTF_PSA_CRYPTO_TEST_DRIVER=On \
          -DTF_PSA_CRYPTO_USER_CONFIG_FILE="../tests/configs/user-config-accel-ecc.h" ..
    cmake --build .

    # Make sure built-in EC alg objects are empty.
    not grep mbedtls_ecdsa_ ${CMAKE_BUILTIN_BUILD_DIR}/ecdsa.c.o
    not grep mbedtls_psa_key_agreement_ecdh ${CMAKE_BUILTIN_BUILD_DIR}/psa_crypto_ecp.c.o
    not grep mbedtls_ecjpake_ ${CMAKE_BUILTIN_BUILD_DIR}/ecjpake.c.o
    # Also ensure that ECP module was not re-enabled
    not grep mbedtls_ecp_ ${CMAKE_BUILTIN_BUILD_DIR}/ecp.c.o

    # Run the tests
    # -------------

    msg "test: full + all ECC accelerated"
    ctest
}

component_test_accel_ecc_all_but_ecp_light() {
    msg "build: full + all ECC accelerated but ECP_LIGHT"

    # Configure
    # ---------

    ./scripts/config.py full

    # Restartable feature is not yet supported by PSA. Once it will in
    # the future, the following line could be removed (see issues
    # 6061, 6332 and following ones)
    scripts/config.py unset MBEDTLS_ECP_RESTARTABLE

    # Emphasize on the configuration that enable ECP_LIGHT. Note that currently
    # ECC key pair derivation acceleration is not supported.
    scripts/config.py set MBEDTLS_PK_PARSE_EC_EXTENDED
    scripts/config.py set MBEDTLS_PK_PARSE_EC_COMPRESSED
    scripts/config.py set PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

    # Build
    # -----

    cd $OUT_OF_SOURCE_DIR
    cmake -DTF_PSA_CRYPTO_TEST_DRIVER=On \
          -DTF_PSA_CRYPTO_USER_CONFIG_FILE="../tests/configs/user-config-accel-ecc.h" ..
    cmake --build .

    # Make sure built-in EC alg objects are empty but ECP one.
    not grep mbedtls_ecdsa_ ${CMAKE_BUILTIN_BUILD_DIR}/ecdsa.c.o
    not grep mbedtls_psa_key_agreement_ecdh ${CMAKE_BUILTIN_BUILD_DIR}/psa_crypto_ecp.c.o
    not grep mbedtls_ecjpake_ ${CMAKE_BUILTIN_BUILD_DIR}/ecjpake.c.o
    not grep mbedtls_ecp_mul ${CMAKE_BUILTIN_BUILD_DIR}/ecp.c.o
    grep mbedtls_ecp_ ${CMAKE_BUILTIN_BUILD_DIR}/ecp.c.o

    # Run the tests
    # -------------

    msg "test: full + all ECC accelerated but ECP_LIGHT"
    ctest
}

# This is a common configuration helper used directly from
# common_test_accel_ecc_ffdh_no_bignum and indirectly from:
# - component_test_accel_ecc_no_bignum
#       - accelerate all EC algs, disable RSA and FFDH
# - component_test_accel_ecc_ffdh_no_bignum
#       - accelerate all EC and FFDH algs, disable only RSA
#
# This function accepts one parameter:
# $1: a string value which states which components are tested. Allowed values
#     are "ECC" or "ECC_DH".
config_accel_ecc_ffdh_no_bignum () {
    test_target="$1"

    scripts/config.py "full"

    # Disable all the features that auto-enable ECP_LIGHT (see build_info.h)
    scripts/config.py unset MBEDTLS_PK_PARSE_EC_EXTENDED
    scripts/config.py unset MBEDTLS_PK_PARSE_EC_COMPRESSED
    scripts/config.py unset PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

    # RSA support is intentionally disabled on this test because RSA_C depends
    # on BIGNUM_C.
    scripts/config.py unset-all "PSA_WANT_KEY_TYPE_RSA_[0-9A-Z_a-z]*"
    scripts/config.py unset-all "PSA_WANT_ALG_RSA_[0-9A-Z_a-z]*"

    if [ "$test_target" = "ECC" ]; then
        # When testing ECC only, we disable FFDH support.
        scripts/config.py unset PSA_WANT_ALG_FFDH
        scripts/config.py unset-all "PSA_WANT_KEY_TYPE_DH_[0-9A-Z_a-z]*"
        scripts/config.py unset-all "PSA_WANT_DH_RFC7919_[0-9]*"
    fi

    # Restartable feature is not yet supported by PSA. Once it will in
    # the future, the following line could be removed (see issues
    # 6061, 6332 and following ones)
    scripts/config.py unset MBEDTLS_ECP_RESTARTABLE
}

# Common helper used by:
# - component_test_accel_ecc_no_bignum
# - component_test_accel_ecc_ffdh_no_bignum
#
# The goal is to build and test accelerating either:
# - ECC only or
# - both ECC and FFDH
common_test_accel_ecc_ffdh_no_bignum () {
    test_target="$1"

    # This is an internal helper to simplify text message handling
    if [ "$test_target" = "ECC_DH" ]; then
        accel_text="ECC/FFDH"
        removed_text="ECP - DH"
    else
        accel_text="ECC"
        removed_text="ECP"
    fi

    msg "build: full + accelerated $accel_text algs - $removed_text - BIGNUM"

    # Configure
    # ---------

    config_accel_ecc_ffdh_no_bignum "$test_target"

    if [ "$test_target" = "ECC_DH" ]; then
        user_config_accel_file_path="../tests/configs/user-config-accel-ecc-ffdh.h"
    else
        user_config_accel_file_path="../tests/configs/user-config-accel-ecc.h"
    fi

    # Build
    # -----

    cd $OUT_OF_SOURCE_DIR
    cmake -DTF_PSA_CRYPTO_TEST_DRIVER=On \
          -DTF_PSA_CRYPTO_USER_CONFIG_FILE="${user_config_accel_file_path}" ..
    cmake --build .

    # Make sure any built-in EC alg was not re-enabled
    not grep mbedtls_ecdsa_ ${CMAKE_BUILTIN_BUILD_DIR}/ecdsa.o
    not grep mbedtls_psa_key_agreement_ecdh ${CMAKE_BUILTIN_BUILD_DIR}/psa_crypto_ecp.o
    not grep mbedtls_ecjpake_ ${CMAKE_BUILTIN_BUILD_DIR}/ecjpake.o
    # Also ensure that ECP, RSA or BIGNUM modules were not re-enabled
    not grep mbedtls_ecp_ ${CMAKE_BUILTIN_BUILD_DIR}/ecp.o
    not grep mbedtls_rsa_ ${CMAKE_BUILTIN_BUILD_DIR}/rsa.o
    not grep mbedtls_mpi_ ${CMAKE_BUILTIN_BUILD_DIR}/bignum.o

    # Run the tests
    # -------------

    msg "test suites: full + accelerated $accel_text algs - $removed_text - BIGNUM"
    ctest
}

component_test_accel_ecc_no_bignum () {
    common_test_accel_ecc_ffdh_no_bignum "ECC"
}

component_test_accel_ecc_ffdh_no_bignum () {
    common_test_accel_ecc_ffdh_no_bignum "ECC_DH"
}

component_test_accel_ecc_some_key_types () {
    msg "build: full with accelerated EC algs and some key types"

    # Configure
    # ---------
    # Start from no builtin ECC at all, like in test_accel_ecc_all. Then, just
    # disable MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE.

    ./scripts/config.py full
    # Disable all the features that auto-enable ECP_LIGHT (see build_info.h)
    scripts/config.py unset MBEDTLS_PK_PARSE_EC_EXTENDED
    scripts/config.py unset MBEDTLS_PK_PARSE_EC_COMPRESSED
    scripts/config.py unset PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

    # Restartable feature is not yet supported by PSA. Once it will in
    # the future, the following line could be removed (see issues
    # 6061, 6332 and following ones)
    scripts/config.py unset MBEDTLS_ECP_RESTARTABLE

    cp "tests/configs/user-config-accel-ecc.h" \
        "$OUT_OF_SOURCE_DIR/user-config-accel-ecc-some-key-types.h"
    cp "tests/configs/user-config-test-driver-extension.h" $OUT_OF_SOURCE_DIR

    scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecc-some-key-types.h" \
         unset MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE

    # Build
    # -----

    cd $OUT_OF_SOURCE_DIR
    cmake -DTF_PSA_CRYPTO_TEST_DRIVER=On \
          -DTF_PSA_CRYPTO_USER_CONFIG_FILE="user-config-accel-ecc-some-key-types.h" ..
    cmake --build .

    # ECP should be enabled but not the others
    not grep mbedtls_psa_key_agreement_ecdh ${CMAKE_BUILTIN_BUILD_DIR}/psa_crypto_ecp.c.o
    not grep mbedtls_ecdsa ${CMAKE_BUILTIN_BUILD_DIR}/ecdsa.c.o
    not grep mbedtls_ecjpake ${CMAKE_BUILTIN_BUILD_DIR}/ecjpake.c.o
    grep mbedtls_ecp_ ${CMAKE_BUILTIN_BUILD_DIR}/ecp.c.o
    grep mbedtls_ecp_mul ${CMAKE_BUILTIN_BUILD_DIR}/ecp.c.o

    # Run the tests
    # -------------

    msg "test suites: full with accelerated EC algs and some key types"
    ctest
}

# Run tests with only (non-)Weierstrass accelerated
# Common code used in:
# - component_test_accel_ecc_weierstrass_curves
# - component_test_accel_ecc_non_weierstrass_curves
common_test_accel_ecc_some_curves () {
    weierstrass=$1
    if [ $weierstrass -eq 1 ]; then
        desc="Weierstrass"
    else
        desc="non-Weierstrass"
    fi
    msg "build: full minus PK with accelerated EC algs and $desc curves"

    # Configure
    # ---------

    # Start with config crypto_full and remove PK_C:
    # that's what's supported now, see docs/driver-only-builds.md.
    ./scripts/config.py full
    scripts/config.py unset MBEDTLS_PK_C
    scripts/config.py unset MBEDTLS_PK_PARSE_C
    scripts/config.py unset MBEDTLS_PK_WRITE_C

    # Restartable feature is not yet supported by PSA. Once it will in
    # the future, the following line could be removed (see issues
    # 6061, 6332 and following ones)
    scripts/config.py unset MBEDTLS_ECP_RESTARTABLE

    # this is not supported by the driver API yet
    scripts/config.py unset PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

    cp "tests/configs/user-config-accel-ecc.h" "$OUT_OF_SOURCE_DIR/user-config-accel-ecc-some-curves.h"
    cp "tests/configs/user-config-test-driver-extension.h" $OUT_OF_SOURCE_DIR

    scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecc-some-curves.h" \
                      unset-all MBEDTLS_PSA_ACCEL_ECC_

    if [ $weierstrass -eq 1 ]; then
        scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecc-some-curves.h" \
                          set-all MBEDTLS_PSA_ACCEL_ECC_SECP
        scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecc-some-curves.h" \
                          set-all MBEDTLS_PSA_ACCEL_ECC_BRAINPOOL
    else
        scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecc-some-curves.h" \
                          set-all MBEDTLS_PSA_ACCEL_ECC_MONTGOMERY
    fi

    # Build
    # -----

    cd $OUT_OF_SOURCE_DIR
    cmake -DTF_PSA_CRYPTO_TEST_DRIVER=On \
          -DTF_PSA_CRYPTO_USER_CONFIG_FILE="user-config-accel-ecc-some-curves.h" ..
    cmake --build .

    # We expect ECDH to be re-enabled for the missing curves
    grep mbedtls_psa_key_agreement_ecdh ${CMAKE_BUILTIN_BUILD_DIR}/psa_crypto_ecp.c.o
    # We expect ECP to be re-enabled, however the parts specific to the
    # families of curves that are accelerated should be ommited.
    # - functions with mxz in the name are specific to Montgomery curves
    # - ecp_muladd is specific to Weierstrass curves
    if [ $weierstrass -eq 1 ]; then
        not grep mbedtls_ecp_muladd ${CMAKE_BUILTIN_BUILD_DIR}/ecp.c.o
        grep mxz ${CMAKE_BUILTIN_BUILD_DIR}/ecp.c.o
    else
        grep mbedtls_ecp_muladd ${CMAKE_BUILTIN_BUILD_DIR}/ecp.c.o
        not grep mxz ${CMAKE_BUILTIN_BUILD_DIR}/ecp.c.o
    fi
    # We expect ECDSA and ECJPAKE to be re-enabled only when
    # Weierstrass curves are not accelerated
    if [ $weierstrass -eq 1 ]; then
        not grep mbedtls_ecdsa ${CMAKE_BUILTIN_BUILD_DIR}/ecdsa.c.o
        not grep mbedtls_ecjpake  ${CMAKE_BUILTIN_BUILD_DIR}/ecjpake.c.o
    else
        grep mbedtls_ecdsa ${CMAKE_BUILTIN_BUILD_DIR}/ecdsa.c.o
        grep mbedtls_ecjpake  ${CMAKE_BUILTIN_BUILD_DIR}/ecjpake.c.o
    fi

    # Run the tests
    # -------------

    msg "test suites: crypto_full minus PK with accelerated EC algs and weirstrass curves"
    ctest
}

component_test_accel_ecc_weierstrass_curves () {
    common_test_accel_ecc_some_curves 1
}

component_test_accel_ecc_non_weierstrass_curves () {
    common_test_accel_ecc_some_curves 0
}

component_test_accel_ecdh() {
    msg "build: accelerated ECDH"

    # Configure
    # ---------

    cp "tests/configs/user-config-accel-ecc.h" \
        "$OUT_OF_SOURCE_DIR/user-config-accel-ecdh.h"
    cp "tests/configs/user-config-test-driver-extension.h" $OUT_OF_SOURCE_DIR
    scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecdh.h" \
         unset-all MBEDTLS_PSA_ACCEL_ALG

    scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecdh.h" \
         set MBEDTLS_PSA_ACCEL_ALG_ECDH

    # Build
    # -----

    cd $OUT_OF_SOURCE_DIR
    cmake -DTF_PSA_CRYPTO_TEST_DRIVER=On \
          -DTF_PSA_CRYPTO_USER_CONFIG_FILE="user-config-accel-ecdh.h" ..
    cmake --build .

    # Make sure built-in ECDH is empty.
    not grep mbedtls_psa_key_agreement_ecdh ${CMAKE_BUILTIN_BUILD_DIR}/psa_crypto_ecp.c.o

    # Run the tests
    # -------------

    msg "test: accelerated ECDH"
    ctest
}

component_test_accel_ecdsa() {
    msg "build: accelerated ECDSA"

    # Configure
    # ---------

    # Note: We accelerate all curves, including Montgomery curves, even though
    # they are not usable for ECDSA. This is done because we want to test with
    # PK enabled, and PK does not support partial acceleration of ECC curves.

    cp "tests/configs/user-config-accel-ecc.h" \
        "$OUT_OF_SOURCE_DIR/user-config-accel-ecdsa.h"
    cp "tests/configs/user-config-test-driver-extension.h" $OUT_OF_SOURCE_DIR
    scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecdsa.h" \
         unset-all MBEDTLS_PSA_ACCEL_ALG

    scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecdsa.h" \
         set MBEDTLS_PSA_ACCEL_ALG_ECDSA
    scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecdsa.h" \
         set MBEDTLS_PSA_ACCEL_ALG_DETERMINISTIC_ECDSA

    # Build
    # -----

    cd $OUT_OF_SOURCE_DIR

    cmake -DTF_PSA_CRYPTO_TEST_DRIVER=On \
          -DTF_PSA_CRYPTO_USER_CONFIG_FILE="user-config-accel-ecdsa.h" ..
    cmake --build .

    # Make sure built-in ECDSA is empty.
    not grep mbedtls_ecdsa_ ${CMAKE_BUILTIN_BUILD_DIR}/ecdsa.c.o

    # Run the tests
    # -------------

    msg "test: accelerated ECDSA"
    ctest
}

component_test_accel_ecjpake() {
    msg "build: full with accelerated EC-JPAKE"

    # Configure
    # ---------

    ./scripts/config.py full
    cp "tests/configs/user-config-accel-ecc.h" \
        "$OUT_OF_SOURCE_DIR/user-config-accel-ecjpake.h"
    cp "tests/configs/user-config-test-driver-extension.h" $OUT_OF_SOURCE_DIR
    scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecjpake.h" \
         unset-all MBEDTLS_PSA_ACCEL_ALG

    scripts/config.py -f "$OUT_OF_SOURCE_DIR/user-config-accel-ecjpake.h" \
         set MBEDTLS_PSA_ACCEL_ALG_JPAKE

    # Build
    # -----

    cd $OUT_OF_SOURCE_DIR
    cmake -DTF_PSA_CRYPTO_TEST_DRIVER=On \
          -DTF_PSA_CRYPTO_USER_CONFIG_FILE="user-config-accel-ecjpake.h" ..
    cmake --build .

    # Make sure built-in EC-JPAKE is empty.
    not grep mbedtls_ecjpake_init ${CMAKE_BUILTIN_BUILD_DIR}/ecjpake.c.o

    # Run the tests
    # -------------

    msg "test: full with accelerated JPAKE"
    ctest
}

component_test_accel_hash () {
    msg "test: accelerated hash"

    # Build
    # -----

    cd $OUT_OF_SOURCE_DIR
    cmake -DTF_PSA_CRYPTO_TEST_DRIVER=On \
          -DTF_PSA_CRYPTO_USER_CONFIG_FILE="../tests/configs/user-config-accel-hash.h" ..
    cmake --build .

    # Make sure built-in hash objects are empty.
    not grep mbedtls_md5 ${CMAKE_BUILTIN_BUILD_DIR}/md5.c.o
    not grep mbedtls_sha1 ${CMAKE_BUILTIN_BUILD_DIR}/sha1.c.o
    not grep mbedtls_sha256 ${CMAKE_BUILTIN_BUILD_DIR}/sha256.c.o
    not grep mbedtls_sha3 ${CMAKE_BUILTIN_BUILD_DIR}/sha3.c.o
    not grep mbedtls_sha512 ${CMAKE_BUILTIN_BUILD_DIR}/sha512.c.o
    not grep mbedtls_ripemd160 ${CMAKE_BUILTIN_BUILD_DIR}/ripemd160.c.o

    # Run the tests
    # -------------

    msg "test: accelerated hash"
    ctest
}
