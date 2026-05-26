#!/usr/bin/env python3
"""Bridge script
See framework/scripts/mbedtls_framework/interface_checks.py for detailed documentation.

This is a convenient place to encode any branch-specific information we might want to add
in the future.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import framework_scripts_path # pylint: disable=unused-import
from mbedtls_framework import interface_checks

if __name__ == "__main__":
    interface_checks.run_main()
