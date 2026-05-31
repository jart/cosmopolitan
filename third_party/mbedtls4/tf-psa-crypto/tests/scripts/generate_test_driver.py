#!/usr/bin/env python3

# generate_test_driver.py
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

"""
Generate a TF-PSA-Crypto test driver
"""
import itertools
import sys

from pathlib import Path
from typing import Set

import scripts_path # pylint: disable=unused-import
from mbedtls_framework import build_tree
from mbedtls_framework import test_driver

from config import TFPSACryptoConfig

EXCLUDE_FILES = {
    "crypto_builtin_key_derivation.h",
}

IDENTIFIER_PREFIXES = {
    "MBEDTLS_",
    "PSA_",
    "TF_PSA_CRYPTO_",
    "mbedtls_",
    "psa_",
    "tf_psa_crypto_",
}

class TFPSACryptoTestDriverGenerator(test_driver.TestDriverGenerator):
    """ TF-PSA-Crypto test driver generator """
    def get_identifiers_to_prefix(self, prefixes: Set[str]) -> Set[str]:
        """
        Adjust the list of identifiers to prefix in the test driver code
        """
        identifiers = super().get_identifiers_to_prefix(prefixes)

        # Get from public and core headers the identifiers that the driver
        # built-in code may reference but does not define.
        directories = ("core", "include")
        files = itertools.chain.from_iterable(Path(directory).rglob("*.h") \
                                              for directory in directories)
        external_identifiers = set()
        for file in files:
            external_identifiers.update(self.get_c_identifiers(file))

        # MBEDTLS_PRIVATE is returned as a prototype by ctags when used in
        # structure members. Just remove it.
        external_identifiers.remove("MBEDTLS_PRIVATE")

        # ctags ignores the configuration options that are commented in
        # crypto_config.h. Ensure we have all of them.
        external_identifiers |= set(TFPSACryptoConfig().settings)

        identifiers.difference_update(external_identifiers)

        # MBEDTLS_ECP_LIGHT is enabled in crypto_adjust_config_enable_builtins.h
        # if either MBEDTLS_PK_PARSE_EC_EXTENDED or MBEDTLS_PK_PARSE_EC_COMPRESSED
        # is enabled. When compiling libtestdriver1 without any ECC support, this
        # triggers the pre-processor error:
        #   #error "Missing definition of LIBTESTDRIVER1_MBEDTLS_ECP_MAX_BITS"
        #
        # To prevent MBEDTLS_PK_PARSE_EC_EXTENDED and MBEDTLS_PK_PARSE_EC_COMPRESSED
        # from affecting libtestdriver1 (as they should not), we prefix them.
        # This turns them into the libtestdriver1-specific configuration options
        #   LIBTESTDRIVER1_MBEDTLS_PK_PARSE_EC_EXTENDED and
        #   LIBTESTDRIVER1_MBEDTLS_PK_PARSE_EC_COMPRESSED
        # which are never enabled. As a result, they do not trigger the
        # enablement of LIBTESTDRIVER1_MBEDTLS_ECP_LIGHT in
        # libtestdriver1-crypto_adjust_config_enable_builtins.h.
        identifiers.add("MBEDTLS_PK_PARSE_EC_EXTENDED")
        identifiers.add("MBEDTLS_PK_PARSE_EC_COMPRESSED")

        # MBEDTLS_PSA_ACCEL_ are not defined in the code base. They are
        # supposed to be passed as extra configuration options. We want to
        # prefix them, especially in 'crypto_adjust_config_enable_builtins.h',
        # thus deduce them from the PSA_WANT_ ones and add them to the list of
        # identifiers to prefix in the test driver code.
        for identifier in external_identifiers:
            if identifier.startswith("PSA_WANT_"):
                identifiers.add(identifier.replace("PSA_WANT_", "MBEDTLS_PSA_ACCEL_", 1))

        return identifiers

def main():
    """
    Main function of this program
    """
    parser = test_driver.get_parsearg_base()
    args = parser.parse_args()

    if not build_tree.looks_like_tf_psa_crypto_root("."):
        raise RuntimeError("This script must be run from TF-PSA-Crypto root.")

    generator = TFPSACryptoTestDriverGenerator(Path("drivers/builtin"), \
                                               Path(args.dst_dir), \
                                               args.driver, EXCLUDE_FILES)

    if args.list_vars_for_cmake:
        fname = f"{args.driver}-list-vars.cmake" \
                if args.list_vars_for_cmake == "__AUTO__" else args.list_vars_for_cmake
        generator.write_list_vars_for_cmake(fname)
        return

    #Create the test driver tree from `drivers/builtin`
    generator.create_test_driver_tree(IDENTIFIER_PREFIXES)

if __name__ == "__main__":
    sys.exit(main())
