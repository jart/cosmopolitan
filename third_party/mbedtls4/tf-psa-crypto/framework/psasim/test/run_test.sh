#!/bin/bash

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This is a simple bash script that tests psa_client/psa_server interaction.
# This script is automatically executed when "make run" is launched by the
# "psasim" root folder. The script can also be launched manually once
# binary files are built (i.e. after "make test" is executed from the "psasim"
# root folder).

set -e

cd "$(dirname "$0")"

CLIENT_BIN=$1
shift

./kill_servers.sh

./start_server.sh
./$CLIENT_BIN "$@"

./kill_servers.sh
