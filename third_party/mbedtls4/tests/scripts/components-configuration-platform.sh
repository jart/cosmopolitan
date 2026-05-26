# components-configuration-platform.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Configuration Testing - Platform
################################################################

component_build_no_std_function () {
    # catch compile bugs in _uninit functions
    msg "build: full config with NO_STD_FUNCTION, make, gcc" # ~ 30s
    scripts/config.py full
    scripts/config.py set MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
    scripts/config.py unset MBEDTLS_ENTROPY_NV_SEED
    scripts/config.py unset MBEDTLS_PLATFORM_NV_SEED_ALT
    CC=gcc cmake -D CMAKE_BUILD_TYPE:String=Check .
    make
}

component_test_psa_driver_get_entropy()
{
    msg "build: default - MBEDTLS_PSA_BUILTIN_GET_ENTROPY + MBEDTLS_PSA_DRIVER_GET_ENTROPY"
    # Use hardware polling as the only source for entropy
    scripts/config.py unset MBEDTLS_PSA_BUILTIN_GET_ENTROPY
    scripts/config.py unset MBEDTLS_ENTROPY_NV_SEED
    scripts/config.py set MBEDTLS_PSA_DRIVER_GET_ENTROPY

    $MAKE_COMMAND

    # Run all the tests
    msg "test: default - MBEDTLS_PSA_BUILTIN_GET_ENTROPY + MBEDTLS_PSA_DRIVER_GET_ENTROPY"
    $MAKE_COMMAND test
}

component_build_no_sockets () {
    # Note, C99 compliance can also be tested with the sockets support disabled,
    # as that requires a POSIX platform (which isn't the same as C99).
    msg "build: full config except net_sockets.c, make, gcc -std=c99 -pedantic" # ~ 30s
    scripts/config.py full
    scripts/config.py unset MBEDTLS_NET_C # getaddrinfo() undeclared, etc.
    scripts/config.py unset MBEDTLS_PSA_BUILTIN_GET_ENTROPY # prevent syscall() on GNU/Linux
    scripts/config.py set MBEDTLS_PSA_DRIVER_GET_ENTROPY
    $MAKE_COMMAND CC=gcc CFLAGS='-Werror -Wall -Wextra -O1 -std=c99 -pedantic' lib
}

component_test_no_date_time () {
    msg "build: default config without MBEDTLS_HAVE_TIME_DATE"
    scripts/config.py unset MBEDTLS_HAVE_TIME_DATE
    cmake -D CMAKE_BUILD_TYPE:String=Check .
    make

    msg "test: !MBEDTLS_HAVE_TIME_DATE - main suites"
    make test
}

component_test_platform_calloc_macro () {
    msg "build: MBEDTLS_PLATFORM_{CALLOC/FREE}_MACRO enabled (ASan build)"
    scripts/config.py set MBEDTLS_PLATFORM_MEMORY
    scripts/config.py set MBEDTLS_PLATFORM_CALLOC_MACRO calloc
    scripts/config.py set MBEDTLS_PLATFORM_FREE_MACRO   free
    CC=$ASAN_CC cmake -D CMAKE_BUILD_TYPE:String=Asan .
    make

    msg "test: MBEDTLS_PLATFORM_{CALLOC/FREE}_MACRO enabled (ASan build)"
    make test
}

component_test_have_int32 () {
    msg "build: gcc, force 32-bit bignum limbs"
    scripts/config.py unset MBEDTLS_HAVE_ASM
    scripts/config.py unset MBEDTLS_AESNI_C
    scripts/config.py unset MBEDTLS_AESCE_C
    $MAKE_COMMAND CC=gcc CFLAGS='-O2 -Werror -Wall -Wextra -DMBEDTLS_HAVE_INT32'

    msg "test: gcc, force 32-bit bignum limbs"
    $MAKE_COMMAND test
}

component_test_have_int64 () {
    msg "build: gcc, force 64-bit bignum limbs"
    scripts/config.py unset MBEDTLS_HAVE_ASM
    scripts/config.py unset MBEDTLS_AESNI_C
    scripts/config.py unset MBEDTLS_AESCE_C
    $MAKE_COMMAND CC=gcc CFLAGS='-O2 -Werror -Wall -Wextra -DMBEDTLS_HAVE_INT64'

    msg "test: gcc, force 64-bit bignum limbs"
    $MAKE_COMMAND test
}

component_test_have_int32_cmake_new_bignum () {
    msg "build: gcc, force 32-bit bignum limbs, new bignum interface, test hooks (ASan build)"
    scripts/config.py unset MBEDTLS_HAVE_ASM
    scripts/config.py unset MBEDTLS_AESNI_C
    scripts/config.py unset MBEDTLS_AESCE_C
    scripts/config.py set MBEDTLS_TEST_HOOKS
    scripts/config.py set MBEDTLS_ECP_WITH_MPI_UINT
    $MAKE_COMMAND CC=gcc CFLAGS="$ASAN_CFLAGS -Werror -Wall -Wextra -DMBEDTLS_HAVE_INT32" LDFLAGS="$ASAN_CFLAGS"

    msg "test: gcc, force 32-bit bignum limbs, new bignum interface, test hooks (ASan build)"
    $MAKE_COMMAND test
}

component_test_no_udbl_division () {
    msg "build: MBEDTLS_NO_UDBL_DIVISION native" # ~ 10s
    scripts/config.py full
    scripts/config.py set MBEDTLS_NO_UDBL_DIVISION
    $MAKE_COMMAND CFLAGS='-Werror -O1'

    msg "test: MBEDTLS_NO_UDBL_DIVISION native" # ~ 10s
    $MAKE_COMMAND test
}

component_test_no_64bit_multiplication () {
    msg "build: MBEDTLS_NO_64BIT_MULTIPLICATION native" # ~ 10s
    scripts/config.py full
    scripts/config.py set MBEDTLS_NO_64BIT_MULTIPLICATION
    $MAKE_COMMAND CFLAGS='-Werror -O1'

    msg "test: MBEDTLS_NO_64BIT_MULTIPLICATION native" # ~ 10s
    $MAKE_COMMAND test
}
