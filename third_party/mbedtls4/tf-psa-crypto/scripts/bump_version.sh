#!/bin/bash
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# Purpose
#
# Sets the version numbers in the source code to those given.
#
# Usage: bump_version.sh [ --version <version> ] [ --so-crypto <version>]
#                           [ -v | --verbose ] [ -h | --help ]
#

set -e

VERSION=""

# Parse arguments
#
until [ -z "$1" ]
do
  case "$1" in
    --version)
      # Version to use
      shift
      VERSION=$1
      ;;
    --so-crypto)
      shift
      SO_CRYPTO=$1
      ;;
    -v|--verbose)
      # Be verbose
      VERBOSE="1"
      ;;
    -h|--help)
      # print help
      echo "Usage: $0"
      echo -e "  -h|--help\t\tPrint this help."
      echo -e "  --version <version>\tVersion to bump to."
      echo -e "  --so-crypto <version>\tSO version to bump libtfpsacrypto to."
      echo -e "  -v|--verbose\t\tVerbose."
      exit 0
      ;;
    *)
      # print error
      echo "Unknown argument: '$1'"
      exit 1
      ;;
  esac
  shift
done

if [ "X" = "X$VERSION" ];
then
  echo "No version specified. Unable to continue."
  exit 1
fi

[ $VERBOSE ] && echo "Bumping VERSION in CMakeLists.txt"
sed -e "s/(TF_PSA_CRYPTO_VERSION [0-9.]\{1,\})/(TF_PSA_CRYPTO_VERSION $VERSION)/g" < CMakeLists.txt > tmp
mv tmp CMakeLists.txt

if [ "X" != "X$SO_CRYPTO" ];
then
  [ $VERBOSE ] && echo "Bumping SOVERSION for libtfpsacrypto in CMakeLists.txt"
  sed -e "s/(TF_PSA_CRYPTO_SOVERSION [0-9]\{1,\})/(TF_PSA_CRYPTO_SOVERSION $SO_CRYPTO)/g" < CMakeLists.txt > tmp
  mv tmp CMakeLists.txt
fi

[ $VERBOSE ] && echo "Bumping VERSION in include/tf-psa-crypto/build_info.h"
read MAJOR MINOR PATCH <<<$(IFS="."; echo $VERSION)
VERSION_NR="$( printf "0x%02X%02X%02X00" $MAJOR $MINOR $PATCH )"
cat include/tf-psa-crypto/build_info.h |                                    \
    sed -e "s/\(# *define  *[A-Z_]*_VERSION\)_MAJOR .\{1,\}/\1_MAJOR  $MAJOR/" |    \
    sed -e "s/\(# *define  *[A-Z_]*_VERSION\)_MINOR .\{1,\}/\1_MINOR  $MINOR/" |    \
    sed -e "s/\(# *define  *[A-Z_]*_VERSION\)_PATCH .\{1,\}/\1_PATCH  $PATCH/" |    \
    sed -e "s/\(# *define  *[A-Z_]*_VERSION\)_NUMBER .\{1,\}/\1_NUMBER         $VERSION_NR/" |    \
    sed -e "s/\(# *define  *[A-Z_]*_VERSION\)_STRING .\{1,\}/\1_STRING         \"$VERSION\"/" |    \
    sed -e "s/\(# *define  *[A-Z_]*_VERSION\)_STRING_FULL .\{1,\}/\1_STRING_FULL    \"TF-PSA-Crypto $VERSION\"/" \
    > tmp
mv tmp include/tf-psa-crypto/build_info.h

[ $VERBOSE ] && echo "Bumping version in tests/suites/test_suite_tf_psa_crypto_version.data"
sed -e "s/version:\".\{1,\}/version:\"$VERSION\"/g" < tests/suites/test_suite_tf_psa_crypto_version.data > tmp
mv tmp tests/suites/test_suite_tf_psa_crypto_version.data
