# components-configuration-crypto.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Configuration Testing - Crypto
################################################################

CMAKE_BUILTIN_BUILD_DIR="tf-psa-crypto/drivers/builtin/CMakeFiles/builtin.dir/src"
CMAKE_EXTRAS_BUILD_DIR="tf-psa-crypto/extras/CMakeFiles/extras.dir"

component_test_psa_crypto_key_id_encodes_owner () {
    msg "build: full config + PSA_CRYPTO_KEY_ID_ENCODES_OWNER, cmake, gcc, ASan"
    scripts/config.py full
    scripts/config.py set MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
    CC=gcc cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: full config - USE_PSA_CRYPTO + PSA_CRYPTO_KEY_ID_ENCODES_OWNER, cmake, gcc, ASan"
    make test
}

component_test_psa_assume_exclusive_buffers () {
    msg "build: full config + MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS, cmake, gcc, ASan"
    scripts/config.py full
    scripts/config.py set MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS
    CC=gcc cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: full config + MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS, cmake, gcc, ASan"
    make test
}

component_test_crypto_with_static_key_slots() {
    msg "build: crypto full + MBEDTLS_PSA_STATIC_KEY_SLOTS"
    scripts/config.py crypto_full
    scripts/config.py set MBEDTLS_PSA_STATIC_KEY_SLOTS
    # Intentionally set MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE to a value that
    # is enough to contain:
    # - all RSA public keys up to 4096 bits (max of PSA_VENDOR_RSA_MAX_KEY_BITS).
    # - RSA key pairs up to 1024 bits, but not 2048 or larger.
    # - all FFDH key pairs and public keys up to 8192 bits (max of PSA_VENDOR_FFDH_MAX_KEY_BITS).
    # - all EC key pairs and public keys up to 521 bits (max of PSA_VENDOR_ECC_MAX_CURVE_BITS).
    scripts/config.py set MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE 1212
    # Disable the fully dynamic key store (default on) since it conflicts
    # with the static behavior that we're testing here.
    scripts/config.py unset MBEDTLS_PSA_KEY_STORE_DYNAMIC

    msg "test: crypto full + MBEDTLS_PSA_STATIC_KEY_SLOTS"
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .
    ctest
}

# check_renamed_symbols HEADER LIB
# Check that if HEADER contains '#define MACRO ...' then MACRO is not a symbol
# name in LIB.
check_renamed_symbols () {
    ! nm "$2" | sed 's/.* //' |
      grep -x -F "$(sed -n 's/^ *# *define  *\([A-Z_a-z][0-9A-Z_a-z]*\)..*/\1/p' "$1")"
}

component_build_psa_crypto_spm () {
    msg "build: full config + PSA_CRYPTO_KEY_ID_ENCODES_OWNER + PSA_CRYPTO_SPM, make, gcc"
    scripts/config.py full
    scripts/config.py unset MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS
    scripts/config.py set MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
    scripts/config.py set MBEDTLS_PSA_CRYPTO_SPM
    # We can only compile, not link, since our test and sample programs
    # aren't equipped for the modified names used when MBEDTLS_PSA_CRYPTO_SPM
    # is active.
    CFLAGS="-I$PWD/framework/tests/include/spe" cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build . --target lib

    # Check that if a symbol is renamed by crypto_spe.h, the non-renamed
    # version is not present.
    echo "Checking for renamed symbols in the library"
    check_renamed_symbols framework/tests/include/spe/crypto_spe.h library/libmbedcrypto.a
}

# The goal of this component is to build a configuration where:
# - test code and libtestdriver1 can make use of calloc/free and
# - core library (including PSA core) cannot use calloc/free.
component_test_psa_crypto_without_heap() {
    msg "crypto without heap: build libtestdriver1"
    # Disable PSA features that cannot be accelerated and whose builtin support
    # requires calloc/free.
    scripts/config.py unset PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE
    scripts/config.py unset-all "^PSA_WANT_ALG_HKDF"
    scripts/config.py unset-all "^PSA_WANT_ALG_PBKDF2_"
    scripts/config.py unset-all "^PSA_WANT_ALG_TLS12_"
    # RSA key support requires ASN1 parse/write support for testing, but ASN1
    # is disabled below.
    scripts/config.py unset-all "^PSA_WANT_KEY_TYPE_RSA_"
    scripts/config.py unset-all "^PSA_WANT_ALG_RSA_"
    # EC-JPAKE use calloc/free in PSA core
    scripts/config.py unset PSA_WANT_ALG_JPAKE
    scripts/config.py set TF_PSA_CRYPTO_ALLOW_REMOVED_MECHANISMS || true

    # Accelerate all PSA features (which are still enabled in CRYPTO_CONFIG_H).
    PSA_SYM_LIST=$(./scripts/config.py get-all-enabled PSA_WANT)
    loc_accel_list=$(echo $PSA_SYM_LIST | sed 's/PSA_WANT_//g')

    helper_libtestdriver1_adjust_config crypto
    helper_libtestdriver1_make_drivers "$loc_accel_list"

    msg "crypto without heap: build main library"
    # Disable all legacy MBEDTLS_xxx symbols.
    scripts/config.py unset-all "^MBEDTLS_"
    # Build the PSA core using the proper config file.
    scripts/config.py set MBEDTLS_PSA_CRYPTO_C
    # Enable fully-static key slots in PSA core.
    scripts/config.py set MBEDTLS_PSA_STATIC_KEY_SLOTS
    # Prevent PSA core from creating a copy of input/output buffers.
    scripts/config.py set MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS
    # Prevent PSA core from using CTR-DRBG or HMAC-DRBG for random generation.
    scripts/config.py set MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
    # Set calloc/free as null pointer functions. Calling them would crash
    # the program so we can use this as a "sentinel" for being sure no module
    # is making use of these functions in the library.
    scripts/config.py set MBEDTLS_PLATFORM_C
    scripts/config.py set MBEDTLS_PLATFORM_MEMORY
    scripts/config.py set MBEDTLS_PLATFORM_STD_CALLOC   NULL
    scripts/config.py set MBEDTLS_PLATFORM_STD_FREE     NULL

    helper_libtestdriver1_make_main "$loc_accel_list" lib

    msg "crypto without heap: build test suites and helpers"
    # Reset calloc/free functions to normal operations so that test code can
    # freely use them.
    scripts/config.py unset MBEDTLS_PLATFORM_MEMORY
    scripts/config.py unset MBEDTLS_PLATFORM_STD_CALLOC
    scripts/config.py unset MBEDTLS_PLATFORM_STD_FREE
    helper_libtestdriver1_make_main "$loc_accel_list" tests

    msg "crypto without heap: test"
    $MAKE_COMMAND test
}

component_test_no_rsa_key_pair_generation () {
    msg "build: default config minus PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE"
    scripts/config.py unset PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE
    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: default config minus PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE"
    ctest
}

component_test_no_pem_no_fs () {
    msg "build: Default + !MBEDTLS_PEM_PARSE_C + !MBEDTLS_FS_IO (ASan build)"
    scripts/config.py unset MBEDTLS_PEM_PARSE_C
    scripts/config.py unset MBEDTLS_FS_IO
    scripts/config.py unset MBEDTLS_PSA_ITS_FILE_C # requires a filesystem
    scripts/config.py unset MBEDTLS_PSA_CRYPTO_STORAGE_C # requires PSA ITS
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: !MBEDTLS_PEM_PARSE_C !MBEDTLS_FS_IO - main suites (inc. selftests) (ASan build)" # ~ 50s
    make test

    msg "test: !MBEDTLS_PEM_PARSE_C !MBEDTLS_FS_IO - ssl-opt.sh (ASan build)" # ~ 6 min
    tests/ssl-opt.sh
}

component_test_rsa_no_crt () {
    msg "build: Default + RSA_NO_CRT (ASan build)" # ~ 6 min
    scripts/config.py set MBEDTLS_RSA_NO_CRT
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: RSA_NO_CRT - main suites (inc. selftests) (ASan build)" # ~ 50s
    make test

    msg "test: RSA_NO_CRT - RSA-related part of ssl-opt.sh (ASan build)" # ~ 5s
    tests/ssl-opt.sh -f RSA

    msg "test: RSA_NO_CRT - RSA-related part of compat.sh (ASan build)" # ~ 3 min
    tests/compat.sh -t RSA

    msg "test: RSA_NO_CRT - RSA-related part of context-info.sh (ASan build)" # ~ 15 sec
    tests/context-info.sh
}

component_test_no_ctr_drbg_use_psa () {
    msg "build: Full minus CTR_DRBG, PSA crypto in TLS"
    scripts/config.py full
    scripts/config.py unset MBEDTLS_CTR_DRBG_C

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: Full minus CTR_DRBG, USE_PSA_CRYPTO - main suites"
    make test

    # In this configuration, the TLS test programs use HMAC_DRBG.
    # The SSL tests are slow, so run a small subset, just enough to get
    # confidence that the SSL code copes with HMAC_DRBG.
    msg "test: Full minus CTR_DRBG, USE_PSA_CRYPTO - ssl-opt.sh (subset)"
    tests/ssl-opt.sh -f 'Default\|SSL async private.*delay=\|tickets enabled on server'

    msg "test: Full minus CTR_DRBG, USE_PSA_CRYPTO - compat.sh (subset)"
    tests/compat.sh -m tls12 -t 'ECDSA PSK' -V NO -p OpenSSL
}

component_test_no_hmac_drbg_use_psa () {
    msg "build: Full minus HMAC_DRBG, PSA crypto in TLS"
    scripts/config.py full
    scripts/config.py unset MBEDTLS_HMAC_DRBG_C
    scripts/config.py unset PSA_WANT_ALG_DETERMINISTIC_ECDSA # requires HMAC_DRBG

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: Full minus HMAC_DRBG, USE_PSA_CRYPTO - main suites"
    make test

    # Normally our ECDSA implementation uses deterministic ECDSA. But since
    # HMAC_DRBG is disabled in this configuration, randomized ECDSA is used
    # instead.
    # Test SSL with non-deterministic ECDSA. Only test features that
    # might be affected by how ECDSA signature is performed.
    msg "test: Full minus HMAC_DRBG, USE_PSA_CRYPTO - ssl-opt.sh (subset)"
    tests/ssl-opt.sh -f 'Default\|SSL async private: sign'

    # To save time, only test one protocol version, since this part of
    # the protocol is identical in (D)TLS up to 1.2.
    msg "test: Full minus HMAC_DRBG, USE_PSA_CRYPTO - compat.sh (ECDSA)"
    tests/compat.sh -m tls12 -t 'ECDSA'
}

component_test_psa_external_rng_no_drbg_use_psa () {
    msg "build: PSA_CRYPTO_EXTERNAL_RNG minus *_DRBG, PSA crypto in TLS"
    scripts/config.py full
    scripts/config.py set MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
    scripts/config.py unset MBEDTLS_ENTROPY_NV_SEED
    scripts/config.py unset MBEDTLS_PLATFORM_NV_SEED_ALT
    scripts/config.py unset MBEDTLS_CTR_DRBG_C
    scripts/config.py unset MBEDTLS_HMAC_DRBG_C
    scripts/config.py unset PSA_WANT_ALG_DETERMINISTIC_ECDSA # Requires HMAC_DRBG

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .

    msg "test: PSA_CRYPTO_EXTERNAL_RNG minus *_DRBG, PSA crypto - main suites"
    ctest

    msg "test: PSA_CRYPTO_EXTERNAL_RNG minus *_DRBG, PSA crypto - ssl-opt.sh (subset)"
    tests/ssl-opt.sh -f 'Default\|opaque'
}

component_test_psa_external_rng_use_psa_crypto () {
    msg "build: full + PSA_CRYPTO_EXTERNAL_RNG + USE_PSA_CRYPTO minus CTR_DRBG/NV_SEED"
    scripts/config.py full
    scripts/config.py set MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
    scripts/config.py unset MBEDTLS_CTR_DRBG_C
    scripts/config.py unset MBEDTLS_ENTROPY_NV_SEED
    scripts/config.py unset MBEDTLS_PLATFORM_NV_SEED_ALT

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .

    msg "test: full + PSA_CRYPTO_EXTERNAL_RNG + USE_PSA_CRYPTO minus CTR_DRBG/NV_SEED"
    ctest

    msg "test: full + PSA_CRYPTO_EXTERNAL_RNG + USE_PSA_CRYPTO minus CTR_DRBG/NV_SEED"
    tests/ssl-opt.sh -f 'Default\|opaque'
}

