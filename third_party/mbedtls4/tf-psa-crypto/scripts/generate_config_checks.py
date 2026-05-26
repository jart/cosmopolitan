#!/usr/bin/env python3

"""Generate C preprocessor code to check for bad configurations.
"""

from typing import Iterator

import framework_scripts_path # pylint: disable=unused-import
from mbedtls_framework.config_checks_generator import * \
    #pylint: disable=wildcard-import,unused-wildcard-import
from mbedtls_framework import config_macros

ALWAYS_ENABLED_SINCE_1_0 = frozenset([
    'MBEDTLS_PSA_CRYPTO_CONFIG',
    'MBEDTLS_USE_PSA_CRYPTO',
])

def checkers_for_removed_options() -> Iterator[Checker]:
    """Discover removed options. Yield corresponding checkers."""
    previous_major = config_macros.History('mbedtls', '3.6')
    # Query historical data about Mbed TLS. We don't query live data
    # because TF-PSA-Crypto has to be able to compile on its own, without
    # a surrounding Mbed TLS source tree.
    tls = config_macros.History('mbedtls', '4.0')
    current = config_macros.Current()
    # Don't complain about options that have moved to Mbed TLS!
    # It's perfectly fine to set Mbed TLS 4.x options in the
    # TF-PSA-Crypto 1.x config file.
    # (If we re-add an option in Mbed TLS 4.x after removing it in 4.0,
    # we'll need to update our tls reference to avoid a complaint here.)
    new_public = current.options() | tls.options()
    old_public = previous_major.options()
    for option in sorted(old_public - new_public):
        if option in ALWAYS_ENABLED_SINCE_1_0:
            continue
        yield Removed(option, 'TF-PSA_Crypto 1.0')
    for option in sorted(current.internal() - new_public - old_public):
        # Macros describing accelerator drivers are not in the config
        # file, but it's ok if integrators put them there.
        if option.startswith('MBEDTLS_PSA_ACCEL_'):
            continue
        yield Internal(option)

def all_checkers() -> Iterator[Checker]:
    """Yield all checkers."""
    yield from checkers_for_removed_options()

CRYPTO_CHECKS = BranchData(
    header_directory='core',
    header_prefix='tf_psa_crypto_',
    project_cpp_prefix='TF_PSA_CRYPTO',
    checkers=list(all_checkers()),
)

if __name__ == '__main__':
    main(CRYPTO_CHECKS)
