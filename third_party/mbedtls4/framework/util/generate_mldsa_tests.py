#!/usr/bin/env python3
"""Generate ML-DSA test cases.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

import sys
from typing import Iterable, List, Optional

# pip install dilithium-py
import dilithium_py.ml_dsa #type: ignore

import scripts_path # pylint: disable=unused-import
from mbedtls_framework import test_case
from mbedtls_framework import test_data_generation

# ML_DSA instances for pure ML-DSA
PURE = {
    #44: dilithium_py.ml_dsa.ML_DSA_44,
    #65: dilithium_py.ml_dsa.ML_DSA_65,
    87: dilithium_py.ml_dsa.ML_DSA_87,
}

# ML_DSA instances for HashML-DSA
HASH = {
    #44: dilithium_py.ml_dsa.HASH_ML_DSA_44_WITH_SHA512,
    #65: dilithium_py.ml_dsa.HASH_ML_DSA_65_WITH_SHA512,
    87: dilithium_py.ml_dsa.HASH_ML_DSA_87_WITH_SHA512,
}

# Seeds (i.e. private keys) to test with.
SEEDS = [
    b'There was once upon a time a ...',
    b'\x00' * 32,
]

class Key:
    """An MLDSA key pair."""
    #pylint: disable=too-few-public-methods

    def __init__(self, kl: int, seed: bytes) -> None:
        self.kl = kl #pylint: disable=invalid-name
        self.seed = seed
        self.public, self.secret = PURE[kl]._keygen_internal(seed)

    def sign_message(self, message: bytes, deterministic: bool) -> bytes:
        PURE[self.kl].set_drbg_seed(bytes(48))
        return PURE[self.kl].sign(self.secret, message,
                                  deterministic=deterministic)

# Key pairs to test with.
KEYS = {kl: [Key(kl, seed) for seed in SEEDS]
        for kl in sorted(PURE.keys())}

# Input messages to test with.
MESSAGES = [
    (b'This is a test', ''),
    (b'', 'empty message'),
    (b'\x00', '"\\x00"'),
    (b'\x01', '"\\x01"'),
    (b'ACBDEFGHIJ' * 100, '1000B'),
]


class API:
    """Abstract base class for the interface of the test functions."""

    @classmethod
    def function(cls, func: str, kl: int) -> str:
        raise NotImplementedError

    @classmethod
    def metadata_arguments(cls,
                           kl: int,
                           pair: bool,
                           deterministic: Optional[bool]) -> List[str]:
        raise NotImplementedError

    @classmethod
    def final_arguments(cls) -> List[str]:
        return []

    @classmethod
    def secret_is_seed(cls) -> bool:
        return True


class PQCPAPI(API):
    """Test mldsa-native entry points."""

    @classmethod
    def function(cls, func: str, kl: int) -> str:
        return f'{func}_{kl}'

    @classmethod
    def metadata_arguments(cls,
                           _kl: int,
                           _pair: bool,
                           _deterministic: Optional[bool]) -> List[str]:
        return []

    @classmethod
    def secret_is_seed(cls) -> bool:
        return False


def one_mldsa_key_pair_from_seed(key: Key,
                                 descr: str) -> test_case.TestCase:
    """Construct one test case for mldsa-native keypair_internal()."""
    tc = test_case.TestCase()
    tc.set_function(f'key_pair_from_seed_{key.kl}')
    tc.set_dependencies([f'TF_PSA_CRYPTO_PQCP_MLDSA_{key.kl}_ENABLED'])
    tc.set_arguments([
        test_case.hex_string(key.seed),
        test_case.hex_string(key.secret),
        test_case.hex_string(key.public),
    ])
    tc.set_description(f'MLDSA-{key.kl} key pair from seed {descr}')
    return tc

def gen_pqcp_key_management(kl: int) -> Iterable[test_case.TestCase]:
    """Generate test cases for mldsa-native keypair_internal()."""
    for i, key in enumerate(KEYS[kl], 1):
        yield one_mldsa_key_pair_from_seed(key, f'key#{i}')

def one_mldsa_sign_deterministic_pure(api: API,
                                      key: Key,
                                      message: bytes,
                                      descr: str) -> test_case.TestCase:
    """Construct one test case for deterministic signature."""
    signature = key.sign_message(message, deterministic=True)
    tc = test_case.TestCase()
    tc.set_function(api.function('sign_deterministic_pure', key.kl))
    tc.set_dependencies([f'TF_PSA_CRYPTO_PQCP_MLDSA_{key.kl}_ENABLED'])
    tc.set_arguments(api.metadata_arguments(key.kl, True, True) + [
        test_case.hex_string(key.seed if api.secret_is_seed() else key.secret),
        test_case.hex_string(message),
        test_case.hex_string(signature),
    ] + api.final_arguments())
    tc.set_description(f'MLDSA-{key.kl} sign deterministic {descr}')
    return tc

def one_mldsa_verify_pure(api: API,
                          key: Key,
                          message: bytes,
                          deterministic: bool,
                          descr: str) -> test_case.TestCase:
    """Construct one test case for verification.

    When deterministic is true, the test case is a deterministic signature.
    When deterministic is false, the test case is some other valid signature.
    """
    signature = key.sign_message(message, deterministic=deterministic)
    tc = test_case.TestCase()
    tc.set_function(api.function('verify_pure', key.kl))
    tc.set_dependencies([f'TF_PSA_CRYPTO_PQCP_MLDSA_{key.kl}_ENABLED'])
    tc.set_arguments(api.metadata_arguments(key.kl, False, True) + [
        test_case.hex_string(key.public),
        test_case.hex_string(message),
        test_case.hex_string(signature),
    ] + api.final_arguments())
    variant = "deterministic" if deterministic else "randomized"
    tc.set_description(f'MLDSA-{key.kl} verify {variant} {descr}')
    return tc

def gen_mldsa_pure(api: API, kl: int) -> Iterable[test_case.TestCase]:
    """Generate all test cases for pure ML-DSA signature and verification."""
    for i, key in enumerate(KEYS[kl], 1):
        yield one_mldsa_sign_deterministic_pure(api, key, MESSAGES[0][0],
                                                f'key#{i}')
    for message, descr in MESSAGES[1:]:
        yield one_mldsa_sign_deterministic_pure(api, KEYS[kl][0], message,
                                                f'key#1 {descr}')
    for i, key in enumerate(KEYS[kl], 1):
        yield one_mldsa_verify_pure(api, key, MESSAGES[0][0], True,
                                    f'key#{i}')
    for message, descr in MESSAGES[1:]:
        yield one_mldsa_verify_pure(api, KEYS[kl][0], message, True,
                                    f'key#1 {descr}')
    for i, key in enumerate(KEYS[kl], 1):
        yield one_mldsa_verify_pure(api, key, MESSAGES[0][0], False,
                                    f'key#{i}')
    for message, descr in MESSAGES[1:]:
        yield one_mldsa_verify_pure(api, KEYS[kl][0], message, False,
                                    f'key#1 {descr}')

def gen_pqcp_mldsa_all() -> Iterable[test_case.TestCase]:
    """Generate all test cases for mldsa-native."""
    api = PQCPAPI()
    for kl in sorted(KEYS.keys()):
        yield from gen_pqcp_key_management(kl)
        yield from gen_mldsa_pure(api, kl)

class MLDSATestGenerator(test_data_generation.TestGenerator):
    """Generate test cases for ML-DSA."""

    def __init__(self, settings) -> None:
        self.targets = {
            'test_suite_pqcp_mldsa.dilithium_py': gen_pqcp_mldsa_all,
        }
        super().__init__(settings)


if __name__ == '__main__':
    test_data_generation.main(sys.argv[1:], __doc__, MLDSATestGenerator)
