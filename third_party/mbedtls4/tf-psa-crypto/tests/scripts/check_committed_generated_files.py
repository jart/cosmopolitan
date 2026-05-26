#!/usr/bin/env python3

"""
Check that TF-PSA-Crypto files that can be regenerated are up-to-date, or update them.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import scripts_path # pylint: disable=unused-import
from mbedtls_framework import config_macros
from mbedtls_framework import generate_files_helper

GENERATORS = [
    config_macros.Current(shadow_missing_ok=True),
    generate_files_helper.TestDataGenerator('framework/util/generate_mldsa_tests.py'),
]

if __name__ == '__main__':
    generate_files_helper.main(generators=GENERATORS,
                               description=__doc__)
