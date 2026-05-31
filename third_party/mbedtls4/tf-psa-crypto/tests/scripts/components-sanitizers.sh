# components-sanitizers.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Sanitizer Testing
################################################################

skip_suites_without_constant_flow () {
    # Skip the test suites that don't have any constant-flow annotations.
    # This will need to be adjusted if we ever start declaring things as
    # secret from macros or functions inside framework/tests/include or framework/tests/src.
    SKIP_TEST_SUITES=$(
        git -C tests/suites grep -L TEST_CF_ 'test_suite_*.function' |
            sed 's/test_suite_//; s/\.function$//' |
            tr '\n' ,)
    export SKIP_TEST_SUITES
}

skip_all_except_given_suite () {
    # Skip all but the given test suite
    SKIP_TEST_SUITES=$(
        ls -1 tests/suites/test_suite_*.function |
        grep -v $1.function |
         sed 's/tests.suites.test_suite_//; s/\.function$//' |
        tr '\n' ,)
    export SKIP_TEST_SUITES
}

component_tf_psa_crypto_test_memsan_constant_flow_psa () {
    # This tests both (1) accesses to undefined memory, and (2) branches or
    # memory access depending on secret values. To distinguish between those:
    # - unset MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN - does the failure persist?
    # - or alternatively, change the build type to MemSanDbg, which enables
    # origin tracking and nicer stack traces (which are useful for debugging
    # anyway), and check if the origin was TEST_CF_SECRET() or something else.
    msg "build: cmake MSan (clang), full config with constant flow testing"
    scripts/config.py full
    scripts/config.py unset MBEDTLS_AESNI_C # memsan doesn't grok asm
    scripts/config.py unset MBEDTLS_HAVE_ASM
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_C_COMPILER=clang -DGEN_FILES=ON "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    $TF_PSA_CRYPTO_ROOT_DIR/scripts/config.py set MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN
    cmake -DCMAKE_C_COMPILER=clang -DGEN_FILES=OFF -DCMAKE_BUILD_TYPE:String=MemSan "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: main suites (Msan + constant flow)"
    make test
}

component_release_tf_psa_crypto_test_valgrind_constant_flow_no_asm () {
    # This tests both (1) everything that valgrind's memcheck usually checks
    # (heap buffer overflows, use of uninitialized memory, use-after-free,
    # etc.) and (2) branches or memory access depending on secret values,
    # which will be reported as uninitialized memory. To distinguish between
    # secret and actually uninitialized:
    # - unset MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND - does the failure persist?
    # - or alternatively, build with debug info and manually run the offending
    # test suite with valgrind --track-origins=yes, then check if the origin
    # was TEST_CF_SECRET() or something else.
    msg "build: cmake release GCC, full config minus MBEDTLS_HAVE_ASM with constant flow testing"
    scripts/config.py full
    scripts/config.py set MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND
    scripts/config.py unset MBEDTLS_AESNI_C
    scripts/config.py unset MBEDTLS_HAVE_ASM
    skip_suites_without_constant_flow
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_BUILD_TYPE:String=Release "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    # this only shows a summary of the results (how many of each type)
    # details are left in Testing/<date>/DynamicAnalysis.xml
    msg "test: some suites (full minus MBEDTLS_HAVE_ASM, valgrind + constant flow)"
    make memcheck
}

component_release_tf_psa_crypto_test_valgrind_constant_flow_psa () {
    # This tests both (1) everything that valgrind's memcheck usually checks
    # (heap buffer overflows, use of uninitialized memory, use-after-free,
    # etc.) and (2) branches or memory access depending on secret values,
    # which will be reported as uninitialized memory. To distinguish between
    # secret and actually uninitialized:
    # - unset MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND - does the failure persist?
    # - or alternatively, build with debug info and manually run the offending
    # test suite with valgrind --track-origins=yes, then check if the origin
    # was TEST_CF_SECRET() or something else.
    msg "build: cmake release GCC, full config with constant flow testing"
    scripts/config.py full
    scripts/config.py set MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND
    skip_suites_without_constant_flow
    cd $OUT_OF_SOURCE_DIR
    cmake -DCMAKE_BUILD_TYPE:String=Release "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    # this only shows a summary of the results (how many of each type)
    # details are left in Testing/<date>/DynamicAnalysis.xml
    msg "test: some suites (valgrind + constant flow)"
    make memcheck
}

component_tf_psa_crypto_test_tsan () {
    msg "build: TSan (clang)"
    scripts/config.py full
    scripts/config.py set MBEDTLS_THREADING_C
    scripts/config.py set MBEDTLS_THREADING_PTHREAD
    # Self-tests do not currently use multiple threads.
    scripts/config.py unset MBEDTLS_SELF_TEST
    # Interruptible ECC tests are not thread safe
    scripts/config.py unset MBEDTLS_ECP_RESTARTABLE

    cd $OUT_OF_SOURCE_DIR
    CC=clang cmake -DCMAKE_BUILD_TYPE:String=TSan "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: main suites (TSan)"
    make test
}

component_tf_psa_crypto_test_memsan () {
    msg "build: MSan (clang)" # ~ 1 min 20s
    scripts/config.py unset MBEDTLS_AESNI_C # memsan doesn't grok asm
    scripts/config.py unset MBEDTLS_HAVE_ASM
    cd $OUT_OF_SOURCE_DIR
    CC=clang cmake -DCMAKE_BUILD_TYPE:String=MemSan "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: main suites (MSan)" # ~ 10s
    make test
}

component_release_tf_psa_crypto_test_valgrind () {
    msg "build: Release (clang)"
    # default config
    cd $OUT_OF_SOURCE_DIR
    CC=clang cmake -DCMAKE_BUILD_TYPE:String=Release "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: main suites, Valgrind (default config)"
    make memcheck
}

component_release_tf_psa_crypto_test_valgrind_psa () {
    msg "build: Release, full (clang)"
    # full config
    scripts/config.py full
    cd $OUT_OF_SOURCE_DIR
    CC=clang cmake -DCMAKE_BUILD_TYPE:String=Release "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "test: main suites, Valgrind (full config)"
    make memcheck
}
