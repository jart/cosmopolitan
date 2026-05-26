#!/usr/bin/env python3
"""Generate test data for PSA cryptographic mechanisms.

With no arguments, generate all test data. With non-option arguments,
generate only the specified files.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import enum
import re
import sys
from typing import Callable, Dict, Iterable, Iterator, List, Optional

from mbedtls_framework import crypto_data_tests
from mbedtls_framework import crypto_knowledge
from mbedtls_framework import macro_collector #pylint: disable=unused-import
from mbedtls_framework import psa_information
from mbedtls_framework import psa_storage
from mbedtls_framework import psa_test_case
from mbedtls_framework import test_case
from mbedtls_framework import test_data_generation



def test_case_for_key_type_not_supported(
        verb: str, key_type: str, bits: int,
        not_supported_mechanism: str,
        *args: str,
        param_descr: str = ''
) -> test_case.TestCase:
    """Return one test case exercising a key creation method
    for an unsupported key type or size.
    """
    tc = psa_test_case.TestCase()
    short_key_type = crypto_knowledge.short_expression(key_type)
    tc.set_description('PSA {} {} {}-bit{} not supported'
                       .format(verb, short_key_type, bits,
                               ' ' + param_descr if param_descr else ''))
    # if tc.description == 'PSA import RSA_KEY_PAIR 1024-bit not supported':
    #     import pdb; pdb.set_trace()
    tc.set_function(verb + '_not_supported')
    tc.set_key_bits(bits)
    tc.set_key_pair_usage([verb.upper()])
    tc.assumes_not_supported(not_supported_mechanism)
    tc.set_arguments([key_type] + list(args))
    return tc

class KeyTypeNotSupported:
    """Generate test cases for when a key type is not supported."""

    def __init__(self, info: psa_information.Information) -> None:
        self.constructors = info.constructors

    ALWAYS_SUPPORTED = frozenset([
        'PSA_KEY_TYPE_DERIVE',
        'PSA_KEY_TYPE_PASSWORD',
        'PSA_KEY_TYPE_PASSWORD_HASH',
        'PSA_KEY_TYPE_RAW_DATA',
        'PSA_KEY_TYPE_HMAC'
    ])
    def test_cases_for_key_type_not_supported(
            self,
            kt: crypto_knowledge.KeyType,
            param: Optional[int] = None,
            param_descr: str = '',
    ) -> Iterator[test_case.TestCase]:
        """Return test cases exercising key creation when the given type is unsupported.

        If param is present and not None, emit test cases conditioned on this
        parameter not being supported. If it is absent or None, emit test cases
        conditioned on the base type not being supported.
        """
        if kt.name in self.ALWAYS_SUPPORTED:
            # Don't generate test cases for key types that are always supported.
            # They would be skipped in all configurations, which is noise.
            return
        if param is None:
            not_supported_mechanism = kt.name
        else:
            assert kt.params is not None
            not_supported_mechanism = kt.params[param]
        for bits in kt.sizes_to_test():
            yield test_case_for_key_type_not_supported(
                'import', kt.expression, bits,
                not_supported_mechanism,
                test_case.hex_string(kt.key_material(bits)),
                param_descr=param_descr,
            )
            # Don't generate not-supported test cases for key generation of
            # public keys. Our implementation always returns
            # PSA_ERROR_INVALID_ARGUMENT when attempting to generate a
            # public key, so we cover this together with the positive cases
            # in the KeyGenerate class.
            if not kt.is_public():
                yield test_case_for_key_type_not_supported(
                    'generate', kt.expression, bits,
                    not_supported_mechanism,
                    str(bits),
                    param_descr=param_descr,
                )
            # To be added: derive

    ECC_KEY_TYPES = ('PSA_KEY_TYPE_ECC_KEY_PAIR',
                     'PSA_KEY_TYPE_ECC_PUBLIC_KEY')
    DH_KEY_TYPES = ('PSA_KEY_TYPE_DH_KEY_PAIR',
                    'PSA_KEY_TYPE_DH_PUBLIC_KEY')

    def test_cases_for_not_supported(self) -> Iterator[test_case.TestCase]:
        """Generate test cases that exercise the creation of keys of unsupported types."""
        for key_type in sorted(self.constructors.key_types):
            if key_type in self.ECC_KEY_TYPES:
                continue
            if key_type in self.DH_KEY_TYPES:
                continue
            kt = crypto_knowledge.KeyType(key_type)
            yield from self.test_cases_for_key_type_not_supported(kt)
        for curve_family in sorted(self.constructors.ecc_curves):
            for constr in self.ECC_KEY_TYPES:
                kt = crypto_knowledge.KeyType(constr, [curve_family])
                yield from self.test_cases_for_key_type_not_supported(
                    kt, param_descr='type')
                yield from self.test_cases_for_key_type_not_supported(
                    kt, 0, param_descr='curve')
        for dh_family in sorted(self.constructors.dh_groups):
            for constr in self.DH_KEY_TYPES:
                kt = crypto_knowledge.KeyType(constr, [dh_family])
                yield from self.test_cases_for_key_type_not_supported(
                    kt, param_descr='type')
                yield from self.test_cases_for_key_type_not_supported(
                    kt, 0, param_descr='group')

def test_case_for_key_generation(
        key_type: str, bits: int,
        *args: str,
        result: str = ''
) -> test_case.TestCase:
    """Return one test case exercising a key generation.
    """
    tc = psa_test_case.TestCase()
    short_key_type = crypto_knowledge.short_expression(key_type)
    tc.set_description('PSA {} {}-bit'
                       .format(short_key_type, bits))
    tc.set_function('generate_key')
    tc.set_key_bits(bits)
    tc.set_key_pair_usage(['GENERATE'])
    tc.set_arguments([key_type] + list(args) + [result])
    return tc

class KeyGenerate:
    """Generate positive and negative (invalid argument) test cases for key generation."""

    def __init__(self, info: psa_information.Information) -> None:
        self.constructors = info.constructors

    ECC_KEY_TYPES = ('PSA_KEY_TYPE_ECC_KEY_PAIR',
                     'PSA_KEY_TYPE_ECC_PUBLIC_KEY')
    DH_KEY_TYPES = ('PSA_KEY_TYPE_DH_KEY_PAIR',
                    'PSA_KEY_TYPE_DH_PUBLIC_KEY')

    @staticmethod
    def test_cases_for_key_type_key_generation(
            kt: crypto_knowledge.KeyType
    ) -> Iterator[test_case.TestCase]:
        """Return test cases exercising key generation.

        All key types can be generated except for public keys. For public key
        PSA_ERROR_INVALID_ARGUMENT status is expected.
        """
        for bits in kt.sizes_to_test():
            tc = test_case_for_key_generation(
                kt.expression, bits,
                str(bits),
                'PSA_ERROR_INVALID_ARGUMENT' if kt.is_public() else 'PSA_SUCCESS'
            )
            if kt.is_public():
                # The library checks whether the key type is a public key generically,
                # before it reaches a point where it needs support for the specific key
                # type, so it returns INVALID_ARGUMENT for unsupported public key types.
                tc.set_dependencies([])
            yield tc

    def test_cases_for_key_generation(self) -> Iterator[test_case.TestCase]:
        """Generate test cases that exercise the generation of keys."""
        for key_type in sorted(self.constructors.key_types):
            if key_type in self.ECC_KEY_TYPES:
                continue
            if key_type in self.DH_KEY_TYPES:
                continue
            kt = crypto_knowledge.KeyType(key_type)
            yield from self.test_cases_for_key_type_key_generation(kt)
        for curve_family in sorted(self.constructors.ecc_curves):
            for constr in self.ECC_KEY_TYPES:
                kt = crypto_knowledge.KeyType(constr, [curve_family])
                yield from self.test_cases_for_key_type_key_generation(kt)
        for dh_family in sorted(self.constructors.dh_groups):
            for constr in self.DH_KEY_TYPES:
                kt = crypto_knowledge.KeyType(constr, [dh_family])
                yield from self.test_cases_for_key_type_key_generation(kt)

class OpFail:
    """Generate test cases for operations that must fail."""
    #pylint: disable=too-few-public-methods

    class Reason(enum.Enum):
        NOT_SUPPORTED = 0
        INVALID = 1
        INCOMPATIBLE = 2
        PUBLIC = 3

    def __init__(self, info: psa_information.Information) -> None:
        self.constructors = info.constructors
        key_type_expressions = self.constructors.generate_expressions(
            sorted(self.constructors.key_types)
        )
        self.key_types = [crypto_knowledge.KeyType(kt_expr)
                          for kt_expr in key_type_expressions]

    def make_test_case(
            self,
            alg: crypto_knowledge.Algorithm,
            category: crypto_knowledge.AlgorithmCategory,
            reason: 'Reason',
            kt: Optional[crypto_knowledge.KeyType] = None,
            not_supported: Optional[str] = None,
    ) -> test_case.TestCase:
        """Construct a failure test case for a one-key or keyless operation.

        If `reason` is `Reason.NOT_SUPPORTED`, pass the not-supported
        dependency symbol as the `not_supported` argument.
        """
        #pylint: disable=too-many-arguments,too-many-locals
        tc = psa_test_case.TestCase()
        pretty_alg = alg.short_expression()
        if reason == self.Reason.NOT_SUPPORTED:
            assert not_supported is not None
            pretty_reason = '!' + re.sub(r'PSA_WANT_[A-Z]+_', r'', not_supported)
        else:
            pretty_reason = reason.name.lower()
        if kt:
            key_type = kt.expression
            pretty_type = kt.short_expression()
        else:
            key_type = ''
            pretty_type = ''
        tc.set_description('PSA {} {}: {}{}'
                           .format(category.name.lower(),
                                   pretty_alg,
                                   pretty_reason,
                                   ' with ' + pretty_type if pretty_type else ''))
        tc.set_function(category.name.lower() + '_fail')
        arguments = [] # type: List[str]
        if kt:
            bits = kt.sizes_to_test()[0]
            if pretty_alg == "XTS" and kt.can_do(alg):
                # XTS mode uses double-size keys for the underlying block cipher
                bits = bits * 2
            tc.set_key_bits(bits)
            tc.set_key_pair_usage(['IMPORT'])
            key_material = kt.key_material(bits)
            arguments += [key_type, test_case.hex_string(key_material)]
        arguments.append(alg.expression)
        if category.is_asymmetric():
            arguments.append('1' if reason == self.Reason.PUBLIC else '0')
        error = ('NOT_SUPPORTED' if reason == self.Reason.NOT_SUPPORTED else
                 'INVALID_ARGUMENT')
        arguments.append('PSA_ERROR_' + error)
        if reason == self.Reason.NOT_SUPPORTED:
            assert not_supported is not None
            tc.assumes_not_supported(not_supported)
            # Special case: if one of deterministic/randomized
            # ECDSA is supported but not the other, then the one
            # that is not supported in the signature direction is
            # still supported in the verification direction,
            # because the two verification algorithms are
            # identical. This property is how Mbed TLS chooses to
            # behave, the specification would also allow it to
            # reject the algorithm. In the generated test cases,
            # we avoid this difficulty by not running the
            # not-supported test case when exactly one of the
            # two variants is supported.
            if not_supported == 'PSA_WANT_ALG_ECDSA':
                tc.add_dependencies(['!PSA_WANT_ALG_DETERMINISTIC_ECDSA'])
            if not_supported == 'PSA_WANT_ALG_DETERMINISTIC_ECDSA':
                tc.add_dependencies(['!PSA_WANT_ALG_ECDSA'])
        tc.set_arguments(arguments)
        return tc

    def no_key_test_cases(
            self,
            alg: crypto_knowledge.Algorithm,
            category: crypto_knowledge.AlgorithmCategory,
    ) -> Iterator[test_case.TestCase]:
        """Generate failure test cases for keyless operations with the specified algorithm."""
        if alg.can_do(category):
            # Compatible operation, unsupported algorithm
            for dep in psa_information.automatic_dependencies(alg.base_expression):
                yield self.make_test_case(alg, category,
                                          self.Reason.NOT_SUPPORTED,
                                          not_supported=dep)
        else:
            # Incompatible operation, supported algorithm
            yield self.make_test_case(alg, category, self.Reason.INVALID)

    def one_key_test_cases(
            self,
            alg: crypto_knowledge.Algorithm,
            category: crypto_knowledge.AlgorithmCategory,
    ) -> Iterator[test_case.TestCase]:
        """Generate failure test cases for one-key operations with the specified algorithm."""
        for kt in self.key_types:
            key_is_compatible = kt.can_do(alg)
            if key_is_compatible and alg.can_do(category):
                # Compatible key and operation, unsupported algorithm
                for dep in psa_information.automatic_dependencies(alg.base_expression):
                    yield self.make_test_case(alg, category,
                                              self.Reason.NOT_SUPPORTED,
                                              kt=kt, not_supported=dep)
                # Public key for a private-key operation
                if category.is_asymmetric() and kt.is_public():
                    yield self.make_test_case(alg, category,
                                              self.Reason.PUBLIC,
                                              kt=kt)
            elif key_is_compatible:
                # Compatible key, incompatible operation, supported algorithm
                yield self.make_test_case(alg, category,
                                          self.Reason.INVALID,
                                          kt=kt)
            elif alg.can_do(category):
                # Incompatible key, compatible operation, supported algorithm
                yield self.make_test_case(alg, category,
                                          self.Reason.INCOMPATIBLE,
                                          kt=kt)
            else:
                # Incompatible key and operation. Don't test cases where
                # multiple things are wrong, to keep the number of test
                # cases reasonable.
                pass

    def test_cases_for_algorithm(
            self,
            alg: crypto_knowledge.Algorithm,
            categories: Iterable[crypto_knowledge.AlgorithmCategory]
    ) -> Iterator[test_case.TestCase]:
        """Generate operation failure test cases for the specified algorithm."""
        for category in categories:
            if category.requires_key():
                yield from self.one_key_test_cases(alg, category)
            else:
                yield from self.no_key_test_cases(alg, category)

    def all_test_cases(self) -> Iterator[test_case.TestCase]:
        """Generate all test cases for operations that must fail."""
        algorithm_constructors = sorted(self.constructors.algorithms)
        algorithms = [crypto_knowledge.Algorithm(alg)
                      for alg in self.constructors.generate_expressions(
                          algorithm_constructors)]
        supported_categories = set()
        for alg in algorithms:
            supported_categories.add(alg.category)
        # We don't have a pake_fail test function yet.
        # https://github.com/Mbed-TLS/mbedtls-framework/issues/263
        supported_categories.remove(crypto_knowledge.AlgorithmCategory.PAKE)
        categories = sorted(supported_categories, key=lambda cat: cat.value)
        assert categories # sanity check: at least one category detected
        for alg in algorithms:
            yield from self.test_cases_for_algorithm(alg, categories)


class StorageKey(psa_storage.Key):
    """Representation of a key for storage format testing."""

    IMPLICIT_USAGE_FLAGS = {
        'PSA_KEY_USAGE_SIGN_HASH': 'PSA_KEY_USAGE_SIGN_MESSAGE',
        'PSA_KEY_USAGE_VERIFY_HASH': 'PSA_KEY_USAGE_VERIFY_MESSAGE'
    } #type: Dict[str, str]
    """Mapping of usage flags to the flags that they imply."""

    def __init__(
            self,
            usage: Iterable[str],
            without_implicit_usage: Optional[bool] = False,
            **kwargs
    ) -> None:
        """Prepare to generate a key.

        * `usage`                 : The usage flags used for the key.
        * `without_implicit_usage`: Flag to define to apply the usage extension
        """
        usage_flags = set(usage)
        if not without_implicit_usage:
            for flag in sorted(usage_flags):
                if flag in self.IMPLICIT_USAGE_FLAGS:
                    usage_flags.add(self.IMPLICIT_USAGE_FLAGS[flag])
        if usage_flags:
            usage_expression = ' | '.join(sorted(usage_flags))
        else:
            usage_expression = '0'
        super().__init__(usage=usage_expression, **kwargs)

class StorageTestData(StorageKey):
    """Representation of test case data for storage format testing."""

    def __init__(
            self,
            description: str,
            expected_usage: Optional[List[str]] = None,
            **kwargs
    ) -> None:
        """Prepare to generate test data

        * `description`   : used for the test case names
        * `expected_usage`: the usage flags generated as the expected usage flags
                            in the test cases. CAn differ from the usage flags
                            stored in the keys because of the usage flags extension.
        """
        super().__init__(**kwargs)
        self.description = description #type: str
        if expected_usage is None:
            self.expected_usage = self.usage #type: psa_storage.Expr
        elif expected_usage:
            self.expected_usage = psa_storage.Expr(' | '.join(expected_usage))
        else:
            self.expected_usage = psa_storage.Expr(0)

class StorageFormat:
    """Storage format stability test cases."""

    def __init__(self, info: psa_information.Information, version: int, forward: bool) -> None:
        """Prepare to generate test cases for storage format stability.

        * `info`: information about the API. See the `Information` class.
        * `version`: the storage format version to generate test cases for.
        * `forward`: if true, generate forward compatibility test cases which
          save a key and check that its representation is as intended. Otherwise
          generate backward compatibility test cases which inject a key
          representation and check that it can be read and used.
        """
        self.constructors = info.constructors #type: macro_collector.PSAMacroEnumerator
        self.version = version #type: int
        self.forward = forward #type: bool

    RSA_OAEP_RE = re.compile(r'PSA_ALG_RSA_OAEP\((.*)\)\Z')
    BRAINPOOL_RE = re.compile(r'PSA_KEY_TYPE_\w+\(PSA_ECC_FAMILY_BRAINPOOL_\w+\)\Z')
    @classmethod
    def exercise_key_with_algorithm(
            cls,
            key_type: psa_storage.Expr, bits: int,
            alg: psa_storage.Expr
    ) -> bool:
        """Whether to exercise the given key with the given algorithm.

        Normally only the type and algorithm matter for compatibility, and
        this is handled in crypto_knowledge.KeyType.can_do(). This function
        exists to detect exceptional cases. Exceptional cases detected here
        are not tested in OpFail and should therefore have manually written
        test cases.
        """
        # Some test keys have the RAW_DATA type and attributes that don't
        # necessarily make sense. We do this to validate numerical
        # encodings of the attributes.
        # Raw data keys have no useful exercise anyway so there is no
        # loss of test coverage.
        if key_type.string == 'PSA_KEY_TYPE_RAW_DATA':
            return False
        # OAEP requires room for two hashes plus wrapping
        m = cls.RSA_OAEP_RE.match(alg.string)
        if m:
            hash_alg = m.group(1)
            hash_length = crypto_knowledge.Algorithm.hash_length(hash_alg)
            key_length = (bits + 7) // 8
            # Leave enough room for at least one byte of plaintext
            return key_length > 2 * hash_length + 2
        # There's nothing wrong with ECC keys on Brainpool curves,
        # but operations with them are very slow. So we only exercise them
        # with a single algorithm, not with all possible hashes. We do
        # exercise other curves with all algorithms so test coverage is
        # perfectly adequate like this.
        m = cls.BRAINPOOL_RE.match(key_type.string)
        if m and alg.string != 'PSA_ALG_ECDSA_ANY':
            return False
        return True

    def make_test_case(self, key: StorageTestData) -> test_case.TestCase:
        """Construct a storage format test case for the given key.

        If ``forward`` is true, generate a forward compatibility test case:
        create a key and validate that it has the expected representation.
        Otherwise generate a backward compatibility test case: inject the
        key representation into storage and validate that it can be read
        correctly.
        """
        verb = 'save' if self.forward else 'read'
        tc = psa_test_case.TestCase()
        tc.set_description(verb + ' ' + key.description)
        tc.add_dependencies(psa_information.generate_deps_from_description(key.description))
        tc.set_function('key_storage_' + verb)
        tc.set_key_bits(key.bits)
        tc.set_key_pair_usage(['IMPORT'] if self.forward else ['EXPORT'])
        if self.forward:
            extra_arguments = []
        else:
            flags = []
            if self.exercise_key_with_algorithm(key.type, key.bits, key.alg):
                flags.append('TEST_FLAG_EXERCISE')
            if 'READ_ONLY' in key.lifetime.string:
                flags.append('TEST_FLAG_READ_ONLY')
            extra_arguments = [' | '.join(flags) if flags else '0']
        tc.set_arguments([key.lifetime.string,
                          key.type.string, str(key.bits),
                          key.expected_usage.string,
                          key.alg.string, key.alg2.string,
                          '"' + key.material.hex() + '"',
                          '"' + key.hex() + '"',
                          *extra_arguments])
        return tc

    def key_for_lifetime(
            self,
            lifetime: str,
    ) -> StorageTestData:
        """Construct a test key for the given lifetime."""
        short = lifetime
        short = re.sub(r'PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION',
                       r'', short)
        short = crypto_knowledge.short_expression(short)
        description = 'lifetime: ' + short
        key = StorageTestData(version=self.version,
                              id=1, lifetime=lifetime,
                              type='PSA_KEY_TYPE_RAW_DATA', bits=8,
                              usage=['PSA_KEY_USAGE_EXPORT'], alg=0, alg2=0,
                              material=b'L',
                              description=description)
        return key

    def all_keys_for_lifetimes(self) -> Iterator[StorageTestData]:
        """Generate test keys covering lifetimes."""
        lifetimes = sorted(self.constructors.lifetimes)
        expressions = self.constructors.generate_expressions(lifetimes)
        for lifetime in expressions:
            # Don't attempt to create or load a volatile key in storage
            if 'VOLATILE' in lifetime:
                continue
            # Don't attempt to create a read-only key in storage,
            # but do attempt to load one.
            if 'READ_ONLY' in lifetime and self.forward:
                continue
            yield self.key_for_lifetime(lifetime)

    def key_for_usage_flags(
            self,
            usage_flags: List[str],
            short: Optional[str] = None,
            test_implicit_usage: Optional[bool] = True
    ) -> StorageTestData:
        """Construct a test key for the given key usage."""
        extra_desc = ' without implication' if test_implicit_usage else ''
        description = 'usage' + extra_desc + ': '
        key1 = StorageTestData(version=self.version,
                               id=1, lifetime=0x00000001,
                               type='PSA_KEY_TYPE_RAW_DATA', bits=8,
                               expected_usage=usage_flags,
                               without_implicit_usage=not test_implicit_usage,
                               usage=usage_flags, alg=0, alg2=0,
                               material=b'K',
                               description=description)
        if short is None:
            usage_expr = key1.expected_usage.string
            key1.description += crypto_knowledge.short_expression(usage_expr)
        else:
            key1.description += short
        return key1

    USAGE_FLAGS_NOT_VALID_IN_POLICIES = frozenset([
        # Only for psa_check_key_usage() (upcoming) and
        # mbedtls_pk_can_do_psa() (since TF-PSA-Crypto 1.0),
        # not allowed in key policies as of TF-PSA-Crypto 1.0.
        # Note that this may become dependent on the TF-PSA-Crypto version
        # in the future; if so this code will require some refactoring.
        'PSA_KEY_USAGE_DERIVE_PUBLIC',
    ])

    def all_policy_flags(self) -> List[str]:
        """Return the list of all usage flags that are valid in key policies."""
        known_flags = frozenset(self.constructors.key_usage_flags)
        policy_flags = known_flags - self.USAGE_FLAGS_NOT_VALID_IN_POLICIES
        return sorted(policy_flags)

    def generate_keys_for_usage_flags(self, **kwargs) -> Iterator[StorageTestData]:
        """Generate test keys covering usage flags."""
        policy_flags = self.all_policy_flags()
        yield self.key_for_usage_flags(['0'], **kwargs)
        for usage_flag in policy_flags:
            yield self.key_for_usage_flags([usage_flag], **kwargs)
        for flag1, flag2 in zip(policy_flags,
                                policy_flags[1:] + [policy_flags[0]]):
            yield self.key_for_usage_flags([flag1, flag2], **kwargs)

    def generate_key_for_all_usage_flags(self) -> Iterator[StorageTestData]:
        policy_flags = self.all_policy_flags()
        yield self.key_for_usage_flags(policy_flags, short='all valid')

    def all_keys_for_usage_flags(self) -> Iterator[StorageTestData]:
        yield from self.generate_keys_for_usage_flags()
        yield from self.generate_key_for_all_usage_flags()

    def key_for_type_and_alg(
            self,
            kt: crypto_knowledge.KeyType,
            bits: int,
            alg: Optional[crypto_knowledge.Algorithm] = None,
    ) -> StorageTestData:
        """Construct a test key of the given type.

        If alg is not None, this key allows it.
        """
        usage_flags = ['PSA_KEY_USAGE_EXPORT']
        alg1 = 0 #type: psa_storage.Exprable
        alg2 = 0
        if alg is not None:
            alg1 = alg.expression
            usage_flags += alg.usage_flags(public=kt.is_public())
        key_material = kt.key_material(bits)
        description = 'type: {} {}-bit'.format(kt.short_expression(1), bits)
        if alg is not None:
            description += ', ' + alg.short_expression(1)
        key = StorageTestData(version=self.version,
                              id=1, lifetime=0x00000001,
                              type=kt.expression, bits=bits,
                              usage=usage_flags, alg=alg1, alg2=alg2,
                              material=key_material,
                              description=description)
        return key

    def keys_for_type(
            self,
            key_type: str,
            all_algorithms: List[crypto_knowledge.Algorithm],
    ) -> Iterator[StorageTestData]:
        """Generate test keys for the given key type."""
        kt = crypto_knowledge.KeyType(key_type)
        for bits in kt.sizes_to_test():
            # Test a non-exercisable key, as well as exercisable keys for
            # each compatible algorithm.
            # To do: test reading a key from storage with an incompatible
            # or unsupported algorithm.
            yield self.key_for_type_and_alg(kt, bits)
            compatible_algorithms = [alg for alg in all_algorithms
                                     if kt.can_do(alg)]
            for alg in compatible_algorithms:
                if alg.expression == 'PSA_ALG_XTS':
                    # XTS mode uses double-size keys for the underlying block cipher
                    # XTS does not use 192-bit keys
                    if bits != 192:
                        bits = bits * 2
                    else:
                        continue
                yield self.key_for_type_and_alg(kt, bits, alg)

    def all_keys_for_types(self) -> Iterator[StorageTestData]:
        """Generate test keys covering key types and their representations."""
        key_types = sorted(self.constructors.key_types)
        all_algorithms = [crypto_knowledge.Algorithm(alg)
                          for alg in self.constructors.generate_expressions(
                              sorted(self.constructors.algorithms)
                          )]
        for key_type in self.constructors.generate_expressions(key_types):
            yield from self.keys_for_type(key_type, all_algorithms)

    def keys_for_algorithm(self, alg: str) -> Iterator[StorageTestData]:
        """Generate test keys for the encoding of the specified algorithm."""
        # These test cases only validate the encoding of algorithms, not
        # whether the key read from storage is suitable for an operation.
        # `keys_for_types` generate read tests with an algorithm and a
        # compatible key.
        descr = crypto_knowledge.short_expression(alg, 1)
        usage = ['PSA_KEY_USAGE_EXPORT']
        key1 = StorageTestData(version=self.version,
                               id=1, lifetime=0x00000001,
                               type='PSA_KEY_TYPE_RAW_DATA', bits=8,
                               usage=usage, alg=alg, alg2=0,
                               material=b'K',
                               description='alg: ' + descr)
        yield key1
        key2 = StorageTestData(version=self.version,
                               id=1, lifetime=0x00000001,
                               type='PSA_KEY_TYPE_RAW_DATA', bits=8,
                               usage=usage, alg=0, alg2=alg,
                               material=b'L',
                               description='alg2: ' + descr)
        yield key2

    def all_keys_for_algorithms(self) -> Iterator[StorageTestData]:
        """Generate test keys covering algorithm encodings."""
        algorithms = sorted(self.constructors.algorithms)
        for alg in self.constructors.generate_expressions(algorithms):
            yield from self.keys_for_algorithm(alg)

    def generate_all_keys(self) -> Iterator[StorageTestData]:
        """Generate all keys for the test cases."""
        yield from self.all_keys_for_lifetimes()
        yield from self.all_keys_for_usage_flags()
        yield from self.all_keys_for_types()
        yield from self.all_keys_for_algorithms()

    def all_test_cases(self) -> Iterator[test_case.TestCase]:
        """Generate all storage format test cases."""
        # First build a list of all keys, then construct all the corresponding
        # test cases. This allows all required information to be obtained in
        # one go, which is a significant performance gain as the information
        # includes numerical values obtained by compiling a C program.
        all_keys = list(self.generate_all_keys())
        for key in all_keys:
            if key.location_value() != 0:
                # Skip keys with a non-default location, because they
                # require a driver and we currently have no mechanism to
                # determine whether a driver is available.
                continue
            yield self.make_test_case(key)

class StorageFormatForward(StorageFormat):
    """Storage format stability test cases for forward compatibility."""

    def __init__(self, info: psa_information.Information, version: int) -> None:
        super().__init__(info, version, True)

class StorageFormatV0(StorageFormat):
    """Storage format stability test cases for version 0 compatibility."""

    def __init__(self, info: psa_information.Information) -> None:
        super().__init__(info, 0, False)

    def all_keys_for_usage_flags(self) -> Iterator[StorageTestData]:
        """Generate test keys covering usage flags."""
        yield from super().all_keys_for_usage_flags()
        yield from self.generate_keys_for_usage_flags(test_implicit_usage=False)

    def keys_for_implicit_usage(
            self,
            implyer_usage: str,
            alg: str,
            key_type: crypto_knowledge.KeyType
    ) -> StorageTestData:
        # pylint: disable=too-many-locals
        """Generate test keys for the specified implicit usage flag,
           algorithm and key type combination.
        """
        bits = key_type.sizes_to_test()[0]
        implicit_usage = StorageKey.IMPLICIT_USAGE_FLAGS[implyer_usage]
        usage_flags = ['PSA_KEY_USAGE_EXPORT']
        material_usage_flags = usage_flags + [implyer_usage]
        expected_usage_flags = material_usage_flags + [implicit_usage]
        alg2 = 0
        key_material = key_type.key_material(bits)
        usage_expression = crypto_knowledge.short_expression(implyer_usage, 1)
        alg_expression = crypto_knowledge.short_expression(alg, 1)
        key_type_expression = key_type.short_expression(1)
        description = 'implied by {}: {} {} {}-bit'.format(
            usage_expression, alg_expression, key_type_expression, bits)
        key = StorageTestData(version=self.version,
                              id=1, lifetime=0x00000001,
                              type=key_type.expression, bits=bits,
                              usage=material_usage_flags,
                              expected_usage=expected_usage_flags,
                              without_implicit_usage=True,
                              alg=alg, alg2=alg2,
                              material=key_material,
                              description=description)
        return key

    def gather_key_types_for_sign_alg(self) -> Dict[str, List[str]]:
        # pylint: disable=too-many-locals
        """Match possible key types for sign algorithms."""
        # To create a valid combination both the algorithms and key types
        # must be filtered. Pair them with keywords created from its names.
        incompatible_alg_keyword = frozenset(['RAW', 'ANY', 'PURE'])
        incompatible_key_type_keywords = frozenset(['MONTGOMERY'])
        keyword_translation = {
            'ECDSA': 'ECC',
            'ED[0-9]*.*' : 'EDWARDS'
        }
        exclusive_keywords = {
            'EDWARDS': 'ECC'
        }
        key_types = set(self.constructors.generate_expressions(self.constructors.key_types))
        algorithms = set(self.constructors.generate_expressions(self.constructors.sign_algorithms))
        alg_with_keys = {} #type: Dict[str, List[str]]
        translation_table = str.maketrans('(', '_', ')')
        for alg in algorithms:
            # Generate keywords from the name of the algorithm
            alg_keywords = set(alg.partition('(')[0].split(sep='_')[2:])
            # Translate keywords for better matching with the key types
            for keyword in alg_keywords.copy():
                for pattern, replace in keyword_translation.items():
                    if re.match(pattern, keyword):
                        alg_keywords.remove(keyword)
                        alg_keywords.add(replace)
            # Filter out incompatible algorithms
            if not alg_keywords.isdisjoint(incompatible_alg_keyword):
                continue

            for key_type in key_types:
                # Generate keywords from the of the key type
                key_type_keywords = set(key_type.translate(translation_table).split(sep='_')[3:])

                # Remove ambiguous keywords
                for keyword1, keyword2 in exclusive_keywords.items():
                    if keyword1 in key_type_keywords:
                        key_type_keywords.remove(keyword2)

                if key_type_keywords.isdisjoint(incompatible_key_type_keywords) and\
                   not key_type_keywords.isdisjoint(alg_keywords):
                    if alg in alg_with_keys:
                        alg_with_keys[alg].append(key_type)
                    else:
                        alg_with_keys[alg] = [key_type]
        return alg_with_keys

    def all_keys_for_implicit_usage(self) -> Iterator[StorageTestData]:
        """Generate test keys for usage flag extensions."""
        # Generate a key type and algorithm pair for each extendable usage
        # flag to generate a valid key for exercising. The key is generated
        # without usage extension to check the extension compatibility.
        alg_with_keys = self.gather_key_types_for_sign_alg()

        for usage in sorted(StorageKey.IMPLICIT_USAGE_FLAGS, key=str):
            for alg in sorted(alg_with_keys):
                for key_type in sorted(alg_with_keys[alg]):
                    # The key types must be filtered to fit the specific usage flag.
                    kt = crypto_knowledge.KeyType(key_type)
                    if kt.is_public() and '_SIGN_' in usage:
                        # Can't sign with a public key
                        continue
                    yield self.keys_for_implicit_usage(usage, alg, kt)

    def generate_all_keys(self) -> Iterator[StorageTestData]:
        yield from super().generate_all_keys()
        yield from self.all_keys_for_implicit_usage()


class PSATestGenerator(test_data_generation.TestGenerator):
    """Test generator subclass including PSA targets and info."""
    # Note that targets whose names contain 'test_format' have their content
    # validated by `abi_check.py`.
    targets = {
        'test_suite_psa_crypto_generate_key.generated':
        lambda info: KeyGenerate(info).test_cases_for_key_generation(),
        'test_suite_psa_crypto_not_supported.generated':
        lambda info: KeyTypeNotSupported(info).test_cases_for_not_supported(),
        'test_suite_psa_crypto_low_hash.generated':
        lambda info: crypto_data_tests.HashPSALowLevel(info).all_test_cases(),
        'test_suite_psa_crypto_op_fail.generated':
        lambda info: OpFail(info).all_test_cases(),
        'test_suite_psa_crypto_storage_format.current':
        lambda info: StorageFormatForward(info, 0).all_test_cases(),
        'test_suite_psa_crypto_storage_format.v0':
        lambda info: StorageFormatV0(info).all_test_cases(),
    } #type: Dict[str, Callable[[psa_information.Information], Iterable[test_case.TestCase]]]

    def __init__(self, options):
        super().__init__(options)
        self.info = psa_information.Information()

    def generate_target(self, name: str, *target_args) -> None:
        super().generate_target(name, self.info)


if __name__ == '__main__':
    test_data_generation.main(sys.argv[1:], __doc__, PSATestGenerator)
