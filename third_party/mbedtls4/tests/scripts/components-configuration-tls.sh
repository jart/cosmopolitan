# components-configuration-tls.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Configuration Testing - TLS
################################################################

component_test_config_suite_b () {
    msg "build: configs/config-suite-b.h"
    MBEDTLS_CONFIG="configs/config-suite-b.h"
    CRYPTO_CONFIG="configs/crypto-config-suite-b.h"
    CC=$ASAN_CC cmake -DMBEDTLS_CONFIG_FILE="$MBEDTLS_CONFIG" -DTF_PSA_CRYPTO_CONFIG_FILE="$CRYPTO_CONFIG" -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: configs/config-suite-b.h - unit tests"
    make test

    msg "test: configs/config-suite-b.h - compat.sh"
    tests/compat.sh -m tls12 -f 'ECDHE_ECDSA.*AES.*GCM' -p mbedTLS

    msg "build: configs/config-suite-b.h + DEBUG"
    MBEDTLS_TEST_CONFIGURATION="$MBEDTLS_TEST_CONFIGURATION+DEBUG"
    make clean
    scripts/config.py -f "$MBEDTLS_CONFIG" set MBEDTLS_DEBUG_C
    scripts/config.py -f "$MBEDTLS_CONFIG" set MBEDTLS_ERROR_C
    make ssl-opt

    msg "test: configs/config-suite-b.h + DEBUG - ssl-opt.sh"
    tests/ssl-opt.sh
}

component_test_no_renegotiation () {
    msg "build: Default + !MBEDTLS_SSL_RENEGOTIATION (ASan build)" # ~ 6 min
    scripts/config.py unset MBEDTLS_SSL_RENEGOTIATION
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: !MBEDTLS_SSL_RENEGOTIATION - main suites (inc. selftests) (ASan build)" # ~ 50s
    make test

    msg "test: !MBEDTLS_SSL_RENEGOTIATION - ssl-opt.sh (ASan build)" # ~ 6 min
    tests/ssl-opt.sh
}

component_test_tls1_2_default_stream_cipher_only () {
    msg "build: default with only stream cipher use psa"

    # Disable AEAD (controlled by the presence of one of GCM_C, CCM_C, CHACHAPOLY_C)
    scripts/config.py unset PSA_WANT_ALG_CCM
    scripts/config.py unset PSA_WANT_ALG_CCM_STAR_NO_TAG
    scripts/config.py unset PSA_WANT_ALG_GCM
    scripts/config.py unset PSA_WANT_ALG_CHACHA20_POLY1305
    #Disable TLS 1.3 (as no AEAD)
    scripts/config.py unset MBEDTLS_SSL_PROTO_TLS1_3
    # Disable CBC. Note: When implemented, PSA_WANT_ALG_CBC_MAC will also need to be unset here to fully disable CBC
    scripts/config.py unset PSA_WANT_ALG_CBC_NO_PADDING
    scripts/config.py unset PSA_WANT_ALG_CBC_PKCS7
    # Disable CBC-EtM (controlled by the same as CBC-legacy plus MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    scripts/config.py unset MBEDTLS_SSL_ENCRYPT_THEN_MAC
    # Enable stream (currently that's just the NULL pseudo-cipher (controlled by MBEDTLS_SSL_NULL_CIPHERSUITES))
    scripts/config.py set MBEDTLS_SSL_NULL_CIPHERSUITES
    # Modules that depend on AEAD
    scripts/config.py unset MBEDTLS_SSL_CONTEXT_SERIALIZATION
    scripts/config.py unset MBEDTLS_SSL_TICKET_C

    $MAKE_COMMAND

    msg "test: default with only stream cipher use psa"
    $MAKE_COMMAND test

    # Not running ssl-opt.sh because most tests require a non-NULL ciphersuite.
}

