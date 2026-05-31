#!/usr/bin/env python3
"""
Generate miscellaneous TLS test cases relating to the handshake.

Transitional wrapper to facilitate the migration of consuming branches.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import sys
from mbedtls_framework import tls_handshake_tests

if __name__ == '__main__':
    sys.argv[1:1] = ["--no-tls12-client-hello-defragmentation-support"]
    tls_handshake_tests.main()
