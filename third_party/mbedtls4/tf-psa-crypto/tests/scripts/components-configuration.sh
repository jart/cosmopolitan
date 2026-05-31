# components-configuration.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

## test_with_valgrind tests/suites/SUITE.data [...]
## Run the specified test suite(s) with Valgrind.
test_with_valgrind () {
    for data_file in "$@"; do
        suite="${data_file##*/}"; suite="${suite%.data}"
        exe="$OUT_OF_SOURCE_DIR/tests/$suite"
        log_file="$OUT_OF_SOURCE_DIR/tests/MemoryChecker.$suite.log"
        make -C "$OUT_OF_SOURCE_DIR" "$suite"
        valgrind -q --tool=memcheck --track-origins=yes --log-file="$log_file" "$exe"
        not grep . -- "$log_file"
    done
}

## Run a small set of dedicated constant-time tests with Valgrind.
## Exclude very slow suites.
## Exclude suites that contain some constant-time tests, but whose focus
## isn't on constant-time tests.
test_with_valgrind_constant_time () {
    # additional valgrind testing on top of non-instrumented testing.
    if [[ $MBEDTLS_TEST_CONFIGURATION != *valgrind_cf* ]]; then
        declare MBEDTLS_TEST_CONFIGURATION="${MBEDTLS_TEST_CONFIGURATION}+valgrind_cf"
    fi
    declare GLOBIGNORE="tests/suites/test_suite_constant_time_hmac.data"
    test_with_valgrind tests/suites/*constant_time*.data
}

################################################################
#### Configuration Testing
################################################################

component_tf_psa_crypto_test_default_out_of_box () {
    msg "build: default config (out-of-box)" # ~1min
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_BUILD_TYPE:String=Check "$TF_PSA_CRYPTO_ROOT_DIR"
    make
    # Disable fancy stuff
    unset MBEDTLS_TEST_OUTCOME_FILE

    msg "test: main suites, default config (out-of-box)" # ~10s
    make test
}

component_tf_psa_crypto_test_default_gcc_asan () {
    msg "build: gcc, ASan" # ~ 1 min 50s
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE:String=Asan "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: main suites (inc. selftests) (ASan build)" # ~ 50s
    make test
}

component_tf_psa_crypto_test_default_gcc_asan_new_bignum () {
    msg "build: gcc, ASan" # ~ 1 min 50s
    scripts/config.py set MBEDTLS_ECP_WITH_MPI_UINT
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE:String=Asan "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: main suites (inc. selftests) (ASan build)" # ~ 50s
    make test
}

component_tf_psa_crypto_test_full_gcc_asan () {
    msg "build: full config, gcc, ASan"
    scripts/config.py full
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE:String=Asan "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: main suites (inc. selftests) (full config, ASan build)"
    make test
}

component_tf_psa_crypto_test_full_gcc_asan_new_bignum () {
    msg "build: full config, gcc, ASan"
    scripts/config.py full
    scripts/config.py set MBEDTLS_ECP_WITH_MPI_UINT
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE:String=Asan "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: main suites (inc. selftests) (full config, new bignum, ASan)"
    make test
}

component_tf_psa_crypto_test_full_clang () {
    msg "build: full config, clang" # ~ 50s
    scripts/config.py full
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang -DCMAKE_BUILD_TYPE:String=Release -DTEST_CPP=1 "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: main suites (full config, clang)" # ~ 5s
    make test
}

component_tf_psa_crypto_test_default_no_deprecated () {
    # Test that removing the deprecated features from the default
    # configuration leaves something consistent.
    msg "build: default + MBEDTLS_DEPRECATED_REMOVED" # ~ 30s
    scripts/config.py set MBEDTLS_DEPRECATED_REMOVED

    cd $OUT_OF_SOURCE_DIR
    cmake "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: default + MBEDTLS_DEPRECATED_REMOVED" # ~ 5s
    make test
}

component_tf_psa_crypto_build_tfm () {
    # TF-M configuration needs a TF-M platform.
    cp configs/ext/crypto_config_profile_medium.h "$CRYPTO_CONFIG_H"

    cd $OUT_OF_SOURCE_DIR
    msg "build: TF-M config, clang, armv7-m thumb2"
    cmake -DCMAKE_C_COMPILER=clang \
        -DCMAKE_C_FLAGS="--target=arm-linux-gnueabihf -march=armv7-m -mthumb -Os -Werror -Wasm-operand-widths -Wunused -I../framework/tests/include/spe" \
        -DCMAKE_C_COMPILER_WORKS=TRUE \
        -DENABLE_TESTING=OFF \
        -DENABLE_PROGRAMS=OFF \
        "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    rm -rf *
    msg "build: TF-M config, gcc native build"
    cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_C_FLAGS="-Os -I../framework/tests/include/spe" "$TF_PSA_CRYPTO_ROOT_DIR"
    make
}

component_tf_psa_crypto_test_malloc_0_null () {
    msg "build: malloc(0) returns NULL (ASan+UBSan build)"
    scripts/config.py full
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_BUILD_TYPE:String=Asan -DTF_PSA_CRYPTO_USER_CONFIG_FILE="$TF_PSA_CRYPTO_ROOT_DIR/tests/configs/user-config-malloc-0-null.h" "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: malloc(0) returns NULL (ASan+UBSan build)"
    make test
}

component_tf_psa_crypto_test_memory_buffer_allocator_backtrace () {
    msg "build: default config with memory buffer allocator and backtrace enabled"
    scripts/config.py set MBEDTLS_MEMORY_BUFFER_ALLOC_C
    scripts/config.py set MBEDTLS_PLATFORM_MEMORY
    scripts/config.py set MBEDTLS_MEMORY_BACKTRACE
    scripts/config.py set MBEDTLS_MEMORY_DEBUG
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_BUILD_TYPE:String=Release "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: MBEDTLS_MEMORY_BUFFER_ALLOC_C and MBEDTLS_MEMORY_BACKTRACE"
    make test
}

component_tf_psa_crypto_test_memory_buffer_allocator () {
    msg "build: default config with memory buffer allocator"
    scripts/config.py set MBEDTLS_MEMORY_BUFFER_ALLOC_C
    scripts/config.py set MBEDTLS_PLATFORM_MEMORY
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_BUILD_TYPE:String=Release "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: MBEDTLS_MEMORY_BUFFER_ALLOC_C"
    make test
}

support_test_chacha20_neon_variations () {
    if "$CC" --version 2>/dev/null | grep -q "clang"; then
        # if using clang, ensure version 7 or later to ensure we get support for "+aes",
        # which seems to be needed due to PSA_WANT_KEY_TYPE_AES
        clang_major_ver=$("$CC" --version | head -n1 | sed -E 's/.*clang version ([0-9]+)\.[0-9]+.*/\1/')
        if [ "$clang_major_ver" -lt 7 ]; then
            false
            return
        fi
    fi

    case $(uname -m) in
        arm64|aarch64) true;;
        *) false;;
    esac
}