component_full_no_pkparse_pkwrite () {
    msg "build: full without pkparse and pkwrite"

    scripts/config.py crypto_full
    scripts/config.py unset MBEDTLS_PK_PARSE_C
    scripts/config.py unset MBEDTLS_PK_WRITE_C

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .

    # Ensure that PK_[PARSE|WRITE]_C were not re-enabled accidentally (additive config).
    if [ -f ${TF_PSA_CRYPTO_ROOT_DIR}/extras/pkparse.c ]; then
        not grep mbedtls_pk_parse_key ${CMAKE_EXTRAS_BUILD_DIR}/pkparse.c.o
        not grep mbedtls_pk_write_key_der ${CMAKE_EXTRAS_BUILD_DIR}/pkwrite.c.o
    else
        not grep mbedtls_pk_parse_key ${CMAKE_BUILTIN_BUILD_DIR}/pkparse.c.o
        not grep mbedtls_pk_write_key_der ${CMAKE_BUILTIN_BUILD_DIR}/pkwrite.c.o
    fi

    msg "test: full without pkparse and pkwrite"
    ctest
}

component_full_no_pkwrite () {
    msg "build: full without pkwrite"

    # Using "full" config here instead of "crypto_full" as in "component_full_no_pkparse_pkwrite"
    # because here we would like to run "test_suite_debug" test cases.
    scripts/config.py full
    scripts/config.py unset MBEDTLS_PK_WRITE_C
    # Disable modules that depend on PK_WRITE_C
    scripts/config.py unset MBEDTLS_X509_CRT_WRITE_C
    scripts/config.py unset MBEDTLS_X509_CSR_WRITE_C

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    # Ensure that PK_WRITE_C was not re-enabled accidentally (additive config).
    if [ -f ${TF_PSA_CRYPTO_ROOT_DIR}/extras/pkwrite.c ]; then
        not grep mbedtls_pk_write_key_der ${CMAKE_EXTRAS_BUILD_DIR}/pkwrite.c.o
    else
        not grep mbedtls_pk_write_key_der ${CMAKE_BUILTIN_BUILD_DIR}/pkwrite.c.o
    fi

    msg "test: full without pkwrite"
    make test
}

component_test_crypto_full_md_light_only () {
    msg "build: crypto_full with only the light subset of MD"
    scripts/config.py crypto_full

    # Disable MD
    scripts/config.py unset MBEDTLS_MD_C
    # Disable direct dependencies of MD_C
    scripts/config.py unset MBEDTLS_HKDF_C
    scripts/config.py unset MBEDTLS_HMAC_DRBG_C
    scripts/config.py unset MBEDTLS_PKCS7_C
    # Disable indirect dependencies of MD_C
    scripts/config.py unset PSA_WANT_ALG_DETERMINISTIC_ECDSA
    # Disable things that would auto-enable MD_C
    scripts/config.py unset MBEDTLS_PKCS5_C

    # Note: MD-light is auto-enabled in build_info.h by modules that need it,
    # which we haven't disabled, so no need to explicitly enable it.
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .

    # Make sure we don't have the HMAC functions, but the hashing functions
    if [ -f ${TF_PSA_CRYPTO_ROOT_DIR}/extras/md.c ]; then
        not grep mbedtls_md_hmac ${CMAKE_EXTRAS_BUILD_DIR}/md.c.o
        grep mbedtls_md ${CMAKE_EXTRAS_BUILD_DIR}/md.c.o
    else
        not grep mbedtls_md_hmac ${CMAKE_BUILTIN_BUILD_DIR}/md.c.o
        grep mbedtls_md ${CMAKE_BUILTIN_BUILD_DIR}/md.c.o
    fi

    msg "test: crypto_full with only the light subset of MD"
    ctest
}

component_test_full_no_cipher () {
    msg "build: full no CIPHER"

    scripts/config.py full

    # The built-in implementation of the following algs/key-types depends
    # on CIPHER_C so we disable them.
    # This does not hold for KEY_TYPE_CHACHA20 and ALG_CHACHA20_POLY1305
    # so we keep them enabled.
    scripts/config.py unset PSA_WANT_ALG_CCM_STAR_NO_TAG
    scripts/config.py unset PSA_WANT_ALG_CMAC
    scripts/config.py unset PSA_WANT_ALG_CBC_NO_PADDING
    scripts/config.py unset PSA_WANT_ALG_CBC_PKCS7
    scripts/config.py unset PSA_WANT_ALG_CFB
    scripts/config.py unset PSA_WANT_ALG_CTR
    scripts/config.py unset PSA_WANT_ALG_ECB_NO_PADDING
    scripts/config.py unset PSA_WANT_ALG_OFB
    scripts/config.py unset PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128
    scripts/config.py unset PSA_WANT_ALG_STREAM_CIPHER

    # The following modules directly depends on CIPHER_C
    scripts/config.py unset MBEDTLS_NIST_KW_C

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .
    # Ensure that CIPHER_C was not re-enabled
    not grep mbedtls_cipher_init ${CMAKE_BUILTIN_BUILD_DIR}/cipher.c.o

    msg "test: full no CIPHER"
    ctest
}

component_test_full_no_ccm () {
    msg "build: full no PSA_WANT_ALG_CCM"

    # Full config enables:
    # - USE_PSA_CRYPTO so that TLS code dispatches cipher/AEAD to PSA
    # - CRYPTO_CONFIG so that PSA_WANT config symbols are evaluated
    scripts/config.py full

    # Disable PSA_WANT_ALG_CCM so that CCM is not supported in PSA. CCM_C is still
    # enabled, but not used from TLS since USE_PSA is set.
    # This is helpful to ensure that TLS tests below have proper dependencies.
    #
    # Note: also PSA_WANT_ALG_CCM_STAR_NO_TAG is enabled, but it does not cause
    # PSA_WANT_ALG_CCM to be re-enabled.
    scripts/config.py unset PSA_WANT_ALG_CCM

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: full no PSA_WANT_ALG_CCM"
    ctest
}

component_test_full_no_ccm_star_no_tag () {
    msg "build: full no PSA_WANT_ALG_CCM_STAR_NO_TAG"

    # Full config enables CRYPTO_CONFIG so that PSA_WANT config symbols are evaluated
    scripts/config.py full

    # Disable CCM_STAR_NO_TAG, which is the target of this test, as well as all
    # other components that enable MBEDTLS_PSA_BUILTIN_CIPHER internal symbol.
    # This basically disables all unauthenticated ciphers on the PSA side, while
    # keeping AEADs enabled.
    #
    # Note: PSA_WANT_ALG_CCM is enabled, but it does not cause
    # PSA_WANT_ALG_CCM_STAR_NO_TAG to be re-enabled.
    scripts/config.py unset PSA_WANT_ALG_CCM_STAR_NO_TAG
    scripts/config.py unset PSA_WANT_ALG_STREAM_CIPHER
    scripts/config.py unset PSA_WANT_ALG_CTR
    scripts/config.py unset PSA_WANT_ALG_CFB
    scripts/config.py unset PSA_WANT_ALG_OFB
    scripts/config.py unset PSA_WANT_ALG_ECB_NO_PADDING
    # NOTE unsettting PSA_WANT_ALG_ECB_NO_PADDING without unsetting NIST_KW_C will
    # mean PSA_WANT_ALG_ECB_NO_PADDING is re-enabled, so disabling it also.
    scripts/config.py unset MBEDTLS_NIST_KW_C
    scripts/config.py unset PSA_WANT_ALG_CBC_NO_PADDING
    scripts/config.py unset PSA_WANT_ALG_CBC_PKCS7

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    # Ensure MBEDTLS_PSA_BUILTIN_CIPHER was not enabled
    not grep mbedtls_psa_cipher ${CMAKE_BUILTIN_BUILD_DIR}/psa_crypto_cipher.c.o

    msg "test: full no PSA_WANT_ALG_CCM_STAR_NO_TAG"
    ctest
}

component_test_config_symmetric_only () {
    msg "build: configs/config-symmetric-only.h"
    MBEDTLS_CONFIG="configs/config-symmetric-only.h"
    CRYPTO_CONFIG="tf-psa-crypto/configs/crypto-config-symmetric-only.h"
    CC=$ASAN_CC cmake -DMBEDTLS_CONFIG_FILE="$MBEDTLS_CONFIG" -DTF_PSA_CRYPTO_CONFIG_FILE="$CRYPTO_CONFIG" -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: configs/config-symmetric-only.h - unit tests"
    make test
}

component_test_everest () {
    msg "build: Everest ECDH context (ASan build)" # ~ 6 min
    scripts/config.py set MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED
    CC=clang cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: Everest ECDH context - main suites (inc. selftests) (ASan build)" # ~ 50s
    make test

    msg "test: metatests (clang, ASan)"
    tests/scripts/run-metatests.sh any asan poison

    msg "test: Everest ECDH context - ECDH-related part of ssl-opt.sh (ASan build)" # ~ 5s
    tests/ssl-opt.sh -f ECDH

    msg "test: Everest ECDH context - compat.sh with some ECDH ciphersuites (ASan build)" # ~ 3 min
    # Exclude some symmetric ciphers that are redundant here to gain time.
    tests/compat.sh -f ECDH -V NO -e 'ARIA\|CAMELLIA\|CHACHA'
}

component_test_everest_curve25519_only () {
    msg "build: Everest ECDH context, only Curve25519" # ~ 6 min
    scripts/config.py set MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED
    scripts/config.py unset PSA_WANT_ALG_DETERMINISTIC_ECDSA
    scripts/config.py unset PSA_WANT_ALG_ECDSA
    scripts/config.py set PSA_WANT_ALG_ECDH
    scripts/config.py unset MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
    scripts/config.py unset MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    scripts/config.py unset PSA_WANT_ALG_JPAKE

    # Disable all curves
    scripts/config.py unset-all "PSA_WANT_ECC_[0-9A-Z_a-z]*$"
    scripts/config.py set PSA_WANT_ECC_MONTGOMERY_255

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .

    msg "test: Everest ECDH context, only Curve25519" # ~ 50s
    ctest
}

component_test_psa_collect_statuses () {
  msg "build+test: psa_collect_statuses" # ~30s
  scripts/config.py full
  tests/scripts/psa_collect_statuses.py
  # Check that psa_crypto_init() succeeded at least once
  grep -q '^0:psa_crypto_init:' tests/statuses.log
  rm -f tests/statuses.log
}

# Check that the specified libraries exist and are empty.
are_empty_libraries () {
  nm "$@" >/dev/null 2>/dev/null
  ! nm "$@" 2>/dev/null | grep -v ':$' | grep .
}

component_test_crypto_for_psa_service () {
  msg "build: make, config for PSA crypto service"
  scripts/config.py crypto
  scripts/config.py set MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
  # Disable things that are not needed for just cryptography, to
  # reach a configuration that would be typical for a PSA cryptography
  # service providing all implemented PSA algorithms.
  # System stuff
  scripts/config.py unset MBEDTLS_ERROR_C
  scripts/config.py unset MBEDTLS_TIMING_C
  scripts/config.py unset MBEDTLS_VERSION_FEATURES
  # Crypto stuff with no PSA interface
  scripts/config.py unset MBEDTLS_BASE64_C
  scripts/config.py unset MBEDTLS_HKDF_C # PSA's HKDF is independent
  # Keep MBEDTLS_MD_C because deterministic ECDSA needs it for HMAC_DRBG.
  scripts/config.py unset MBEDTLS_NIST_KW_C
  scripts/config.py unset MBEDTLS_PEM_PARSE_C
  scripts/config.py unset MBEDTLS_PEM_WRITE_C
  scripts/config.py unset MBEDTLS_PKCS12_C
  scripts/config.py unset MBEDTLS_PKCS5_C
  # MBEDTLS_PK_PARSE_C and MBEDTLS_PK_WRITE_C are actually currently needed
  # in PSA code to work with RSA keys. We don't require users to set those:
  # they will be reenabled in build_info.h.
  scripts/config.py unset MBEDTLS_PK_C
  scripts/config.py unset MBEDTLS_PK_PARSE_C
  scripts/config.py unset MBEDTLS_PK_WRITE_C
  CFLAGS="-O1" cmake .
  cmake --build .
  ctest
  are_empty_libraries library/libmbedx509.* library/libmbedtls.*
}

component_build_crypto_baremetal () {
  msg "build: make, crypto only, baremetal config"
  scripts/config.py crypto_baremetal
  CFLAGS="-O1 -I$PWD/framework/tests/include/baremetal-override/ -DMBEDTLS_TEST_PLATFORM_IS_NOT_UNIXLIKE" cmake .
  cmake --build .
  ctest
  are_empty_libraries library/libmbedx509.* library/libmbedtls.*
}

support_build_crypto_baremetal () {
    support_build_baremetal "$@"
}

# depends.py family of tests
component_test_depends_py_cipher_id () {
    msg "test/build: depends.py cipher_id (gcc)"
    tests/scripts/depends.py cipher_id
}

component_test_depends_py_cipher_chaining () {
    msg "test/build: depends.py cipher_chaining (gcc)"
    tests/scripts/depends.py cipher_chaining
}

component_test_depends_py_curves () {
    msg "test/build: depends.py curves (gcc)"
    tests/scripts/depends.py curves
}

