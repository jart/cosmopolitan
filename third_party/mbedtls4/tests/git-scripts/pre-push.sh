#!/bin/sh
# pre-push.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#
# Purpose
#
# Called by "git push" after it has checked the remote status, but before anything has been
# pushed.  If this script exits with a non-zero status nothing will be pushed.
# This script can also be used independently, not using git.
#
# This hook is called with the following parameters:
#
# $1 -- Name of the remote to which the push is being done
# $2 -- URL to which the push is being done
#
# If pushing without using a named remote those arguments will be equal.
#
# Information about the commits which are being pushed is supplied as lines to
# the standard input in the form:
#
#   <local ref> <local sha1> <remote ref> <remote sha1>
#

REMOTE="$1"
URL="$2"

echo "REMOTE is $REMOTE"
echo "URL is $URL"

set -eu

tests/scripts/all.sh -q -k 'check_*'