component_test_tls1_2_default_cbc_legacy_cipher_only () {
    msg "build: default with only CBC-legacy cipher use psa"

    # Disable AEAD (controlled by the presence of one of GCM_C, CCM_C, CHACHAPOLY_C)
    scripts/config.py unset PSA_WANT_ALG_CCM
    scripts/config.py unset PSA_WANT_ALG_CCM_STAR_NO_TAG
    scripts/config.py unset PSA_WANT_ALG_GCM
    scripts/config.py unset PSA_WANT_ALG_CHACHA20_POLY1305
    #Disable TLS 1.3 (as no AEAD)
    scripts/config.py unset MBEDTLS_SSL_PROTO_TLS1_3
    # Enable CBC-legacy
    scripts/config.py set PSA_WANT_ALG_CBC_NO_PADDING
    # Disable CBC-EtM (controlled by the same as CBC-legacy plus MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    scripts/config.py unset MBEDTLS_SSL_ENCRYPT_THEN_MAC
    # Disable stream (currently that's just the NULL pseudo-cipher (controlled by MBEDTLS_SSL_NULL_CIPHERSUITES))
    scripts/config.py unset MBEDTLS_SSL_NULL_CIPHERSUITES
    # Modules that depend on AEAD
    scripts/config.py unset MBEDTLS_SSL_CONTEXT_SERIALIZATION
    scripts/config.py unset MBEDTLS_SSL_TICKET_C

    $MAKE_COMMAND

    msg "test: default with only CBC-legacy cipher use psa"
    $MAKE_COMMAND test

    msg "test: default with only CBC-legacy cipher use psa - ssl-opt.sh (subset)"
    tests/ssl-opt.sh -f "TLS 1.2"
}

component_test_tls1_2_default_cbc_legacy_cbc_etm_cipher_only () {
    msg "build: default with only CBC-legacy and CBC-EtM ciphers use psa"

    # Disable AEAD (controlled by the presence of one of GCM_C, CCM_C, CHACHAPOLY_C)
    scripts/config.py unset PSA_WANT_ALG_CCM
    scripts/config.py unset PSA_WANT_ALG_CCM_STAR_NO_TAG
    scripts/config.py unset PSA_WANT_ALG_GCM
    scripts/config.py unset PSA_WANT_ALG_CHACHA20_POLY1305
    #Disable TLS 1.3 (as no AEAD)
    scripts/config.py unset MBEDTLS_SSL_PROTO_TLS1_3
    # Enable CBC-legacy
    scripts/config.py set PSA_WANT_ALG_CBC_NO_PADDING
    # Enable CBC-EtM (controlled by the same as CBC-legacy plus MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    scripts/config.py set MBEDTLS_SSL_ENCRYPT_THEN_MAC
    # Disable stream (currently that's just the NULL pseudo-cipher (controlled by MBEDTLS_SSL_NULL_CIPHERSUITES))
    scripts/config.py unset MBEDTLS_SSL_NULL_CIPHERSUITES
    # Modules that depend on AEAD
    scripts/config.py unset MBEDTLS_SSL_CONTEXT_SERIALIZATION
    scripts/config.py unset MBEDTLS_SSL_TICKET_C

    $MAKE_COMMAND

    msg "test: default with only CBC-legacy and CBC-EtM ciphers use psa"
    $MAKE_COMMAND test

    msg "test: default with only CBC-legacy and CBC-EtM ciphers use psa - ssl-opt.sh (subset)"
    tests/ssl-opt.sh -f "TLS 1.2"
}

component_test_config_thread () {
    msg "build: configs/config-thread.h"
    MBEDTLS_CONFIG="configs/config-thread.h"
    CRYPTO_CONFIG="configs/crypto-config-thread.h"
    CC=$ASAN_CC cmake -DMBEDTLS_CONFIG_FILE="$MBEDTLS_CONFIG" -DTF_PSA_CRYPTO_CONFIG_FILE="$CRYPTO_CONFIG" -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: configs/config-thread.h - unit tests"
    make test

    msg "test: configs/config-thread.h - ssl-opt.sh"
    tests/ssl-opt.sh -f 'ECJPAKE.*nolog'
}

component_test_tls1_2_ccm_psk () {
    msg "build: configs/config-ccm-psk-tls1_2.h"
    MBEDTLS_CONFIG="configs/config-ccm-psk-tls1_2.h"
    CRYPTO_CONFIG="configs/crypto-config-ccm-psk-tls1_2.h"
    CC=$ASAN_CC cmake -DMBEDTLS_CONFIG_FILE="$MBEDTLS_CONFIG" -DTF_PSA_CRYPTO_CONFIG_FILE="$CRYPTO_CONFIG" -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: configs/config-ccm-psk-tls1_2.h - unit tests"
    make test

    msg "test: configs/config-ccm-psk-tls1_2.h - compat.sh"
    tests/compat.sh -m tls12 -f '^TLS_PSK_WITH_AES_..._CCM_8'
}