component_test_depends_py_hashes () {
    msg "test/build: depends.py hashes (gcc)"
    tests/scripts/depends.py hashes
}

component_test_depends_py_pkalgs () {
    msg "test/build: depends.py pkalgs (gcc)"
    tests/scripts/depends.py pkalgs
}

component_test_psa_crypto_config_ffdh_2048_only () {
    msg "build: full config - only DH 2048"

    scripts/config.py full

    # Disable all DH groups other than 2048.
    scripts/config.py unset PSA_WANT_DH_RFC7919_3072
    scripts/config.py unset PSA_WANT_DH_RFC7919_4096
    scripts/config.py unset PSA_WANT_DH_RFC7919_6144
    scripts/config.py unset PSA_WANT_DH_RFC7919_8192

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .

    msg "test: full config - only DH 2048"
    ctest

    msg "ssl-opt: full config - only DH 2048"
    tests/ssl-opt.sh -f "ffdh"
}

component_test_psa_crypto_config_accel_ecdsa () {
    msg "build: accelerated ECDSA"

    # Configure
    # ---------

    # Start from default config + TLS 1.3
    helper_libtestdriver1_adjust_config "default"

    # Algorithms and key types to accelerate
    loc_accel_list="ALG_ECDSA ALG_DETERMINISTIC_ECDSA \
                    $(helper_get_psa_key_type_list "ECC") \
                    $(helper_get_psa_curve_list)"

    # Disable things that depend on it
    scripts/config.py unset MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED

    # Build
    # -----

    # These hashes are needed for some ECDSA signature tests.
    loc_extra_list="ALG_SHA_1 ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512 \
                    ALG_SHA3_224 ALG_SHA3_256 ALG_SHA3_384 ALG_SHA3_512"

    helper_libtestdriver1_make_drivers "$loc_accel_list" "$loc_extra_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure this was not re-enabled by accident (additive config)
    not grep mbedtls_ecdsa_ ${BUILTIN_SRC_PATH}/ecdsa.o

    # Run the tests
    # -------------

    msg "test: accelerated ECDSA"
    $MAKE_COMMAND test
}

component_test_psa_crypto_config_accel_ecdh () {
    msg "build: accelerated ECDH"

    # Configure
    # ---------

    # Start from default config (no USE_PSA)
    helper_libtestdriver1_adjust_config "default"

    # Algorithms and key types to accelerate
    loc_accel_list="ALG_ECDH \
                    $(helper_get_psa_key_type_list "ECC") \
                    $(helper_get_psa_curve_list)"

    # Disable things that depend on it
    scripts/config.py unset MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    scripts/config.py unset MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
    scripts/config.py unset MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED

    # Build
    # -----

    helper_libtestdriver1_make_drivers "$loc_accel_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure this was not re-enabled by accident (additive config)
    not grep mbedtls_psa_key_agreement_ecdh ${BUILTIN_SRC_PATH}/psa_crypto_ecp.o

    # Run the tests
    # -------------

    msg "test: accelerated ECDH"
    $MAKE_COMMAND test
}

component_test_psa_crypto_config_accel_ffdh () {
    msg "build: full with accelerated FFDH"

    # Configure
    # ---------

    # start with full (USE_PSA and TLS 1.3)
    helper_libtestdriver1_adjust_config "full"

    # Algorithms and key types to accelerate
    loc_accel_list="ALG_FFDH \
                    $(helper_get_psa_key_type_list "DH") \
                    $(helper_get_psa_dh_group_list)"

    # Build
    # -----

    helper_libtestdriver1_make_drivers "$loc_accel_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure this was not re-enabled by accident (additive config)
    not grep mbedtls_psa_ffdh_key_agreement ${BUILTIN_SRC_PATH}/psa_crypto_ffdh.o

    # Run the tests
    # -------------

    msg "test: full with accelerated FFDH"
    $MAKE_COMMAND test

    msg "ssl-opt: full with accelerated FFDH alg"
    tests/ssl-opt.sh -f "ffdh"
}

component_test_psa_crypto_config_reference_ffdh () {
    msg "build: full with non-accelerated FFDH"

    # Start with full (USE_PSA and TLS 1.3)
    helper_libtestdriver1_adjust_config "full"

    $MAKE_COMMAND

    msg "test suites: full with non-accelerated FFDH alg"
    $MAKE_COMMAND test

    msg "ssl-opt: full with non-accelerated FFDH alg"
    tests/ssl-opt.sh -f "ffdh"
}

component_test_psa_crypto_config_accel_pake () {
    msg "build: full with accelerated PAKE"

    # Configure
    # ---------

    helper_libtestdriver1_adjust_config "full"

    loc_accel_list="ALG_JPAKE \
                    $(helper_get_psa_key_type_list "ECC") \
                    $(helper_get_psa_curve_list)"

    # Make built-in fallback not available
    scripts/config.py unset MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED

    # Build
    # -----

    helper_libtestdriver1_make_drivers "$loc_accel_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure this was not re-enabled by accident (additive config)
    not grep mbedtls_ecjpake_init ${BUILTIN_SRC_PATH}/ecjpake.o

    # Run the tests
    # -------------

    msg "test: full with accelerated PAKE"
    $MAKE_COMMAND test
}

component_test_psa_crypto_config_accel_ecc_some_key_types () {
    msg "build: full with accelerated EC algs and some key types"

    # Configure
    # ---------

    # start with config full for maximum coverage (also enables USE_PSA)
    helper_libtestdriver1_adjust_config "full"

    # Algorithms and key types to accelerate
    # For key types, use an explicitly list to omit GENERATE (and DERIVE)
    loc_accel_list="ALG_ECDSA ALG_DETERMINISTIC_ECDSA \
                    ALG_ECDH \
                    ALG_JPAKE \
                    KEY_TYPE_ECC_PUBLIC_KEY \
                    KEY_TYPE_ECC_KEY_PAIR_BASIC \
                    KEY_TYPE_ECC_KEY_PAIR_IMPORT \
                    KEY_TYPE_ECC_KEY_PAIR_EXPORT \
                    $(helper_get_psa_curve_list)"

    # Disable all curves - those that aren't accelerated should be re-enabled
    helper_disable_builtin_curves

    # Restartable feature is not yet supported by PSA. Once it will in
    # the future, the following line could be removed (see issues
    # 6061, 6332 and following ones)
    scripts/config.py unset MBEDTLS_ECP_RESTARTABLE

    # this is not supported by the driver API yet
    scripts/config.py unset PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

    # Build
    # -----

    # These hashes are needed for some ECDSA signature tests.
    loc_extra_list="ALG_SHA_1 ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512 \
                    ALG_SHA3_224 ALG_SHA3_256 ALG_SHA3_384 ALG_SHA3_512"
    helper_libtestdriver1_make_drivers "$loc_accel_list" "$loc_extra_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # ECP should be re-enabled but not the others
    not grep mbedtls_psa_key_agreement_ecdh ${BUILTIN_SRC_PATH}/psa_crypto_ecp.o
    not grep mbedtls_ecdsa ${BUILTIN_SRC_PATH}/ecdsa.o
    not grep mbedtls_ecjpake  ${BUILTIN_SRC_PATH}/ecjpake.o
    grep mbedtls_ecp ${BUILTIN_SRC_PATH}/ecp.o

    # Run the tests
    # -------------

    msg "test suites: full with accelerated EC algs and some key types"
    $MAKE_COMMAND test
}

# Run tests with only (non-)Weierstrass accelerated
# Common code used in:
# - component_test_psa_crypto_config_accel_ecc_weierstrass_curves
# - component_test_psa_crypto_config_accel_ecc_non_weierstrass_curves
common_test_psa_crypto_config_accel_ecc_some_curves () {
    weierstrass=$1
    if [ $weierstrass -eq 1 ]; then
        desc="Weierstrass"
    else
        desc="non-Weierstrass"
    fi

    msg "build: crypto_full minus PK with accelerated EC algs and $desc curves"

    # Configure
    # ---------

    # Start with config crypto_full and remove PK_C:
    # that's what's supported now, see docs/driver-only-builds.md.
    helper_libtestdriver1_adjust_config "crypto_full"
    scripts/config.py unset MBEDTLS_PK_C
    scripts/config.py unset MBEDTLS_PK_PARSE_C
    scripts/config.py unset MBEDTLS_PK_WRITE_C

    # Disable all curves - those that aren't accelerated should be re-enabled
    helper_disable_builtin_curves

    # Note: Curves are handled in a special way by the libtestdriver machinery,
    # so we only want to include them in the accel list when building the main
    # libraries, hence the use of a separate variable.
    # Note: the following loop is a modified version of
    # helper_get_psa_curve_list that only keeps Weierstrass families.
    loc_weierstrass_list=""
    loc_non_weierstrass_list=""
    for item in $(sed -n 's/^#define PSA_WANT_\(ECC_[0-9A-Z_a-z]*\).*/\1/p' <"$CRYPTO_CONFIG_H"); do
        case $item in
            ECC_BRAINPOOL*|ECC_SECP*)
                loc_weierstrass_list="$loc_weierstrass_list $item"
                ;;
            *)
                loc_non_weierstrass_list="$loc_non_weierstrass_list $item"
                ;;
        esac
    done
    if [ $weierstrass -eq 1 ]; then
        loc_curve_list=$loc_weierstrass_list
    else
        loc_curve_list=$loc_non_weierstrass_list
    fi

    # Algorithms and key types to accelerate
    loc_accel_list="ALG_ECDSA ALG_DETERMINISTIC_ECDSA \
                    ALG_ECDH \
                    ALG_JPAKE \
                    $(helper_get_psa_key_type_list "ECC") \
                    $loc_curve_list"

    # Restartable feature is not yet supported by PSA. Once it will in
    # the future, the following line could be removed (see issues
    # 6061, 6332 and following ones)
    scripts/config.py unset MBEDTLS_ECP_RESTARTABLE

    # this is not supported by the driver API yet
    scripts/config.py unset PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

    # Build
    # -----

    # These hashes are needed for some ECDSA signature tests.
    loc_extra_list="ALG_SHA_1 ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512 \
                    ALG_SHA3_224 ALG_SHA3_256 ALG_SHA3_384 ALG_SHA3_512"
    helper_libtestdriver1_make_drivers "$loc_accel_list" "$loc_extra_list"

    # For grep to work below we need less inlining in ecp.c
    ASAN_CFLAGS="$ASAN_CFLAGS -O0" helper_libtestdriver1_make_main "$loc_accel_list"

    # We expect ECDH to be re-enabled for the missing curves
    grep mbedtls_psa_key_agreement_ecdh ${BUILTIN_SRC_PATH}/psa_crypto_ecp.o
    # We expect ECP to be re-enabled, however the parts specific to the
    # families of curves that are accelerated should be ommited.
    # - functions with mxz in the name are specific to Montgomery curves
    # - ecp_muladd is specific to Weierstrass curves
    ##nm ${BUILTIN_SRC_PATH}/ecp.o | tee ecp.syms
    if [ $weierstrass -eq 1 ]; then
        not grep mbedtls_ecp_muladd ${BUILTIN_SRC_PATH}/ecp.o
        grep mxz ${BUILTIN_SRC_PATH}/ecp.o
    else
        grep mbedtls_ecp_muladd ${BUILTIN_SRC_PATH}/ecp.o
        not grep mxz ${BUILTIN_SRC_PATH}/ecp.o
    fi
    # We expect ECDSA and ECJPAKE to be re-enabled only when
    # Weierstrass curves are not accelerated
    if [ $weierstrass -eq 1 ]; then
        not grep mbedtls_ecdsa ${BUILTIN_SRC_PATH}/ecdsa.o
        not grep mbedtls_ecjpake  ${BUILTIN_SRC_PATH}/ecjpake.o
    else
        grep mbedtls_ecdsa ${BUILTIN_SRC_PATH}/ecdsa.o
        grep mbedtls_ecjpake  ${BUILTIN_SRC_PATH}/ecjpake.o
    fi

    # Run the tests
    # -------------

    msg "test suites: crypto_full minus PK with accelerated EC algs and $desc curves"
    $MAKE_COMMAND test
}

component_test_psa_crypto_config_accel_ecc_weierstrass_curves () {
    common_test_psa_crypto_config_accel_ecc_some_curves 1
}

component_test_psa_crypto_config_accel_ecc_non_weierstrass_curves () {
    common_test_psa_crypto_config_accel_ecc_some_curves 0
}

