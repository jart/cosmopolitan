#!/usr/bin/env python3
"""Generate test data for ecp functions.

The command line usage, class structure and available methods are the same
as in generate_bignum_tests.py.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import sys

from mbedtls_framework import test_data_generation
# Import modules containing additional test classes
# Test function classes in these modules will be registered by
# the framework
from mbedtls_framework import ecp # pylint: disable=unused-import

if __name__ == '__main__':
    # Use the section of the docstring relevant to the CLI as description
    test_data_generation.main(sys.argv[1:], "\n".join(__doc__.splitlines()[:4]))
