"""Collect information about PSA cryptographic mechanisms.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import re
from collections import OrderedDict
from typing import List, Optional

from . import build_tree
from . import macro_collector


class Information:
    """Gather information about PSA constructors."""

    def __init__(self) -> None:
        self.constructors = self.read_psa_interface()

    @staticmethod
    def remove_unwanted_macros(
            constructors: macro_collector.PSAMacroEnumerator
    ) -> None:
        """Remove constructors that should be exckuded from systematic testing."""
        # Mbed TLS does not support finite-field DSA, but 3.6 defines DSA
        # identifiers for historical reasons.
        # Mbed TLS and TF-PSA-Crypto 1.0 do not support SPAKE2+, although
        # TF-PSA-Crypto 1.0 defines SPAKE2+ identifiers to be able to build
        # the psa-arch-tests compliance test suite.
        #
        # Don't attempt to generate any related test case.
        # The corresponding test cases would be commented out anyway,
        # but for these types, we don't have enough support in the test scripts
        # to generate these test cases.
        constructors.key_types.discard('PSA_KEY_TYPE_DSA_KEY_PAIR')
        constructors.key_types.discard('PSA_KEY_TYPE_DSA_PUBLIC_KEY')
        constructors.key_types.discard('PSA_KEY_TYPE_SPAKE2P_KEY_PAIR')
        constructors.key_types.discard('PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY')

    def read_psa_interface(self) -> macro_collector.PSAMacroEnumerator:
        """Return the list of known key types, algorithms, etc."""
        constructors = macro_collector.InputsForTest()

        if build_tree.looks_like_root('.'):
            if build_tree.looks_like_mbedtls_root('.') and \
               (not build_tree.is_mbedtls_3_6()):
                header_file_names = ['tf-psa-crypto/include/psa/crypto_values.h',
                                     'tf-psa-crypto/include/psa/crypto_extra.h']
                test_suites = ['tf-psa-crypto/tests/suites/test_suite_psa_crypto_metadata.data']
            else:
                header_file_names = ['include/psa/crypto_values.h',
                                     'include/psa/crypto_extra.h']
                test_suites = ['tests/suites/test_suite_psa_crypto_metadata.data']

        for header_file_name in header_file_names:
            constructors.parse_header(header_file_name)
        for test_cases in test_suites:
            constructors.parse_test_cases(test_cases)
        self.remove_unwanted_macros(constructors)
        constructors.gather_arguments()
        return constructors


def psa_want_symbol(name: str, prefix: Optional[str] = None) -> str:
    """Return the PSA_WANT_xxx symbol associated with a PSA crypto feature.

    You can use an altenative `prefix`, e.g. 'MBEDTLS_PSA_BUILTIN_'
    when specifically testing builtin implementations.
    """
    if prefix is None:
        prefix = 'PSA_WANT_'
    if name.startswith('PSA_'):
        return prefix + name[4:]
    else:
        raise ValueError('Unable to determine the PSA_WANT_ symbol for ' + name)

def finish_family_dependency(dep: str, bits: int) -> str:
    """Finish dep if it's a family dependency symbol prefix.

    A family dependency symbol prefix is a PSA_WANT_ symbol that needs to be
    qualified by the key size. If dep is such a symbol, finish it by adjusting
    the prefix and appending the key size. Other symbols are left unchanged.
    """
    return re.sub(r'_FAMILY_(.*)', r'_\1_' + str(bits), dep)

def finish_family_dependencies(dependencies: List[str], bits: int) -> List[str]:
    """Finish any family dependency symbol prefixes.

    Apply `finish_family_dependency` to each element of `dependencies`.
    """
    return [finish_family_dependency(dep, bits) for dep in dependencies]

SYMBOLS_WITHOUT_DEPENDENCY = frozenset([
    'PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG', # modifier, only in policies
    'PSA_ALG_AEAD_WITH_SHORTENED_TAG', # modifier
    'PSA_ALG_ANY_HASH', # only in policies
    'PSA_ALG_AT_LEAST_THIS_LENGTH_MAC', # modifier, only in policies
    'PSA_ALG_KEY_AGREEMENT', # chaining
    'PSA_ALG_TRUNCATED_MAC', # modifier
])
def automatic_dependencies(*expressions: str,
                           prefix: Optional[str] = None) -> List[str]:
    """Infer dependencies of a test case by looking for PSA_xxx symbols.

    The arguments are strings which should be C expressions. Do not use
    string literals or comments as this function is not smart enough to
    skip them.

    `prefix`: prefix to use in dependencies. Defaults to ``'PSA_WANT_'``.
              Use ``'MBEDTLS_PSA_BUILTIN_'`` when specifically testing
              builtin implementations.
    """
    used = set()
    for expr in expressions:
        used.update(re.findall(r'PSA_(?:ALG|ECC_FAMILY|DH_FAMILY|KEY_TYPE)_\w+', expr))
    used.difference_update(SYMBOLS_WITHOUT_DEPENDENCY)
    return sorted(psa_want_symbol(name, prefix=prefix) for name in used)

# Define set of regular expressions and dependencies to optionally append
# extra dependencies for test case based on key description.

# Skip AES test cases which require 192- or 256-bit key
# if MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH defined
AES_128BIT_ONLY_DEP_REGEX = re.compile(r'AES\s(192|256)')
AES_128BIT_ONLY_DEP = ['!MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH']
# Skip AES/ARIA/CAMELLIA test cases which require decrypt operation in ECB mode
# if MBEDTLS_BLOCK_CIPHER_NO_DECRYPT enabled.
ECB_NO_PADDING_DEP_REGEX = re.compile(r'(AES|ARIA|CAMELLIA).*ECB_NO_PADDING')
ECB_NO_PADDING_DEP = ['!MBEDTLS_BLOCK_CIPHER_NO_DECRYPT']

DEPENDENCY_FROM_DESCRIPTION = OrderedDict()
DEPENDENCY_FROM_DESCRIPTION[AES_128BIT_ONLY_DEP_REGEX] = AES_128BIT_ONLY_DEP
DEPENDENCY_FROM_DESCRIPTION[ECB_NO_PADDING_DEP_REGEX] = ECB_NO_PADDING_DEP
def generate_deps_from_description(
        description: str
    ) -> List[str]:
    """Return additional dependencies based on test case description and REGEX.
    """
    dep_list = []
    for regex, deps in DEPENDENCY_FROM_DESCRIPTION.items():
        if re.search(regex, description):
            dep_list += deps

    return dep_list

def tweak_key_pair_dependency(dep: str, usages: List[str]) -> List[str]:
    """
    This helper function add the proper suffix to PSA_WANT_KEY_TYPE_xxx_KEY_PAIR
    symbols according to the required usage.
    """
    if dep.endswith('KEY_PAIR'):
        return [dep + '_' + usage for usage in usages]
    return [dep]

def fix_key_pair_dependencies(dep_list: List[str], usages: List[str]) -> List[str]:
    new_list = [new_deps
                for dep in dep_list
                for new_deps in tweak_key_pair_dependency(dep, usages)]

    return new_list
