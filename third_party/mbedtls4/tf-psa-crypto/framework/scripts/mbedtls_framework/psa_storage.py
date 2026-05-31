"""Knowledge about the PSA key store as implemented in Mbed TLS.

Note that if you need to make a change that affects how keys are
stored, this may indicate that the key store is changing in a
backward-incompatible way! Think carefully about backward compatibility
before changing how test data is constructed or validated.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import re
import struct
from typing import Dict, List, Optional, Set, Union
import unittest

from . import c_build_helper
from . import build_tree


class Expr:
    """Representation of a C expression with a known or knowable numerical value."""

    def __init__(self, content: Union[int, str]):
        if isinstance(content, int):
            digits = 8 if content > 0xffff else 4
            self.string = '{0:#0{1}x}'.format(content, digits + 2)
            self.value_if_known = content #type: Optional[int]
        else:
            self.string = content
            self.unknown_values.add(self.normalize(content))
            self.value_if_known = None

    value_cache = {} #type: Dict[str, int]
    """Cache of known values of expressions."""

    unknown_values = set() #type: Set[str]
    """Expressions whose values are not present in `value_cache` yet."""

    def update_cache(self) -> None:
        """Update `value_cache` for expressions registered in `unknown_values`."""
        expressions = sorted(self.unknown_values)
        # Temporary, while Mbed TLS does not just rely on the TF-PSA-Crypto
        # build system to build its crypto library. When it does, the first
        # case can just be removed.

        if build_tree.looks_like_root('.'):
            includes = ['include']
            if build_tree.looks_like_tf_psa_crypto_root('.'):
                includes.append('drivers/builtin/include')
                includes.append('drivers/everest/include')
                includes.append('drivers/everest/include/tf-psa-crypto/private/')
                includes.append('drivers/pqcp/include')
            elif not build_tree.is_mbedtls_3_6():
                includes.append('tf-psa-crypto/include')
                includes.append('tf-psa-crypto/drivers/builtin/include')
                includes.append('tf-psa-crypto/drivers/everest/include')
                includes.append('tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/')
                includes.append('tf-psa-crypto/drivers/pqcp/include')

        values = c_build_helper.get_c_expression_values(
            'unsigned long', '%lu',
            expressions,
            header="""
            #include <psa/crypto.h>
            """,
            include_path=includes) #type: List[str]
        for e, v in zip(expressions, values):
            self.value_cache[e] = int(v, 0)
        self.unknown_values.clear()

    @staticmethod
    def normalize(string: str) -> str:
        """Put the given C expression in a canonical form.

        This function is only intended to give correct results for the
        relatively simple kind of C expression typically used with this
        module.
        """
        return re.sub(r'\s+', r'', string)

    def value(self) -> int:
        """Return the numerical value of the expression."""
        if self.value_if_known is None:
            if re.match(r'([0-9]+|0x[0-9a-f]+)\Z', self.string, re.I):
                return int(self.string, 0)
            normalized = self.normalize(self.string)
            if normalized not in self.value_cache:
                self.update_cache()
            self.value_if_known = self.value_cache[normalized]
        return self.value_if_known

Exprable = Union[str, int, Expr]
"""Something that can be converted to a C expression with a known numerical value."""

def as_expr(thing: Exprable) -> Expr:
    """Return an `Expr` object for `thing`.

    If `thing` is already an `Expr` object, return it. Otherwise build a new
    `Expr` object from `thing`. `thing` can be an integer or a string that
    contains a C expression.
    """
    if isinstance(thing, Expr):
        return thing
    else:
        return Expr(thing)


class Key:
    """Representation of a PSA crypto key object and its storage encoding.
    """

    LATEST_VERSION = 0
    """The latest version of the storage format."""

    def __init__(self, *,
                 version: Optional[int] = None,
                 id: Optional[int] = None, #pylint: disable=redefined-builtin
                 lifetime: Exprable = 'PSA_KEY_LIFETIME_PERSISTENT',
                 type: Exprable, #pylint: disable=redefined-builtin
                 bits: int,
                 usage: Exprable, alg: Exprable, alg2: Exprable,
                 material: bytes #pylint: disable=used-before-assignment
                ) -> None:
        self.version = self.LATEST_VERSION if version is None else version
        self.id = id #pylint: disable=invalid-name #type: Optional[int]
        self.lifetime = as_expr(lifetime) #type: Expr
        self.type = as_expr(type) #type: Expr
        self.bits = bits #type: int
        self.usage = as_expr(usage) #type: Expr
        self.alg = as_expr(alg) #type: Expr
        self.alg2 = as_expr(alg2) #type: Expr
        self.material = material #type: bytes

    MAGIC = b'PSA\000KEY\000'

    @staticmethod
    def pack(
            fmt: str,
            *args: Union[int, Expr]
    ) -> bytes: #pylint: disable=used-before-assignment
        """Pack the given arguments into a byte string according to the given format.

        This function is similar to `struct.pack`, but with the following differences:
        * All integer values are encoded with standard sizes and in
          little-endian representation. `fmt` must not include an endianness
          prefix.
        * Arguments can be `Expr` objects instead of integers.
        * Only integer-valued elements are supported.
        """
        return struct.pack('<' + fmt, # little-endian, standard sizes
                           *[arg.value() if isinstance(arg, Expr) else arg
                             for arg in args])

    def bytes(self) -> bytes:
        """Return the representation of the key in storage as a byte array.

        This is the content of the PSA storage file. When PSA storage is
        implemented over stdio files, this does not include any wrapping made
        by the PSA-storage-over-stdio-file implementation.

        Note that if you need to make a change in this function,
        this may indicate that the key store is changing in a
        backward-incompatible way! Think carefully about backward
        compatibility before making any change here.
        """
        header = self.MAGIC + self.pack('L', self.version)
        if self.version == 0:
            attributes = self.pack('LHHLLL',
                                   self.lifetime, self.type, self.bits,
                                   self.usage, self.alg, self.alg2)
            material = self.pack('L', len(self.material)) + self.material
        else:
            raise NotImplementedError
        return header + attributes + material

    def hex(self) -> str:
        """Return the representation of the key as a hexadecimal string.

        This is the hexadecimal representation of `self.bytes`.
        """
        return self.bytes().hex()

    def location_value(self) -> int:
        """The numerical value of the location encoded in the key's lifetime."""
        return self.lifetime.value() >> 8