# Auxiliary function to build config for all EC based algorithms (EC-JPAKE,
# ECDH, ECDSA) with and without drivers.
# The input parameter is a boolean value which indicates:
# - 0 keep built-in EC algs,
# - 1 exclude built-in EC algs (driver only).
#
# This is used by the two following components to ensure they always use the
# same config, except for the use of driver or built-in EC algorithms:
# - component_test_psa_crypto_config_accel_ecc_ecp_light_only;
# - component_test_psa_crypto_config_reference_ecc_ecp_light_only.
# This supports comparing their test coverage with analyze_outcomes.py.
config_psa_crypto_config_ecp_light_only () {
    driver_only="$1"
    # start with config full for maximum coverage (also enables USE_PSA)
    helper_libtestdriver1_adjust_config "full"

    # Restartable feature is not yet supported by PSA. Once it will in
    # the future, the following line could be removed (see issues
    # 6061, 6332 and following ones)
    scripts/config.py unset MBEDTLS_ECP_RESTARTABLE
}

# Keep in sync with component_test_psa_crypto_config_reference_ecc_ecp_light_only
component_test_psa_crypto_config_accel_ecc_ecp_light_only () {
    msg "build: full with accelerated EC algs"

    # Configure
    # ---------

    # Use the same config as reference, only without built-in EC algs
    config_psa_crypto_config_ecp_light_only 1

    # Algorithms and key types to accelerate
    loc_accel_list="ALG_ECDSA ALG_DETERMINISTIC_ECDSA \
                    ALG_ECDH \
                    ALG_JPAKE \
                    $(helper_get_psa_key_type_list "ECC") \
                    $(helper_get_psa_curve_list)"

    # Do not disable builtin curves because that support is required for:
    # - MBEDTLS_PK_PARSE_EC_EXTENDED
    # - MBEDTLS_PK_PARSE_EC_COMPRESSED

    # Build
    # -----

    # These hashes are needed for some ECDSA signature tests.
    loc_extra_list="ALG_SHA_1 ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512 \
                    ALG_SHA3_224 ALG_SHA3_256 ALG_SHA3_384 ALG_SHA3_512"
    helper_libtestdriver1_make_drivers "$loc_accel_list" "$loc_extra_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure any built-in EC alg was not re-enabled by accident (additive config)
    not grep mbedtls_ecdsa_ ${BUILTIN_SRC_PATH}/ecdsa.o
    not grep mbedtls_psa_key_agreement_ecdh ${BUILTIN_SRC_PATH}/psa_crypto_ecp.o
    not grep mbedtls_ecjpake_ ${BUILTIN_SRC_PATH}/ecjpake.o
    not grep mbedtls_ecp_mul ${BUILTIN_SRC_PATH}/ecp.o

    # Run the tests
    # -------------

    msg "test suites: full with accelerated EC algs"
    $MAKE_COMMAND test

    msg "ssl-opt: full with accelerated EC algs"
    tests/ssl-opt.sh
}

# Keep in sync with component_test_psa_crypto_config_accel_ecc_ecp_light_only
component_test_psa_crypto_config_reference_ecc_ecp_light_only () {
    msg "build: non-accelerated EC algs"

    config_psa_crypto_config_ecp_light_only 0

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test suites: full with non-accelerated EC algs"
    ctest

    msg "ssl-opt: full with non-accelerated EC algs"
    tests/ssl-opt.sh
}

# This helper function is used by:
# - component_test_psa_crypto_config_accel_ecc_no_ecp_at_all()
# - component_test_psa_crypto_config_reference_ecc_no_ecp_at_all()
# to ensure that both tests use the same underlying configuration when testing
# driver's coverage with analyze_outcomes.py.
#
# This functions accepts 1 boolean parameter as follows:
# - 1: building with accelerated EC algorithms (ECDSA, ECDH, ECJPAKE), therefore
#      excluding their built-in implementation as well as ECP_C & ECP_LIGHT
# - 0: include built-in implementation of EC algorithms.
#
# PK_C and RSA_C are always disabled to ensure there is no remaining dependency
# on the ECP module.
config_psa_crypto_no_ecp_at_all () {
    driver_only="$1"
    # start with full config for maximum coverage (also enables USE_PSA)
    helper_libtestdriver1_adjust_config "full"

    # Disable all the features that auto-enable ECP_LIGHT (see build_info.h)
    scripts/config.py unset MBEDTLS_PK_PARSE_EC_EXTENDED
    scripts/config.py unset MBEDTLS_PK_PARSE_EC_COMPRESSED
    scripts/config.py unset PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

    # Restartable feature is not yet supported by PSA. Once it will in
    # the future, the following line could be removed (see issues
    # 6061, 6332 and following ones)
    scripts/config.py unset MBEDTLS_ECP_RESTARTABLE
}

# Build and test a configuration where driver accelerates all EC algs while
# all support and dependencies from ECP and ECP_LIGHT are removed on the library
# side.
#
# Keep in sync with component_test_psa_crypto_config_reference_ecc_no_ecp_at_all()
component_test_psa_crypto_config_accel_ecc_no_ecp_at_all () {
    msg "build: full + accelerated EC algs - ECP"

    # Configure
    # ---------

    # Set common configurations between library's and driver's builds
    config_psa_crypto_no_ecp_at_all 1
    # Disable all the builtin curves. All the required algs are accelerated.
    helper_disable_builtin_curves

    # Algorithms and key types to accelerate
    loc_accel_list="ALG_ECDSA ALG_DETERMINISTIC_ECDSA \
                    ALG_ECDH \
                    ALG_JPAKE \
                    $(helper_get_psa_key_type_list "ECC") \
                    $(helper_get_psa_curve_list)"

    # Build
    # -----

    # Things we wanted supported in libtestdriver1, but not accelerated in the main library:
    # SHA-1 and all SHA-2/3 variants, as they are used by ECDSA deterministic.
    loc_extra_list="ALG_SHA_1 ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512 \
                    ALG_SHA3_224 ALG_SHA3_256 ALG_SHA3_384 ALG_SHA3_512"

    helper_libtestdriver1_make_drivers "$loc_accel_list" "$loc_extra_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure any built-in EC alg was not re-enabled by accident (additive config)
    not grep mbedtls_ecdsa_ ${BUILTIN_SRC_PATH}/ecdsa.o
    not grep mbedtls_psa_key_agreement_ecdh ${BUILTIN_SRC_PATH}/psa_crypto_ecp.o
    not grep mbedtls_ecjpake_ ${BUILTIN_SRC_PATH}/ecjpake.o
    # Also ensure that ECP module was not re-enabled
    not grep mbedtls_ecp_ ${BUILTIN_SRC_PATH}/ecp.o

    # Run the tests
    # -------------

    msg "test: full + accelerated EC algs - ECP"
    $MAKE_COMMAND test

    msg "ssl-opt: full + accelerated EC algs - ECP"
    tests/ssl-opt.sh
}

# Reference function used for driver's coverage analysis in analyze_outcomes.py
# in conjunction with component_test_psa_crypto_config_accel_ecc_no_ecp_at_all().
# Keep in sync with its accelerated counterpart.
component_test_psa_crypto_config_reference_ecc_no_ecp_at_all () {
    msg "build: full + non accelerated EC algs"

    config_psa_crypto_no_ecp_at_all 0

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: full + non accelerated EC algs"
    ctest

    msg "ssl-opt: full + non accelerated EC algs"
    tests/ssl-opt.sh
}

