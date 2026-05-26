# components-configuration.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Configuration Testing
################################################################

component_test_default_out_of_box () {
    msg "build: make, default config (out-of-box)" # ~1min
    $MAKE_COMMAND
    # Disable fancy stuff
    unset MBEDTLS_TEST_OUTCOME_FILE

    msg "test: main suites make, default config (out-of-box)" # ~10s
    $MAKE_COMMAND test

    msg "selftest: make, default config (out-of-box)" # ~10s
    programs/test/selftest

    msg "program demos: make, default config (out-of-box)" # ~10s
    tests/scripts/run_demos.py
}

component_test_default_cmake_gcc_asan () {
    msg "build: cmake, gcc, ASan" # ~ 1 min 50s
    CC=gcc cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: main suites (inc. selftests) (ASan build)" # ~ 50s
    make test

    msg "program demos (ASan build)" # ~10s
    tests/scripts/run_demos.py

    msg "test: selftest (ASan build)" # ~ 10s
    programs/test/selftest

    msg "test: metatests (GCC, ASan build)"
    tests/scripts/run-metatests.sh any asan poison

    msg "test: ssl-opt.sh (ASan build)" # ~ 1 min
    tests/ssl-opt.sh

    msg "test: compat.sh (ASan build)" # ~ 6 min
    tests/compat.sh

    msg "test: context-info.sh (ASan build)" # ~ 15 sec
    tests/context-info.sh
}

component_test_default_cmake_gcc_asan_new_bignum () {
    msg "build: cmake, gcc, ASan" # ~ 1 min 50s
    scripts/config.py set MBEDTLS_ECP_WITH_MPI_UINT
    CC=gcc cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: main suites (inc. selftests) (ASan build)" # ~ 50s
    make test

    msg "test: selftest (ASan build)" # ~ 10s
    programs/test/selftest

    msg "test: ssl-opt.sh (ASan build)" # ~ 1 min
    tests/ssl-opt.sh

    msg "test: compat.sh (ASan build)" # ~ 6 min
    tests/compat.sh

    msg "test: context-info.sh (ASan build)" # ~ 15 sec
    tests/context-info.sh
}

component_test_full_cmake_gcc_asan () {
    msg "build: full config, cmake, gcc, ASan"
    scripts/config.py full
    CC=gcc cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: main suites (inc. selftests) (full config, ASan build)"
    make test

    msg "test: selftest (full config, ASan build)" # ~ 10s
    programs/test/selftest

    msg "test: ssl-opt.sh (full config, ASan build)"
    tests/ssl-opt.sh

    # Note: the next two invocations cover all compat.sh test cases.
    # We should use the same here and in basic-build-test.sh.
    msg "test: compat.sh: default version (full config, ASan build)"
    tests/compat.sh -e 'ARIA\|CHACHA'

    msg "test: compat.sh: next: ARIA, Chacha (full config, ASan build)"
    env OPENSSL="$OPENSSL_NEXT" tests/compat.sh -e '^$' -f 'ARIA\|CHACHA'

    msg "test: context-info.sh (full config, ASan build)" # ~ 15 sec
    tests/context-info.sh
}

component_test_full_cmake_gcc_asan_new_bignum () {
    msg "build: full config, cmake, gcc, ASan"
    scripts/config.py full
    scripts/config.py set MBEDTLS_ECP_WITH_MPI_UINT
    CC=gcc cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: main suites (inc. selftests) (full config, new bignum, ASan)"
    make test

    msg "test: selftest (full config, new bignum, ASan)" # ~ 10s
    programs/test/selftest

    msg "test: ssl-opt.sh (full config, new bignum, ASan)"
    tests/ssl-opt.sh

    # Note: the next two invocations cover all compat.sh test cases.
    # We should use the same here and in basic-build-test.sh.
    msg "test: compat.sh: default version (full config, new bignum, ASan)"
    tests/compat.sh -e 'ARIA\|CHACHA'

    msg "test: compat.sh: next: ARIA, Chacha (full config, new bignum, ASan)"
    env OPENSSL="$OPENSSL_NEXT" tests/compat.sh -e '^$' -f 'ARIA\|CHACHA'

    msg "test: context-info.sh (full config, new bignum, ASan)" # ~ 15 sec
    tests/context-info.sh
}

