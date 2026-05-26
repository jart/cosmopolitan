# all-core.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

################################################################
#### Documentation
################################################################

# Purpose
# -------
#
# To run all tests possible or available on the platform.
#
# Files structure
# ---------------
#
# The executable entry point for users and the CI is tests/scripts/all.sh.
#
# The actual content is in the following files:
# - all-core.sh contains the core logic for running test components,
#   processing command line options, reporting results, etc.
# - all-helpers.sh contains helper functions used by more than 1 component.
# - components-*.sh contain the definitions of the various components.
#
# The first two parts are shared between repos and branches;
# the component files are repo&branch-specific.
#
# The files all-*.sh and components-*.sh should only define functions and not
# run code when sourced; the only exception being that all-core.sh runs
# 'shopt' because that is necessary for the rest of the file to parse.
#
# Notes for users
# ---------------
#
# Warning: the test is destructive. It includes various build modes and
# configurations, and can and will arbitrarily change the current CMake
# configuration. The following files must be committed into git:
#    * include/mbedtls/mbedtls_config.h
#    * Makefile, library/Makefile, programs/Makefile, tests/Makefile,
#      programs/fuzz/Makefile
# After running this script, the CMake cache will be lost and CMake
# will no longer be initialised.
#
# The script assumes the presence of a number of tools:
#   * Basic Unix tools (Windows users note: a Unix-style find must be before
#     the Windows find in the PATH)
#   * Perl
#   * GNU Make
#   * CMake
#   * GCC and Clang (recent enough for using ASan with gcc and MemSan with clang, or valgrind)
#   * G++
#   * arm-gcc and mingw-gcc
#   * ArmCC 6 (aka armclang), unless invoked with --no-armcc
#   * OpenSSL and GnuTLS command line tools, in suitable versions for the
#     interoperability tests. The following are the official versions at the
#     time of writing:
#     * GNUTLS_{CLI,SERV} = 3.4.10
#     * GNUTLS_NEXT_{CLI,SERV} = 3.7.2
#     * OPENSSL = 1.0.2g (without Debian/Ubuntu patches)
#     * OPENSSL_NEXT = 3.1.2
# See the invocation of check_tools below for details.
#
# This script must be invoked from the toplevel directory of a git
# working copy of Mbed TLS.
#
# The behavior on an error depends on whether --keep-going (alias -k)
# is in effect.
#  * Without --keep-going: the script stops on the first error without
#    cleaning up. This lets you work in the configuration of the failing
#    component.
#  * With --keep-going: the script runs all requested components and
#    reports failures at the end. In particular the script always cleans
#    up on exit.
#
# Note that the output is not saved. You may want to run
#   script -c tests/scripts/all.sh
# or
#   tests/scripts/all.sh >all.log 2>&1
#
# Notes for maintainers
# ---------------------
#
# The bulk of the code is organized into functions that follow one of the
# following naming conventions:
# * in all-core.sh:
#   * pre_XXX: things to do before running the tests, in order.
#   * post_XXX: things to do after running the tests.
# * in components-*.sh:
#   * component_XXX: independent components. They can be run in any order.
#     * component_check_XXX: quick tests that aren't worth parallelizing.
#     * component_build_XXX: build things but don't run them.
#     * component_test_XXX: build and test.
#     * component_release_XXX: tests that the CI should skip during PR testing.
#   * support_XXX: if support_XXX exists and returns false then
#     component_XXX is not run by default.
# * in various files:
#   * other: miscellaneous support functions.
#
# Each component must start by invoking `msg` with a short informative message.
#
# Warning: due to the way bash detects errors, the failure of a command
# inside 'if' or '!' is not detected. Use the 'not' function instead of '!'.
#
# Each component is executed in a separate shell process. The component
# fails if any command in it returns a non-zero status.
#
# The framework performs some cleanup tasks after each component. This
# means that components can assume that the working directory is in a
# cleaned-up state, and don't need to perform the cleanup themselves.
# * Run `make clean`.
# * Restore the various config files (potentially modified by config.py) from
#   a backup made when starting the script.
# * If in Mbed TLS, restore the various `Makefile`s (potentially modified by
#   in-tree use of CMake) from a backup made when starting the script. (Note:
#   if the files look generated when starting the script, they will be
#   restored from the git index before making the backup.)


################################################################
#### Initialization and command line parsing
################################################################

# Enable ksh/bash extended file matching patterns.
# Must come before function definitions or some of them wouldn't parse.
shopt -s extglob

pre_set_shell_options () {
    # Abort on errors (even on the left-hand side of a pipe).
    # Treat uninitialised variables as errors.
    set -e -o pipefail -u
}

pre_check_environment () {

    source $FRAMEWORK/scripts/project_detection.sh

    if in_mbedtls_repo || in_tf_psa_crypto_repo; then :; else
        echo "Must be run from Mbed TLS / TF-PSA-Crypto root" >&2
        exit 1
    fi
}