# This is a common configuration helper used directly from:
# - common_test_psa_crypto_config_accel_ecc_ffdh_no_bignum
# - common_test_psa_crypto_config_reference_ecc_ffdh_no_bignum
# and indirectly from:
# - component_test_psa_crypto_config_accel_ecc_no_bignum
#       - accelerate all EC algs, disable RSA and FFDH
# - component_test_psa_crypto_config_reference_ecc_no_bignum
#       - this is the reference component of the above
#       - it still disables RSA and FFDH, but it uses builtin EC algs
# - component_test_psa_crypto_config_accel_ecc_ffdh_no_bignum
#       - accelerate all EC and FFDH algs, disable only RSA
# - component_test_psa_crypto_config_reference_ecc_ffdh_no_bignum
#       - this is the reference component of the above
#       - it still disables RSA, but it uses builtin EC and FFDH algs
#
# This function accepts 2 parameters:
# $1: a boolean value which states if we are testing an accelerated scenario
#     or not.
# $2: a string value which states which components are tested. Allowed values
#     are "ECC" or "ECC_DH".
config_psa_crypto_config_accel_ecc_ffdh_no_bignum () {
    driver_only="$1"
    test_target="$2"
    # start with full config for maximum coverage (also enables USE_PSA)
    helper_libtestdriver1_adjust_config "full"

    # Disable all the features that auto-enable ECP_LIGHT (see build_info.h)
    scripts/config.py unset MBEDTLS_PK_PARSE_EC_EXTENDED
    scripts/config.py unset MBEDTLS_PK_PARSE_EC_COMPRESSED
    scripts/config.py unset PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

    # RSA support is intentionally disabled on this test because RSA_C depends
    # on BIGNUM_C.
    scripts/config.py unset-all "PSA_WANT_KEY_TYPE_RSA_[0-9A-Z_a-z]*"
    scripts/config.py unset-all "PSA_WANT_ALG_RSA_[0-9A-Z_a-z]*"
    scripts/config.py unset MBEDTLS_X509_RSASSA_PSS_SUPPORT
    # Also disable key exchanges that depend on RSA
    scripts/config.py unset MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED

    if [ "$test_target" = "ECC" ]; then
        # When testing ECC only, we disable FFDH support, both from builtin and
        # PSA sides.
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
# - component_test_psa_crypto_config_accel_ecc_no_bignum
# - component_test_psa_crypto_config_accel_ecc_ffdh_no_bignum
#
# The goal is to build and test accelerating either:
# - ECC only or
# - both ECC and FFDH
#
# It is meant to be used in conjunction with
# common_test_psa_crypto_config_reference_ecc_ffdh_no_bignum() for drivers
# coverage analysis in the "analyze_outcomes.py" script.
common_test_psa_crypto_config_accel_ecc_ffdh_no_bignum () {
    test_target="$1"

    # This is an internal helper to simplify text message handling
    if [ "$test_target" = "ECC_DH" ]; then
        accel_text="ECC/FFDH"
        removed_text="ECP - DH"
    else
        accel_text="ECC"
        removed_text="ECP"
    fi

    msg "build: full + accelerated $accel_text algs + USE_PSA - $removed_text - BIGNUM"

    # Configure
    # ---------

    # Set common configurations between library's and driver's builds
    config_psa_crypto_config_accel_ecc_ffdh_no_bignum 1 "$test_target"
    # Disable all the builtin curves. All the required algs are accelerated.
    helper_disable_builtin_curves

    # By default we accelerate all EC keys/algs
    loc_accel_list="ALG_ECDSA ALG_DETERMINISTIC_ECDSA \
                    ALG_ECDH \
                    ALG_JPAKE \
                    $(helper_get_psa_key_type_list "ECC") \
                    $(helper_get_psa_curve_list)"
    # Optionally we can also add DH to the list of accelerated items
    if [ "$test_target" = "ECC_DH" ]; then
        loc_accel_list="$loc_accel_list \
                        ALG_FFDH \
                        $(helper_get_psa_key_type_list "DH") \
                        $(helper_get_psa_dh_group_list)"
    fi

    # Build
    # -----

    # Things we wanted supported in libtestdriver1, but not accelerated in the main library:
    # SHA-1 and all SHA-2/3 variants, as they are used by ECDSA deterministic.
    loc_extra_list="ALG_SHA_1 ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512 \
                    ALG_SHA3_224 ALG_SHA3_256 ALG_SHA3_384 ALG_SHA3_512"

    helper_libtestdriver1_make_drivers "$loc_accel_list" "$loc_extra_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure any built-in EC alg was not re-enabled by accident (additive config)
    not grep mbedtls_ecdsa_ ${BUILTIN_SRC_PATH}/ecdsa.o
    not grep mbedtls_psa_key_agreement_ecdh ${BUILTIN_SRC_PATH}/psa_crypto_ecp.o
    not grep mbedtls_ecjpake_ ${BUILTIN_SRC_PATH}/ecjpake.o
    # Also ensure that ECP, RSA or BIGNUM modules were not re-enabled
    not grep mbedtls_ecp_ ${BUILTIN_SRC_PATH}/ecp.o
    not grep mbedtls_rsa_ ${BUILTIN_SRC_PATH}/rsa.o
    not grep mbedtls_mpi_ ${BUILTIN_SRC_PATH}/bignum.o

    # Run the tests
    # -------------

    msg "test suites: full + accelerated $accel_text algs + USE_PSA - $removed_text - BIGNUM"

    $MAKE_COMMAND test

    msg "ssl-opt: full + accelerated $accel_text algs + USE_PSA - $removed_text - BIGNUM"
    tests/ssl-opt.sh
}

# Common helper used by:
# - component_test_psa_crypto_config_reference_ecc_no_bignum
# - component_test_psa_crypto_config_reference_ecc_ffdh_no_bignum
#
# The goal is to build and test a reference scenario (i.e. with builtin
# components) compared to the ones used in
# common_test_psa_crypto_config_accel_ecc_ffdh_no_bignum() above.
#
# It is meant to be used in conjunction with
# common_test_psa_crypto_config_accel_ecc_ffdh_no_bignum() for drivers'
# coverage analysis in "analyze_outcomes.py" script.
common_test_psa_crypto_config_reference_ecc_ffdh_no_bignum () {
    test_target="$1"

    # This is an internal helper to simplify text message handling
    if [ "$test_target" = "ECC_DH" ]; then
        accel_text="ECC/FFDH"
    else
        accel_text="ECC"
    fi

    msg "build: full + non accelerated $accel_text algs + USE_PSA"

    config_psa_crypto_config_accel_ecc_ffdh_no_bignum 0 "$test_target"

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test suites: full + non accelerated EC algs + USE_PSA"
    ctest

    msg "ssl-opt: full + non accelerated $accel_text algs + USE_PSA"
    tests/ssl-opt.sh
}

component_test_psa_crypto_config_accel_ecc_no_bignum () {
    common_test_psa_crypto_config_accel_ecc_ffdh_no_bignum "ECC"
}

component_test_psa_crypto_config_reference_ecc_no_bignum () {
    common_test_psa_crypto_config_reference_ecc_ffdh_no_bignum "ECC"
}

component_test_psa_crypto_config_accel_ecc_ffdh_no_bignum () {
    common_test_psa_crypto_config_accel_ecc_ffdh_no_bignum "ECC_DH"
}

component_test_psa_crypto_config_reference_ecc_ffdh_no_bignum () {
    common_test_psa_crypto_config_reference_ecc_ffdh_no_bignum "ECC_DH"
}

component_test_tfm_config_as_is () {
    msg "build: configs/config-tfm.h"
    MBEDTLS_CONFIG="configs/config-tfm.h"
    CRYPTO_CONFIG="tf-psa-crypto/configs/ext/crypto_config_profile_medium.h"
    CC=$ASAN_CC cmake -DMBEDTLS_CONFIG_FILE="$MBEDTLS_CONFIG" -DTF_PSA_CRYPTO_CONFIG_FILE="$CRYPTO_CONFIG" -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: configs/config-tfm.h - unit tests"
    make test
}

# Helper for setting common configurations between:
# - component_test_tfm_config_p256m_driver_accel_ec()
# - component_test_tfm_config_no_p256m()
common_tfm_config () {
    # Enable TF-M config
    cp configs/config-tfm.h "$CONFIG_H"
    cp tf-psa-crypto/configs/ext/crypto_config_profile_medium.h "$CRYPTO_CONFIG_H"

    # Config adjustment for better test coverage in our environment.
    # This is not needed just to build and pass tests.
    #
    # Enable filesystem I/O for the benefit of PK parse/write tests.
    sed -i '/PROFILE_M_PSA_CRYPTO_CONFIG_H/i #define MBEDTLS_FS_IO' "$CRYPTO_CONFIG_H"
}

# Keep this in sync with component_test_tfm_config() as they are both meant
# to be used in analyze_outcomes.py for driver's coverage analysis.
component_test_tfm_config_p256m_driver_accel_ec () {
    msg "build: TF-M config + p256m driver + accel ECDH(E)/ECDSA"

    common_tfm_config

    # Build crypto library
    CC=$ASAN_CC CFLAGS="$ASAN_CFLAGS -I$PWD/framework/tests/include/spe" cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .

    # Make sure any built-in EC alg was not re-enabled by accident (additive config)
    not grep mbedtls_ecdsa_ ${CMAKE_BUILTIN_BUILD_DIR}/ecdsa.c.o
    not grep mbedtls_psa_key_agreement_ecdh ${CMAKE_BUILTIN_BUILD_DIR}/psa_crypto_ecp.c.o
    not grep mbedtls_ecjpake_ ${CMAKE_BUILTIN_BUILD_DIR}/ecjpake.c.o
    # Also ensure that ECP, RSA or BIGNUM modules were not re-enabled
    not grep mbedtls_ecp_ ${CMAKE_BUILTIN_BUILD_DIR}/ecp.c.o
    not grep mbedtls_rsa_ ${CMAKE_BUILTIN_BUILD_DIR}/rsa.c.o
    not grep mbedtls_mpi_ ${CMAKE_BUILTIN_BUILD_DIR}/bignum.c.o
    # Check that p256m was built
    grep -q p256_ecdsa_ library/libmbedcrypto.a

    # In "config-tfm.h" we disabled CIPHER_C tweaking TF-M's configuration
    # files, so we want to ensure that it has not be re-enabled accidentally.
    not grep mbedtls_cipher ${CMAKE_BUILTIN_BUILD_DIR}/cipher.c.o

    # Run the tests
    msg "test: TF-M config + p256m driver + accel ECDH(E)/ECDSA"
    ctest
}

# Keep this in sync with component_test_tfm_config_p256m_driver_accel_ec() as
# they are both meant to be used in analyze_outcomes.py for driver's coverage
# analysis.
component_test_tfm_config_no_p256m () {
    common_tfm_config

    # Disable P256M driver, which is on by default, so that analyze_outcomes
    # can compare this test with test_tfm_config_p256m_driver_accel_ec
    scripts/config.py -f "$CRYPTO_CONFIG_H" unset MBEDTLS_PSA_P256M_DRIVER_ENABLED

    msg "build: TF-M config without p256m"
    CFLAGS="-I$PWD/framework/tests/include/spe" cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .
    # Check that p256m was not built
    not grep p256_ecdsa_ library/libmbedcrypto.a

    # In "config-tfm.h" we disabled CIPHER_C tweaking TF-M's configuration
    # files, so we want to ensure that it has not be re-enabled accidentally.
    not grep mbedtls_cipher ${CMAKE_BUILTIN_BUILD_DIR}/cipher.c.o

    msg "test: TF-M config without p256m"
    ctest
}

# This is an helper used by:
# - component_test_psa_ecc_key_pair_no_derive
# - component_test_psa_ecc_key_pair_no_generate
# The goal is to test with all PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_yyy symbols
# enabled, but one. Input arguments are as follows:
# - $1 is the configuration to start from
# - $2 is the key type under test, i.e. ECC/RSA/DH
# - $3 is the key option to be unset (i.e. generate, derive, etc)
build_and_test_psa_want_key_pair_partial () {
    base_config=$1
    key_type=$2
    unset_option=$3
    disabled_psa_want="PSA_WANT_KEY_TYPE_${key_type}_KEY_PAIR_${unset_option}"

    msg "build: $base_config - ${disabled_psa_want}"
    scripts/config.py "$base_config"

    # All the PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_yyy are enabled by default in
    # crypto_config.h so we just disable the one we don't want.
    scripts/config.py unset "$disabled_psa_want"

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .

    msg "test: $base_config - ${disabled_psa_want}"
    ctest
}

component_test_psa_ecc_key_pair_no_derive () {
    build_and_test_psa_want_key_pair_partial full "ECC" "DERIVE"
}

component_test_psa_ecc_key_pair_no_generate () {
    # TLS needs ECC key generation whenever ephemeral ECDH is enabled.
    # We don't have proper guards for configurations with ECC key generation
    # disabled (https://github.com/Mbed-TLS/mbedtls/issues/9481). Until
    # then (if ever), just test the crypto part of the library.
    build_and_test_psa_want_key_pair_partial crypto_full "ECC" "GENERATE"
}

config_psa_crypto_accel_rsa () {
    driver_only=$1

    # Start from crypto_full config (no X.509, no TLS)
    helper_libtestdriver1_adjust_config "crypto_full"

    if [ "$driver_only" -eq 1 ]; then
        # We need PEM parsing in the test library as well to support the import
        # of PEM encoded RSA keys.
        scripts/config.py -c "$CONFIG_TEST_DRIVER_H" set MBEDTLS_PEM_PARSE_C
        scripts/config.py -c "$CONFIG_TEST_DRIVER_H" set MBEDTLS_BASE64_C
    fi
}

component_test_psa_crypto_config_accel_rsa_crypto () {
    msg "build: crypto_full with accelerated RSA"

    loc_accel_list="ALG_RSA_OAEP ALG_RSA_PSS \
                    ALG_RSA_PKCS1V15_CRYPT ALG_RSA_PKCS1V15_SIGN \
                    KEY_TYPE_RSA_PUBLIC_KEY \
                    KEY_TYPE_RSA_KEY_PAIR_BASIC \
                    KEY_TYPE_RSA_KEY_PAIR_GENERATE \
                    KEY_TYPE_RSA_KEY_PAIR_IMPORT \
                    KEY_TYPE_RSA_KEY_PAIR_EXPORT"

    # Configure
    # ---------

    config_psa_crypto_accel_rsa 1

    # Build
    # -----

    # These hashes are needed for unit tests.
    loc_extra_list="ALG_SHA_1 ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512 \
                    ALG_SHA3_224 ALG_SHA3_256 ALG_SHA3_384 ALG_SHA3_512 ALG_MD5"
    helper_libtestdriver1_make_drivers "$loc_accel_list" "$loc_extra_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure this was not re-enabled by accident (additive config)
    not grep mbedtls_rsa ${BUILTIN_SRC_PATH}/rsa.o

    # Run the tests
    # -------------

    msg "test: crypto_full with accelerated RSA"
    $MAKE_COMMAND test
}

component_test_psa_crypto_config_reference_rsa_crypto () {
    msg "build: crypto_full with non-accelerated RSA"

    # Configure
    # ---------
    config_psa_crypto_accel_rsa 0

    # Build
    # -----
    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    # Run the tests
    # -------------
    msg "test: crypto_full with non-accelerated RSA"
    ctest
}

# This is a temporary test to verify that full RSA support is present even when
# only one single new symbols (PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC) is defined.
component_test_new_psa_want_key_pair_symbol () {
    msg "Build: crypto config - PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC"

    # Create a temporary output file unless there is already one set
    if [ "$MBEDTLS_TEST_OUTCOME_FILE" ]; then
        REMOVE_OUTCOME_ON_EXIT="no"
    else
        REMOVE_OUTCOME_ON_EXIT="yes"
        MBEDTLS_TEST_OUTCOME_FILE="$PWD/out.csv"
        export MBEDTLS_TEST_OUTCOME_FILE
    fi

    # Start from crypto configuration
    scripts/config.py crypto

    # Remove RSA dependencies
    scripts/config.py unset MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
    scripts/config.py unset MBEDTLS_X509_RSASSA_PSS_SUPPORT

    # Keep only PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC enabled in order to ensure
    # that proper translations is done in crypto_legacy.h.
    scripts/config.py unset PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT
    scripts/config.py unset PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT
    scripts/config.py unset PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "Test: crypto config - PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC"
    ctest

    # Parse only 1 relevant line from the outcome file, i.e. a test which is
    # performing RSA signature.
    msg "Verify that 'RSA PKCS1 Sign #1 (SHA512, 1536 bits RSA)' is PASS"
    cat $MBEDTLS_TEST_OUTCOME_FILE | grep 'RSA PKCS1 Sign #1 (SHA512, 1536 bits RSA)' | grep -q "PASS"

    if [ "$REMOVE_OUTCOME_ON_EXIT" == "yes" ]; then
        rm $MBEDTLS_TEST_OUTCOME_FILE
    fi
}

component_test_psa_crypto_config_accel_hash () {
    msg "test: accelerated hash"

    loc_accel_list="ALG_MD5 ALG_RIPEMD160 ALG_SHA_1 \
                    ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512 \
                    ALG_SHA3_224 ALG_SHA3_256 ALG_SHA3_384 ALG_SHA3_512"

    # Configure
    # ---------

    # Start from default config (no USE_PSA)
    helper_libtestdriver1_adjust_config "default"

    # Build
    # -----

    helper_libtestdriver1_make_drivers "$loc_accel_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # There's a risk of something getting re-enabled via config_psa.h;
    # make sure it did not happen. Note: it's OK for MD_C to be enabled.
    not grep mbedtls_md5 ${BUILTIN_SRC_PATH}/md5.o
    not grep mbedtls_sha1 ${BUILTIN_SRC_PATH}/sha1.o
    not grep mbedtls_sha256 ${BUILTIN_SRC_PATH}/sha256.o
    not grep mbedtls_sha512 ${BUILTIN_SRC_PATH}/sha512.o
    not grep mbedtls_ripemd160 ${BUILTIN_SRC_PATH}/ripemd160.o

    # Run the tests
    # -------------

    msg "test: accelerated hash"
    $MAKE_COMMAND test
}

# Auxiliary function to build config for hashes with and without drivers
config_psa_crypto_hash_use_psa () {
    driver_only="$1"
    # start with config full for maximum coverage (also enables USE_PSA)
    helper_libtestdriver1_adjust_config "full"
    if [ "$driver_only" -eq 1 ]; then
        # disable the built-in implementation of hashes
        scripts/config.py unset MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT
        scripts/config.py unset MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT
    fi
}

# Note that component_test_psa_crypto_config_reference_hash_use_psa
# is related to this component and both components need to be kept in sync.
# For details please see comments for component_test_psa_crypto_config_reference_hash_use_psa.
component_test_psa_crypto_config_accel_hash_use_psa () {
    msg "test: full with accelerated hashes"

    loc_accel_list="ALG_MD5 ALG_RIPEMD160 ALG_SHA_1 \
                    ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512 \
                    ALG_SHA3_224 ALG_SHA3_256 ALG_SHA3_384 ALG_SHA3_512"

    # Configure
    # ---------

    config_psa_crypto_hash_use_psa 1

    # Build
    # -----

    helper_libtestdriver1_make_drivers "$loc_accel_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # There's a risk of something getting re-enabled via config_psa.h;
    # make sure it did not happen. Note: it's OK for MD_C to be enabled.
    not grep mbedtls_md5 ${BUILTIN_SRC_PATH}/md5.o
    not grep mbedtls_sha1 ${BUILTIN_SRC_PATH}/sha1.o
    not grep mbedtls_sha256 ${BUILTIN_SRC_PATH}/sha256.o
    not grep mbedtls_sha512 ${BUILTIN_SRC_PATH}/sha512.o
    not grep mbedtls_ripemd160 ${BUILTIN_SRC_PATH}/ripemd160.o

    # Run the tests
    # -------------

    msg "test: full with accelerated hashes"
    $MAKE_COMMAND test

    # This is mostly useful so that we can later compare outcome files with
    # the reference config in analyze_outcomes.py, to check that the
    # dependency declarations in ssl-opt.sh and in TLS code are correct.
    msg "test: ssl-opt.sh, full with accelerated hashes"
    tests/ssl-opt.sh

    # This is to make sure all ciphersuites are exercised, but we don't need
    # interop testing (besides, we already got some from ssl-opt.sh).
    msg "test: compat.sh, full with accelerated hashes"
    tests/compat.sh -p mbedTLS -V YES
}

# This component provides reference configuration for test_psa_crypto_config_accel_hash_use_psa
# without accelerated hash. The outcome from both components are used by the analyze_outcomes.py
# script to find regression in test coverage when accelerated hash is used (tests and ssl-opt).
# Both components need to be kept in sync.
component_test_psa_crypto_config_reference_hash_use_psa () {
    msg "test: full without accelerated hashes"

    config_psa_crypto_hash_use_psa 0

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: full without accelerated hashes"
    ctest

    msg "test: ssl-opt.sh, full without accelerated hashes"
    tests/ssl-opt.sh
}

# Auxiliary function to build config for hashes with and without drivers
config_psa_crypto_hmac_use_psa () {
    driver_only="$1"
    # start with config full for maximum coverage (also enables USE_PSA)
    helper_libtestdriver1_adjust_config "full"

    if [ "$driver_only" -eq 1 ]; then
        # Disable MD_C in order to disable the builtin support for HMAC. MD_LIGHT
        # is still enabled though (for ENTROPY_C among others).
        scripts/config.py unset MBEDTLS_MD_C
        # Also disable the configuration options that tune the builtin hashes,
        # since those hashes are disabled.
        scripts/config.py unset-all MBEDTLS_SHA
    fi

    # Direct dependencies of MD_C. We disable them also in the reference
    # component to work with the same set of features.
    scripts/config.py unset MBEDTLS_PKCS7_C
    scripts/config.py unset MBEDTLS_PKCS5_C
    scripts/config.py unset MBEDTLS_HMAC_DRBG_C
    scripts/config.py unset MBEDTLS_HKDF_C
    # Dependencies of HMAC_DRBG
    scripts/config.py unset PSA_WANT_ALG_DETERMINISTIC_ECDSA
}

component_test_psa_crypto_config_accel_hmac () {
    msg "test: full with accelerated hmac"

    loc_accel_list="ALG_HMAC KEY_TYPE_HMAC \
                    ALG_MD5 ALG_RIPEMD160 ALG_SHA_1 \
                    ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512 \
                    ALG_SHA3_224 ALG_SHA3_256 ALG_SHA3_384 ALG_SHA3_512"

    # Configure
    # ---------

    config_psa_crypto_hmac_use_psa 1

    # Build
    # -----

    helper_libtestdriver1_make_drivers "$loc_accel_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Ensure that built-in support for HMAC is disabled.
    if [ -f ${TF_PSA_CRYPTO_ROOT_DIR}/extras/md.c ]; then
        not grep mbedtls_md_hmac ${TF_PSA_CRYPTO_ROOT_DIR}/extras/md.o
    else
        not grep mbedtls_md_hmac ${BUILTIN_SRC_PATH}/md.o
    fi

    # Run the tests
    # -------------

    msg "test: full with accelerated hmac"
    $MAKE_COMMAND test
}

component_test_psa_crypto_config_reference_hmac () {
    msg "test: full without accelerated hmac"

    config_psa_crypto_hmac_use_psa 0

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: full without accelerated hmac"
    ctest
}

component_test_psa_crypto_config_accel_aead () {
    msg "test: accelerated AEAD"

    loc_accel_list="ALG_GCM ALG_CCM ALG_CHACHA20_POLY1305 \
                    KEY_TYPE_AES KEY_TYPE_CHACHA20 KEY_TYPE_ARIA KEY_TYPE_CAMELLIA"

    # Configure
    # ---------

    # Start from full config
    helper_libtestdriver1_adjust_config "full"

    # Disable CCM_STAR_NO_TAG because this re-enables CCM_C.
    scripts/config.py unset PSA_WANT_ALG_CCM_STAR_NO_TAG

    # Build
    # -----

    helper_libtestdriver1_make_drivers "$loc_accel_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure this was not re-enabled by accident (additive config)
    not grep mbedtls_ccm ${BUILTIN_SRC_PATH}/ccm.o
    not grep mbedtls_gcm ${BUILTIN_SRC_PATH}/gcm.o
    not grep mbedtls_chachapoly ${BUILTIN_SRC_PATH}/chachapoly.o

    # Run the tests
    # -------------

    msg "test: accelerated AEAD"
    $MAKE_COMMAND test
}

# This is a common configuration function used in:
# - component_test_psa_crypto_config_accel_cipher_aead_cmac
# - component_test_psa_crypto_config_reference_cipher_aead_cmac
common_psa_crypto_config_accel_cipher_aead_cmac () {
    # Start from the full config
    helper_libtestdriver1_adjust_config "full"

    scripts/config.py unset MBEDTLS_NIST_KW_C
}

# The 2 following test components, i.e.
# - component_test_psa_crypto_config_accel_cipher_aead_cmac
# - component_test_psa_crypto_config_reference_cipher_aead_cmac
# are meant to be used together in analyze_outcomes.py script in order to test
# driver's coverage for ciphers and AEADs.
component_test_psa_crypto_config_accel_cipher_aead_cmac () {
    msg "build: full config with accelerated cipher inc. AEAD and CMAC"

    loc_accel_list="ALG_ECB_NO_PADDING ALG_CBC_NO_PADDING ALG_CBC_PKCS7 ALG_CTR ALG_CFB \
                    ALG_OFB ALG_XTS ALG_STREAM_CIPHER ALG_CCM_STAR_NO_TAG \
                    ALG_GCM ALG_CCM ALG_CHACHA20_POLY1305 ALG_CMAC \
                    KEY_TYPE_AES KEY_TYPE_ARIA KEY_TYPE_CHACHA20 KEY_TYPE_CAMELLIA"

    # Configure
    # ---------

    common_psa_crypto_config_accel_cipher_aead_cmac

    # Build
    # -----

    helper_libtestdriver1_make_drivers "$loc_accel_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure this was not re-enabled by accident (additive config)
    not grep mbedtls_cipher ${BUILTIN_SRC_PATH}/cipher.o
    not grep mbedtls_aes ${BUILTIN_SRC_PATH}/aes.o
    not grep mbedtls_aria ${BUILTIN_SRC_PATH}/aria.o
    not grep mbedtls_camellia ${BUILTIN_SRC_PATH}/camellia.o
    not grep mbedtls_ccm ${BUILTIN_SRC_PATH}/ccm.o
    not grep mbedtls_gcm ${BUILTIN_SRC_PATH}/gcm.o
    not grep mbedtls_chachapoly ${BUILTIN_SRC_PATH}/chachapoly.o
    not grep mbedtls_cmac ${BUILTIN_SRC_PATH}/cmac.o
    not grep mbedtls_poly1305 ${BUILTIN_SRC_PATH}/poly1305.o

    # Run the tests
    # -------------

    msg "test: full config with accelerated cipher inc. AEAD and CMAC"
    $MAKE_COMMAND test

    msg "ssl-opt: full config with accelerated cipher inc. AEAD and CMAC"
    # Exclude password-protected key tests — they require built-in CBC and AES.
    tests/ssl-opt.sh -e "TLS: password protected"

    msg "compat.sh: full config with accelerated cipher inc. AEAD and CMAC"
    tests/compat.sh -V NO -p mbedTLS
}

component_test_psa_crypto_config_reference_cipher_aead_cmac () {
    msg "build: full config with non-accelerated cipher inc. AEAD and CMAC"
    common_psa_crypto_config_accel_cipher_aead_cmac

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: full config with non-accelerated cipher inc. AEAD and CMAC"
    ctest

    msg "ssl-opt: full config with non-accelerated cipher inc. AEAD and CMAC"
    # Exclude password-protected key tests as in test_psa_crypto_config_accel_cipher_aead_cmac.
    tests/ssl-opt.sh -e "TLS: password protected"

    msg "compat.sh: full config with non-accelerated cipher inc. AEAD and CMAC"
    tests/compat.sh -V NO -p mbedTLS
}

common_block_cipher_dispatch () {
    TEST_WITH_DRIVER="$1"

    # Start from the full config
    helper_libtestdriver1_adjust_config "full"

    # Disable cipher's modes that, when not accelerated, cause
    # legacy key types to be re-enabled in "config_adjust_legacy_from_psa.h".
    # Keep this also in the reference component in order to skip the same tests
    # that were skipped in the accelerated one.
    scripts/config.py unset PSA_WANT_ALG_CTR
    scripts/config.py unset PSA_WANT_ALG_CFB
    scripts/config.py unset PSA_WANT_ALG_OFB
    scripts/config.py unset PSA_WANT_ALG_CBC_NO_PADDING
    scripts/config.py unset PSA_WANT_ALG_CBC_PKCS7
    scripts/config.py unset PSA_WANT_ALG_CMAC
    scripts/config.py unset PSA_WANT_ALG_CCM_STAR_NO_TAG
    scripts/config.py unset PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128

    # Disable direct dependency on AES_C
    scripts/config.py unset MBEDTLS_NIST_KW_C

    # Prevent the cipher module from using deprecated PSA path. The reason is
    # that otherwise there will be tests relying on "aes_info" (defined in
    # "cipher_wrap.c") whose functions are not available when AES_C is
    # not defined. ARIA and Camellia are not a problem in this case because
    # the PSA path is not tested for these key types.
    scripts/config.py set MBEDTLS_DEPRECATED_REMOVED
}

component_test_full_block_cipher_psa_dispatch_static_keystore () {
    msg "build: full + PSA dispatch in block_cipher with static keystore"
    # Check that the static key store works well when CTR_DRBG uses a
    # PSA key for AES.
    scripts/config.py unset MBEDTLS_PSA_KEY_STORE_DYNAMIC

    loc_accel_list="ALG_ECB_NO_PADDING \
                    KEY_TYPE_AES KEY_TYPE_ARIA KEY_TYPE_CAMELLIA"

    # Configure
    # ---------

    common_block_cipher_dispatch 1

    # Build
    # -----

    helper_libtestdriver1_make_drivers "$loc_accel_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure disabled components were not re-enabled by accident (additive
    # config)
    not grep mbedtls_aes_ ${BUILTIN_SRC_PATH}/aes.o
    not grep mbedtls_aria_ ${BUILTIN_SRC_PATH}/aria.o
    not grep mbedtls_camellia_ ${BUILTIN_SRC_PATH}/camellia.o

    # Run the tests
    # -------------

    msg "test: full + PSA dispatch in block_cipher with static keystore"
    $MAKE_COMMAND test
}

component_test_full_block_cipher_psa_dispatch () {
    msg "build: full + PSA dispatch in block_cipher"

    loc_accel_list="ALG_ECB_NO_PADDING \
                    KEY_TYPE_AES KEY_TYPE_ARIA KEY_TYPE_CAMELLIA"

    # Configure
    # ---------

    common_block_cipher_dispatch 1

    # Build
    # -----

    helper_libtestdriver1_make_drivers "$loc_accel_list"

    helper_libtestdriver1_make_main "$loc_accel_list"

    # Make sure disabled components were not re-enabled by accident (additive
    # config)
    not grep mbedtls_aes_ ${BUILTIN_SRC_PATH}/aes.o
    not grep mbedtls_aria_ ${BUILTIN_SRC_PATH}/aria.o
    not grep mbedtls_camellia_ ${BUILTIN_SRC_PATH}/camellia.o

    # Run the tests
    # -------------

    msg "test: full + PSA dispatch in block_cipher"
    $MAKE_COMMAND test
}

# This is the reference component of component_test_full_block_cipher_psa_dispatch
component_test_full_block_cipher_legacy_dispatch () {
    msg "build: full + legacy dispatch in block_cipher"

    common_block_cipher_dispatch 0

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: full + legacy dispatch in block_cipher"
    ctest
}

component_test_aead_chachapoly_disabled () {
    msg "build: full minus CHACHAPOLY"
    scripts/config.py full
    scripts/config.py unset PSA_WANT_ALG_CHACHA20_POLY1305

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .

    msg "test: full minus CHACHAPOLY"
    ctest
}

component_test_aead_only_ccm () {
    msg "build: full minus CHACHAPOLY and GCM"
    scripts/config.py full
    scripts/config.py unset PSA_WANT_ALG_CHACHA20_POLY1305
    scripts/config.py unset PSA_WANT_ALG_GCM

    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    cmake --build .

    msg "test: full minus CHACHAPOLY and GCM"
    ctest
}

component_test_ccm_aes_sha256 () {
    msg "build: CCM + AES + SHA256 configuration"

    # Setting a blank config disables everyhing in the library side.
    echo '#define MBEDTLS_CONFIG_H ' >"$CONFIG_H"
    cp tf-psa-crypto/configs/crypto-config-ccm-aes-sha256.h "$CRYPTO_CONFIG_H"

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .
    msg "test: CCM + AES + SHA256 configuration"
    ctest
}

# Test that the given .o file builds with all (valid) combinations of the given options.
#
# Syntax: build_test_config_combos FILE VALIDATOR_FUNCTION OPT1 OPT2 ...
#
# The validator function is the name of a function to validate the combination of options.
# It may be "" if all combinations are valid.
# It receives a string containing a combination of options, as passed to the compiler,
# e.g. "-DOPT1 -DOPT2 ...". It must return 0 iff the combination is valid, non-zero if invalid.
build_test_config_combos () {
    file=$1
    shift
    validate_options=$1
    shift
    options=("$@")

    # clear all of the options so that they can be overridden on the clang commandline
    for opt in "${options[@]}"; do
        ./scripts/config.py unset ${opt}
    done

    # enter the library directory
    cd library

    # The most common issue is unused variables/functions, so ensure -Wunused is set.
    warning_flags="-Werror -Wall -Wextra -Wwrite-strings -Wpointer-arith -Wimplicit-fallthrough -Wshadow -Wvla -Wformat=2 -Wno-format-nonliteral -Wshadow -Wasm-operand-widths -Wunused"

    # Extract the command generated by the Makefile to build the target file.
    # This ensures that we have any include paths, macro definitions, etc
    # that may be applied by make.
    # Add -fsyntax-only as we only want a syntax check and don't need to generate a file.
    compile_cmd="clang \$(LOCAL_CFLAGS) ${warning_flags} -fsyntax-only -c"

    makefile=$(TMPDIR=. mktemp)
    deps=""

    len=${#options[@]}
    source_file=../${file%.o}.c

    targets=0
    echo 'include Makefile' >${makefile}

    for ((i = 0; i < $((2**${len})); i++)); do
        # generate each of 2^n combinations of options
        # each bit of $i is used to determine if options[i] will be set or not
        target="t"
        clang_args=""
        for ((j = 0; j < ${len}; j++)); do
            if (((i >> j) & 1)); then
                opt=-D${options[$j]}
                clang_args="${clang_args} ${opt}"
                target="${target}${opt}"
            fi
        done

        # if combination is not known to be invalid, add it to the makefile
        if [[ -z $validate_options ]] || $validate_options "${clang_args}"; then
            cmd="${compile_cmd} ${clang_args}"
            echo "${target}: ${source_file}; $cmd ${source_file}" >> ${makefile}

            deps="${deps} ${target}"
            ((++targets))
        fi
    done

    echo "build_test_config_combos: ${deps}" >> ${makefile}

    # execute all of the commands via Make (probably in parallel)
    make -s -f ${makefile} build_test_config_combos
    echo "$targets targets checked"

    # clean up the temporary makefile
    rm ${makefile}
}

validate_aes_config_variations () {
    if [[ "$1" == *"MBEDTLS_AES_USE_HARDWARE_ONLY"* ]]; then
        if [[ !(("$HOSTTYPE" == "aarch64" && "$1" != *"MBEDTLS_AESCE_C"*) || \
                ("$HOSTTYPE" == "x86_64"  && "$1" != *"MBEDTLS_AESNI_C"*)) ]]; then
            return 1
        fi
    fi
    return 0
}

component_build_aes_variations () {
    # 18s - around 90ms per clang invocation on M1 Pro
    #
    # aes.o has many #if defined(...) guards that intersect in complex ways.
    # Test that all the combinations build cleanly.

    MBEDTLS_ROOT_DIR="$PWD"
    msg "build: aes.o for all combinations of relevant config options"

    build_test_config_combos ${BUILTIN_SRC_PATH}/aes.o validate_aes_config_variations \
        "MBEDTLS_AES_ROM_TABLES" \
        "MBEDTLS_AES_FEWER_TABLES" "MBEDTLS_AES_USE_HARDWARE_ONLY" \
        "MBEDTLS_AESNI_C" "MBEDTLS_AESCE_C" "MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH"

    cd "$MBEDTLS_ROOT_DIR"
    msg "build: aes.o for all combinations of relevant config options + BLOCK_CIPHER_NO_DECRYPT"

    # MBEDTLS_BLOCK_CIPHER_NO_DECRYPT is incompatible with ECB in PSA, CBC/XTS/NIST_KW,
    # manually set or unset those configurations to check
    # MBEDTLS_BLOCK_CIPHER_NO_DECRYPT with various combinations in aes.o.
    scripts/config.py set MBEDTLS_BLOCK_CIPHER_NO_DECRYPT
    scripts/config.py unset MBEDTLS_NIST_KW_C

    scripts/config.py unset PSA_WANT_ALG_CBC_NO_PADDING
    scripts/config.py unset PSA_WANT_ALG_CBC_PKCS7
    scripts/config.py unset PSA_WANT_ALG_ECB_NO_PADDING

    build_test_config_combos ${BUILTIN_SRC_PATH}/aes.o validate_aes_config_variations \
        "MBEDTLS_AES_ROM_TABLES" \
        "MBEDTLS_AES_FEWER_TABLES" "MBEDTLS_AES_USE_HARDWARE_ONLY" \
        "MBEDTLS_AESNI_C" "MBEDTLS_AESCE_C" "MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH"
}

support_build_aes_aesce_armcc () {
    support_build_armcc
}

# For timebeing, no aarch64 gcc available in CI and no arm64 CI node.
component_build_aes_aesce_armcc () {
    msg "Build: AESCE test on arm64 platform without plain C."
    scripts/config.py baremetal

    # armc[56] don't support SHA-512 intrinsics
    scripts/config.py unset MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT

    # Stop armclang warning about feature detection for A64_CRYPTO.
    # With this enabled, the library does build correctly under armclang,
    # but in baremetal builds (as tested here), feature detection is
    # unavailable, and the user is notified via a #warning. So enabling
    # this feature would prevent us from building with -Werror on
    # armclang. Tracked in #7198.
    scripts/config.py unset MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT
    scripts/config.py set MBEDTLS_HAVE_ASM

    msg "AESCE, build with default configuration."
    scripts/config.py set MBEDTLS_AESCE_C
    scripts/config.py unset MBEDTLS_AES_USE_HARDWARE_ONLY
    helper_armc6_build_test "-O1 --target=aarch64-arm-none-eabi -march=armv8-a+crypto"

    msg "AESCE, build AESCE only"
    scripts/config.py set MBEDTLS_AESCE_C
    scripts/config.py set MBEDTLS_AES_USE_HARDWARE_ONLY
    helper_armc6_build_test "-O1 --target=aarch64-arm-none-eabi -march=armv8-a+crypto"
}

component_test_aes_only_128_bit_keys () {
    msg "build: default config + AES_ONLY_128_BIT_KEY_LENGTH"
    scripts/config.py set MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH
    scripts/config.py set MBEDTLS_PSA_CRYPTO_RNG_STRENGTH 128

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: default config + AES_ONLY_128_BIT_KEY_LENGTH"
    ctest
}

component_test_no_ctr_drbg_aes_only_128_bit_keys () {
    msg "build: default config + AES_ONLY_128_BIT_KEY_LENGTH - CTR_DRBG_C"
    scripts/config.py set MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH
    scripts/config.py set MBEDTLS_PSA_CRYPTO_RNG_STRENGTH 128
    scripts/config.py unset MBEDTLS_CTR_DRBG_C

    CC=clang cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: default config + AES_ONLY_128_BIT_KEY_LENGTH - CTR_DRBG_C"
    ctest
}

component_test_aes_only_128_bit_keys_have_builtins () {
    msg "build: default config + AES_ONLY_128_BIT_KEY_LENGTH - AESNI_C - AESCE_C"
    scripts/config.py set MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH
    scripts/config.py set MBEDTLS_PSA_CRYPTO_RNG_STRENGTH 128
    scripts/config.py unset MBEDTLS_AESNI_C
    scripts/config.py unset MBEDTLS_AESCE_C

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: default config + AES_ONLY_128_BIT_KEY_LENGTH - AESNI_C - AESCE_C"
    ctest

    msg "selftest: default config + AES_ONLY_128_BIT_KEY_LENGTH - AESNI_C - AESCE_C"
    programs/test/selftest
}

component_test_gcm_largetable () {
    msg "build: default config + GCM_LARGE_TABLE - AESNI_C - AESCE_C"
    scripts/config.py set MBEDTLS_GCM_LARGE_TABLE
    scripts/config.py unset MBEDTLS_AESNI_C
    scripts/config.py unset MBEDTLS_AESCE_C

    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: default config - GCM_LARGE_TABLE - AESNI_C - AESCE_C"
    ctest
}

component_test_aes_fewer_tables () {
    msg "build: default config with AES_FEWER_TABLES enabled"
    scripts/config.py set MBEDTLS_AES_FEWER_TABLES
    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: AES_FEWER_TABLES"
    ctest
}

component_test_aes_rom_tables () {
    msg "build: default config with AES_ROM_TABLES enabled"
    scripts/config.py set MBEDTLS_AES_ROM_TABLES
    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: AES_ROM_TABLES"
    ctest
}

component_test_aes_fewer_tables_and_rom_tables () {
    msg "build: default config with AES_ROM_TABLES and AES_FEWER_TABLES enabled"
    scripts/config.py set MBEDTLS_AES_FEWER_TABLES
    scripts/config.py set MBEDTLS_AES_ROM_TABLES
    cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build .

    msg "test: AES_FEWER_TABLES + AES_ROM_TABLES"
    ctest
}

# helper for component_test_block_cipher_no_decrypt_aesni() which:
# - enable/disable the list of config options passed from -s/-u respectively.
# - build
# - test for tests_suite_xxx
# - selftest
#
# Usage: helper_block_cipher_no_decrypt_build_test
#        [-s set_opts] [-u unset_opts] [-c cflags] [-l ldflags] [option [...]]
# Options:  -s set_opts     the list of config options to enable
#           -u unset_opts   the list of config options to disable
#           -c cflags       the list of options passed to CFLAGS
#           -l ldflags      the list of options passed to LDFLAGS
helper_block_cipher_no_decrypt_build_test () {
    while [ $# -gt 0 ]; do
        case "$1" in
            -s)
                shift; local set_opts="$1";;
            -u)
                shift; local unset_opts="$1";;
            -c)
                shift; local cflags="-Werror -Wall -Wextra $1";;
            -l)
                shift; local ldflags="$1";;
        esac
        shift
    done
    set_opts="${set_opts:-}"
    unset_opts="${unset_opts:-}"
    cflags="${cflags:-}"
    ldflags="${ldflags:-}"

    [ -n "$set_opts" ] && echo "Enabling: $set_opts" && scripts/config.py set-all $set_opts
    [ -n "$unset_opts" ] && echo "Disabling: $unset_opts" && scripts/config.py unset-all $unset_opts

    msg "build: default config + BLOCK_CIPHER_NO_DECRYPT${set_opts:+ + $set_opts}${unset_opts:+ - $unset_opts} with $cflags${ldflags:+, $ldflags}"
    CFLAGS="-O2 $cflags" LDFLAGS="$ldflags" cmake .
    cmake --build .

    # Make sure we don't have mbedtls_xxx_setkey_dec in AES/ARIA/CAMELLIA
    not grep mbedtls_aes_setkey_dec ${BUILTIN_SRC_PATH}/aes.o
    not grep mbedtls_aria_setkey_dec ${BUILTIN_SRC_PATH}/aria.o
    not grep mbedtls_camellia_setkey_dec ${BUILTIN_SRC_PATH}/camellia.o
    # Make sure we don't have mbedtls_internal_aes_decrypt in AES
    not grep mbedtls_internal_aes_decrypt ${BUILTIN_SRC_PATH}/aes.o
    # Make sure we don't have mbedtls_aesni_inverse_key in AESNI
    not grep mbedtls_aesni_inverse_key ${BUILTIN_SRC_PATH}/aesni.o

    msg "test: default config + BLOCK_CIPHER_NO_DECRYPT${set_opts:+ + $set_opts}${unset_opts:+ - $unset_opts} with $cflags${ldflags:+, $ldflags}"
    ctest

    msg "selftest: default config + BLOCK_CIPHER_NO_DECRYPT${set_opts:+ + $set_opts}${unset_opts:+ - $unset_opts} with $cflags${ldflags:+, $ldflags}"
    programs/test/selftest
    cmake --build . --target clean
}