component_test_chacha20_neon_variations () {
    msg "ChaCha20 Neon scalar and multiblock variations"

    scripts/config.py set PSA_WANT_KEY_TYPE_CHACHA20

    cd $OUT_OF_SOURCE_DIR

    for x in 0 1 2 3 4 5 6; do
        msg "multiblock = $x"
        cmake -DCMAKE_BUILD_TYPE:String=Release -DCMAKE_C_FLAGS="-DMBEDTLS_CHACHA20_NEON_MULTIBLOCK=$x" "$TF_PSA_CRYPTO_ROOT_DIR"
        make -C tests test_suite_chacha20
        ./tests/test_suite_chacha20
    done
}

component_test_default_valgrind_cf () {
    msg "build: default config, constant flow with Valgrind"
    scripts/config.py set MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND

    cd "$OUT_OF_SOURCE_DIR"
    cmake -DCMAKE_BUILD_TYPE:String=Release "$TF_PSA_CRYPTO_ROOT_DIR"
    make tfpsacrypto
    cd "$TF_PSA_CRYPTO_ROOT_DIR"

    msg "test: default config, constant flow with Valgrind, selected suites"
    test_with_valgrind_constant_time tests/suites/*constant_time*.data
}

component_test_psa_assume_exclusive_buffers_valgrind_cf () {
    msg "build: full config + MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS, constant flow with Valgrind"
    scripts/config.py full
    scripts/config.py set MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS
    scripts/config.py set MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND

    cd "$OUT_OF_SOURCE_DIR"
    cmake -DCMAKE_BUILD_TYPE:String=Release "$TF_PSA_CRYPTO_ROOT_DIR"
    make tfpsacrypto
    cd "$TF_PSA_CRYPTO_ROOT_DIR"

    msg "test: full config + MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS, constant flow with Valgrind, selected suites"
    test_with_valgrind_constant_time tests/suites/*constant_time*.data
}

common_tf_psa_crypto_full_pkparse_pkwrite () {
    PK_PARSE=$1
    PK_WRITE=$2

    message="full without"
    if [ $PK_PARSE -eq 0 ]; then
        message="$message pkparse"
    fi
    if [ $PK_WRITE -eq 0 ]; then
        message="$message pkwrite"
    fi

    msg "build: $message"

    scripts/config.py full
    if [ $PK_PARSE -eq 0 ]; then
        scripts/config.py unset MBEDTLS_PK_PARSE_C
    fi
    if [ $PK_WRITE -eq 0 ]; then
        scripts/config.py unset MBEDTLS_PK_WRITE_C
    fi

    cd "$OUT_OF_SOURCE_DIR"
    cmake -DCMAKE_BUILD_TYPE:String=Asan "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    # Ensure that PK_PARSE_C and/or PK_PARSE_C are correctly disabled or enabled
    # depending on the current scenario.
    if [ $PK_PARSE -eq 0 ]; then
        not grep mbedtls_pk_parse_key "core/libtfpsacrypto.a"
    else
        grep mbedtls_pk_parse_key "core/libtfpsacrypto.a"
    fi
    if [ $PK_WRITE -eq 0 ]; then
        not grep mbedtls_pk_write_key_der "core/libtfpsacrypto.a"
    else
        grep mbedtls_pk_write_key_der "core/libtfpsacrypto.a"
    fi

    msg "test: $message"
    make test
}

component_tf_psa_crypto_full_no_pkparse_pkwrite () {
    common_tf_psa_crypto_full_pkparse_pkwrite 0 0
}

component_tf_psa_crypto_full_no_pkwrite () {
    common_tf_psa_crypto_full_pkparse_pkwrite 1 0
}

component_tf_psa_crypto_full_no_pkparse () {
    common_tf_psa_crypto_full_pkparse_pkwrite 0 1
}
