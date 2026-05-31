#!/usr/bin/env python3
"""Run the PSA Crypto API compliance test suite.

Clone the repo and check out the commit specified by PSA_ARCH_TEST_REPO and PSA_ARCH_TEST_REF,
then compile and run the test suite. The clone is stored at <repository root>/psa-arch-tests.
Known defects in either the test suite or Mbed TLS - identified by their test
number - are ignored, while unexpected failures AND successes are reported as errors, to help
keep the list of known defects as up to date as possible.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

from typing import List

import scripts_path # pylint: disable=unused-import
from mbedtls_framework import psa_compliance

PSA_ARCH_TESTS_REF = 'v25.02_API1.8_CRYPTO_1.2.1'

# PSA Compliance tests we expect to fail due to known defects in Mbed TLS /
# TF-PSA-Crypto (or the test suite).
# The test numbers correspond to the numbers used by the console output of the test suite.
# Test number 2xx corresponds to the files in the folder
# psa-arch-tests/api-tests/dev_apis/crypto/test_c0xx
EXPECTED_FAILURES = [
] # type: List[int]

if __name__ == '__main__':
    psa_compliance.main(PSA_ARCH_TESTS_REF,
                        expected_failures=EXPECTED_FAILURES)