component_test_tls1_2_ccm_psk_dtls () {
    msg "build: configs/config-ccm-psk-dtls1_2.h"
    MBEDTLS_CONFIG="configs/config-ccm-psk-dtls1_2.h"
    CRYPTO_CONFIG="configs/crypto-config-ccm-psk-tls1_2.h"
    CC=$ASAN_CC cmake -DMBEDTLS_CONFIG_FILE="$MBEDTLS_CONFIG" -DTF_PSA_CRYPTO_CONFIG_FILE="$CRYPTO_CONFIG" -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: configs/config-ccm-psk-dtls1_2.h - unit tests"
    make test

    msg "test: configs/config-ccm-psk-dtls1_2.h - compat.sh"
    tests/compat.sh -m dtls12 -f '^TLS_PSK_WITH_AES_..._CCM_8'

    msg "build: configs/config-ccm-psk-dtls1_2.h + DEBUG"
    MBEDTLS_TEST_CONFIGURATION="$MBEDTLS_TEST_CONFIGURATION+DEBUG"
    make clean
    scripts/config.py -f "$MBEDTLS_CONFIG" set MBEDTLS_DEBUG_C
    scripts/config.py -f "$MBEDTLS_CONFIG" set MBEDTLS_ERROR_C
    make ssl-opt

    msg "test: configs/config-ccm-psk-dtls1_2.h + DEBUG - ssl-opt.sh"
    tests/ssl-opt.sh
}

component_test_small_ssl_out_content_len () {
    msg "build: small SSL_OUT_CONTENT_LEN (ASan build)"
    scripts/config.py set MBEDTLS_SSL_IN_CONTENT_LEN 16384
    scripts/config.py set MBEDTLS_SSL_OUT_CONTENT_LEN 4096
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: small SSL_OUT_CONTENT_LEN - ssl-opt.sh MFL and large packet tests"
    tests/ssl-opt.sh -f "Max fragment\|Large packet"
}

component_test_small_ssl_in_content_len () {
    msg "build: small SSL_IN_CONTENT_LEN (ASan build)"
    scripts/config.py set MBEDTLS_SSL_IN_CONTENT_LEN 4096
    scripts/config.py set MBEDTLS_SSL_OUT_CONTENT_LEN 16384
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: small SSL_IN_CONTENT_LEN - ssl-opt.sh MFL tests"
    tests/ssl-opt.sh -f "Max fragment"
}

component_test_small_ssl_dtls_max_buffering () {
    msg "build: small MBEDTLS_SSL_DTLS_MAX_BUFFERING #0"
    scripts/config.py set MBEDTLS_SSL_DTLS_MAX_BUFFERING 1000
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: small MBEDTLS_SSL_DTLS_MAX_BUFFERING #0 - ssl-opt.sh specific reordering test"
    tests/ssl-opt.sh -f "DTLS reordering: Buffer out-of-order hs msg before reassembling next, free buffered msg"
}

component_test_small_mbedtls_ssl_dtls_max_buffering () {
    msg "build: small MBEDTLS_SSL_DTLS_MAX_BUFFERING #1"
    scripts/config.py set MBEDTLS_SSL_DTLS_MAX_BUFFERING 190
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: small MBEDTLS_SSL_DTLS_MAX_BUFFERING #1 - ssl-opt.sh specific reordering test"
    tests/ssl-opt.sh -f "DTLS reordering: Buffer encrypted Finished message, drop for fragmented NewSessionTicket"
}

# Common helper for component_full_without_ecdhe_ecdsa(),
# component_full_without_ecdhe_ecdsa_and_tls13() and component_full_without_tls13 which:
# - starts from the "full" configuration minus the list of symbols passed in
#   as 1st parameter
# - build
# - test only TLS (i.e. test_suite_tls and ssl-opt)
build_full_minus_something_and_test_tls () {
    symbols_to_disable="$1"
    filter="${2-.}"

    msg "build: full minus something, test TLS"

    scripts/config.py full
    for sym in $symbols_to_disable; do
        echo "Disabling $sym"
        scripts/config.py unset $sym
    done

    $MAKE_COMMAND

    msg "test: full minus something, test TLS"
    ( cd tests; ./test_suite_ssl )

    msg "ssl-opt: full minus something, test TLS"
    tests/ssl-opt.sh -f "$filter"
}

