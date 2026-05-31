#!/bin/bash -eu

# make-in-docker.sh
#
# Purpose
# -------
# This runs make in a Docker container.
#
# See also:
# - scripts/docker_env.sh for general Docker prerequisites and other information.
#
# WARNING: the Dockerfile used by this script is no longer maintained! See
# https://github.com/Mbed-TLS/mbedtls-test/blob/master/README.md#quick-start
# for the set of Docker images we use on the CI.

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

source tests/scripts/docker_env.sh

run_in_docker make $@