# This is a configuration function used in component_test_block_cipher_no_decrypt_xxx:
config_block_cipher_no_decrypt () {
    scripts/config.py set MBEDTLS_BLOCK_CIPHER_NO_DECRYPT
    scripts/config.py unset MBEDTLS_NIST_KW_C

    # Enable support for cryptographic mechanisms through the PSA API.
    # Note: XTS, KW are not yet supported via the PSA API in Mbed TLS.
    scripts/config.py unset PSA_WANT_ALG_CBC_NO_PADDING
    scripts/config.py unset PSA_WANT_ALG_CBC_PKCS7
    scripts/config.py unset PSA_WANT_ALG_ECB_NO_PADDING
}

component_test_block_cipher_no_decrypt_aesni () {
    # Test BLOCK_CIPHER_NO_DECRYPT with AESNI intrinsics, AESNI assembly and
    # AES C implementation on x86_64 and with AESNI intrinsics on x86.

    # This consistently causes an llvm crash on clang 3.8, so use gcc
    export CC=gcc
    config_block_cipher_no_decrypt

    # test AESNI intrinsics
    helper_block_cipher_no_decrypt_build_test \
        -s "MBEDTLS_AESNI_C" \
        -c "-mpclmul -msse2 -maes"

    # test AESNI assembly
    helper_block_cipher_no_decrypt_build_test \
        -s "MBEDTLS_AESNI_C" \
        -c "-mno-pclmul -mno-sse2 -mno-aes"

    # test AES C implementation
    helper_block_cipher_no_decrypt_build_test \
        -u "MBEDTLS_AESNI_C"

    # test AESNI intrinsics for i386 target
    helper_block_cipher_no_decrypt_build_test \
        -s "MBEDTLS_AESNI_C" \
        -c "-m32 -mpclmul -msse2 -maes" \
        -l "-m32"
}