#These tests are temporarily disabled due to an unknown dependency of static ecdh as described in https://github.com/Mbed-TLS/mbedtls/issues/10385.
component_full_without_ecdhe_ecdsa () {
    build_full_minus_something_and_test_tls "MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED" 'psk\|PSK\|1\.3'
}

component_full_without_ecdhe_ecdsa_and_tls13 () {
    build_full_minus_something_and_test_tls "MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
                                             MBEDTLS_SSL_PROTO_TLS1_3"
}

component_full_without_tls13 () {
    build_full_minus_something_and_test_tls "MBEDTLS_SSL_PROTO_TLS1_3"
}

component_build_no_ssl_srv () {
    msg "build: full config except SSL server, make, gcc" # ~ 30s
    scripts/config.py full
    scripts/config.py unset MBEDTLS_SSL_SRV_C
    $MAKE_COMMAND CC=gcc CFLAGS='-Werror -Wall -Wextra -O1 -Wmissing-prototypes'
}

component_build_no_ssl_cli () {
    msg "build: full config except SSL client, make, gcc" # ~ 30s
    scripts/config.py full
    scripts/config.py unset MBEDTLS_SSL_CLI_C
    $MAKE_COMMAND CC=gcc CFLAGS='-Werror -Wall -Wextra -O1 -Wmissing-prototypes'
}

component_test_no_max_fragment_length () {
    # Run max fragment length tests with MFL disabled
    msg "build: default config except MFL extension (ASan build)" # ~ 30s
    scripts/config.py unset MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: ssl-opt.sh, MFL-related tests"
    tests/ssl-opt.sh -f "Max fragment length"
}

component_test_asan_remove_peer_certificate () {
    msg "build: default config with MBEDTLS_SSL_KEEP_PEER_CERTIFICATE disabled (ASan build)"
    scripts/config.py unset MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
    scripts/config.py unset MBEDTLS_SSL_PROTO_TLS1_3
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: !MBEDTLS_SSL_KEEP_PEER_CERTIFICATE"
    make test

    msg "test: ssl-opt.sh, !MBEDTLS_SSL_KEEP_PEER_CERTIFICATE"
    tests/ssl-opt.sh

    msg "test: compat.sh, !MBEDTLS_SSL_KEEP_PEER_CERTIFICATE"
    tests/compat.sh

    msg "test: context-info.sh, !MBEDTLS_SSL_KEEP_PEER_CERTIFICATE"
    tests/context-info.sh
}

component_test_no_max_fragment_length_small_ssl_out_content_len () {
    msg "build: no MFL extension, small SSL_OUT_CONTENT_LEN (ASan build)"
    scripts/config.py unset MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
    scripts/config.py set MBEDTLS_SSL_IN_CONTENT_LEN 16384
    scripts/config.py set MBEDTLS_SSL_OUT_CONTENT_LEN 4096
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: MFL tests (disabled MFL extension case) & large packet tests"
    tests/ssl-opt.sh -f "Max fragment length\|Large buffer"

    msg "test: context-info.sh (disabled MFL extension case)"
    tests/context-info.sh
}

component_test_variable_ssl_in_out_buffer_len () {
    msg "build: MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH enabled (ASan build)"
    scripts/config.py set MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH enabled"
    make test

    msg "test: ssl-opt.sh, MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH enabled"
    tests/ssl-opt.sh

    msg "test: compat.sh, MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH enabled"
    tests/compat.sh
}

component_test_ssl_alloc_buffer_and_mfl () {
    msg "build: default config with memory buffer allocator and MFL extension"
    scripts/config.py set MBEDTLS_MEMORY_BUFFER_ALLOC_C
    scripts/config.py set MBEDTLS_PLATFORM_MEMORY
    scripts/config.py set MBEDTLS_MEMORY_DEBUG
    scripts/config.py set MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
    scripts/config.py set MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH
    cmake -DCMAKE_BUILD_TYPE:String=Release .
    make

    msg "test: MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH, MBEDTLS_MEMORY_BUFFER_ALLOC_C, MBEDTLS_MEMORY_DEBUG and MBEDTLS_SSL_MAX_FRAGMENT_LENGTH"
    make test

    msg "test: MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH, MBEDTLS_MEMORY_BUFFER_ALLOC_C, MBEDTLS_MEMORY_DEBUG and MBEDTLS_SSL_MAX_FRAGMENT_LENGTH"
    tests/ssl-opt.sh -f "Handshake memory usage"
}

