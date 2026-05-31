#!/bin/bash

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

set -e

pkill psa_server || true

# Remove temporary files
rm -f psa_notify_*

# Remove all IPCs
# Not just ipcrm -all=msg as it is not supported on macOS.
# Filter out header and empty lines, choosing to select based on keys being
# output in hex.
ipcs -q | fgrep 0x | awk '{ printf " -q " $2 }' | xargs ipcrm > /dev/null 2>&1 || true