support_test_block_cipher_no_decrypt_aesce_armcc () {
    support_build_armcc
}

component_test_block_cipher_no_decrypt_aesce_armcc () {
    scripts/config.py baremetal

    # armc[56] don't support SHA-512 intrinsics
    scripts/config.py unset MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT

    # Stop armclang warning about feature detection for A64_CRYPTO.
    # With this enabled, the library does build correctly under armclang,
    # but in baremetal builds (as tested here), feature detection is
    # unavailable, and the user is notified via a #warning. So enabling
    # this feature would prevent us from building with -Werror on
    # armclang. Tracked in #7198.
    scripts/config.py unset MBEDTLS_SHA256_USE_A64_CRYPTO_IF_PRESENT
    scripts/config.py set MBEDTLS_HAVE_ASM

    config_block_cipher_no_decrypt

    # test AESCE baremetal build
    scripts/config.py set MBEDTLS_AESCE_C
    msg "build: default config + BLOCK_CIPHER_NO_DECRYPT with AESCE"
    helper_armc6_build_test "-O1 --target=aarch64-arm-none-eabi -march=armv8-a+crypto -Werror -Wall -Wextra"

    # Make sure we don't have mbedtls_xxx_setkey_dec in AES/ARIA/CAMELLIA
    not grep mbedtls_aes_setkey_dec ${CMAKE_BUILTIN_BUILD_DIR}/aes.c.o
    not grep mbedtls_aria_setkey_dec ${CMAKE_BUILTIN_BUILD_DIR}/aria.c.o
    not grep mbedtls_camellia_setkey_dec ${CMAKE_BUILTIN_BUILD_DIR}/camellia.c.o
    # Make sure we don't have mbedtls_internal_aes_decrypt in AES
    not grep mbedtls_internal_aes_decrypt ${CMAKE_BUILTIN_BUILD_DIR}/aes.c.o
    # Make sure we don't have mbedtls_aesce_inverse_key and aesce_decrypt_block in AESCE
    not grep mbedtls_aesce_inverse_key ${CMAKE_BUILTIN_BUILD_DIR}/aesce.c.o
    not grep aesce_decrypt_block ${CMAKE_BUILTIN_BUILD_DIR}/aesce.c.o
}

