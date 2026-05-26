#!/usr/bin/env bash
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
#
# HOL Light server for programmatic communication
# Based on https://github.com/monadius/hol_server
#
# Usage: hol-server [port]

set -e

PORT=${1:-2012}

# These are replaced by nix
HOL_LIGHT_DIR="@hol_light@/lib/hol_light"
HOL_SERVER_SRC="@hol_server_src@"

# cd to x86_64 proofs directory if in mldsa-native repo
PROOF_DIR="$(git rev-parse --show-toplevel 2>/dev/null)/proofs/hol_light/x86_64"
[ -d "$PROOF_DIR" ] && cd "$PROOF_DIR"

echo "Starting HOL Light server on port $PORT..."

{
  # Load required libraries for server2.ml
  echo '#directory "+unix";;'
  echo '#directory "+threads";;'
  echo '#load "unix.cma";;'
  echo '#load "threads.cma";;'

  # Load the server using #use (not loads) for proper evaluation
  echo "#use \"$HOL_SERVER_SRC/server2.ml\";;"

  # Start the server
  echo "start ~single_connection:false $PORT;;"

  # Keep stdin open for the server to continue running
  cat
} | exec "$HOL_LIGHT_DIR/hol.sh"