component_test_full_cmake_clang () {
    msg "build: cmake, full config, clang" # ~ 50s
    scripts/config.py full
    CC=clang CXX=clang++ cmake -D CMAKE_BUILD_TYPE:String=Release \
                               -D ENABLE_TESTING=On -D TEST_CPP=1 .
    make

    msg "test: main suites (full config, clang)" # ~ 5s
    make test

    msg "test: cpp_dummy_build (full config, clang)" # ~ 1s
    programs/test/cpp_dummy_build

    msg "test: metatests (clang)"
    tests/scripts/run-metatests.sh any pthread

    msg "program demos (full config, clang)" # ~10s
    tests/scripts/run_demos.py

    msg "test: psa_constant_names (full config, clang)" # ~ 1s
    $FRAMEWORK/scripts/test_psa_constant_names.py

    msg "test: ssl-opt.sh default, ECJPAKE, SSL async (full config)" # ~ 1s
    tests/ssl-opt.sh -f 'Default\|ECJPAKE\|SSL async private'
}

component_test_default_no_deprecated () {
    # Test that removing the deprecated features from the default
    # configuration leaves something consistent.
    msg "build: make, default + MBEDTLS_DEPRECATED_REMOVED" # ~ 30s
    scripts/config.py set MBEDTLS_DEPRECATED_REMOVED
    $MAKE_COMMAND CFLAGS='-O -Werror -Wall -Wextra'

    msg "test: make, default + MBEDTLS_DEPRECATED_REMOVED" # ~ 5s
    $MAKE_COMMAND test
}

component_test_full_no_deprecated () {
    msg "build: make, full_no_deprecated config" # ~ 30s
    scripts/config.py full_no_deprecated
    $MAKE_COMMAND CFLAGS='-O -Werror -Wall -Wextra'

    msg "test: make, full_no_deprecated config" # ~ 5s
    $MAKE_COMMAND test

    msg "test: ensure that X509 has no direct dependency on BIGNUM_C"
    not grep mbedtls_mpi library/libmbedx509.a
}

component_test_full_no_deprecated_deprecated_warning () {
    # Test that there is nothing deprecated in "full_no_deprecated".
    # A deprecated feature would trigger a warning (made fatal) from
    # MBEDTLS_DEPRECATED_WARNING.
    msg "build: make, full_no_deprecated config, MBEDTLS_DEPRECATED_WARNING" # ~ 30s
    scripts/config.py full_no_deprecated
    scripts/config.py unset MBEDTLS_DEPRECATED_REMOVED
    scripts/config.py set MBEDTLS_DEPRECATED_WARNING
    $MAKE_COMMAND CFLAGS='-O -Werror -Wall -Wextra'

    msg "test: make, full_no_deprecated config, MBEDTLS_DEPRECATED_WARNING" # ~ 5s
    $MAKE_COMMAND test
}

component_test_full_deprecated_warning () {
    # Test that when MBEDTLS_DEPRECATED_WARNING is enabled, the build passes
    # with only certain whitelisted types of warnings.
    msg "build: make, full config + MBEDTLS_DEPRECATED_WARNING, expect warnings" # ~ 30s
    scripts/config.py full
    scripts/config.py set MBEDTLS_DEPRECATED_WARNING
    # Expect warnings from '#warning' directives in check_config.h.
    # Note that gcc is required to allow the use of -Wno-error=cpp, which allows us to
    # display #warning messages without them being treated as errors.
    $MAKE_COMMAND CC=gcc CFLAGS='-O -Werror -Wall -Wextra -Wno-error=cpp' lib programs

    msg "build: make tests, full config + MBEDTLS_DEPRECATED_WARNING, expect warnings" # ~ 30s
    # Set MBEDTLS_TEST_DEPRECATED to enable tests for deprecated features.
    # By default those are disabled when MBEDTLS_DEPRECATED_WARNING is set.
    # Expect warnings from '#warning' directives in check_config.h and
    # from the use of deprecated functions in test suites.
    $MAKE_COMMAND CC=gcc CFLAGS='-O -Werror -Wall -Wextra -Wno-error=deprecated-declarations -Wno-error=cpp -DMBEDTLS_TEST_DEPRECATED' tests

    msg "test: full config + MBEDTLS_TEST_DEPRECATED" # ~ 30s
    $MAKE_COMMAND test

    msg "program demos: full config + MBEDTLS_TEST_DEPRECATED" # ~10s
    tests/scripts/run_demos.py
}