component_test_ctr_drbg_aes_256_sha_512 () {
    msg "build: full + MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_512 (ASan build)"
    scripts/config.py full
    scripts/config.py unset MBEDTLS_MEMORY_BUFFER_ALLOC_C
    scripts/config.py set MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_512
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: full + MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_512 (ASan build)"
    make test
}

component_test_ctr_drbg_aes_256_sha_256 () {
    msg "build: full + MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_256 (ASan build)"
    scripts/config.py full
    scripts/config.py unset MBEDTLS_MEMORY_BUFFER_ALLOC_C
    scripts/config.py set MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_256
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: full + MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_256 (ASan build)"
    make test
}

component_test_ctr_drbg_aes_128_sha_512 () {
    msg "build: full + set MBEDTLS_PSA_CRYPTO_RNG_STRENGTH 128 (ASan build)"
    scripts/config.py full
    scripts/config.py unset MBEDTLS_MEMORY_BUFFER_ALLOC_C
    scripts/config.py set MBEDTLS_PSA_CRYPTO_RNG_STRENGTH 128
    scripts/config.py set MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_512
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: full + set MBEDTLS_PSA_CRYPTO_RNG_STRENGTH 128 (ASan build)"
    make test
}

component_test_ctr_drbg_aes_128_sha_256 () {
    msg "build: full + set MBEDTLS_PSA_CRYPTO_RNG_STRENGTH 128 + MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_256 (ASan build)"
    scripts/config.py full
    scripts/config.py unset MBEDTLS_MEMORY_BUFFER_ALLOC_C
    scripts/config.py set MBEDTLS_PSA_CRYPTO_RNG_STRENGTH 128
    scripts/config.py set MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_256
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: full + set MBEDTLS_PSA_CRYPTO_RNG_STRENGTH 128 + MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_256 (ASan build)"
    make test
}

component_test_full_static_keystore () {
    msg "build: full config - MBEDTLS_PSA_KEY_STORE_DYNAMIC"
    scripts/config.py full
    scripts/config.py unset MBEDTLS_PSA_KEY_STORE_DYNAMIC
    CFLAGS="$ASAN_CFLAGS -Os" LDFLAGS="$ASAN_CFLAGS" cmake .
    cmake --build .
    msg "test: full config - MBEDTLS_PSA_KEY_STORE_DYNAMIC"
    ctest
}

component_test_psa_crypto_drivers () {
    # Test dispatch to drivers and fallbacks with
    # test_suite_psa_crypto_driver_wrappers test suite. The test drivers that
    # are wrappers around the builtin drivers are activated by
    # PSA_CRYPTO_DRIVER_TEST.
    #
    # For the time being, some test cases in test_suite_block_cipher and
    # test_suite_md.psa rely on this component to be run at least once by the
    # CI. This should disappear as we progress the 4.x work. See
    # config_adjust_test_accelerators.h for more information.
    msg "build: full + test drivers dispatching to builtins"
    scripts/config.py full
    loc_cflags="-DPSA_CRYPTO_DRIVER_TEST -DMBEDTLS_CONFIG_ADJUST_TEST_ACCELERATORS"
    loc_cflags="${loc_cflags} -I../framework/tests/include -I${MBEDTLS_ROOT_DIR}/include"

    CC=$ASAN_CC CFLAGS="${loc_cflags}" cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: full + test drivers dispatching to builtins"
    make test
}

component_build_psa_config_file () {
    msg "build: make with TF_PSA_CRYPTO_CONFIG_FILE" # ~40s
    cp "$CRYPTO_CONFIG_H" psa_test_config.h
    echo '#error "TF_PSA_CRYPTO_CONFIG_FILE is not working"' >"$CRYPTO_CONFIG_H"
    $MAKE_COMMAND CFLAGS="-I '$PWD' -DTF_PSA_CRYPTO_CONFIG_FILE='\"psa_test_config.h\"'"
    # Make sure this feature is enabled. We'll disable it in the next phase.
    programs/test/query_compile_time_config PSA_WANT_ALG_CMAC
    $MAKE_COMMAND clean

    msg "build: make with TF_PSA_CRYPTO_CONFIG_FILE + TF_PSA_CRYPTO_USER_CONFIG_FILE" # ~40s
    # In the user config, disable one feature and its dependencies, which will
    # reflect on the mbedtls configuration so we can query it with
    # query_compile_time_config.
    echo '#undef PSA_WANT_ALG_CMAC' >psa_user_config.h
    echo '#undef PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128' >> psa_user_config.h
    $MAKE_COMMAND CFLAGS="-I '$PWD' -DTF_PSA_CRYPTO_CONFIG_FILE='\"psa_test_config.h\"' -DTF_PSA_CRYPTO_USER_CONFIG_FILE='\"psa_user_config.h\"'"
    not programs/test/query_compile_time_config PSA_WANT_ALG_CMAC

    rm -f psa_test_config.h psa_user_config.h
}

component_build_psa_alt_headers () {
    msg "build: make with PSA alt headers" # ~20s

    PSA_ALT_HDRS="$PWD/tests/include/alt-dummy"
    mkdir -p "$PSA_ALT_HDRS/psa"
    # Generate alternative versions of the substitutable headers with the
    # same content except different include guards.
    sed -E 's/^(# *(define|ifndef) +[A-Za-z0-9_]+)_H\b/\1_ALT_H/' \
        tf-psa-crypto/include/psa/crypto_platform.h \
        > "$PSA_ALT_HDRS/psa/crypto_platform_alt.h"

    sed -E 's/^(# *(define|ifndef) +[A-Za-z0-9_]+)_H\b/\1_ALT_H/' \
        tf-psa-crypto/include/psa/crypto_struct.h \
        > "$PSA_ALT_HDRS/psa/crypto_struct_alt.h"

    # Build the library and some programs.
    CFLAGS="-I$PSA_ALT_HDRS -DMBEDTLS_PSA_CRYPTO_PLATFORM_FILE='\"psa/crypto_platform_alt.h\"' -DMBEDTLS_PSA_CRYPTO_STRUCT_FILE='\"psa/crypto_struct_alt.h\"'" cmake -D CMAKE_BUILD_TYPE:String=Release .
    cmake --build . --target lib
    cmake --build . --target programs

    # Check that we're getting the alternative include guards and not the
    # original include guards.
    programs/test/query_included_headers | grep -x PSA_CRYPTO_PLATFORM_ALT_H
    programs/test/query_included_headers | grep -x PSA_CRYPTO_STRUCT_ALT_H
    programs/test/query_included_headers | not grep -x PSA_CRYPTO_PLATFORM_H
    programs/test/query_included_headers | not grep -x PSA_CRYPTO_STRUCT_H

    # Explicitly clean up generated alt headers
    rm -f "$PSA_ALT_HDRS/psa/crypto_platform_alt.h" "$PSA_ALT_HDRS/psa/crypto_struct_alt.h"
}

component_test_min_mpi_window_size () {
    msg "build: Default + MBEDTLS_MPI_WINDOW_SIZE=1 (ASan build)" # ~ 10s
    scripts/config.py set MBEDTLS_MPI_WINDOW_SIZE 1
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: MBEDTLS_MPI_WINDOW_SIZE=1 - main suites (inc. selftests) (ASan build)" # ~ 10s
    make test
}

component_test_xts () {
    # Component dedicated to run XTS unit test cases while XTS is not
    # supported through the PSA API.
    msg "build: Default + MBEDTLS_CIPHER_MODE_XTS"

    cat <<'EOF' >psa_user_config.h
#define MBEDTLS_CIPHER_MODE_XTS
#define TF_PSA_CRYPTO_CONFIG_CHECK_BYPASS
EOF
    cmake -DTF_PSA_CRYPTO_USER_CONFIG_FILE="psa_user_config.h"
    make

    rm -f psa_user_config.h

    msg "test: Default + MBEDTLS_CIPHER_MODE_XTS"
    make test
}
