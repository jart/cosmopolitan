# components-compiler.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Compiler Testing
################################################################

support_build_tfm_armcc () {
    support_build_armcc
}

component_build_tfm_armcc () {
    # test the TF-M configuration can build cleanly with various warning flags enabled
    cp configs/config-tfm.h "$CONFIG_H"
    cp tf-psa-crypto/configs/ext/crypto_config_profile_medium.h "$CRYPTO_CONFIG_H"

    msg "build: TF-M config, armclang armv7-m thumb2"
    helper_armc6_build_test "--target=arm-arm-none-eabi -march=armv7-m -mthumb -Os -std=c99 -Werror -Wall -Wextra -Wwrite-strings -Wpointer-arith -Wimplicit-fallthrough -Wshadow -Wvla -Wformat=2 -Wno-format-nonliteral -Wshadow -Wasm-operand-widths -Wunused -I../framework/tests/include/spe"
}

test_build_opt () {
    info=$1 cc=$2; shift 2
    $cc --version
    for opt in "$@"; do
          msg "build/test: $cc $opt, $info" # ~ 30s
          $MAKE_COMMAND CC="$cc" CFLAGS="$opt -std=c99 -pedantic -Wall -Wextra -Werror"
          # We're confident enough in compilers to not run _all_ the tests,
          # but at least run the unit tests. In particular, runs with
          # optimizations use inline assembly whereas runs with -O0
          # skip inline assembly.
          $MAKE_COMMAND test # ~30s
          $MAKE_COMMAND clean
    done
}

# For FreeBSD we invoke the function by name so this condition is added
# to disable the existing test_clang_opt function for linux.
if [[ $(uname) != "Linux" ]]; then
    component_test_clang_opt () {
        scripts/config.py full
        test_build_opt 'full config' clang -O0 -Os -O2
    }
fi

component_test_clang_latest_opt () {
    scripts/config.py full
    test_build_opt 'full config' "$CLANG_LATEST" -O0 -Os -O2
}

support_test_clang_latest_opt () {
    type "$CLANG_LATEST" >/dev/null 2>/dev/null
}

component_test_clang_earliest_opt () {
    scripts/config.py full
    test_build_opt 'full config' "$CLANG_EARLIEST" -O2
}

support_test_clang_earliest_opt () {
    type "$CLANG_EARLIEST" >/dev/null 2>/dev/null
}

component_test_gcc_latest_opt () {
    scripts/config.py full
    test_build_opt 'full config' "$GCC_LATEST" -O0 -Os -O2
}

support_test_gcc_latest_opt () {
    type "$GCC_LATEST" >/dev/null 2>/dev/null
}

# Prepare for a non-regression for https://github.com/Mbed-TLS/mbedtls/issues/9814 :
# test with GCC 15.
# Eventually, $GCC_LATEST will be GCC 15 or above, and we can remove this
# separate component.
# For the time being, we don't make $GCC_LATEST be GCC 15 on the CI
# platform, because that would break branches where #9814 isn't fixed yet.
support_test_gcc15_drivers_opt () {
    if type gcc-15 >/dev/null 2>/dev/null; then
        GCC_15=gcc-15
    elif [ -x /usr/local/gcc-15/bin/gcc-15 ]; then
        GCC_15=/usr/local/gcc-15/bin/gcc-15
    else
        return 1
    fi
}
component_test_gcc15_drivers_opt () {
    msg "build: GCC 15: full + test drivers dispatching to builtins"
    scripts/config.py full
    loc_cflags="$ASAN_CFLAGS -DPSA_CRYPTO_DRIVER_TEST -DMBEDTLS_CONFIG_ADJUST_TEST_ACCELERATORS"
    loc_cflags="${loc_cflags} -I../framework/tests/include -O2"

    $MAKE_COMMAND CC=$GCC_15 CFLAGS="${loc_cflags}" LDFLAGS="$ASAN_CFLAGS"

    msg "test: GCC 15: full + test drivers dispatching to builtins"
    $MAKE_COMMAND test
}

component_test_gcc_earliest_opt () {
    scripts/config.py full
    test_build_opt 'full config' "$GCC_EARLIEST" -O2
}

