# all-helpers.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains helpers for test components that are executed by all.sh.
# See "Files structure" in all-core.sh for other files used by all.sh.
#
# This file is the right place for helpers:
# - that are used by more than one component living in more than one file;
# - or (inclusive) that we want to share accross repos or branches.
#
# Helpers that are used in a single component file that is
# repo&branch-specific can be defined in the file where they are used.

################################################################
#### Helpers for components using libtestdriver1
################################################################

# How to use libtestdriver1
# -------------------------
#
# 1. Define the list algorithms and key types to accelerate,
#    designated the same way as PSA_WANT_ macros but without PSA_WANT_.
#    Examples:
#      - loc_accel_list="ALG_JPAKE"
#      - loc_accel_list="ALG_FFDH KEY_TYPE_DH_KEY_PAIR KEY_TYPE_DH_PUBLIC_KEY"
# 2. Make configurations changes for the driver and/or main libraries.
#    2a. Call helper_libtestdriver1_adjust_config <base>, where the argument
#        can be either "default" to start with the default config, or a name
#        supported by scripts/config.py (for example, "full"). This selects
#        the base to use, and makes common adjustments.
#    2b. If desired, adjust the PSA_WANT symbols in psa/crypto_config.h.
#        These changes affect both the driver and the main libraries.
#        (Note: they need to have the same set of PSA_WANT symbols, as that
#        determines the ABI between them.)
#    2c. Adjust MBEDTLS_ symbols in mbedtls_config.h. This only affects the
#        main libraries. Typically, you want to disable the module(s) that are
#        being accelerated. You may need to also disable modules that depend
#        on them or options that are not supported with drivers.
#    2d. On top of psa/crypto_config.h, the driver library uses its own config
#        file: tests/configs/config_test_driver.h. You usually don't need to
#        edit it: using loc_extra_list (see below) is preferred. However, when
#        there's no PSA symbol for what you want to enable, calling
#        scripts/config.py on this file remains the only option.
# 3. Build the driver library, then the main libraries, test, and programs.
#    3a. Call helper_libtestdriver1_make_drivers "$loc_accel_list". You may
#        need to enable more algorithms here, typically hash algorithms when
#        accelerating some signature algorithms (ECDSA, RSAv2). This is done
#        by passing a 2nd argument listing the extra algorithms.
#        Example:
#          loc_extra_list="ALG_SHA_224 ALG_SHA_256 ALG_SHA_384 ALG_SHA_512"
#          helper_libtestdriver1_make_drivers "$loc_accel_list" "$loc_extra_list"
#    3b. Call helper_libtestdriver1_make_main "$loc_accel_list". Any
#        additional arguments will be passed to make: this can be useful if
#        you don't want to build everything when iterating during development.
#        Example:
#          helper_libtestdriver1_make_main "$loc_accel_list" -C tests test_suite_foo
# 4. Run the tests you want.

# Adjust the configuration - for both libtestdriver1 and main library,
# as they should have the same PSA_WANT macros.
helper_libtestdriver1_adjust_config() {
    base_config=$1
    # Select the base configuration
    if [ "$base_config" != "default" ]; then
        scripts/config.py "$base_config"
    fi

    if in_mbedtls_repo && in_3_6_branch; then
        # Enable PSA-based config (necessary to use drivers)
        # MBEDTLS_PSA_CRYPTO_CONFIG is a legacy setting which should only be set on 3.6 LTS branches.
        scripts/config.py set MBEDTLS_PSA_CRYPTO_CONFIG

        # Dynamic secure element support is a deprecated feature and needs to be disabled here.
        # This is done to have the same form of psa_key_attributes_s for libdriver and library.
        scripts/config.py unset MBEDTLS_PSA_CRYPTO_SE_C
    fi

    # If threading is enabled on the normal build, then we need to enable it in the drivers as well,
    # otherwise we will end up running multithreaded tests without mutexes to protect them.
    if scripts/config.py get MBEDTLS_THREADING_C; then
        if in_3_6_branch; then
            scripts/config.py -f "$CONFIG_TEST_DRIVER_H" set MBEDTLS_THREADING_C
        else
            scripts/config.py -c "$CONFIG_TEST_DRIVER_H" set MBEDTLS_THREADING_C
        fi
    fi

    if scripts/config.py get MBEDTLS_THREADING_PTHREAD; then
        if in_3_6_branch; then
            scripts/config.py -f "$CONFIG_TEST_DRIVER_H" set MBEDTLS_THREADING_PTHREAD
        else
            scripts/config.py -c "$CONFIG_TEST_DRIVER_H" set MBEDTLS_THREADING_PTHREAD
        fi
    fi
}

