#!/usr/bin/env python3

"""Mbed TLS and PSA configuration file manipulation library and tool

Basic usage, to read the Mbed TLS configuration:
    config = CombinedConfigFile()
    if 'MBEDTLS_SSL_TLS_C' in config: print('TLS is enabled')
"""

## Copyright The Mbed TLS Contributors
## SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
##

import os
import re
import sys

import framework_scripts_path # pylint: disable=unused-import
from mbedtls_framework import config_common


def is_boolean_setting(name, value):
    """Is this a boolean setting?

    Mbed TLS boolean settings are enabled if the preprocessor macro is
    defined, and disabled if the preprocessor macro is not defined. The
    macro definition line in the configuration file has an empty expansion.

    PSA_WANT_xxx settings are also boolean, but when they are enabled,
    they expand to a nonzero value. We leave them undefined when they
    are disabled. (Setting them to 0 currently means to enable them, but
    this might change to mean disabling them. Currently we just never set
    them to 0.)
    """
    if name.startswith('PSA_WANT_'):
        return True
    if not value:
        return True
    return False

def realfull_adapter(_name, _value, _active):
    """Activate all symbols.

    This is intended for building the documentation, including the
    documentation of settings that are activated by defining an optional
    preprocessor macro. There is no expectation that the resulting
    configuration can be built.
    """
    return True

PSA_UNSUPPORTED_FEATURE = frozenset([
    'PSA_WANT_ALG_CBC_MAC',
    'PSA_WANT_ALG_XTS',
    'PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_DERIVE',
    'PSA_WANT_KEY_TYPE_DH_KEY_PAIR_DERIVE'
])

PSA_DEPRECATED_FEATURE = frozenset([
    'PSA_WANT_KEY_TYPE_ECC_KEY_PAIR',
    'PSA_WANT_KEY_TYPE_RSA_KEY_PAIR'
])

EXCLUDE_FROM_CRYPTO = PSA_UNSUPPORTED_FEATURE | \
                      PSA_DEPRECATED_FEATURE

# The goal of the full configuration is to have everything that can be tested
# together. This includes deprecated or insecure options. It excludes:
# * Options that require additional build dependencies or unusual hardware.
# * Options that make testing less effective.
# * Options that are incompatible with other options, or more generally that
#   interact with other parts of the code in such a way that a bulk enabling
#   is not a good way to test them.
# * Options that remove features.
EXCLUDE_FROM_FULL = frozenset([
    #pylint: disable=line-too-long
    'MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH', # interacts with CTR_DRBG_128_BIT_KEY
    'MBEDTLS_AES_USE_HARDWARE_ONLY', # hardware dependency
    'MBEDTLS_BLOCK_CIPHER_NO_DECRYPT', # incompatible with ECB in PSA, CBC/XTS/NIST_KW
    'MBEDTLS_DEPRECATED_REMOVED', # conflicts with deprecated options
    'MBEDTLS_DEPRECATED_WARNING', # conflicts with deprecated options
    'MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED', # influences the use of ECDH in TLS
    'MBEDTLS_ECP_WITH_MPI_UINT', # disables the default ECP and is experimental
    'MBEDTLS_HAVE_SSE2', # hardware dependency
    'MBEDTLS_MEMORY_BACKTRACE', # depends on MEMORY_BUFFER_ALLOC_C
    'MBEDTLS_MEMORY_BUFFER_ALLOC_C', # makes sanitizers (e.g. ASan) less effective
    'MBEDTLS_MEMORY_DEBUG', # depends on MEMORY_BUFFER_ALLOC_C
    'MBEDTLS_NO_64BIT_MULTIPLICATION', # influences anything that uses bignum
    'MBEDTLS_NO_UDBL_DIVISION', # influences anything that uses bignum
    'MBEDTLS_PSA_DRIVER_GET_ENTROPY', # incompatible with MBEDTLS_PSA_BUILTIN_GET_ENTROPY
    'MBEDTLS_PSA_P256M_DRIVER_ENABLED', # influences SECP256R1 KeyGen/ECDH/ECDSA
    'MBEDTLS_PLATFORM_NO_STD_FUNCTIONS', # removes a feature
    'MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS', # removes a feature
    'MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG', # behavior change + build dependency
    'MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER', # interface and behavior change
    'MBEDTLS_PSA_CRYPTO_SPM', # platform dependency (PSA SPM)
    'MBEDTLS_RSA_NO_CRT', # influences the use of RSA in X.509 and TLS
    'MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY', # interacts with *_USE_ARMV8_A_CRYPTO_IF_PRESENT
    'MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY', # interacts with *_USE_A64_CRYPTO_IF_PRESENT
    'MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN', # build dependency (clang+memsan)
    'MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND', # build dependency (valgrind headers)
    'MBEDTLS_X509_REMOVE_INFO', # removes a feature
    'MBEDTLS_PSA_STATIC_KEY_SLOTS', # only relevant for embedded devices
    'MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE', # only relevant for embedded devices
    *PSA_UNSUPPORTED_FEATURE,
    *PSA_DEPRECATED_FEATURE,
])

