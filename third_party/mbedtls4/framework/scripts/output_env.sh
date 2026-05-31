#! /usr/bin/env sh

# output_env.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# Purpose
#
# To print out all the relevant information about the development environment.
#
# This includes:
#   - architecture of the system
#   - type and version of the operating system
#   - version of make and cmake
#   - version of armcc, clang, gcc-arm and gcc compilers
#   - version of libc, clang, asan and valgrind if installed
#   - version of gnuTLS and OpenSSL

print_version()
{
    BIN="$1"
    shift
    ARGS="$1"
    shift
    VARIANT="$1"
    shift

    if [ -n "$VARIANT" ]; then
        VARIANT=" ($VARIANT)"
    fi

    if ! type "$BIN" > /dev/null 2>&1; then
        echo " * ${BIN##*/}$VARIANT: Not found."
        return 0
    fi

    BIN=`which "$BIN"`
    VERSION_STR=`$BIN $ARGS 2>&1`

    # Apply all filters
    while [ $# -gt 0 ]; do
        FILTER="$1"
        shift
        VERSION_STR=`echo "$VERSION_STR" | $FILTER`
    done

    if [ -z "$VERSION_STR" ]; then
        VERSION_STR="Version could not be determined."
    fi

    echo " * ${BIN##*/}$VARIANT: ${BIN} : ${VERSION_STR} "
}

echo "** Platform:"
echo

if [ `uname -s` = "Linux" ]; then
    echo "Linux variant"
    lsb_release -d -c
else
    echo "Unknown Unix variant"
fi

echo

print_version "uname" "-a" ""

echo
echo
echo "** Tool Versions:"
echo

print_version "make" "--version" "" "head -n 1"
echo

print_version "cmake" "--version" "" "head -n 1"
echo

if [ "${RUN_ARMCC:-1}" -ne 0 ]; then
    : "${ARMC6_CC:=armclang}"
    print_version "$ARMC6_CC" "--vsn" "" "head -n 2"
    echo
fi

print_version "arm-none-eabi-gcc" "--version" "" "head -n 1"
echo

print_version "gcc" "--version" "" "head -n 1"
echo

if [ -n "${GCC_EARLIEST+set}" ]; then
    print_version "${GCC_EARLIEST}" "--version" "" "head -n 1"
else
    echo " GCC_EARLIEST : Not configured."
fi
echo

if [ -n "${GCC_LATEST+set}" ]; then
    print_version "${GCC_LATEST}" "--version" "" "head -n 1"
else
    echo " GCC_LATEST : Not configured."
fi
echo

print_version "clang" "--version" "" "head -n 2"
echo

if [ -n "${CLANG_EARLIEST+set}" ]; then
    print_version "${CLANG_EARLIEST}" "--version" "" "head -n 2"
else
    echo " CLANG_EARLIEST : Not configured."
fi
echo

if [ -n "${CLANG_LATEST+set}" ]; then
    print_version "${CLANG_LATEST}" "--version" "" "head -n 2"
else
    echo " CLANG_LATEST : Not configured."
fi
echo

print_version "ldd" "--version" "" "head -n 1"
echo

print_version "valgrind" "--version" ""
echo

print_version "ctags" "--version" "" "head -n 1"
echo

print_version "gdb" "--version" "" "head -n 1"
echo

print_version "perl" "--version" "" "head -n 2" "grep ."
echo

print_version "python" "--version" "" "head -n 1"
echo

print_version "python3" "--version" "" "head -n 1"
echo

# Find the installed version of Pylint. Installed as a distro package this can
# be pylint3 and as a PEP egg, pylint. In test scripts We prefer pylint over
# pylint3
if type pylint >/dev/null 2>/dev/null; then
    print_version "pylint" "--version" "" "sed /^.*config/d" "grep pylint"
elif type pylint3 >/dev/null 2>/dev/null; then
    print_version "pylint3" "--version" "" "sed /^.*config/d" "grep pylint"
else
    echo " * pylint or pylint3: Not found."
fi
echo

: ${OPENSSL:=openssl}
print_version "$OPENSSL" "version" "default"
echo

if [ -n "${OPENSSL_NEXT+set}" ]; then
    print_version "$OPENSSL_NEXT" "version" "next"
else
    echo " * openssl (next): Not configured."
fi
echo

: ${GNUTLS_CLI:=gnutls-cli}
print_version "$GNUTLS_CLI" "--version" "default" "head -n 1"
echo

: ${GNUTLS_SERV:=gnutls-serv}
print_version "$GNUTLS_SERV" "--version" "default" "head -n 1"
echo

echo " * Installed asan versions:"
if type dpkg-query >/dev/null 2>/dev/null; then
    if ! dpkg-query -f '${Status} ${Package}: ${Version}\n' -W 'libasan*' |
         awk '$3 == "installed" && $4 !~ /-/ {print $4, $5}' |
         grep .
    then
        echo "   No asan versions installed."
    fi
else
    echo "  Unable to determine the asan version without dpkg."
fi
echo