# Build the drivers library libtestdriver1.a (with ASan).
#
# Parameters:
# 1. a space-separated list of things to accelerate;
# 2. optional: a space-separate list of things to also support.
# Here "things" are PSA_WANT_ symbols but with PSA_WANT_ removed.
helper_libtestdriver1_make_drivers() {
    loc_accel_flags=$( echo "$1 ${2-}" | sed 's/[^ ]* */-DLIBTESTDRIVER1_MBEDTLS_PSA_ACCEL_&/g' )
    make CC=$ASAN_CC -C tests libtestdriver1.a CFLAGS=" $ASAN_CFLAGS $loc_accel_flags" LDFLAGS="$ASAN_CFLAGS"
}

# Build the main libraries, programs and tests,
# linking to the drivers library (with ASan).
#
# Parameters:
# 1. a space-separated list of things to accelerate;
# *. remaining arguments if any are passed directly to make
#    (examples: lib, -C tests test_suite_xxx, etc.)
# Here "things" are PSA_WANT_ symbols but with PSA_WANT_ removed.
helper_libtestdriver1_make_main() {
    loc_accel_list=$1
    shift

    # we need flags both with and without the LIBTESTDRIVER1_ prefix
    loc_accel_flags=$( echo "$loc_accel_list" | sed 's/[^ ]* */-DLIBTESTDRIVER1_MBEDTLS_PSA_ACCEL_&/g' )
    loc_accel_flags="$loc_accel_flags $( echo "$loc_accel_list" | sed 's/[^ ]* */-DMBEDTLS_PSA_ACCEL_&/g' )"
    $MAKE_COMMAND CC=$ASAN_CC CFLAGS="$ASAN_CFLAGS -I../tests/include -I../framework/tests/include -I../tests -I../../tests -DPSA_CRYPTO_DRIVER_TEST -DMBEDTLS_TEST_LIBTESTDRIVER1 $loc_accel_flags" LDFLAGS="-ltestdriver1 $ASAN_CFLAGS" "$@"
}

################################################################
#### Helpers for components using psasim
################################################################

# Set some default values $CONFIG_H in order to build server or client sides
# in PSASIM. There is only 1 mandatory parameter:
# - $1: target which can be "client" or "server"
helper_psasim_config() {
    TARGET=$1

    if [ "$TARGET" == "client" ]; then
        scripts/config.py full
        scripts/config.py unset MBEDTLS_PSA_CRYPTO_C
        scripts/config.py unset MBEDTLS_PSA_CRYPTO_STORAGE_C
        scripts/config.py unset MBEDTLS_ENTROPY_NV_SEED
        scripts/config.py unset MBEDTLS_PLATFORM_NV_SEED_ALT
        if in_mbedtls_repo && in_3_6_branch; then
            # Dynamic secure element support is a deprecated feature and it is not
            # available when CRYPTO_C and PSA_CRYPTO_STORAGE_C are disabled.
            scripts/config.py unset MBEDTLS_PSA_CRYPTO_SE_C
        fi
        # Disable potentially problematic features
        scripts/config.py unset MBEDTLS_X509_RSASSA_PSS_SUPPORT
        scripts/config.py unset MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
        scripts/config.py unset MBEDTLS_ECP_RESTARTABLE
        scripts/config.py unset MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
        scripts/config.py unset MBEDTLS_PK_PARSE_EC_EXTENDED
        scripts/config.py unset MBEDTLS_PK_PARSE_EC_COMPRESSED

        scripts/config.py unset-all MBEDTLS_SHA256_USE_.*_CRYPTO_
        scripts/config.py unset-all MBEDTLS_SHA512_USE_.*_CRYPTO_
    else
        scripts/config.py crypto_full
        scripts/config.py unset MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS
        if in_mbedtls_repo && in_3_6_branch; then
            # We need to match the client with MBEDTLS_PSA_CRYPTO_SE_C
            scripts/config.py unset MBEDTLS_PSA_CRYPTO_SE_C
        fi
        # Also ensure MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER not set (to match client)
        scripts/config.py unset MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
    fi
}

