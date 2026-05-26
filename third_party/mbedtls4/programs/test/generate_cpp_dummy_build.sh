#!/bin/sh

DEFAULT_OUTPUT_FILE=programs/test/cpp_dummy_build.cpp

if [ "$1" = "--help" ]; then
    cat <<EOF
Usage: $0 [OUTPUT]
Generate a C++ dummy build program that includes all the headers.
OUTPUT defaults to "programs/test/cpp_dummy_build.cpp".
Run this program from the root of an Mbed TLS directory tree or from
its "programs" or "programs/test" subdirectory.
EOF
    exit
fi

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

set -e

# Ensure a reproducible order for *.h
export LC_ALL=C

print_cpp () {
    cat <<'EOF'
/* Automatically generated file. Do not edit.
 *
 *  This program is a dummy C++ program to ensure Mbed TLS library header files
 *  can be included and built with a C++ compiler.
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 *
 */

#include "mbedtls/build_info.h"

EOF

    for header in include/mbedtls/*.h; do
        case ${header#include/} in
            mbedtls/mbedtls_config.h) :;; # not meant for direct inclusion
            mbedtls/config_*.h) :;; # not meant for direct inclusion
            *) echo "#include \"${header#include/}\"";;
        esac
    done

    for header in tf-psa-crypto/drivers/builtin/include/mbedtls/*.h; do
        case ${header#tf-psa-crypto/drivers/builtin/include/} in
            mbedtls/config_*.h) :;; # not meant for direct inclusion
            *) echo "#include \"${header#tf-psa-crypto/drivers/builtin/include/}\"";;
        esac
    done

    if [ -d "tf-psa-crypto/include/mbedtls" ]; then
        for header in tf-psa-crypto/include/mbedtls/*.h; do
            echo "#include \"${header#tf-psa-crypto/include/}\""
        done
    fi

    for header in tf-psa-crypto/include/psa/*.h; do
        case ${header#tf-psa-crypto/include/} in
            psa/crypto_config.h) :;; # not meant for direct inclusion
            psa/crypto_ajdust_config*.h) :;; # not meant for direct inclusion
            # Some of the psa/crypto_*.h headers are not meant to be included
            # directly. They do have include guards that make them no-ops if
            # psa/crypto.h has been included before. Since psa/crypto.h comes
            # before psa/crypto_*.h in the wildcard enumeration, we don't need
            # to skip those headers.
            *) echo "#include \"${header#tf-psa-crypto/include/}\"";;
        esac
    done

    cat <<'EOF'

#include <iostream>

int main()
{
    std::cout << "CPP dummy build\n";

    mbedtls_platform_context *ctx = NULL;
    mbedtls_platform_setup(ctx);
    mbedtls_printf("CPP Build test passed\n");
    mbedtls_platform_teardown(ctx);
}
EOF
}

if [ -d include/mbedtls ]; then
    :
elif [ -d ../include/mbedtls ]; then
    cd ..
elif [ -d ../../include/mbedtls ]; then
    cd ../..
else
    echo >&2 "This script must be run from an Mbed TLS source tree."
    exit 3
fi

print_cpp >"${1:-$DEFAULT_OUTPUT_FILE}"