def is_seamless_alt(name):
    """Whether the xxx_ALT symbol should be included in the full configuration.

    Include alternative implementations of platform functions, which are
    configurable function pointers that default to the built-in function.
    This way we test that the function pointers exist and build correctly
    without changing the behavior, and tests can verify that the function
    pointers are used by modifying those pointers.

    Exclude alternative implementations of library functions since they require
    an implementation of the relevant functions and an xxx_alt.h header.
    """
    if name in (
            'MBEDTLS_PLATFORM_GMTIME_R_ALT',
            'MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT',
            'MBEDTLS_PLATFORM_MS_TIME_ALT',
            'MBEDTLS_PLATFORM_ZEROIZE_ALT',
    ):
        # Similar to non-platform xxx_ALT, requires platform_alt.h
        return False
    return name.startswith('MBEDTLS_PLATFORM_')

def include_in_full(name):
    """Rules for symbols in the "full" configuration."""
    if name in EXCLUDE_FROM_FULL:
        return False
    if name.endswith('_ALT'):
        return is_seamless_alt(name)
    return True

def full_adapter(name, value, active):
    """Config adapter for "full"."""
    if not is_boolean_setting(name, value):
        return active
    return include_in_full(name)

# The baremetal configuration excludes options that require a library or
# operating system feature that is typically not present on bare metal
# systems. Features that are excluded from "full" won't be in "baremetal"
# either (unless explicitly turned on in baremetal_adapter) so they don't
# need to be repeated here.
EXCLUDE_FROM_BAREMETAL = frozenset([
    #pylint: disable=line-too-long
    'MBEDTLS_ENTROPY_NV_SEED', # requires a filesystem and FS_IO or alternate NV seed hooks
    'MBEDTLS_FS_IO', # requires a filesystem
    'MBEDTLS_HAVE_TIME', # requires a clock
    'MBEDTLS_HAVE_TIME_DATE', # requires a clock
    'MBEDTLS_NET_C', # requires POSIX-like networking
    'MBEDTLS_PLATFORM_FPRINTF_ALT', # requires FILE* from stdio.h
    'MBEDTLS_PLATFORM_NV_SEED_ALT', # requires a filesystem and ENTROPY_NV_SEED
    'MBEDTLS_PLATFORM_TIME_ALT', # requires a clock and HAVE_TIME
    'MBEDTLS_PSA_CRYPTO_STORAGE_C', # requires a filesystem
    'MBEDTLS_PSA_ITS_FILE_C', # requires a filesystem
    'MBEDTLS_THREADING_C', # requires a threading interface
    'MBEDTLS_THREADING_PTHREAD', # requires pthread
    'MBEDTLS_TIMING_C', # requires a clock
    'MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT', # requires an OS for runtime-detection
    'MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT', # requires an OS for runtime-detection
])