component_test_when_no_ciphersuites_have_mac () {
    msg "build: when no ciphersuites have MAC"
    scripts/config.py unset PSA_WANT_ALG_CBC_NO_PADDING
    scripts/config.py unset PSA_WANT_ALG_CBC_PKCS7
    scripts/config.py unset PSA_WANT_ALG_CMAC
    scripts/config.py unset PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128

    scripts/config.py unset MBEDTLS_SSL_NULL_CIPHERSUITES

    $MAKE_COMMAND

    msg "test: !MBEDTLS_SSL_SOME_SUITES_USE_MAC"
    $MAKE_COMMAND test

    msg "test ssl-opt.sh: !MBEDTLS_SSL_SOME_SUITES_USE_MAC"
    tests/ssl-opt.sh -f 'Default\|EtM' -e 'without EtM'
}

component_test_tls12_only () {
    msg "build: default config without MBEDTLS_SSL_PROTO_TLS1_3, cmake, gcc, ASan"
    scripts/config.py unset MBEDTLS_SSL_PROTO_TLS1_3
    CC=gcc cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: main suites (inc. selftests) (ASan build)"
    make test

    msg "test: ssl-opt.sh (ASan build)"
    tests/ssl-opt.sh

    msg "test: compat.sh (ASan build)"
    tests/compat.sh
}

component_test_tls13_only () {
    msg "build: default config without MBEDTLS_SSL_PROTO_TLS1_2"
    scripts/config.py set MBEDTLS_SSL_EARLY_DATA
    scripts/config.py set MBEDTLS_SSL_RECORD_SIZE_LIMIT

    scripts/config.py set MBEDTLS_TEST_HOOKS
    $MAKE_COMMAND CFLAGS="'-DMBEDTLS_USER_CONFIG_FILE=\"../tests/configs/tls13-only.h\"'"

    msg "test: TLS 1.3 only, all key exchange modes enabled"
    $MAKE_COMMAND test

    msg "ssl-opt.sh: TLS 1.3 only, all key exchange modes enabled"
    tests/ssl-opt.sh
}

component_test_tls13_only_psk () {
    msg "build: TLS 1.3 only from default, only PSK key exchange mode"
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
    scripts/config.py unset MBEDTLS_X509_CRT_PARSE_C
    scripts/config.py unset MBEDTLS_X509_RSASSA_PSS_SUPPORT
    scripts/config.py unset MBEDTLS_SSL_SERVER_NAME_INDICATION
    scripts/config.py unset MBEDTLS_PKCS7_C
    scripts/config.py set   MBEDTLS_SSL_EARLY_DATA

    scripts/config.py set MBEDTLS_TEST_HOOKS
    scripts/config.py unset PSA_WANT_ALG_ECDH
    scripts/config.py unset PSA_WANT_ALG_ECDSA
    scripts/config.py unset PSA_WANT_ALG_DETERMINISTIC_ECDSA
    scripts/config.py unset PSA_WANT_ALG_RSA_OAEP
    scripts/config.py unset PSA_WANT_ALG_RSA_PSS
    scripts/config.py unset PSA_WANT_ALG_FFDH
    scripts/config.py unset PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY
    scripts/config.py unset PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC
    scripts/config.py unset PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT
    scripts/config.py unset PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT
    scripts/config.py unset PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE
    scripts/config.py unset PSA_WANT_DH_RFC7919_2048
    scripts/config.py unset PSA_WANT_DH_RFC7919_3072
    scripts/config.py unset PSA_WANT_DH_RFC7919_4096
    scripts/config.py unset PSA_WANT_DH_RFC7919_6144
    scripts/config.py unset PSA_WANT_DH_RFC7919_8192

    $MAKE_COMMAND CFLAGS="'-DMBEDTLS_USER_CONFIG_FILE=\"../tests/configs/tls13-only.h\"'"

    msg "test_suite_ssl: TLS 1.3 only, only PSK key exchange mode enabled"
    cd tests; ./test_suite_ssl; cd ..

    msg "ssl-opt.sh: TLS 1.3 only, only PSK key exchange mode enabled"
    tests/ssl-opt.sh
}

