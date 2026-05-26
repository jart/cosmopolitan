#!/usr/bin/env python3
"""Generate, check and list the generated files
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import sys
from pathlib import Path

import framework_scripts_path # pylint: disable=unused-import

from mbedtls_framework import build_tree
from mbedtls_framework import generated_files
from mbedtls_framework.generated_files import GenerationScript, get_generation_script_files

GENERATION_SCRIPTS = [
    GenerationScript(
        Path("scripts/generate_errors.pl"),
        [Path("library/error.c")],
        None, "tf-psa-crypto/drivers/builtin/include/mbedtls \
               include/mbedtls/ \
               scripts/data_files"
    ),
    GenerationScript(
        Path("scripts/generate_features.pl"),
        [Path("library/version_features.c")],
        None, "include/mbedtls/ scripts/data_files"
    ),
    GenerationScript(
        Path("framework/scripts/generate_ssl_debug_helpers.py"),
        [Path("library/ssl_debug_helpers_generated.c")],
        "", None
    ),
    GenerationScript(
        Path("framework/scripts/generate_test_keys.py"),
        [Path("tests/include/test/test_keys.h")],
        None, "--output"
    ),
    GenerationScript(
        Path("framework/scripts/generate_test_cert_macros.py"),
        [Path("tests/include/test/test_certs.h")],
        None, "--output"
    ),
    GenerationScript(
        Path("scripts/generate_query_config.pl"),
        [Path("programs/test/query_config.c")],
        None, "include/mbedtls/mbedtls_config.h \
               tf-psa-crypto/include/psa/crypto_config.h \
               scripts/data_files/query_config.fmt"
    ),
    GenerationScript(
        Path("framework/scripts/generate_config_tests.py"),
        get_generation_script_files("framework/scripts/generate_config_tests.py"),
        "--directory", None
    ),
    GenerationScript(
        Path("framework/scripts/generate_tls13_compat_tests.py"),
        [Path("tests/opt-testcases/tls13-compat.sh")],
        None, "--output"
    ),
    GenerationScript(
        Path("scripts/generate_tls_handshake_tests.py"),
        [Path("tests/opt-testcases/handshake-generated.sh")],
        None, "--output"
    ),
    GenerationScript(
        Path("scripts/generate_config_checks.py"),
        get_generation_script_files("scripts/generate_config_checks.py"),
        output_dir_option="",
        optional=True)
]

def main() -> int:
    if not build_tree.looks_like_mbedtls_root("."):
        raise RuntimeError("This script must be run from Mbed TLS.")

    return generated_files.main(GENERATION_SCRIPTS)

if __name__ == "__main__":
    sys.exit(main())