def keep_in_baremetal(name):
    """Rules for symbols in the "baremetal" configuration."""
    if name in EXCLUDE_FROM_BAREMETAL:
        return False
    return True

def baremetal_adapter(name, value, active):
    """Config adapter for "baremetal"."""
    if not is_boolean_setting(name, value):
        return active
    if name == 'MBEDTLS_PSA_BUILTIN_GET_ENTROPY':
        # No OS-provided entropy source
        return False
    if name == 'MBEDTLS_PSA_DRIVER_GET_ENTROPY':
        return True
    return include_in_full(name) and keep_in_baremetal(name)

# This set contains options that are mostly for debugging or test purposes,
# and therefore should be excluded when doing code size measurements.
# Options that are their own module (such as MBEDTLS_ERROR_C) are not listed
# and therefore will be included when doing code size measurements.
EXCLUDE_FOR_SIZE = frozenset([
    'MBEDTLS_DEBUG_C', # large code size increase in TLS
    'MBEDTLS_SELF_TEST', # increases the size of many modules
    'MBEDTLS_TEST_HOOKS', # only useful with the hosted test framework, increases code size
])

def baremetal_size_adapter(name, value, active):
    if name in EXCLUDE_FOR_SIZE:
        return False
    return baremetal_adapter(name, value, active)

def include_in_crypto(name):
    """Rules for symbols in a crypto configuration."""
    if name.startswith('MBEDTLS_X509_') or \
       name.startswith('MBEDTLS_VERSION_') or \
       name.startswith('MBEDTLS_SSL_') or \
       name.startswith('MBEDTLS_KEY_EXCHANGE_'):
        return False
    if name in [
            'MBEDTLS_DEBUG_C', # part of libmbedtls
            'MBEDTLS_NET_C', # part of libmbedtls
            'MBEDTLS_PKCS7_C', # part of libmbedx509
            'MBEDTLS_TIMING_C', # part of libmbedtls
            'MBEDTLS_ERROR_C', # part of libmbedx509
            'MBEDTLS_ERROR_STRERROR_DUMMY', # part of libmbedx509
    ]:
        return False
    if name in EXCLUDE_FROM_CRYPTO:
        return False
    return True

def crypto_adapter(adapter):
    """Modify an adapter to disable non-crypto symbols.

    ``crypto_adapter(adapter)(name, value, active)`` is like
    ``adapter(name, value, active)``, but unsets all X.509 and TLS symbols.
    """
    def continuation(name, value, active):
        if not include_in_crypto(name):
            return False
        if adapter is None:
            return active
        return adapter(name, value, active)
    return continuation

DEPRECATED = frozenset([
    *PSA_DEPRECATED_FEATURE
])
def no_deprecated_adapter(adapter):
    """Modify an adapter to disable deprecated symbols.

    ``no_deprecated_adapter(adapter)(name, value, active)`` is like
    ``adapter(name, value, active)``, but unsets all deprecated symbols
    and sets ``MBEDTLS_DEPRECATED_REMOVED``.
    """
    def continuation(name, value, active):
        if name == 'MBEDTLS_DEPRECATED_REMOVED':
            return True
        if name in DEPRECATED:
            return False
        if adapter is None:
            return active
        return adapter(name, value, active)
    return continuation

def no_platform_adapter(adapter):
    """Modify an adapter to disable platform symbols.

    ``no_platform_adapter(adapter)(name, value, active)`` is like
    ``adapter(name, value, active)``, but unsets all platform symbols other
    ``than MBEDTLS_PLATFORM_C.
    """
    def continuation(name, value, active):
        # Allow MBEDTLS_PLATFORM_C but remove all other platform symbols.
        if name.startswith('MBEDTLS_PLATFORM_') and name != 'MBEDTLS_PLATFORM_C':
            return False
        if adapter is None:
            return active
        return adapter(name, value, active)
    return continuation