# Must be called before pre_initialize_variables which sets ALL_COMPONENTS.
pre_load_components () {
    # Include the components from components.sh
    # Use a path relative to the current directory, aka project's root.
    for file in tests/scripts/components-*.sh; do
        source $file
    done
}

pre_initialize_variables () {
    if in_mbedtls_repo; then
        CONFIG_H='include/mbedtls/mbedtls_config.h'
        if in_3_6_branch; then
            CRYPTO_CONFIG_H='include/psa/crypto_config.h'
            # helper_armc6_build_test() relies on these being defined,
            # but empty if the paths don't exist (as in 3.6).
            PSA_CORE_PATH=''
            BUILTIN_SRC_PATH=''
            CONFIG_TEST_DRIVER_H='tests/configs/config_test_driver.h'
        else
            CRYPTO_CONFIG_H='tf-psa-crypto/include/psa/crypto_config.h'
            PSA_CORE_PATH='tf-psa-crypto/core'
            BUILTIN_SRC_PATH='tf-psa-crypto/drivers/builtin/src'
            CONFIG_TEST_DRIVER_H='tf-psa-crypto/tests/configs/crypto_config_test_driver.h'
            MBEDTLS_ROOT_DIR="$PWD"
            TF_PSA_CRYPTO_ROOT_DIR="$PWD/tf-psa-crypto"
        fi
        config_files="$CONFIG_H $CRYPTO_CONFIG_H $CONFIG_TEST_DRIVER_H"
    else
        CRYPTO_CONFIG_H='include/psa/crypto_config.h'
        PSA_CORE_PATH='core'
        BUILTIN_SRC_PATH='drivers/builtin/src'
        CONFIG_TEST_DRIVER_H='tests/configs/config_test_driver.h'
        TF_PSA_CRYPTO_ROOT_DIR="$PWD"
        MBEDTLS_ROOT_DIR=""

        config_files="$CRYPTO_CONFIG_H $CONFIG_TEST_DRIVER_H"
    fi

    # Files that are clobbered by some jobs will be backed up. Use a different
    # suffix from auxiliary scripts so that all.sh and auxiliary scripts can
    # independently decide when to remove the backup file.
    backup_suffix='.all.bak'
    # Files clobbered by config.py
    files_to_back_up="$config_files"
    if in_mbedtls_repo; then
        # Files clobbered by in-tree cmake
        if [ -e Makefile ]; then
            files_to_back_up="$files_to_back_up Makefile"
        fi
        files_to_back_up="$files_to_back_up library/Makefile programs/Makefile tests/Makefile programs/fuzz/Makefile"
    fi

    append_outcome=0
    MEMORY=0
    FORCE=0
    QUIET=0
    KEEP_GOING=0

    # Seed value used with the --release-test option.
    #
    # See also RELEASE_SEED in basic-build-test.sh. Debugging is easier if
    # both values are kept in sync. If you change the value here because it
    # breaks some tests, you'll definitely want to change it in
    # basic-build-test.sh as well.
    RELEASE_SEED=1

    # Specify character collation for regular expressions and sorting with C locale
    export LC_COLLATE=C

    : ${MBEDTLS_TEST_OUTCOME_FILE=}
    : ${MBEDTLS_TEST_PLATFORM="$(uname -s | tr -c \\n0-9A-Za-z _)-$(uname -m | tr -c \\n0-9A-Za-z _)"}
    export MBEDTLS_TEST_OUTCOME_FILE
    export MBEDTLS_TEST_PLATFORM

    # Default commands, can be overridden by the environment
    : ${OPENSSL:="openssl"}
    : ${OPENSSL_NEXT:="$OPENSSL"}
    : ${GNUTLS_CLI:="gnutls-cli"}
    : ${GNUTLS_SERV:="gnutls-serv"}
    : ${OUT_OF_SOURCE_DIR:=$PWD/out_of_source_build}
    : ${ARMC6_BIN_DIR:=/usr/bin}
    : ${ARM_NONE_EABI_GCC_PREFIX:=arm-none-eabi-}
    : ${ARM_LINUX_GNUEABI_GCC_PREFIX:=arm-linux-gnueabi-}
    : ${ARM_LINUX_GNUEABIHF_GCC_PREFIX:=arm-linux-gnueabihf-}
    : ${AARCH64_LINUX_GNU_GCC_PREFIX:=aarch64-linux-gnu-}
    : ${CLANG_LATEST:="clang-latest"}
    : ${CLANG_EARLIEST:="clang-earliest"}
    : ${GCC_LATEST:="gcc-latest"}
    : ${GCC_EARLIEST:="gcc-earliest"}
    : ${MAKE_COMMAND:="make"}

    if [ -e "scripts/legacy.make" ]; then
        MAKE_COMMAND="${MAKE_COMMAND} -f ./scripts/legacy.make"
    fi

    # if MAKEFLAGS is not set add the -j option to speed up invocations of make
    if [ -z "${MAKEFLAGS+set}" ]; then
        export MAKEFLAGS="-j$(all_sh_nproc)"
    fi
    # if CC is not set, use clang by default (if present) to improve build times
    if [ -z "${CC+set}" ] && (type clang > /dev/null 2>&1); then
        export CC="clang"
    fi

    if [ -n "${OPENSSL_3+set}" ]; then
        export OPENSSL_NEXT="$OPENSSL_3"
    fi

    # Include more verbose output for failing tests run by CMake or make
    export CTEST_OUTPUT_ON_FAILURE=1

    # CFLAGS and LDFLAGS for Asan builds that don't use CMake
    # default to -O2, use -Ox _after_ this if you want another level
    ASAN_CFLAGS='-O2 -Werror -fsanitize=address,undefined -fno-sanitize-recover=all'
    # Normally, tests should use this compiler for ASAN testing
    ASAN_CC=clang

    # Platform tests have an allocation that returns null
    export ASAN_OPTIONS="allocator_may_return_null=1"
    export MSAN_OPTIONS="allocator_may_return_null=1"

    # Gather the list of available components. These are the functions
    # defined in this script whose name starts with "component_".
    ALL_COMPONENTS=$(compgen -A function component_ | sed 's/component_//')


    # Allow overriding PSASIM_PATH with the following priority:
    #  1) Environment override
    #  2) Mbed TLS's version of psa-client-server
    #  3) Fallback to the framework location (default after move)
    if [ -z "${PSASIM_PATH+set}" ]; then
        if [ -d tests/psa-client-server ]; then
            PSASIM_PATH='tests/psa-client-server/psasim/'
        else
            PSASIM_PATH='framework/psasim/'
        fi
    fi

    # Delay determining SUPPORTED_COMPONENTS until the command line options have a chance to override
    # the commands set by the environment
}

