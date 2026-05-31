# project-detection.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# Purpose
#
# This script contains functions for shell scripts to
# help detect which project (Mbed TLS, TF-PSA-Crypto)
# or which Mbed TLS branch they are in.

# Project detection.
#
# Both Mbed TLS and TF-PSA-Cryto repos have a "scripts/project_name.txt" file
# which contains the name of the project. They are used in scripts to know in
# which project/folder we're in.
# This function accepts 2 parameters:
# - $1: boolean value which defines the behavior in case
#       "scripts/project_name.txt" is not found:
#       - 1: exit with error message
#       - 0: simply return an error
# - $2: mandatory value which defined the root folder where to look for
#       "scripts/project_name.txt".
read_project_name_file () {
    EXIT_IF_NOT_FOUND=$1
    ROOT_PATH=$2

    PROJECT_NAME_FILE="scripts/project_name.txt"

    # Check if file exists.
    if [ ! -f "$ROOT_PATH/$PROJECT_NAME_FILE" ]; then
        if $EXIT_IF_NOT_FOUND ; then
            echo "$ROOT_PATH/$PROJECT_NAME_FILE does not exist... Exiting..." >&2
            exit 1
        fi
        # Simply return a failure in case we were asked not to fail in case of
        # missing file.
        return 1
    fi

    if read -r PROJECT_NAME < "$ROOT_PATH/$PROJECT_NAME_FILE"; then :; else
        return 1
    fi
}

# Check if the current folder is the Mbed TLS root one.
#
# Warning: if this is not run from Mbed TLS/TF-PSA-Crypto root folder, the
# script is terminated with a failure.
in_mbedtls_repo () {
    read_project_name_file true .
    test "$PROJECT_NAME" = "Mbed TLS"
}

# Check if the current folder is the TF-PSA-Crypto root one.
#
# Warning: if this is not run from Mbed TLS/TF-PSA-Crypto root folder, the
# script is terminated with a failure.
in_tf_psa_crypto_repo () {
    read_project_name_file true .
    test "$PROJECT_NAME" = "TF-PSA-Crypto"
}

# Check if $1 is an Mbed TLS root folder.
#
# Differently from in_mbedtls_repo() above, this can be run on any folder
# without causing the script to exit.
is_mbedtls_root() {
    if ! read_project_name_file false $1 ; then
        return 1
    fi

    test "$PROJECT_NAME" = "Mbed TLS"
}

# Check if $1 is a TF-PSA-Crypto root folder.
#
# Differently from in_tf_psa_crypto_repo() above, this can be run on any folder
# without causing the script to exit.
is_tf_psa_crypto_root() {
    if ! read_project_name_file false $1 ; then
        return 1
    fi

    test "$PROJECT_NAME" = "TF-PSA-Crypto"
}

#Branch detection
read_build_info () {
    SCRIPT_DIR=$(pwd)

    BUILD_INFO_FILE="include/mbedtls/build_info.h"

    if [ ! -f "$BUILD_INFO_FILE" ]; then
        echo "File $BUILD_INFO_FILE not found."
        exit 1
    fi

    MBEDTLS_VERSION_MAJOR=$(grep "^#define MBEDTLS_VERSION_MAJOR" "$BUILD_INFO_FILE" | awk '{print $3}')
    MBEDTLS_VERSION_MINOR=$(grep "^#define MBEDTLS_VERSION_MINOR" "$BUILD_INFO_FILE" | awk '{print $3}')

    if [ -z "$MBEDTLS_VERSION_MAJOR" ]; then
        echo "MBEDTLS_VERSION_MAJOR not found in $BUILD_INFO_FILE."
        exit 1
    fi

    if [ -z "$MBEDTLS_VERSION_MINOR" ]; then
        echo "MBEDTLS_VERSION_MINOR not found in $BUILD_INFO_FILE."
        exit 1
    fi
}

in_3_6_branch () {
    read_build_info
    test $MBEDTLS_VERSION_MAJOR = "3" && test $MBEDTLS_VERSION_MINOR = "6"
}

in_4_x_branch () {
    read_build_info
    test $MBEDTLS_VERSION_MAJOR = "4"
}