class MbedTLSConfigFile(config_common.ConfigFile):
    """Representation of an MbedTLS configuration file."""

    _path_in_tree = 'include/mbedtls/mbedtls_config.h'
    default_path = [_path_in_tree,
                    os.path.join(os.path.dirname(__file__),
                                 os.pardir,
                                 _path_in_tree),
                    os.path.join(os.path.dirname(os.path.abspath(os.path.dirname(__file__))),
                                 _path_in_tree)]

    def __init__(self, filename=None):
        super().__init__(self.default_path, 'Mbed TLS', filename)
        self.current_section = 'header'


class CryptoConfigFile(config_common.ConfigFile):
    """Representation of a Crypto configuration file."""

    # Temporary, while Mbed TLS does not just rely on the TF-PSA-Crypto
    # build system to build its crypto library. When it does, the
    # condition can just be removed.
    _path_in_tree = ('include/psa/crypto_config.h'
                     if not os.path.isdir(os.path.join(os.path.dirname(__file__),
                                                       os.pardir,
                                                       'tf-psa-crypto')) else
                     'tf-psa-crypto/include/psa/crypto_config.h')
    default_path = [_path_in_tree,
                    os.path.join(os.path.dirname(__file__),
                                 os.pardir,
                                 _path_in_tree),
                    os.path.join(os.path.dirname(os.path.abspath(os.path.dirname(__file__))),
                                 _path_in_tree)]

    def __init__(self, filename=None):
        super().__init__(self.default_path, 'Crypto', filename)


class MbedTLSConfig(config_common.Config):
    """Representation of the Mbed TLS configuration.

    See the documentation of the `Config` class for methods to query
    and modify the configuration.
    """

    def __init__(self, filename=None):
        """Read the Mbed TLS configuration file."""

        super().__init__()
        configfile = MbedTLSConfigFile(filename)
        self.configfiles.append(configfile)
        self.settings.update({name: config_common.Setting(configfile, active, name, value, section)
                              for (active, name, value, section)
                              in configfile.parse_file()})

    def set(self, name, value=None):
        """Set name to the given value and make it active."""

        if name not in self.settings:
            self._get_configfile().templates.append((name, '', '#define ' + name + ' '))

        super().set(name, value)


class CryptoConfig(config_common.Config):
    """Representation of the PSA crypto configuration.

    See the documentation of the `Config` class for methods to query
    and modify the configuration.
    """

    def __init__(self, filename=None):
        """Read the PSA crypto configuration file."""

        super().__init__()
        configfile = CryptoConfigFile(filename)
        self.configfiles.append(configfile)
        self.settings.update({name: config_common.Setting(configfile, active, name, value, section)
                              for (active, name, value, section)
                              in configfile.parse_file()})

    def set(self, name, value='1'):
        """Set name to the given value and make it active."""

        if name in PSA_UNSUPPORTED_FEATURE:
            raise ValueError(f'Feature is unsupported: \'{name}\'')

        if name not in self.settings:
            self._get_configfile().templates.append((name, '', '#define ' + name + ' '))

        super().set(name, value)