component_test_tls13_only_ephemeral () {
    msg "build: TLS 1.3 only from default, only ephemeral key exchange mode"
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
    scripts/config.py unset MBEDTLS_SSL_EARLY_DATA

    scripts/config.py set MBEDTLS_TEST_HOOKS
    $MAKE_COMMAND CFLAGS="'-DMBEDTLS_USER_CONFIG_FILE=\"../tests/configs/tls13-only.h\"'"

    msg "test_suite_ssl: TLS 1.3 only, only ephemeral key exchange mode"
    cd tests; ./test_suite_ssl; cd ..

    msg "ssl-opt.sh: TLS 1.3 only, only ephemeral key exchange mode"
    tests/ssl-opt.sh
}

#These tests are temporarily disabled due to an unknown dependency of static ecdh as described in https://github.com/Mbed-TLS/mbedtls/issues/10385.
component_test_tls13_only_ephemeral_ffdh () {
    msg "build: TLS 1.3 only from default, only ephemeral ffdh key exchange mode"
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
    scripts/config.py unset MBEDTLS_SSL_EARLY_DATA

    scripts/config.py set MBEDTLS_TEST_HOOKS
    scripts/config.py unset PSA_WANT_ALG_ECDH

    $MAKE_COMMAND CFLAGS="'-DMBEDTLS_USER_CONFIG_FILE=\"../tests/configs/tls13-only.h\"'"

    msg "test_suite_ssl: TLS 1.3 only, only ephemeral ffdh key exchange mode"
    cd tests; ./test_suite_ssl; cd ..

    msg "ssl-opt.sh: TLS 1.3 only, only ephemeral ffdh key exchange mode"
    tests/ssl-opt.sh -f "ffdh"
}

component_test_tls13_only_psk_ephemeral () {
    msg "build: TLS 1.3 only from default, only PSK ephemeral key exchange mode"
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
    scripts/config.py unset MBEDTLS_X509_CRT_PARSE_C
    scripts/config.py unset MBEDTLS_X509_RSASSA_PSS_SUPPORT
    scripts/config.py unset MBEDTLS_SSL_SERVER_NAME_INDICATION
    scripts/config.py unset MBEDTLS_PKCS7_C
    scripts/config.py set   MBEDTLS_SSL_EARLY_DATA

    scripts/config.py set MBEDTLS_TEST_HOOKS
    scripts/config.py unset PSA_WANT_ALG_ECDSA
    scripts/config.py unset PSA_WANT_ALG_DETERMINISTIC_ECDSA
    scripts/config.py unset PSA_WANT_ALG_RSA_OAEP
    scripts/config.py unset PSA_WANT_ALG_RSA_PSS

    $MAKE_COMMAND CFLAGS="'-DMBEDTLS_USER_CONFIG_FILE=\"../tests/configs/tls13-only.h\"'"

    msg "test_suite_ssl: TLS 1.3 only, only PSK ephemeral key exchange mode"
    cd tests; ./test_suite_ssl; cd ..

    msg "ssl-opt.sh: TLS 1.3 only, only PSK ephemeral key exchange mode"
    tests/ssl-opt.sh
}

component_test_tls13_only_psk_ephemeral_ffdh () {
    msg "build: TLS 1.3 only from default, only PSK ephemeral ffdh key exchange mode"
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
    scripts/config.py unset MBEDTLS_X509_CRT_PARSE_C
    scripts/config.py unset MBEDTLS_X509_RSASSA_PSS_SUPPORT
    scripts/config.py unset MBEDTLS_SSL_SERVER_NAME_INDICATION
    scripts/config.py unset MBEDTLS_PKCS7_C
    scripts/config.py set   MBEDTLS_SSL_EARLY_DATA

    scripts/config.py set MBEDTLS_TEST_HOOKS
    scripts/config.py unset PSA_WANT_ALG_ECDH
    scripts/config.py unset PSA_WANT_ALG_ECDSA
    scripts/config.py unset PSA_WANT_ALG_DETERMINISTIC_ECDSA
    scripts/config.py unset PSA_WANT_ALG_RSA_OAEP
    scripts/config.py unset PSA_WANT_ALG_RSA_PSS

    $MAKE_COMMAND CFLAGS="'-DMBEDTLS_USER_CONFIG_FILE=\"../tests/configs/tls13-only.h\"'"

    msg "test_suite_ssl: TLS 1.3 only, only PSK ephemeral ffdh key exchange mode"
    cd tests; ./test_suite_ssl; cd ..

    msg "ssl-opt.sh: TLS 1.3 only, only PSK ephemeral ffdh key exchange mode"
    tests/ssl-opt.sh
}