component_build_baremetal () {
  msg "build: make, baremetal config"
  scripts/config.py baremetal
  $MAKE_COMMAND CFLAGS="-O1 -Werror -I$PWD/framework/tests/include/baremetal-override/ -DMBEDTLS_TEST_PLATFORM_IS_NOT_UNIXLIKE"
}

support_build_baremetal () {
    # Older Glibc versions include time.h from other headers such as stdlib.h,
    # which makes the no-time.h-in-baremetal check fail. Ubuntu 16.04 has this
    # problem, Ubuntu 18.04 is ok.
    ! grep -q -F time.h /usr/include/x86_64-linux-gnu/sys/types.h
}

component_build_tfm () {
    # Check that the TF-M configuration can build cleanly with various
    # warning flags enabled. We don't build or run tests, since the
    # TF-M configuration needs a TF-M platform. A tweaked version of
    # the configuration that works on mainstream platforms is in
    # configs/config-tfm.h, tested via test-ref-configs.pl.
    cp configs/config-tfm.h "$CONFIG_H"
    cp tf-psa-crypto/configs/ext/crypto_config_profile_medium.h "$CRYPTO_CONFIG_H"

    msg "build: TF-M config, clang, armv7-m thumb2"
    $MAKE_COMMAND lib CC="clang" CFLAGS="--target=arm-linux-gnueabihf -march=armv7-m -mthumb -Os -std=c99 -Werror -Wall -Wextra -Wwrite-strings -Wpointer-arith -Wimplicit-fallthrough -Wshadow -Wvla -Wformat=2 -Wno-format-nonliteral -Wshadow -Wasm-operand-widths -Wunused -I../framework/tests/include/spe"

    msg "build: TF-M config, gcc native build"
    $MAKE_COMMAND clean
    $MAKE_COMMAND lib CC="gcc" CFLAGS="-Os -std=c99 -Werror -Wall -Wextra -Wwrite-strings -Wpointer-arith -Wshadow -Wvla -Wformat=2 -Wno-format-nonliteral -Wshadow -Wformat-signedness -Wlogical-op -I../framework/tests/include/spe"
}

component_test_malloc_0_null () {
    msg "build: malloc(0) returns NULL (ASan+UBSan build)"
    scripts/config.py full
    $MAKE_COMMAND CC=$ASAN_CC CFLAGS="'-DTF_PSA_CRYPTO_USER_CONFIG_FILE=\"$PWD/tests/configs/user-config-malloc-0-null.h\"' $ASAN_CFLAGS" LDFLAGS="$ASAN_CFLAGS"

    msg "test: malloc(0) returns NULL (ASan+UBSan build)"
    $MAKE_COMMAND test

    msg "selftest: malloc(0) returns NULL (ASan+UBSan build)"
    # Just the calloc selftest. "make test" ran the others as part of the
    # test suites.
    programs/test/selftest calloc

    msg "test ssl-opt.sh: malloc(0) returns NULL (ASan+UBSan build)"
    # Run a subset of the tests. The choice is a balance between coverage
    # and time (including time indirectly wasted due to flaky tests).
    # The current choice is to skip tests whose description includes
    # "proxy", which is an approximation of skipping tests that use the
    # UDP proxy, which tend to be slower and flakier.
    tests/ssl-opt.sh -e 'proxy'
}

