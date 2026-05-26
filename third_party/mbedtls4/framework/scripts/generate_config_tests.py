#!/usr/bin/env python3
"""Generate test data for configuration reporting.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import inspect
import re
import sys
from typing import Iterable, Iterator, List, Optional, Tuple

import project_scripts # pylint: disable=unused-import
import config
from mbedtls_framework import build_tree
from mbedtls_framework import config_common
from mbedtls_framework import test_case
from mbedtls_framework import test_data_generation


def single_setting_case(setting: config_common.Setting, when_on: bool,
                        dependencies: List[str],
                        note: Optional[str]) -> test_case.TestCase:
    """Construct a test case for a boolean setting.

    This test case passes if the setting and its dependencies are enabled,
    and is skipped otherwise.

    * setting: the setting to be tested.
    * when_on: True to test with the setting enabled, or False to test
      with the setting disabled.
    * dependencies: extra dependencies for the test case.
    * note: a note to add after the setting name in the test description.
      This is generally a summary of dependencies, and is generally empty
      if the given setting is only tested once.
    """
    base = setting.name if when_on else '!' + setting.name
    tc = test_case.TestCase()
    tc.set_function('pass')
    description_suffix = ' (' + note + ')' if note else ''
    tc.set_description('Config: ' + base + description_suffix)
    tc.set_dependencies([base] + dependencies)
    return tc


PSA_WANT_KEY_TYPE_KEY_PAIR_RE = \
    re.compile(r'(?P<prefix>PSA_WANT_KEY_TYPE_(?P<type>\w+)_KEY_PAIR_)(?P<operation>\w+)\Z')

# If foo is a setting that is only meaningful when bar is enabled, set
# SIMPLE_DEPENDENCIES[foo]=bar. More generally, bar can be a colon-separated
# list of settings, meaning that all the settings must be enabled. Each setting
# in bar can be prefixed with '!' to negate it. This is the same syntax as a
# depends_on directive in test data.
# See also `dependencies_of_settting`.
SIMPLE_DEPENDENCIES = {
    'MBEDTLS_AESCE_C': 'MBEDTLS_AES_C',
    'MBEDTLS_AESNI_C': 'MBEDTLS_AES_C',
    'MBEDTLS_ERROR_STRERROR_DUMMY': '!MBEDTLS_ERROR_C',
    'MBEDTLS_GENPRIME': 'MBEDTLS_RSA_C',
    'MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES': 'MBEDTLS_ENTROPY_C',
    'MBEDTLS_PKCS1_V15': 'MBEDTLS_RSA_C',
    'MBEDTLS_PKCS1_V21': 'MBEDTLS_RSA_C',
    'MBEDTLS_PSA_CRYPTO_CLIENT': '!MBEDTLS_PSA_CRYPTO_C',
    'MBEDTLS_PSA_INJECT_ENTROPY': 'MBEDTLS_PSA_CRYPTO_C',
    'MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS': 'MBEDTLS_PSA_CRYPTO_C',
}

if build_tree.is_mbedtls_3_6():
    SIMPLE_DEPENDENCIES['MBEDTLS_NO_PLATFORM_ENTROPY'] = 'MBEDTLS_ENTROPY_C'

BUILTIN_MODULE_ENABLEMENT_MACROS = [
    'MBEDTLS_AES_C', 'MBEDTLS_CAMELLIA_C', 'MBEDTLS_GCM_C',
    'MBEDTLS_ECP_C',
    'MBEDTLS_RSA_C',
    'MBEDTLS_SHA256_C', 'MBEDTLS_SHA512_C',
]

def dependencies_of_setting(cfg: config_common.Config,
                            setting: config_common.Setting) -> Optional[str]:
    """Return dependencies without which a setting is not meaningful.

    The dependencies of a setting express when a setting can be enabled and
    is relevant. For example, if ``check_config.h`` errors out when
    ``defined(FOO) && !defined(BAR)``, then ``BAR`` is a dependency of ``FOO``.
    If ``FOO`` has no effect when ``CORGE`` is disabled, then ``CORGE``
    is a dependency of ``FOO``.

    The return value can be a colon-separated list of settings, if the setting
    is only meaningful when all of these settings are enabled. Each setting can
    be negated by prefixing them with '!'. This is the same syntax as a
    depends_on directive in test data.
    """
    #pylint: disable=too-many-branches,too-many-return-statements
    name = setting.name
    if name in SIMPLE_DEPENDENCIES:
        return SIMPLE_DEPENDENCIES[name]
    if name.startswith('MBEDTLS_') and not name.endswith('_C'):
        if name.startswith('MBEDTLS_CIPHER_PADDING_'):
            return 'MBEDTLS_CIPHER_C:MBEDTLS_CIPHER_MODE_CBC'
        if name.startswith('MBEDTLS_PK_PARSE_EC_'):
            return 'MBEDTLS_PK_C:' + test_case.psa_or_3_6_feature_macro(
                'PSA_KEY_TYPE_ECC_PUBLIC_KEY', test_case.Domain36.USE_PSA)

        # For TLS settings, insist on having them once off and once on in
        # a configuration where both client support and server support are
        # enabled. The settings are also meaningful when only one side is
        # enabled, but there isn't much point in having separate records
        # for client-side and server-side, so we keep things simple.
        # Requiring both sides to be enabled also means we know we'll run
        # tests that only run Mbed TLS against itself, which only run in
        # configurations with both sides enabled.
        if name.startswith('MBEDTLS_SSL_TLS1_3_') or \
           name == 'MBEDTLS_SSL_EARLY_DATA':
            return 'MBEDTLS_SSL_CLI_C:MBEDTLS_SSL_SRV_C:MBEDTLS_SSL_PROTO_TLS1_3'
        if name.startswith('MBEDTLS_SSL_DTLS_'):
            return 'MBEDTLS_SSL_CLI_C:MBEDTLS_SSL_SRV_C:MBEDTLS_SSL_PROTO_DTLS'
        if name.startswith('MBEDTLS_SSL_'):
            return 'MBEDTLS_SSL_CLI_C:MBEDTLS_SSL_SRV_C'
        if name == 'MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED' and \
           not build_tree.is_mbedtls_3_6():
            # In 1.x the module ecdh.c is removed. This option remains, with its
            # historical name for compatibility. It is still only relevant when
            # the built-in implementation of ECDH is enabled, but this condition
            # is no longer expressed as MBEDTLS_ECDH_C.
            return 'MBEDTLS_PSA_BUILTIN_ALG_ECDH'
        for pos in re.finditer(r'_', name):
            super_name = name[:pos.start()] + '_C'
            if cfg.known(super_name):
                return super_name
            # If super_name refers to a macro that still enables a
            # cryptographic module, but is no longer exposed as a configuration
            # option in 4.0/1.0, return it as a dependency.
            if super_name in BUILTIN_MODULE_ENABLEMENT_MACROS:
                return super_name
    if name.startswith('PSA_WANT_'):
        deps = 'MBEDTLS_PSA_CRYPTO_CLIENT'
        m = PSA_WANT_KEY_TYPE_KEY_PAIR_RE.match(name)
        if m and m.group('operation') != 'BASIC':
            deps += ':' + m.group('prefix') + 'BASIC'
        return deps
    return None

def conditions_for_setting(cfg: config_common.Config,
                           setting: config_common.Setting
                           ) -> Iterator[Tuple[List[str], str]]:
    """Enumerate the conditions under which to test the given setting.

    * cfg: all configuration settings.
    * setting: the setting to be tested.

    Generate a stream of conditions, i.e. extra dependencies to test with
    together with a human-readable explanation of each dependency. Some
    typical cases:

    * By default, generate a one-element stream with no extra dependencies.
    * If the setting is ignored unless some other setting is enabled, generate
      a one-element stream with that other setting as an extra dependency.
    * If the setting is known to interact with some other setting, generate
      a stream with one element where this setting is on and one where it's off.
    * To skip the setting altogether, generate an empty stream.
    """
    name = setting.name
    if name.endswith('_ALT') and not config.is_seamless_alt(name):
        # We don't test alt implementations, except (most) platform alts
        return
    dependencies = dependencies_of_setting(cfg, setting)
    if dependencies:
        yield [dependencies], ''
        return
    yield [], ''


def enumerate_boolean_setting_cases(cfg: config_common.Config
                                   ) -> Iterable[test_case.TestCase]:
    """Emit test cases for all boolean settings."""
    for name in sorted(cfg.settings.keys()):
        setting = cfg.settings[name]
        if not name.startswith('PSA_WANT_') and setting.value:
            continue # non-boolean setting
        for when_on in True, False:
            for deps, note in conditions_for_setting(cfg, setting):
                yield single_setting_case(setting, when_on, deps, note)



class ConfigTestGenerator(test_data_generation.TestGenerator):
    """Generate test cases for configuration reporting."""

    def __init__(self, settings):
        # pylint: disable=no-member
        config_members = dict(inspect.getmembers(config))
        if 'MbedTLSConfig' in config_members:
            self.mbedtls_config = config.MbedTLSConfig()
            self.targets['test_suite_config.mbedtls_boolean'] = \
                lambda: enumerate_boolean_setting_cases(self.mbedtls_config)
        if 'CryptoConfig' in config_members:
            if build_tree.is_mbedtls_3_6():
                self.psa_config = config.CryptoConfig()
                self.targets['test_suite_config.psa_boolean'] = \
                    lambda: enumerate_boolean_setting_cases(self.psa_config)
        elif 'TFPSACryptoConfig' in config_members:
            self.psa_config = config.TFPSACryptoConfig()
            self.targets['test_suite_config.psa_boolean'] = \
                lambda: enumerate_boolean_setting_cases(self.psa_config)
        super().__init__(settings)


if __name__ == '__main__':
    test_data_generation.main(sys.argv[1:], __doc__, ConfigTestGenerator)
