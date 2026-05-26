"""Generate test data for cryptographic mechanisms.

This module is a work in progress, only implementing a few cases for now.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import hashlib
from typing import Callable, Dict, Iterator, List, Optional #pylint: disable=unused-import

from . import crypto_knowledge
from . import psa_information
from . import psa_test_case
from . import test_case


class HashPSALowLevel:
    """Generate test cases for the PSA low-level hash interface."""

    def __init__(self, info: psa_information.Information) -> None:
        self.info = info
        base_algorithms = sorted(info.constructors.algorithms)
        all_algorithms = \
            [crypto_knowledge.Algorithm(expr)
             for expr in info.constructors.generate_expressions(base_algorithms)]
        self.algorithms = \
            [alg
             for alg in all_algorithms
             if (not alg.is_wildcard and
                 alg.can_do(crypto_knowledge.AlgorithmCategory.HASH))]

    # CALCULATE[alg] = function to return the hash of its argument in hex
    # TO-DO: implement the None entries with a third-party library, because
    # hashlib might not have everything, depending on the Python version and
    # the underlying OpenSSL. On Ubuntu 16.04, truncated sha512 and sha3/shake
    # are not available. On Ubuntu 22.04, md2, md4 and ripemd160 are not
    # available. As of Python 3.14, Ascon is not available.
    CALCULATE = {
        'PSA_ALG_MD5': lambda data: hashlib.md5(data).hexdigest(),
        'PSA_ALG_RIPEMD160': None, #lambda data: hashlib.new('ripdemd160').hexdigest()
        'PSA_ALG_SHA_1': lambda data: hashlib.sha1(data).hexdigest(),
        'PSA_ALG_SHA_224': lambda data: hashlib.sha224(data).hexdigest(),
        'PSA_ALG_SHA_256': lambda data: hashlib.sha256(data).hexdigest(),
        'PSA_ALG_SHA_256_192': None, #lambda data: hashlib.new('sha256').hexdigest()[:48]
        'PSA_ALG_SHA_384': lambda data: hashlib.sha384(data).hexdigest(),
        'PSA_ALG_SHA_512': lambda data: hashlib.sha512(data).hexdigest(),
        'PSA_ALG_SHA_512_224': None, #lambda data: hashlib.new('sha512_224').hexdigest()
        'PSA_ALG_SHA_512_256': None, #lambda data: hashlib.new('sha512_256').hexdigest()
        'PSA_ALG_SHA3_224': None, #lambda data: hashlib.sha3_224(data).hexdigest(),
        'PSA_ALG_SHA3_256': None, #lambda data: hashlib.sha3_256(data).hexdigest(),
        'PSA_ALG_SHA3_384': None, #lambda data: hashlib.sha3_384(data).hexdigest(),
        'PSA_ALG_SHA3_512': None, #lambda data: hashlib.sha3_512(data).hexdigest(),
        'PSA_ALG_SHAKE128_192': None, #lambda data: hashlib.shake_128(data).hexdigest(24),
        'PSA_ALG_SHAKE128_256': None, #lambda data: hashlib.shake_128(data).hexdigest(32),
        'PSA_ALG_SHAKE256_256': None, #lambda data: hashlib.shake_256(data).hexdigest(32),
        'PSA_ALG_SHAKE256_512': None, #lambda data: hashlib.shake_256(data).hexdigest(64),
        'PSA_ALG_ASCON_HASH256': None, #lambda data: ascon.ascon_hash(data),
    } #type: Dict[str, Optional[Callable[[bytes], str]]]

    @staticmethod
    def one_test_case(alg: crypto_knowledge.Algorithm,
                      function: str, note: str,
                      arguments: List[str]) -> test_case.TestCase:
        """Construct one test case involving a hash."""
        tc = psa_test_case.TestCase(dependency_prefix='MBEDTLS_PSA_BUILTIN_')
        tc.set_description('{}{} {}'
                           .format(function,
                                   ' ' + note if note else '',
                                   alg.short_expression()))
        tc.set_function(function)
        tc.set_arguments([alg.expression] +
                         ['"{}"'.format(arg) for arg in arguments])
        return tc

    def test_cases_for_hash(self,
                            alg: crypto_knowledge.Algorithm
                            ) -> Iterator[test_case.TestCase]:
        """Enumerate all test cases for one hash algorithm."""
        calc = self.CALCULATE[alg.expression]
        if calc is None:
            return # not implemented yet

        short = b'abc'
        hash_short = calc(short)
        long = (b'Hello, world. Here are 16 unprintable bytes: ['
                b'\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a'
                b'\x80\x81\x82\x83\xfe\xff]. '
                b' This message was brought to you by a natural intelligence. '
                b' If you can read this, good luck with your debugging!')
        hash_long = calc(long)

        yield self.one_test_case(alg, 'hash_empty', '', [calc(b'')])
        yield self.one_test_case(alg, 'hash_valid_one_shot', '',
                                 [short.hex(), hash_short])
        for n in [0, 1, 64, len(long) - 1, len(long)]:
            yield self.one_test_case(alg, 'hash_valid_multipart',
                                     '{} + {}'.format(n, len(long) - n),
                                     [long[:n].hex(), calc(long[:n]),
                                      long[n:].hex(), hash_long])

    def all_test_cases(self) -> Iterator[test_case.TestCase]:
        """Enumerate all test cases for all hash algorithms."""
        for alg in self.algorithms:
            yield from self.test_cases_for_hash(alg)