support_test_gcc_earliest_opt () {
    type "$GCC_EARLIEST" >/dev/null 2>/dev/null
}

component_build_mingw () {
    msg "build: Windows cross build - mingw64, make (Link Library)" # ~ 30s
    $MAKE_COMMAND CC=i686-w64-mingw32-gcc AR=i686-w64-mingw32-ar CFLAGS='-Werror -Wall -Wextra -maes -msse2 -mpclmul' WINDOWS_BUILD=1 lib programs

    # note Make tests only builds the tests, but doesn't run them
    $MAKE_COMMAND CC=i686-w64-mingw32-gcc AR=i686-w64-mingw32-ar CFLAGS='-Werror -maes -msse2 -mpclmul' WINDOWS_BUILD=1 tests
    $MAKE_COMMAND WINDOWS_BUILD=1 clean

    msg "build: Windows cross build - mingw64, make (DLL)" # ~ 30s
    $MAKE_COMMAND CC=i686-w64-mingw32-gcc AR=i686-w64-mingw32-ar CFLAGS='-Werror -Wall -Wextra -maes -msse2 -mpclmul' WINDOWS_BUILD=1 SHARED=1 lib programs
    $MAKE_COMMAND CC=i686-w64-mingw32-gcc AR=i686-w64-mingw32-ar CFLAGS='-Werror -Wall -Wextra -maes -msse2 -mpclmul' WINDOWS_BUILD=1 SHARED=1 tests
    $MAKE_COMMAND WINDOWS_BUILD=1 clean

    msg "build: Windows cross build - mingw64, make (Library only, default config without MBEDTLS_AESNI_C)" # ~ 30s
    ./scripts/config.py unset MBEDTLS_AESNI_C #
    $MAKE_COMMAND CC=i686-w64-mingw32-gcc AR=i686-w64-mingw32-ar CFLAGS='-Werror -Wall -Wextra' WINDOWS_BUILD=1 lib
    $MAKE_COMMAND WINDOWS_BUILD=1 clean
}

support_build_mingw () {
    case $(i686-w64-mingw32-gcc -dumpversion 2>/dev/null) in
        [0-5]*|"") false;;
        *) true;;
    esac
}

component_build_zeroize_checks () {
    msg "build: check for obviously wrong calls to mbedtls_platform_zeroize()"

    scripts/config.py full

    # Only compile - we're looking for sizeof-pointer-memaccess warnings
    $MAKE_COMMAND CFLAGS="'-DTF_PSA_CRYPTO_USER_CONFIG_FILE=\"$TF_PSA_CRYPTO_ROOT_DIR/tests/configs/user-config-zeroize-memset.h\"' -DMBEDTLS_TEST_DEFINES_ZEROIZE -Werror -Wsizeof-pointer-memaccess"
}

component_test_zeroize () {
    # Test that the function mbedtls_platform_zeroize() is not optimized away by
    # different combinations of compilers and optimization flags by using an
    # auxiliary GDB script. Unfortunately, GDB does not return error values to the
    # system in all cases that the script fails, so we must manually search the
    # output to check whether the pass string is present and no failure strings
    # were printed.

    # Don't try to disable ASLR. We don't care about ASLR here. We do care
    # about a spurious message if Gdb tries and fails, so suppress that.
    gdb_disable_aslr=
    if [ -z "$(gdb -batch -nw -ex 'set disable-randomization off' 2>&1)" ]; then
        gdb_disable_aslr='set disable-randomization off'
    fi

    for optimization_flag in -O2 -O3 -Ofast -Os; do
        for compiler in clang gcc; do
            msg "test: $compiler $optimization_flag, mbedtls_platform_zeroize()"
            $MAKE_COMMAND programs CC="$compiler" DEBUG=1 CFLAGS="$optimization_flag"
            gdb -ex "$gdb_disable_aslr" -x $FRAMEWORK/tests/programs/test_zeroize.gdb -nw -batch -nx 2>&1 | tee test_zeroize.log
            grep "The buffer was correctly zeroized" test_zeroize.log
            not grep -i "error" test_zeroize.log
            rm -f test_zeroize.log
            $MAKE_COMMAND clean
        done
    done
}
