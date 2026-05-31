#!/usr/bin/env python3

"""Generate C preprocessor code to check for bad configurations.
"""

from typing import Iterator

import framework_scripts_path # pylint: disable=unused-import
from mbedtls_framework.config_checks_generator import * \
    #pylint: disable=wildcard-import,unused-wildcard-import
from mbedtls_framework import config_macros

class CryptoInternal(SubprojectInternal):
    SUBPROJECT = 'TF-PSA-Crypto'

class CryptoOption(SubprojectOption):
    SUBPROJECT = 'psa/crypto_config.h'

ALWAYS_ENABLED_SINCE_4_0 = frozenset([
    'MBEDTLS_PSA_CRYPTO_CONFIG',
    'MBEDTLS_USE_PSA_CRYPTO',
])

def checkers_for_removed_options() -> Iterator[Checker]:
    """Discover removed options. Yield corresponding checkers."""
    previous_major = config_macros.History('mbedtls', '3.6')
    current = config_macros.Current()
    crypto = config_macros.Current('tf-psa-crypto')
    old_public = previous_major.options()
    new_public = current.options()
    for option in sorted(old_public - new_public):
        if option in ALWAYS_ENABLED_SINCE_4_0:
            continue
        if option in crypto.options():
            yield CryptoOption(option)
        elif option in crypto.internal():
            yield CryptoInternal(option)
        else:
            yield Removed(option, 'Mbed TLS 4.0')
    for option in sorted(current.internal() - new_public - old_public -
                         crypto.options() - crypto.internal()):
        yield Internal(option)

def all_checkers() -> Iterator[Checker]:
    """Yield all checkers."""
    yield from checkers_for_removed_options()

MBEDTLS_CHECKS = BranchData(
    header_directory='library',
    header_prefix='mbedtls_',
    project_cpp_prefix='MBEDTLS',
    checkers=list(all_checkers()),
)

if __name__ == '__main__':
    main(MBEDTLS_CHECKS)