component_test_tls13_only_psk_all () {
    msg "build: TLS 1.3 only from default, without ephemeral key exchange mode"
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
    scripts/config.py unset MBEDTLS_X509_CRT_PARSE_C
    scripts/config.py unset MBEDTLS_X509_RSASSA_PSS_SUPPORT
    scripts/config.py unset MBEDTLS_SSL_SERVER_NAME_INDICATION
    scripts/config.py unset MBEDTLS_PKCS7_C
    scripts/config.py set   MBEDTLS_SSL_EARLY_DATA

    scripts/config.py set MBEDTLS_TEST_HOOKS
    scripts/config.py unset PSA_WANT_ALG_ECDSA
    scripts/config.py unset PSA_WANT_ALG_DETERMINISTIC_ECDSA
    scripts/config.py unset PSA_WANT_ALG_RSA_OAEP
    scripts/config.py unset PSA_WANT_ALG_RSA_PSS

    $MAKE_COMMAND CFLAGS="'-DMBEDTLS_USER_CONFIG_FILE=\"../tests/configs/tls13-only.h\"'"

    msg "test_suite_ssl: TLS 1.3 only, PSK and PSK ephemeral key exchange modes"
    cd tests; ./test_suite_ssl; cd ..

    msg "ssl-opt.sh: TLS 1.3 only, PSK and PSK ephemeral key exchange modes"
    tests/ssl-opt.sh
}

component_test_tls13_only_ephemeral_all () {
    msg "build: TLS 1.3 only from default, without PSK key exchange mode"
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
    scripts/config.py set   MBEDTLS_SSL_EARLY_DATA

    scripts/config.py set MBEDTLS_TEST_HOOKS
    $MAKE_COMMAND CFLAGS="'-DMBEDTLS_USER_CONFIG_FILE=\"../tests/configs/tls13-only.h\"'"

    msg "test_suite_ssl: TLS 1.3 only, ephemeral and PSK ephemeral key exchange modes"
    cd tests; ./test_suite_ssl; cd ..

    msg "ssl-opt.sh: TLS 1.3 only, ephemeral and PSK ephemeral key exchange modes"
    tests/ssl-opt.sh
}

component_test_tls13_no_padding () {
    msg "build: default config plus early data minus padding"
    scripts/config.py set MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY 1
    scripts/config.py set MBEDTLS_SSL_EARLY_DATA
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make
    msg "test: default config plus early data minus padding"
    make test
    msg "ssl-opt.sh (TLS 1.3 no padding)"
    tests/ssl-opt.sh
}

component_test_tls13_no_compatibility_mode () {
    msg "build: default config plus early data minus middlebox compatibility mode"
    scripts/config.py unset MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
    scripts/config.py set   MBEDTLS_SSL_EARLY_DATA
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make
    msg "test: default config plus early data minus middlebox compatibility mode"
    make test
    msg "ssl-opt.sh (TLS 1.3 no compatibility mode)"
    tests/ssl-opt.sh
}

component_test_full_minus_session_tickets () {
    msg "build: full config without session tickets"
    scripts/config.py full
    scripts/config.py unset MBEDTLS_SSL_SESSION_TICKETS
    scripts/config.py unset MBEDTLS_SSL_EARLY_DATA
    CC=gcc cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make
    msg "test: full config without session tickets"
    make test
    msg "ssl-opt.sh (full config without session tickets)"
    tests/ssl-opt.sh
}

component_test_depends_py_kex () {
    msg "test/build: depends.py kex (gcc)"
    tests/scripts/depends.py kex
}