# This is a helper function to be used in psasim builds. It is meant to clean
# up the library's workspace after the server build and before the client
# build. Built libraries (mbedcrypto, mbedx509 and mbedtls) are supposed to be
# already copied to psasim folder at this point.
helper_psasim_cleanup_before_client() {
    # Clean up library files
    make -C library clean

    # Restore files that were backup before building library files. This
    # includes $CONFIG_H and $CRYPTO_CONFIG_H.
    restore_backed_up_files
}

# Helper to build the libraries for client/server in PSASIM. If the server is
# being built, then it builds also the final executable.
# There is only 1 mandatory parameter:
# - $1: target which can be "client" or "server"
helper_psasim_build() {
    TARGET=$1
    shift
    TARGET_LIB=${TARGET}_libs

    make -C $PSASIM_PATH CFLAGS="$ASAN_CFLAGS" LDFLAGS="$ASAN_CFLAGS" $TARGET_LIB "$@"

    # Build also the server application after its libraries have been built.
    if [ "$TARGET" == "server" ]; then
        make -C $PSASIM_PATH CFLAGS="$ASAN_CFLAGS" LDFLAGS="$ASAN_CFLAGS" test/psa_server
    fi
}

################################################################
#### Configuration helpers
################################################################

# When called with no parameter this function disables all builtin curves.
# The function optionally accepts 1 parameter: a space-separated list of the
# curves that should be kept enabled.
helper_disable_builtin_curves() {
    allowed_list="${1:-}"
    scripts/config.py unset-all "MBEDTLS_ECP_DP_[0-9A-Z_a-z]*_ENABLED"

    for curve in $allowed_list; do
        scripts/config.py set $curve
    done
}

# Helper returning the list of supported elliptic curves from CRYPTO_CONFIG_H,
# without the "PSA_WANT_" prefix. This becomes handy for accelerating curves
# in the following helpers.
helper_get_psa_curve_list () {
    loc_list=""
    for item in $(sed -n 's/^#define PSA_WANT_\(ECC_[0-9A-Z_a-z]*\).*/\1/p' <"$CRYPTO_CONFIG_H"); do
        loc_list="$loc_list $item"
    done

    echo "$loc_list"
}

# Helper returning the list of supported DH groups from CRYPTO_CONFIG_H,
# without the "PSA_WANT_" prefix. This becomes handy for accelerating DH groups
# in the following helpers.
helper_get_psa_dh_group_list () {
    loc_list=""
    for item in $(sed -n 's/^#define PSA_WANT_\(DH_RFC7919_[0-9]*\).*/\1/p' <"$CRYPTO_CONFIG_H"); do
        loc_list="$loc_list $item"
    done

    echo "$loc_list"
}

# Get the list of uncommented PSA_WANT_KEY_TYPE_xxx_ from CRYPTO_CONFIG_H. This
# is useful to easily get a list of key type symbols to accelerate.
# The function accepts a single argument which is the key type: ECC, DH, RSA.
helper_get_psa_key_type_list() {
    key_type="$1"
    loc_list=""
    for item in $(sed -n "s/^#define PSA_WANT_\(KEY_TYPE_${key_type}_[0-9A-Z_a-z]*\).*/\1/p" <"$CRYPTO_CONFIG_H"); do
        # Skip DERIVE for elliptic keys since there is no driver dispatch for
        # it so it cannot be accelerated.
        if [ "$item" != "KEY_TYPE_ECC_KEY_PAIR_DERIVE" ]; then
            loc_list="$loc_list $item"
        fi
    done

    echo "$loc_list"
}

################################################################
#### Misc. helpers for components
################################################################