setup_quiet_wrappers()
{
    # Pick up "quiet" wrappers for make and cmake, which don't output very much
    # unless there is an error. This reduces logging overhead in the CI.
    #
    # Note that the cmake wrapper breaks unless we use an absolute path here.
    if [[ -e ${PWD}/framework/scripts/quiet ]]; then
        export PATH=${PWD}/framework/scripts/quiet:$PATH
    fi
}

# Test whether the component $1 is included in the command line patterns.
is_component_included()
{
    # Temporarily disable wildcard expansion so that $COMMAND_LINE_COMPONENTS
    # only does word splitting.
    set -f
    for pattern in $COMMAND_LINE_COMPONENTS; do
        set +f
        case ${1#component_} in $pattern) return 0;; esac
    done
    set +f
    return 1
}

usage()
{
    cat <<EOF
Usage: $0 [OPTION]... [COMPONENT]...
Run mbedtls release validation tests.
By default, run all tests. With one or more COMPONENT, run only those.
COMPONENT can be the name of a component or a shell wildcard pattern.

Examples:
  $0 "check_*"
    Run all sanity checks.
  $0 --no-armcc --except test_memsan
    Run everything except builds that require armcc and MemSan.

Special options:
  -h|--help             Print this help and exit.
  --list-all-components List all available test components and exit.
  --list-components     List components supported on this platform and exit.

General options:
  -q|--quiet            Only output component names, and errors if any.
  -f|--force            Force the tests to overwrite any modified files.
  -k|--keep-going       Run all tests and report errors at the end.
  -m|--memory           Additional optional memory tests.
     --append-outcome   Append to the outcome file (if used).
     --arm-none-eabi-gcc-prefix=<string>
                        Prefix for a cross-compiler for arm-none-eabi
                        (default: "${ARM_NONE_EABI_GCC_PREFIX}")
     --arm-linux-gnueabi-gcc-prefix=<string>
                        Prefix for a cross-compiler for arm-linux-gnueabi
                        (default: "${ARM_LINUX_GNUEABI_GCC_PREFIX}")
     --arm-linux-gnueabihf-gcc-prefix=<string>
                        Prefix for a cross-compiler for arm-linux-gnueabihf
                        (default: "${ARM_LINUX_GNUEABIHF_GCC_PREFIX}")
     --aarch64-linux-gnu-gcc-prefix=<string>
                        Prefix for a cross-compiler for aarch64-linux-gnu
                        (default: "${AARCH64_LINUX_GNU_GCC_PREFIX}")
     --armcc            Run ARM Compiler builds (on by default).
     --restore          First clean up the build tree, restoring backed up
                        files. Do not run any components unless they are
                        explicitly specified.
     --error-test       Error test mode: run a failing function in addition
                        to any specified component. May be repeated.
     --except           Exclude the COMPONENTs listed on the command line,
                        instead of running only those.
     --no-append-outcome    Write a new outcome file and analyze it (default).
     --no-armcc         Skip ARM Compiler builds.
     --no-force         Refuse to overwrite modified files (default).
     --no-keep-going    Stop at the first error (default).
     --no-memory        No additional memory tests (default).
     --no-quiet         Print full output from components.
     --out-of-source-dir=<path>  Directory used for CMake out-of-source build tests.
     --outcome-file=<path>  File where test outcomes are written (not done if
                            empty; default: \$MBEDTLS_TEST_OUTCOME_FILE).
     --random-seed      Use a random seed value for randomized tests (default).
  -r|--release-test     Run this script in release mode. This fixes the seed value to ${RELEASE_SEED}.
  -s|--seed             Integer seed value to use for this test run.

Tool path options:
     --armc6-bin-dir=<ARMC6_bin_dir_path>       ARM Compiler 6 bin directory.
     --clang-earliest=<Clang_earliest_path>     Earliest version of clang available
     --clang-latest=<Clang_latest_path>         Latest version of clang available
     --gcc-earliest=<GCC_earliest_path>         Earliest version of GCC available
     --gcc-latest=<GCC_latest_path>             Latest version of GCC available
     --gnutls-cli=<GnuTLS_cli_path>             GnuTLS client executable to use for most tests.
     --gnutls-serv=<GnuTLS_serv_path>           GnuTLS server executable to use for most tests.
     --openssl=<OpenSSL_path>                   OpenSSL executable to use for most tests.
     --openssl-next=<OpenSSL_path>              OpenSSL executable to use for recent things like ARIA
EOF
}

# list_git_files PATTERN...
# List files known to git, matching pattern.
# Equivalent to `git ls-files --recurse-submodules PATTERN...`, but
# works with older Git (especially on Ubuntu 16.04) that understand
# submodules but not `git ls-files --recurse-submodules`.
list_git_files ()
{
    git ls-files -- "$@"
    for d in $(git submodule status --recursive | awk '{print $2}'); do
        git ls-files "$@" | sed "s!^!$d/!"
    done
}

# Cleanup before/after running a component.
# Remove built files as well as the cmake cache/config.
# Does not remove generated source files.
cleanup()
{
    if in_mbedtls_repo; then
        command $MAKE_COMMAND clean
    fi

    # Remove files left over by an in-tree CMake build.
    # Take care to only hit in-tree builds, not out-of-tree builds in
    # subdirectories.
    # Remove **/Makefile only if it looks like it was created by an in-tree
    # CMake build.
    local cmakelists=($(list_git_files 'CMakeLists.txt' '**/CMakeLists.txt'))
    for f in "${cmakelists[@]}"; do
        local d="$(dirname -- "$f")"
        if [ -d "$d/CMakeFiles" ]; then
            rm -rf "$d/CMakeFiles" \
               "$d/cmake_install.cmake" \
               "$d/CTestTestfile.cmake" \
               "$d/CMakeCache.txt" \
               "$d/Makefile"
            rm -rf "$d/cmake"/*.cmake
        fi
    done

    # Remove any artifacts from the component_test_cmake_as_subdirectory test.
    rm -rf programs/test/cmake_subproject/build
    rm -f programs/test/cmake_subproject/Makefile
    rm -f programs/test/cmake_subproject/cmake_subproject

    # Remove any artifacts from the component_test_cmake_as_package test.
    rm -rf programs/test/cmake_package/build
    rm -f programs/test/cmake_package/Makefile
    rm -f programs/test/cmake_package/cmake_package

    # Remove any artifacts from the component_test_cmake_as_installed_package test.
    rm -rf programs/test/cmake_package_install/build
    rm -f programs/test/cmake_package_install/Makefile
    rm -f programs/test/cmake_package_install/cmake_package_install

    # Remove out of source directory
    if in_tf_psa_crypto_repo; then
        rm -rf "$OUT_OF_SOURCE_DIR"
    fi

    # Restore files that may have been clobbered by the job
    restore_backed_up_files
}

# Restore files that may have been clobbered
restore_backed_up_files () {
    for x in $files_to_back_up; do
        if [[ -e "$x$backup_suffix" ]]; then
            cp -p "$x$backup_suffix" "$x"
        fi
    done
}

# Final cleanup when this script exits (except when exiting on a failure
# in non-keep-going mode).
final_cleanup () {
    cleanup

    for x in $files_to_back_up; do
        rm -f "$x$backup_suffix"
    done
}

# Executed on exit. May be redefined depending on command line options.
final_report () {
    :
}

fatal_signal () {
    final_cleanup
    final_report $1
    trap - $1
    kill -$1 $$
}

pre_set_signal_handlers () {
    trap 'fatal_signal HUP' HUP
    trap 'fatal_signal INT' INT
    trap 'fatal_signal TERM' TERM
}

# Number of processors on this machine. Used as the default setting
# for parallel make.
all_sh_nproc ()
{
    {
        nproc || # Linux
        sysctl -n hw.ncpuonline || # NetBSD, OpenBSD
        sysctl -n hw.ncpu || # FreeBSD
        echo 1
    } 2>/dev/null
}

msg()
{
    if [ -n "${current_component:-}" ]; then
        current_section="${current_component#component_}: $1"
    else
        current_section="$1"
    fi

    if [ $QUIET -eq 1 ]; then
        return
    fi

    echo ""
    echo "******************************************************************"
    echo "* $current_section "
    printf "* "; date
    echo "******************************************************************"
}

err_msg()
{
    echo "$1" >&2
}

check_tools()
{
    for tool in "$@"; do
        if ! `type "$tool" >/dev/null 2>&1`; then
            err_msg "$tool not found!"
            exit 1
        fi
    done
}

pre_parse_command_line () {
    COMMAND_LINE_COMPONENTS=
    all_except=0
    error_test=0
    list_components=0
    restore_first=0
    no_armcc=

    # Note that legacy options are ignored instead of being omitted from this
    # list of options, so invocations that worked with previous version of
    # all.sh will still run and work properly.
    while [ $# -gt 0 ]; do
        case "$1" in
            --append-outcome) append_outcome=1;;
            --arm-none-eabi-gcc-prefix) shift; ARM_NONE_EABI_GCC_PREFIX="$1";;
            --arm-linux-gnueabi-gcc-prefix) shift; ARM_LINUX_GNUEABI_GCC_PREFIX="$1";;
            --arm-linux-gnueabihf-gcc-prefix) shift; ARM_LINUX_GNUEABIHF_GCC_PREFIX="$1";;
            --aarch64-linux-gnu-gcc-prefix) shift; AARCH64_LINUX_GNU_GCC_PREFIX="$1";;
            --armcc) no_armcc=;;
            --armc6-bin-dir) shift; ARMC6_BIN_DIR="$1";;
            --clang-earliest) shift; CLANG_EARLIEST="$1";;
            --clang-latest) shift; CLANG_LATEST="$1";;
            --error-test) error_test=$((error_test + 1));;
            --except) all_except=1;;
            --force|-f) FORCE=1;;
            --gcc-earliest) shift; GCC_EARLIEST="$1";;
            --gcc-latest) shift; GCC_LATEST="$1";;
            --gnutls-cli) shift; GNUTLS_CLI="$1";;
            --gnutls-legacy-cli) shift;; # ignored for backward compatibility
            --gnutls-legacy-serv) shift;; # ignored for backward compatibility
            --gnutls-serv) shift; GNUTLS_SERV="$1";;
            --help|-h) usage; exit;;
            --keep-going|-k) KEEP_GOING=1;;
            --list-all-components) printf '%s\n' $ALL_COMPONENTS; exit;;
            --list-components) list_components=1;;
            --memory|-m) MEMORY=1;;
            --no-append-outcome) append_outcome=0;;
            --no-armcc) no_armcc=1;;
            --no-force) FORCE=0;;
            --no-keep-going) KEEP_GOING=0;;
            --no-memory) MEMORY=0;;
            --no-quiet) QUIET=0;;
            --openssl) shift; OPENSSL="$1";;
            --openssl-next) shift; OPENSSL_NEXT="$1";;
            --outcome-file) shift; MBEDTLS_TEST_OUTCOME_FILE="$1";;
            --out-of-source-dir) shift; OUT_OF_SOURCE_DIR="$1";;
            --quiet|-q) QUIET=1;;
            --random-seed) unset SEED;;
            --release-test|-r) SEED=$RELEASE_SEED;;
            --restore) restore_first=1;;
            --seed|-s) shift; SEED="$1";;
            -*)
                echo >&2 "Unknown option: $1"
                echo >&2 "Run $0 --help for usage."
                exit 120
                ;;
            *) COMMAND_LINE_COMPONENTS="$COMMAND_LINE_COMPONENTS $1";;
        esac
        shift
    done

    # Exclude components that are not supported on this platform.
    SUPPORTED_COMPONENTS=
    for component in $ALL_COMPONENTS; do
        case $(type "support_$component" 2>&1) in
            *' function'*)
                if ! support_$component; then continue; fi;;
        esac
        SUPPORTED_COMPONENTS="$SUPPORTED_COMPONENTS $component"
    done

    if [ $list_components -eq 1 ]; then
        printf '%s\n' $SUPPORTED_COMPONENTS
        exit
    fi

    # With no list of components, run everything.
    if [ -z "$COMMAND_LINE_COMPONENTS" ] && [ $restore_first -eq 0 ]; then
        all_except=1
    fi

    # --no-armcc is a legacy option. The modern way is --except '*_armcc*'.
    # Ignore it if components are listed explicitly on the command line.
    if [ -n "$no_armcc" ] && [ $all_except -eq 1 ]; then
        COMMAND_LINE_COMPONENTS="$COMMAND_LINE_COMPONENTS *_armcc*"
    fi

    # Error out if an explicitly requested component doesn't exist.
    if [ $all_except -eq 0 ]; then
        unsupported=0
        # Temporarily disable wildcard expansion so that $COMMAND_LINE_COMPONENTS
        # only does word splitting.
        set -f
        for component in $COMMAND_LINE_COMPONENTS; do
            set +f
            # If the requested name includes a wildcard character, don't
            # check it. Accept wildcard patterns that don't match anything.
            case $component in
                *[*?\[]*) continue;;
            esac
            case " $SUPPORTED_COMPONENTS " in
                *" $component "*) :;;
                *)
                    echo >&2 "Component $component was explicitly requested, but is not known or not supported."
                    unsupported=$((unsupported + 1));;
            esac
        done
        set +f
        if [ $unsupported -ne 0 ]; then
            exit 2
        fi
    fi

    # Build the list of components to run.
    RUN_COMPONENTS=
    for component in $SUPPORTED_COMPONENTS; do
        if is_component_included "$component"; [ $? -eq $all_except ]; then
            RUN_COMPONENTS="$RUN_COMPONENTS $component"
        fi
    done

    unset all_except
    unset no_armcc
}

pre_check_git () {
    if [ $FORCE -eq 1 ]; then
        rm -rf "$OUT_OF_SOURCE_DIR"
        git checkout-index -f -q $config_files
        cleanup
    else

        if [ -d "$OUT_OF_SOURCE_DIR" ]; then
            echo "Warning - there is an existing directory at '$OUT_OF_SOURCE_DIR'" >&2
            echo "You can either delete this directory manually, or force the test by rerunning"
            echo "the script as: $0 --force --out-of-source-dir $OUT_OF_SOURCE_DIR"
            exit 1
        fi

        for config in $config_files; do
            if ! git diff --quiet "$config"; then
                err_msg "Warning - the configuration file '$config' has been edited. "
                echo "You can either delete or preserve your work, or force the test by rerunning the"
                echo "script as: $0 --force"
                exit 1
            fi
        done
    fi
}

pre_restore_files () {
    # If the makefiles have been generated by a framework such as cmake,
    # restore them from git. If the makefiles look like modifications from
    # the ones checked into git, take care not to modify them. Whatever
    # this function leaves behind is what the script will restore before
    # each component.
    case "$(head -n1 Makefile)" in
        *[Gg]enerated*)
            git update-index --no-skip-worktree Makefile library/Makefile programs/Makefile tests/Makefile programs/fuzz/Makefile
            git checkout -- Makefile library/Makefile programs/Makefile tests/Makefile programs/fuzz/Makefile
            ;;
    esac
}

pre_back_up () {
    for x in $files_to_back_up; do
        cp -p "$x" "$x$backup_suffix"
    done
}

pre_setup_keep_going () {
    failure_count=0 # Number of failed components
    last_failure_status=0 # Last failure status in this component

    # See err_trap
    previous_failure_status=0
    previous_failed_command=
    previous_failure_funcall_depth=0
    unset report_failed_command

    start_red=
    end_color=
    if [ -t 1 ]; then
        case "${TERM:-}" in
            *color*|cygwin|linux|rxvt*|screen|[Eex]term*)
                start_red=$(printf '\033[31m')
                end_color=$(printf '\033[0m')
                ;;
        esac
    fi

    # Keep a summary of failures in a file. We'll print it out at the end.
    failure_summary_file=$PWD/all-sh-failures-$$.log
    : >"$failure_summary_file"

    # Whether it makes sense to keep a component going after the specified
    # command fails (test command) or not (configure or build).
    # This function normally receives the failing simple command
    # ($BASH_COMMAND) as an argument, but if $report_failed_command is set,
    # this is passed instead.
    # This doesn't have to be 100% accurate: all failures are recorded anyway.
    # False positives result in running things that can't be expected to
    # work. False negatives result in things not running after something else
    # failed even though they might have given useful feedback.
    can_keep_going_after_failure () {
        case "$1" in
            "msg "*) false;;
            "cd "*) false;;
            "diff "*) true;;
            *make*[\ /]tests*) false;; # make tests, make CFLAGS=-I../tests, ...
            *test*) true;; # make test, tests/stuff, env V=v tests/stuff, ...
            *make*check*) true;;
            "grep "*) true;;
            "[ "*) true;;
            "! "*) true;;
            *) false;;
        esac
    }

    # This function runs if there is any error in a component.
    # It must either exit with a nonzero status, or set
    # last_failure_status to a nonzero value.
    err_trap () {
        # Save $? (status of the failing command). This must be the very
        # first thing, before $? is overridden.
        last_failure_status=$?
        failed_command=${report_failed_command-$BASH_COMMAND}

        if [[ $last_failure_status -eq $previous_failure_status &&
              "$failed_command" == "$previous_failed_command" &&
              ${#FUNCNAME[@]} == $((previous_failure_funcall_depth - 1)) ]]
        then
            # The same command failed twice in a row, but this time one level
            # less deep in the function call stack. This happens when the last
            # command of a function returns a nonzero status, and the function
            # returns that same status. Ignore the second failure.
            previous_failure_funcall_depth=${#FUNCNAME[@]}
            return
        fi
        previous_failure_status=$last_failure_status
        previous_failed_command=$failed_command
        previous_failure_funcall_depth=${#FUNCNAME[@]}

        text="$current_section: $failed_command -> $last_failure_status"
        echo "${start_red}^^^^$text^^^^${end_color}" >&2
        echo "$text" >>"$failure_summary_file"

        # If the command is fatal (configure or build command), stop this
        # component. Otherwise (test command) keep the component running
        # (run more tests from the same build).
        if ! can_keep_going_after_failure "$failed_command"; then
            exit $last_failure_status
        fi
    }

    final_report () {
        if [ $failure_count -gt 0 ]; then
            echo
            echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
            echo "${start_red}FAILED: $failure_count components${end_color}"
            cat "$failure_summary_file"
            echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        elif [ -z "${1-}" ]; then
            echo "SUCCESS :)"
        fi
        if [ -n "${1-}" ]; then
            echo "Killed by SIG$1."
        fi
        rm -f "$failure_summary_file"
        if [ $failure_count -gt 0 ]; then
            exit 1
        fi
    }
}

# '! true' does not trigger the ERR trap. Arrange to trigger it, with
# a reasonably informative error message (not just "$@").
not () {
    if "$@"; then
        report_failed_command="! $*"
        false
        unset report_failed_command
    fi
}

pre_prepare_outcome_file () {
    case "$MBEDTLS_TEST_OUTCOME_FILE" in
      [!/]*) MBEDTLS_TEST_OUTCOME_FILE="$PWD/$MBEDTLS_TEST_OUTCOME_FILE";;
    esac
    if [ -n "$MBEDTLS_TEST_OUTCOME_FILE" ] && [ "$append_outcome" -eq 0 ]; then
        rm -f "$MBEDTLS_TEST_OUTCOME_FILE"
    fi
}

pre_print_configuration () {
    if [ $QUIET -eq 1 ]; then
        return
    fi

    msg "info: $0 configuration"
    echo "MEMORY: $MEMORY"
    echo "FORCE: $FORCE"
    echo "MBEDTLS_TEST_OUTCOME_FILE: ${MBEDTLS_TEST_OUTCOME_FILE:-(none)}"
    echo "SEED: ${SEED-"UNSET"}"
    echo
    echo "OPENSSL: $OPENSSL"
    echo "OPENSSL_NEXT: $OPENSSL_NEXT"
    echo "GNUTLS_CLI: $GNUTLS_CLI"
    echo "GNUTLS_SERV: $GNUTLS_SERV"
    echo "ARMC6_BIN_DIR: $ARMC6_BIN_DIR"
}

# Make sure the tools we need are available.
pre_check_tools () {
    # Build the list of variables to pass to output_env.sh.
    set env

    case " $RUN_COMPONENTS " in
        # Require OpenSSL and GnuTLS if running any tests (as opposed to
        # only doing builds). Not all tests run OpenSSL and GnuTLS, but this
        # is a good enough approximation in practice.
        *" test_"* | *" release_test_"*)
            # To avoid setting OpenSSL and GnuTLS for each call to compat.sh
            # and ssl-opt.sh, we just export the variables they require.
            export OPENSSL="$OPENSSL"
            export GNUTLS_CLI="$GNUTLS_CLI"
            export GNUTLS_SERV="$GNUTLS_SERV"
            # Avoid passing --seed flag in every call to ssl-opt.sh
            if [ -n "${SEED-}" ]; then
                export SEED
            fi
            set "$@" OPENSSL="$OPENSSL"
            set "$@" GNUTLS_CLI="$GNUTLS_CLI" GNUTLS_SERV="$GNUTLS_SERV"
            check_tools "$OPENSSL" "$OPENSSL_NEXT" \
                        "$GNUTLS_CLI" "$GNUTLS_SERV"
            ;;
    esac

    case " $RUN_COMPONENTS " in
        *_doxygen[_\ ]*) check_tools "doxygen" "dot";;
    esac

    case " $RUN_COMPONENTS " in
        *_arm_none_eabi_gcc[_\ ]*) check_tools "${ARM_NONE_EABI_GCC_PREFIX}gcc";;
    esac

    case " $RUN_COMPONENTS " in
        *_mingw[_\ ]*) check_tools "i686-w64-mingw32-gcc";;
    esac

    case " $RUN_COMPONENTS " in
        *" test_zeroize "*) check_tools "gdb";;
    esac

    case " $RUN_COMPONENTS " in
        *_armcc*)
            ARMC6_CC="$ARMC6_BIN_DIR/armclang"
            ARMC6_LINK="$ARMC6_BIN_DIR/armlink"
            ARMC6_AR="$ARMC6_BIN_DIR/armar"
            ARMC6_FROMELF="$ARMC6_BIN_DIR/fromelf"
            check_tools "$ARMC6_CC" "$ARMC6_AR" "$ARMC6_FROMELF";;
    esac

    # past this point, no call to check_tool, only printing output
    if [ $QUIET -eq 1 ]; then
        return
    fi

    msg "info: output_env.sh"
    case $RUN_COMPONENTS in
        *_armcc*)
            set "$@" ARMC6_CC="$ARMC6_CC" RUN_ARMCC=1;;
        *) set "$@" RUN_ARMCC=0;;
    esac
    # Use a path relative to the currently-sourced file.
    "$@" "${BASH_SOURCE%/*}"/output_env.sh
}

pre_generate_files() {
    # since make doesn't have proper dependencies, remove any possibly outdate
    # file that might be around before generating fresh ones
    $MAKE_COMMAND neat
    if [ $QUIET -eq 1 ]; then
        $MAKE_COMMAND generated_files >/dev/null
    else
        $MAKE_COMMAND generated_files
    fi
}

pre_load_helpers () {
    # Use a path relative to the currently-sourced file.
    test_script_dir="${BASH_SOURCE%/*}"
    source "$test_script_dir"/all-helpers.sh
}

################################################################
#### Termination
################################################################

post_report () {
    msg "Done, cleaning up"
    final_cleanup

    final_report
}

################################################################
#### Run all the things
################################################################

# Function invoked by --error-test to test error reporting.
pseudo_component_error_test () {
    msg "Testing error reporting $error_test_i"
    if [ $KEEP_GOING -ne 0 ]; then
        echo "Expect three failing commands."
    fi
    # If the component doesn't run in a subshell, changing error_test_i to an
    # invalid integer will cause an error in the loop that runs this function.
    error_test_i=this_should_not_be_used_since_the_component_runs_in_a_subshell
    # Expected error: 'grep non_existent /dev/null -> 1'
    grep non_existent /dev/null
    # Expected error: '! grep -q . tests/scripts/all.sh -> 1'
    not grep -q . "$0"
    # Expected error: 'make unknown_target -> 2'
    $MAKE_COMMAND unknown_target
    false "this should not be executed"
}

# Run one component and clean up afterwards.
run_component () {
    current_component="$1"
    export MBEDTLS_TEST_CONFIGURATION="$current_component"

    # Unconditionally create a seedfile that's sufficiently long.
    # Do this before each component, because a previous component may
    # have messed it up or shortened it.
    local dd_cmd
    dd_cmd=(dd if=/dev/urandom of=./tests/seedfile bs=64 count=1)
    case $OSTYPE in
        linux*|freebsd*|openbsd*) dd_cmd+=(status=none)
    esac
    "${dd_cmd[@]}"

    if in_mbedtls_repo && in_4_x_branch; then
        dd_cmd=(dd if=/dev/urandom of=./tf-psa-crypto/tests/seedfile bs=64 count=1)
        case $OSTYPE in
            linux*|freebsd*|openbsd*) dd_cmd+=(status=none)
        esac
        "${dd_cmd[@]}"
    fi

    if in_tf_psa_crypto_repo; then
        pre_create_tf_psa_crypto_out_of_source_directory
    fi

    # Run the component in a subshell, with error trapping and output
    # redirection set up based on the relevant options.
    if [ $KEEP_GOING -eq 1 ]; then
        # We want to keep running if the subshell fails, so 'set -e' must
        # be off when the subshell runs.
        set +e
    fi
    (
        if [ $QUIET -eq 1 ]; then
            # msg() will be silenced, so just print the component name here.
            echo "${current_component#component_}"
            exec >/dev/null
        fi
        if [ $KEEP_GOING -eq 1 ]; then
            # Keep "set -e" off, and run an ERR trap instead to record failures.
            set -E
            trap err_trap ERR
        fi
        # The next line is what runs the component
        "$@"
        if [ $KEEP_GOING -eq 1 ]; then
            trap - ERR
            exit $last_failure_status
        fi
    )
    component_status=$?
    if [ $KEEP_GOING -eq 1 ]; then
        set -e
        if [ $component_status -ne 0 ]; then
            failure_count=$((failure_count + 1))
        fi
    fi

    # Restore the build tree to a clean state.
    cleanup
    unset current_component
}

pre_create_tf_psa_crypto_out_of_source_directory () {
    rm -rf "$OUT_OF_SOURCE_DIR"
    mkdir "$OUT_OF_SOURCE_DIR"
}

################################################################
#### Main
################################################################

main () {
    # Preliminary setup
    pre_set_shell_options
    pre_set_signal_handlers
    pre_check_environment
    pre_load_helpers
    pre_load_components
    pre_initialize_variables
    pre_parse_command_line "$@"

    setup_quiet_wrappers
    pre_check_git
    pre_restore_files
    pre_back_up

    build_status=0
    if [ $KEEP_GOING -eq 1 ]; then
        pre_setup_keep_going
    fi
    pre_prepare_outcome_file
    pre_print_configuration
    pre_check_tools
    cleanup
    if in_mbedtls_repo; then
        pre_generate_files
    fi

    # Run the requested tests.
    for ((error_test_i=1; error_test_i <= error_test; error_test_i++)); do
        run_component pseudo_component_error_test
    done
    unset error_test_i
    for component in $RUN_COMPONENTS; do
        run_component "component_$component"
    done

    # We're done.
    post_report
}