class TestKey(unittest.TestCase):
    # pylint: disable=line-too-long
    """A few smoke tests for the functionality of the `Key` class."""

    def test_numerical(self):
        key = Key(version=0,
                  id=1, lifetime=0x00000001,
                  type=0x2400, bits=128,
                  usage=0x00000300, alg=0x05500200, alg2=0x04c01000,
                  material=b'@ABCDEFGHIJKLMNO')
        expected_hex = '505341004b45590000000000010000000024800000030000000250050010c00410000000404142434445464748494a4b4c4d4e4f'
        self.assertEqual(key.bytes(), bytes.fromhex(expected_hex))
        self.assertEqual(key.hex(), expected_hex)

    def test_names(self):
        length = 0xfff8 // 8 # PSA_MAX_KEY_BITS in bytes
        key = Key(version=0,
                  id=1, lifetime='PSA_KEY_LIFETIME_PERSISTENT',
                  type='PSA_KEY_TYPE_RAW_DATA', bits=length*8,
                  usage=0, alg=0, alg2=0,
                  material=b'\x00' * length)
        expected_hex = '505341004b45590000000000010000000110f8ff000000000000000000000000ff1f0000' + '00' * length
        self.assertEqual(key.bytes(), bytes.fromhex(expected_hex))
        self.assertEqual(key.hex(), expected_hex)

    def test_defaults(self):
        key = Key(type=0x1001, bits=8,
                  usage=0, alg=0, alg2=0,
                  material=b'\x2a')
        expected_hex = '505341004b455900000000000100000001100800000000000000000000000000010000002a'
        self.assertEqual(key.bytes(), bytes.fromhex(expected_hex))
        self.assertEqual(key.hex(), expected_hex)
