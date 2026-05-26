#!/usr/bin/env python3
"""Run the PSA Crypto API compliance test suite.

Transitional wrapper to facilitate the migration of consuming branches.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

from typing import List

from mbedtls_framework import psa_compliance

PSA_ARCH_TESTS_REF = 'v23.06_API1.5_ADAC_EAC'

# PSA Compliance tests we expect to fail due to known defects in Mbed TLS /
# TF-PSA-Crypto (or the test suite).
# The test numbers correspond to the numbers used by the console output of the test suite.
# Test number 2xx corresponds to the files in the folder
# psa-arch-tests/api-tests/dev_apis/crypto/test_c0xx
EXPECTED_FAILURES = [] # type: List[int]

if __name__ == '__main__':
    psa_compliance.main(PSA_ARCH_TESTS_REF, expected_failures=EXPECTED_FAILURES)