component_test_no_platform () {
    # Full configuration build, without platform support, file IO and net sockets.
    # This should catch missing mbedtls_printf definitions, and by disabling file
    # IO, it should catch missing '#include <stdio.h>'
    msg "build: full config except platform/fsio/net, make, gcc, C99" # ~ 30s
    scripts/config.py full_no_platform
    scripts/config.py unset MBEDTLS_PLATFORM_C
    scripts/config.py unset MBEDTLS_NET_C
    scripts/config.py unset MBEDTLS_FS_IO
    scripts/config.py unset MBEDTLS_PSA_CRYPTO_STORAGE_C
    scripts/config.py unset MBEDTLS_PSA_ITS_FILE_C
    scripts/config.py unset MBEDTLS_ENTROPY_NV_SEED
    # Use the test alternative implementation of mbedtls_platform_get_entropy()
    # which is provided in "framework/tests/src/fake_external_rng_for_test.c"
    # since the default one is excluded in this scenario.
    scripts/config.py unset MBEDTLS_PSA_BUILTIN_GET_ENTROPY
    scripts/config.py set MBEDTLS_PSA_DRIVER_GET_ENTROPY
    # Note, _DEFAULT_SOURCE needs to be defined for platforms using glibc version >2.19,
    # to re-enable platform integration features otherwise disabled in C99 builds
    $MAKE_COMMAND CC=gcc CFLAGS='-Werror -Wall -Wextra -std=c99 -pedantic -Os -D_DEFAULT_SOURCE' lib programs
    $MAKE_COMMAND CC=gcc CFLAGS='-Werror -Wall -Wextra -Os' test
}

component_build_mbedtls_config_file () {
    msg "build: make with MBEDTLS_CONFIG_FILE" # ~40s
    scripts/config.py -w full_config.h full
    echo '#error "MBEDTLS_CONFIG_FILE is not working"' >"$CONFIG_H"
    $MAKE_COMMAND CFLAGS="-I '$PWD' -DMBEDTLS_CONFIG_FILE='\"full_config.h\"'"
    # Make sure this feature is enabled. We'll disable it in the next phase.
    programs/test/query_compile_time_config MBEDTLS_SSL_ALL_ALERT_MESSAGES
    $MAKE_COMMAND clean

    msg "build: make with MBEDTLS_CONFIG_FILE + MBEDTLS_USER_CONFIG_FILE"
    # In the user config, disable one feature (for simplicity, pick a feature
    # that nothing else depends on).
    echo '#undef MBEDTLS_SSL_ALL_ALERT_MESSAGES' >user_config.h
    $MAKE_COMMAND CFLAGS="-I '$PWD' -DMBEDTLS_CONFIG_FILE='\"full_config.h\"' -DMBEDTLS_USER_CONFIG_FILE='\"user_config.h\"'"
    not programs/test/query_compile_time_config MBEDTLS_SSL_ALL_ALERT_MESSAGES

    rm -f user_config.h full_config.h
}

component_test_no_strings () {
    msg "build: no strings" # ~10s
    scripts/config.py full
    # Disable options that activate a large amount of string constants.
    scripts/config.py unset MBEDTLS_DEBUG_C
    scripts/config.py unset MBEDTLS_ERROR_C
    scripts/config.py set MBEDTLS_ERROR_STRERROR_DUMMY
    scripts/config.py unset MBEDTLS_VERSION_FEATURES
    $MAKE_COMMAND CFLAGS='-Werror -Os'

    msg "test: no strings" # ~ 10s
    $MAKE_COMMAND test
}

component_test_memory_buffer_allocator_backtrace () {
    msg "build: default config with memory buffer allocator and backtrace enabled"
    scripts/config.py set MBEDTLS_MEMORY_BUFFER_ALLOC_C
    scripts/config.py set MBEDTLS_PLATFORM_MEMORY
    scripts/config.py set MBEDTLS_MEMORY_BACKTRACE
    scripts/config.py set MBEDTLS_MEMORY_DEBUG
    cmake -DCMAKE_BUILD_TYPE:String=Release .
    make

    msg "test: MBEDTLS_MEMORY_BUFFER_ALLOC_C and MBEDTLS_MEMORY_BACKTRACE"
    make test
}

component_test_memory_buffer_allocator () {
    msg "build: default config with memory buffer allocator"
    scripts/config.py set MBEDTLS_MEMORY_BUFFER_ALLOC_C
    scripts/config.py set MBEDTLS_PLATFORM_MEMORY
    cmake -DCMAKE_BUILD_TYPE:String=Release .
    make

    msg "test: MBEDTLS_MEMORY_BUFFER_ALLOC_C"
    make test

    msg "test: ssl-opt.sh, MBEDTLS_MEMORY_BUFFER_ALLOC_C"
    # MBEDTLS_MEMORY_BUFFER_ALLOC is slow. Skip tests that tend to time out.
    tests/ssl-opt.sh -e '^DTLS proxy'
}
