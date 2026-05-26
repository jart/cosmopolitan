#!/bin/bash

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

set -e

# The server creates some local files when it starts up so we can wait for this
# event as signal that the server is ready so that we can start client(s).
function wait_for_server_startup() {
    SECONDS=0
    TIMEOUT=10

    while [ $(find . -name "psa_notify_*" | wc -l) -eq 0 ]; do
        if [ "$SECONDS" -ge "$TIMEOUT" ]; then
            echo "Timeout: psa_server not started within $TIMEOUT seconds."
            return 1
        fi
        sleep 0.1
    done
}

$(dirname "$0")/psa_server &
wait_for_server_startup
