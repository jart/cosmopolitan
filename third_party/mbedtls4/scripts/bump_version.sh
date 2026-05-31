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
#                           [ --so-x509 <version> ] [ --so-tls <version> ]
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
    --so-x509)
      shift
      SO_X509=$1
      ;;
    --so-tls)
      shift
      SO_TLS=$1
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
      echo -e "  --so-crypto <version>\tSO version to bump libmbedcrypto to."
      echo -e "  --so-x509 <version>\tSO version to bump libmbedx509 to."
      echo -e "  --so-tls <version>\tSO version to bump libmbedtls to."
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
sed -e "s/(MBEDTLS_VERSION [0-9.]\{1,\})/(MBEDTLS_VERSION $VERSION)/g" < CMakeLists.txt > tmp
mv tmp CMakeLists.txt

if [ "X" != "X$SO_CRYPTO" ];
then
  [ $VERBOSE ] && echo "Bumping SOVERSION for libmbedcrypto in CMakeLists.txt"
  sed -e "s/(MBEDTLS_CRYPTO_SOVERSION [0-9]\{1,\})/(MBEDTLS_CRYPTO_SOVERSION $SO_CRYPTO)/g" < CMakeLists.txt > tmp
  mv tmp CMakeLists.txt

  [ $VERBOSE ] && echo "Bumping SOVERSION for libmbedcrypto in library/Makefile"
  sed -e "s/SOEXT_CRYPTO?=so.[0-9]\{1,\}/SOEXT_CRYPTO?=so.$SO_CRYPTO/g" < library/Makefile > tmp
  mv tmp library/Makefile
fi

if [ "X" != "X$SO_X509" ];
then
  [ $VERBOSE ] && echo "Bumping SOVERSION for libmbedx509 in CMakeLists.txt"
  sed -e "s/(MBEDTLS_X509_SOVERSION [0-9]\{1,\})/(MBEDTLS_X509_SOVERSION $SO_X509)/g" < CMakeLists.txt > tmp
  mv tmp CMakeLists.txt

  [ $VERBOSE ] && echo "Bumping SOVERSION for libmbedx509 in library/Makefile"
  sed -e "s/SOEXT_X509?=so.[0-9]\{1,\}/SOEXT_X509?=so.$SO_X509/g" < library/Makefile > tmp
  mv tmp library/Makefile
fi

if [ "X" != "X$SO_TLS" ];
then
  [ $VERBOSE ] && echo "Bumping SOVERSION for libmbedtls in CMakeLists.txt"
  sed -e "s/(MBEDTLS_TLS_SOVERSION [0-9]\{1,\})/(MBEDTLS_TLS_SOVERSION $SO_TLS)/g" < CMakeLists.txt > tmp
  mv tmp CMakeLists.txt

  [ $VERBOSE ] && echo "Bumping SOVERSION for libmbedtls in library/Makefile"
  sed -e "s/SOEXT_TLS?=so.[0-9]\{1,\}/SOEXT_TLS?=so.$SO_TLS/g" < library/Makefile > tmp
  mv tmp library/Makefile
fi

[ $VERBOSE ] && echo "Bumping VERSION in include/mbedtls/build_info.h"
read MAJOR MINOR PATCH <<<$(IFS="."; echo $VERSION)
VERSION_NR="$( printf "0x%02X%02X%02X00" $MAJOR $MINOR $PATCH )"
cat include/mbedtls/build_info.h |                                    \
    sed -e "s/\(# *define  *[A-Z]*_VERSION\)_MAJOR .\{1,\}/\1_MAJOR  $MAJOR/" |    \
    sed -e "s/\(# *define  *[A-Z]*_VERSION\)_MINOR .\{1,\}/\1_MINOR  $MINOR/" |    \
    sed -e "s/\(# *define  *[A-Z]*_VERSION\)_PATCH .\{1,\}/\1_PATCH  $PATCH/" |    \
    sed -e "s/\(# *define  *[A-Z]*_VERSION\)_NUMBER .\{1,\}/\1_NUMBER         $VERSION_NR/" |    \
    sed -e "s/\(# *define  *[A-Z]*_VERSION\)_STRING .\{1,\}/\1_STRING         \"$VERSION\"/" |    \
    sed -e "s/\(# *define  *[A-Z]*_VERSION\)_STRING_FULL .\{1,\}/\1_STRING_FULL    \"Mbed TLS $VERSION\"/" \
    > tmp
mv tmp include/mbedtls/build_info.h

[ $VERBOSE ] && echo "Bumping version in tests/suites/test_suite_version.data"
sed -e "s/version:\".\{1,\}/version:\"$VERSION\"/g" < tests/suites/test_suite_version.data > tmp
mv tmp tests/suites/test_suite_version.data

[ $VERBOSE ] && echo "Bumping PROJECT_NAME in doxygen/mbedtls.doxyfile and doxygen/input/doc_mainpage.h"
for i in doxygen/mbedtls.doxyfile doxygen/input/doc_mainpage.h;
do
  sed -e "s/\\([Mm]bed TLS v\\)[0-9][0-9.]*/\\1$VERSION/g" < $i > tmp
  mv tmp $i
done

[ $VERBOSE ] && echo "Re-generating library/error.c"
scripts/generate_errors.pl

[ $VERBOSE ] && echo "Re-generating programs/test/query_config.c"
scripts/generate_query_config.pl

[ $VERBOSE ] && echo "Re-generating library/version_features.c"
scripts/generate_features.pl