helper_armc6_build_test()
{
    FLAGS="$1"

    msg "build: ARM Compiler 6 ($FLAGS)"

    $MAKE_COMMAND clean
    ARM_TOOL_VARIANT="ult" CC="$ARMC6_CC" AR="$ARMC6_AR" CFLAGS="$FLAGS" \
                        WARNING_CFLAGS='-Werror -xc -std=c99' $MAKE_COMMAND lib

    msg "size: ARM Compiler 6 ($FLAGS)"
    "$ARMC6_FROMELF" -z library/*.o
    if [ -n "${PSA_CORE_PATH}" ]; then
        "$ARMC6_FROMELF" -z ${PSA_CORE_PATH}/*.o
    fi
    if [ -n "${BUILTIN_SRC_PATH}" ]; then
        "$ARMC6_FROMELF" -z ${BUILTIN_SRC_PATH}/*.o
    fi
}

helper_armc6_cmake_build_test()
{
    FLAGS="$1"

    msg "build: CMake + ARM Compiler 6 ($FLAGS)"

    cmake -DCMAKE_SYSTEM_NAME="Generic" -DCMAKE_SYSTEM_PROCESSOR="cortex-m0" \
            -DCMAKE_C_COMPILER="$ARMC6_CC" -DCMAKE_C_LINKER="$ARMC6_LINK" \
            -DCMAKE_AR="$ARMC6_AR" -DCMAKE_C_FLAGS="$FLAGS" \
            -DCMAKE_C_COMPILER_WORKS=TRUE -DENABLE_TESTING=OFF \
            -DENABLE_PROGRAMS=OFF "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "size: ARM Compiler 6 ($FLAGS)"
    "$ARMC6_FROMELF" -z ${PSA_CORE_PATH}/CMakeFiles/tfpsacrypto.dir/*.o
    "$ARMC6_FROMELF" -z ${BUILTIN_SRC_PATH}/../CMakeFiles/builtin.dir/src/*.o
}

clang_version() {
    if command -v clang > /dev/null ; then
        clang --version|grep version|sed -E 's#.*version ([0-9]+).*#\1#'
    else
        echo 0  # report version 0 for "no clang"
    fi
}

gcc_version() {
    gcc="$1"
    if command -v "$gcc" > /dev/null ; then
        "$gcc" --version | sed -En '1s/^[^ ]* \([^)]*\) ([0-9]+).*/\1/p'
    else
        echo 0  # report version 0 for "no gcc"
    fi
}

can_run_cc_output() {
    cc="$1"
    result=false
    if type "$cc" >/dev/null 2>&1; then
        testbin=$(mktemp)
        if echo 'int main(void){return 0;}' | "$cc" -o "$testbin" -x c -; then
            if "$testbin" 2>/dev/null; then
                result=true
            fi
        fi
        rm -f "$testbin"
    fi
    $result
}

can_run_arm_linux_gnueabi=
can_run_arm_linux_gnueabi () {
    if [ -z "$can_run_arm_linux_gnueabi" ]; then
        if can_run_cc_output "${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc"; then
            can_run_arm_linux_gnueabi=true
        else
            can_run_arm_linux_gnueabi=false
        fi
    fi
    $can_run_arm_linux_gnueabi
}

can_run_arm_linux_gnueabihf=
can_run_arm_linux_gnueabihf () {
    if [ -z "$can_run_arm_linux_gnueabihf" ]; then
        if can_run_cc_output "${ARM_LINUX_GNUEABIHF_GCC_PREFIX}gcc"; then
            can_run_arm_linux_gnueabihf=true
        else
            can_run_arm_linux_gnueabihf=false
        fi
    fi
    $can_run_arm_linux_gnueabihf
}

can_run_aarch64_linux_gnu=
can_run_aarch64_linux_gnu () {
    if [ -z "$can_run_aarch64_linux_gnu" ]; then
        if can_run_cc_output "${AARCH64_LINUX_GNU_GCC_PREFIX}gcc"; then
            can_run_aarch64_linux_gnu=true
        else
            can_run_aarch64_linux_gnu=false
        fi
    fi
    $can_run_aarch64_linux_gnu
}
