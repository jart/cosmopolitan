#!/bin/bash -eu

# compat-in-docker.sh
#
# Purpose
# -------
# This runs compat.sh in a Docker container.
#
# WARNING: the Dockerfile used by this script is no longer maintained! See
# https://github.com/Mbed-TLS/mbedtls-test/blob/master/README.md#quick-start
# for the set of Docker images we use on the CI.
#
# Notes for users
# ---------------
# If OPENSSL, GNUTLS_CLI, or GNUTLS_SERV are specified the path must
# correspond to an executable inside the Docker container. The special
# values "next" (OpenSSL only) and "legacy" are also allowed as shorthand
# for the installations inside the container.
#
# See also:
# - scripts/docker_env.sh for general Docker prerequisites and other information.
# - compat.sh for notes about invocation of that script.

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

source tests/scripts/docker_env.sh

case "${OPENSSL:-default}" in
    "legacy")  export OPENSSL="/usr/local/openssl-1.0.1j/bin/openssl";;
    "next")    export OPENSSL="/usr/local/openssl-1.1.1a/bin/openssl";;
    *) ;;
esac

case "${GNUTLS_CLI:-default}" in
    "legacy")  export GNUTLS_CLI="/usr/local/gnutls-3.3.8/bin/gnutls-cli";;
    "next")  export GNUTLS_CLI="/usr/local/gnutls-3.7.2/bin/gnutls-cli";;
    *) ;;
esac

case "${GNUTLS_SERV:-default}" in
    "legacy")  export GNUTLS_SERV="/usr/local/gnutls-3.3.8/bin/gnutls-serv";;
    "next")  export GNUTLS_SERV="/usr/local/gnutls-3.7.2/bin/gnutls-serv";;
    *) ;;
esac

run_in_docker \
    -e M_CLI \
    -e M_SRV \
    -e GNUTLS_CLI \
    -e GNUTLS_SERV \
    -e OPENSSL \
    -e OSSL_NO_DTLS \
    tests/compat.sh \
    $@