class CombinedConfig(config_common.Config):
    """Representation of MbedTLS and PSA crypto configuration

    See the documentation of the `Config` class for methods to query
    and modify the configuration.
    """

    def __init__(self, *configs):
        super().__init__()
        for config in configs:
            if isinstance(config, MbedTLSConfigFile):
                self.mbedtls_configfile = config
            elif isinstance(config, CryptoConfigFile):
                self.crypto_configfile = config
            else:
                raise ValueError(f'Invalid configfile: {config}')
            self.configfiles.append(config)

        self.settings.update({name: config_common.Setting(configfile, active, name, value, section)
                              for configfile in [self.mbedtls_configfile, self.crypto_configfile]
                              for (active, name, value, section) in configfile.parse_file()})

    _crypto_regexp = re.compile(r'^PSA_.*')
    def _get_configfile(self, name=None):
        """Find a config type for a setting name"""

        if name in self.settings:
            return self.settings[name].configfile
        elif re.match(self._crypto_regexp, name):
            return self.crypto_configfile
        else:
            return self.mbedtls_configfile

    def set(self, name, value=None):
        """Set name to the given value and make it active."""

        configfile = self._get_configfile(name)

        if configfile == self.crypto_configfile:
            if name in PSA_UNSUPPORTED_FEATURE:
                raise ValueError(f'Feature is unsupported: \'{name}\'')

            # The default value in the crypto config is '1'
            if not value and re.match(self._crypto_regexp, name):
                value = '1'

        if name not in self.settings:
            configfile.templates.append((name, '', '#define ' + name + ' '))

        super().set(name, value)

    #pylint: disable=arguments-differ
    def write(self, mbedtls_file=None, crypto_file=None):
        """Write the whole configuration to the file it was read from.

        If mbedtls_file or crypto_file is specified, write the specific configuration
        to the corresponding file instead.

        Two file name parameters and not only one as in the super class as we handle
        two configuration files in this class.
        """

        self.mbedtls_configfile.write(self.settings, mbedtls_file)
        self.crypto_configfile.write(self.settings, crypto_file)

    def filename(self, name=None):
        """Get the name of the config files.

        If 'name' is specified return the name of the config file where it is defined.
        """

        if not name:
            return [config.filename for config in [self.mbedtls_configfile, self.crypto_configfile]]

        return self._get_configfile(name).filename


class MbedTLSConfigTool(config_common.ConfigTool):
    """Command line mbedtls_config.h and crypto_config.h manipulation tool."""

    def __init__(self):
        super().__init__(MbedTLSConfigFile.default_path)
        self.config = CombinedConfig(MbedTLSConfigFile(self.args.file),
                                     CryptoConfigFile(self.args.cryptofile))

    def custom_parser_options(self):
        """Adds MbedTLS specific options for the parser."""

        self.parser.add_argument(
            '--cryptofile', '-c',
            help="""Crypto file to read (and modify if requested). Default: {}."""
            .format(CryptoConfigFile.default_path))

        self.add_adapter(
            'baremetal', baremetal_adapter,
            """Like full, but exclude features that require platform features
            such as file input-output.
            """)
        self.add_adapter(
            'baremetal_size', baremetal_size_adapter,
            """Like baremetal, but exclude debugging features. Useful for code size measurements.
            """)
        self.add_adapter(
            'full', full_adapter,
            """Uncomment most features.
            Exclude alternative implementations and platform support options, as well as
            some options that are awkward to test.
            """)
        self.add_adapter(
            'full_no_deprecated', no_deprecated_adapter(full_adapter),
            """Uncomment most non-deprecated features.
            Like "full", but without deprecated features.
            """)
        self.add_adapter(
            'full_no_platform', no_platform_adapter(full_adapter),
            """Uncomment most non-platform features. Like "full", but without platform features.
            """)
        self.add_adapter(
            'realfull', realfull_adapter,
            """Uncomment all boolean #defines.
            Suitable for generating documentation, but not for building.
            """)
        self.add_adapter(
            'crypto', crypto_adapter(None),
            """Only include crypto features. Exclude X.509 and TLS.""")
        self.add_adapter(
            'crypto_baremetal', crypto_adapter(baremetal_adapter),
            """Like baremetal, but with only crypto features, excluding X.509 and TLS.""")
        self.add_adapter(
            'crypto_full', crypto_adapter(full_adapter),
            """Like full, but with only crypto features, excluding X.509 and TLS.""")


if __name__ == '__main__':
    sys.exit(MbedTLSConfigTool().main())
