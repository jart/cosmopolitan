"""Knowledge about cryptographic mechanisms implemented in Mbed TLS.

This module is entirely based on the PSA API.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import enum
import re
from typing import FrozenSet, Iterable, List, Optional, Tuple, Dict

from .asymmetric_key_data import ASYMMETRIC_KEY_DATA


def short_expression(original: str, level: int = 0) -> str:
    """Abbreviate the expression, keeping it human-readable.

    If `level` is 0, just remove parts that are implicit from context,
    such as a leading ``PSA_KEY_TYPE_``.
    For larger values of `level`, also abbreviate some names in an
    unambiguous, but ad hoc way.
    """
    short = original
    short = re.sub(r'\bPSA_(?:ALG|DH_FAMILY|ECC_FAMILY|KEY_[A-Z]+)_', r'', short)
    short = re.sub(r' +', r'', short)
    if level >= 1:
        short = re.sub(r'PUBLIC_KEY\b', r'PUB', short)
        short = re.sub(r'KEY_PAIR\b', r'PAIR', short)
        short = re.sub(r'\bBRAINPOOL_P', r'BP', short)
        short = re.sub(r'\bMONTGOMERY\b', r'MGM', short)
        short = re.sub(r'AEAD_WITH_SHORTENED_TAG\b', r'AEAD_SHORT', short)
        short = re.sub(r'\bDETERMINISTIC_', r'DET_', short)
        short = re.sub(r'\bKEY_AGREEMENT\b', r'KA', short)
        short = re.sub(r'_PSK_TO_MS\b', r'_PSK2MS', short)
    return short


BLOCK_CIPHERS = frozenset(['AES', 'ARIA', 'CAMELLIA', 'DES'])
BLOCK_MAC_MODES = frozenset(['CBC_MAC', 'CMAC'])
BLOCK_CIPHER_MODES = frozenset([
    'CTR', 'CFB', 'OFB', 'XTS', 'CCM_STAR_NO_TAG',
    'ECB_NO_PADDING', 'CBC_NO_PADDING', 'CBC_PKCS7',
])
BLOCK_AEAD_MODES = frozenset(['CCM', 'GCM'])

class EllipticCurveCategory(enum.Enum):
    """Categorization of elliptic curve families.

    The category of a curve determines what algorithms are defined over it.
    """

    SHORT_WEIERSTRASS = 0
    MONTGOMERY = 1
    TWISTED_EDWARDS = 2

    @staticmethod
    def from_family(family: str) -> 'EllipticCurveCategory':
        if family == 'PSA_ECC_FAMILY_MONTGOMERY':
            return EllipticCurveCategory.MONTGOMERY
        if family == 'PSA_ECC_FAMILY_TWISTED_EDWARDS':
            return EllipticCurveCategory.TWISTED_EDWARDS
        # Default to SW, which most curves belong to.
        return EllipticCurveCategory.SHORT_WEIERSTRASS


class KeyType:
    """Knowledge about a PSA key type."""

    def __init__(self, name: str, params: Optional[Iterable[str]] = None) -> None:
        """Analyze a key type.

        The key type must be specified in PSA syntax. In its simplest form,
        `name` is a string 'PSA_KEY_TYPE_xxx' which is the name of a PSA key
        type macro. For key types that take arguments, the arguments can
        be passed either through the optional argument `params` or by
        passing an expression of the form 'PSA_KEY_TYPE_xxx(param1, ...)'
        in `name` as a string.
        """

        self.name = name.strip()
        """The key type macro name (``PSA_KEY_TYPE_xxx``).

        For key types constructed from a macro with arguments, this is the
        name of the macro, and the arguments are in `self.params`.
        """
        if params is None:
            if '(' in self.name:
                m = re.match(r'(\w+)\s*\((.*)\)\Z', self.name)
                assert m is not None
                self.name = m.group(1)
                params = m.group(2).split(',')
        self.params = (None if params is None else
                       [param.strip() for param in params])
        """The parameters of the key type, if there are any.

        None if the key type is a macro without arguments.
        """
        assert re.match(r'PSA_KEY_TYPE_\w+\Z', self.name)

        self.expression = self.name
        """A C expression whose value is the key type encoding."""
        if self.params is not None:
            self.expression += '(' + ', '.join(self.params) + ')'

        m = re.match(r'PSA_KEY_TYPE_(\w+)', self.name)
        assert m
        self.head = re.sub(r'_(?:PUBLIC_KEY|KEY_PAIR)\Z', r'', m.group(1))
        """The key type macro name, with common prefixes and suffixes stripped."""

        self.private_type = re.sub(r'_PUBLIC_KEY\Z', r'_KEY_PAIR', self.name)
        """The key type macro name for the corresponding key pair type.

        For everything other than a public key type, this is the same as
        `self.name`.
        """

    def short_expression(self, level: int = 0) -> str:
        """Abbreviate the expression, keeping it human-readable.

        See `crypto_knowledge.short_expression`.
        """
        return short_expression(self.expression, level=level)

    def is_public(self) -> bool:
        """Whether the key type is for public keys."""
        return self.name.endswith('_PUBLIC_KEY')

    DH_KEY_SIZES = {
        'PSA_DH_FAMILY_RFC3526': (1536, 2048, 3072, 4096, 6144, 8192),
        'PSA_DH_FAMILY_RFC7919': (2048, 3072, 4096, 6144, 8192),
    } # type: Dict[str, Tuple[int, ...]]
    ECC_KEY_SIZES = {
        'PSA_ECC_FAMILY_SECP_K1': (192, 225, 256),
        'PSA_ECC_FAMILY_SECP_R1': (224, 256, 384, 521),
        'PSA_ECC_FAMILY_SECP_R2': (160,),
        'PSA_ECC_FAMILY_SECT_K1': (163, 233, 239, 283, 409, 571),
        'PSA_ECC_FAMILY_SECT_R1': (163, 233, 283, 409, 571),
        'PSA_ECC_FAMILY_SECT_R2': (163,),
        'PSA_ECC_FAMILY_BRAINPOOL_P_R1': (160, 192, 224, 256, 320, 384, 512),
        'PSA_ECC_FAMILY_FRP': (256,),
        'PSA_ECC_FAMILY_MONTGOMERY': (255, 448),
        'PSA_ECC_FAMILY_TWISTED_EDWARDS': (255, 448),
    } # type: Dict[str, Tuple[int, ...]]
    KEY_TYPE_SIZES = {
        'PSA_KEY_TYPE_AES': (128, 192, 256), # exhaustive
        'PSA_KEY_TYPE_ARC4': (8, 128, 2048), # extremes + sensible
        'PSA_KEY_TYPE_ARIA': (128, 192, 256), # exhaustive
        'PSA_KEY_TYPE_ASCON': (128, 256), # exhaustive
        'PSA_KEY_TYPE_CAMELLIA': (128, 192, 256), # exhaustive
        'PSA_KEY_TYPE_CHACHA20': (256,), # exhaustive
        'PSA_KEY_TYPE_DERIVE': (120, 128), # sample
        'PSA_KEY_TYPE_DES': (64, 128, 192), # exhaustive
        'PSA_KEY_TYPE_HMAC': (128, 160, 224, 256, 384, 512), # standard size for each supported hash
        'PSA_KEY_TYPE_PASSWORD': (48, 168, 336), # sample
        'PSA_KEY_TYPE_PASSWORD_HASH': (128, 256), # sample
        'PSA_KEY_TYPE_PEPPER': (128, 256), # sample
        'PSA_KEY_TYPE_RAW_DATA': (8, 40, 128), # sample
        'PSA_KEY_TYPE_RSA_KEY_PAIR': (1024, 1536), # small sample
        'PSA_KEY_TYPE_SM4': (128,), # exhaustive
        'PSA_KEY_TYPE_XCHACHA20': (256,), # exhaustive
    } # type: Dict[str, Tuple[int, ...]]
    def sizes_to_test(self) -> Tuple[int, ...]:
        """Return a tuple of key sizes to test.

        For key types that only allow a single size, or only a small set of
        sizes, these are all the possible sizes. For key types that allow a
        wide range of sizes, these are a representative sample of sizes,
        excluding large sizes for which a typical resource-constrained platform
        may run out of memory.
        """
        if self.private_type == 'PSA_KEY_TYPE_ECC_KEY_PAIR':
            assert self.params is not None
            return self.ECC_KEY_SIZES[self.params[0]]
        if self.private_type == 'PSA_KEY_TYPE_DH_KEY_PAIR':
            assert self.params is not None
            return self.DH_KEY_SIZES[self.params[0]]
        return self.KEY_TYPE_SIZES[self.private_type]

    # "48657265006973206b6579a064617461"
    DATA_BLOCK = b'Here\000is key\240data'
    def key_material(self, bits: int) -> bytes:
        """Return a byte string containing suitable key material with the given bit length.

        Use the PSA export representation. The resulting byte string is one that
        can be obtained with the following code:
        ```
        psa_set_key_type(&attributes, `self.expression`);
        psa_set_key_bits(&attributes, `bits`);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);
        psa_generate_key(&attributes, &id);
        psa_export_key(id, `material`, ...);
        ```
        """
        if self.expression in ASYMMETRIC_KEY_DATA:
            if bits not in ASYMMETRIC_KEY_DATA[self.expression]:
                raise ValueError('No key data for {}-bit {}'
                                 .format(bits, self.expression))
            return ASYMMETRIC_KEY_DATA[self.expression][bits]
        if bits % 8 != 0:
            raise ValueError('Non-integer number of bytes: {} bits for {}'
                             .format(bits, self.expression))
        length = bits // 8
        if self.name == 'PSA_KEY_TYPE_DES':
            # "644573206b457901644573206b457902644573206b457904"
            des3 = b'dEs kEy\001dEs kEy\002dEs kEy\004'
            return des3[:length]
        return b''.join([self.DATA_BLOCK] * (length // len(self.DATA_BLOCK)) +
                        [self.DATA_BLOCK[:length % len(self.DATA_BLOCK)]])

    def can_do(self, alg: 'Algorithm') -> bool:
        """Whether this key type can be used for operations with the given algorithm.

        This function does not currently handle key derivation or PAKE.
        """
        #pylint: disable=too-many-branches,too-many-return-statements
        if not alg.is_valid_for_operation():
            return False
        if self.head == 'HMAC' and alg.head == 'HMAC':
            return True
        if self.head == 'DES':
            # 64-bit block ciphers only allow a reduced set of modes.
            return alg.head in [
                'CBC_NO_PADDING', 'CBC_PKCS7',
                'ECB_NO_PADDING',
            ]
        if self.head in BLOCK_CIPHERS and \
           alg.head in frozenset.union(BLOCK_MAC_MODES,
                                       BLOCK_CIPHER_MODES,
                                       BLOCK_AEAD_MODES):
            if alg.head in ['CMAC', 'OFB'] and \
               self.head in ['ARIA', 'CAMELLIA']:
                return False # not implemented in Mbed TLS
            if alg.head == 'XTS' and self.head != 'AES':
                return False # not implemented in Mbed TLS
            return True
        if self.head == 'CHACHA20' and alg.head == 'CHACHA20_POLY1305':
            return True
        if self.head in {'ARC4', 'CHACHA20'} and \
           alg.head == 'STREAM_CIPHER':
            return True
        if self.head == 'RSA' and alg.head.startswith('RSA_'):
            return True
        if alg.category == AlgorithmCategory.KEY_AGREEMENT and \
           self.is_public():
            # The PSA API does not use public key objects in key agreement
            # operations: it imports the public key as a formatted byte string.
            # So a public key object with a key agreement algorithm is not
            # a valid combination.
            return False
        if alg.is_invalid_key_agreement_with_derivation():
            return False
        if self.head == 'ECC':
            assert self.params is not None
            eccc = EllipticCurveCategory.from_family(self.params[0])
            if alg.head == 'ECDH' and \
               eccc in {EllipticCurveCategory.SHORT_WEIERSTRASS,
                        EllipticCurveCategory.MONTGOMERY}:
                return True
            if alg.head == 'ECDSA' and \
               eccc == EllipticCurveCategory.SHORT_WEIERSTRASS:
                return True
            if alg.head in {'PURE_EDDSA', 'EDDSA_PREHASH'} and \
               eccc == EllipticCurveCategory.TWISTED_EDWARDS:
                return True
        if self.head == 'DH' and alg.head == 'FFDH':
            return True
        return False


class AlgorithmCategory(enum.Enum):
    """PSA algorithm categories."""
    # The numbers are aligned with the category bits in numerical values of
    # algorithms. The names match the `PSA_ALG_IS_xxx` macros.
    HASH = 2
    MAC = 3
    CIPHER = 4
    AEAD = 5
    SIGN = 6
    ASYMMETRIC_ENCRYPTION = 7
    KEY_DERIVATION = 8
    KEY_AGREEMENT = 9
    PAKE = 10
    KEY_WRAP = 11
    KEY_ENCAPSULATION = 12
    XOF = 13

    def requires_key(self) -> bool:
        """Whether operations in this category are set up with a key."""
        return self not in {
            self.HASH,
            self.XOF,
            self.KEY_DERIVATION, # key passed separately from setup
        }

    def is_asymmetric(self) -> bool:
        """Whether operations in this category involve asymmetric keys."""
        return self in {
            self.SIGN,
            self.ASYMMETRIC_ENCRYPTION,
            self.KEY_AGREEMENT,
            self.KEY_ENCAPSULATION,
        }


class AlgorithmNotRecognized(Exception):
    def __init__(self, expr: str) -> None:
        super().__init__('Algorithm not recognized: ' + expr)
        self.expr = expr


class Algorithm:
    """Knowledge about a PSA algorithm."""

    @staticmethod
    def determine_base(expr: str) -> str:
        """Return an expression for the "base" of the algorithm.

        This strips off variants of algorithms such as MAC truncation.

        This function does not attempt to detect invalid inputs.
        """
        m = re.match(r'PSA_ALG_(?:'
                     r'(?:TRUNCATED|AT_LEAST_THIS_LENGTH)_MAC|'
                     r'AEAD_WITH_(?:SHORTENED|AT_LEAST_THIS_LENGTH)_TAG'
                     r')\((.*),[^,]+\)\Z', expr)
        if m:
            expr = m.group(1)
        return expr

    @staticmethod
    def determine_head(expr: str) -> str:
        """Return the head of an algorithm expression.

        The head is the first (outermost) constructor, without its PSA_ALG_
        prefix, and with some normalization of similar algorithms.
        """
        m = re.match(r'PSA_ALG_(?:DETERMINISTIC_)?(\w+)', expr)
        if not m:
            raise AlgorithmNotRecognized(expr)
        head = m.group(1)
        if head == 'KEY_AGREEMENT':
            m = re.match(r'PSA_ALG_KEY_AGREEMENT\s*\(\s*PSA_ALG_(\w+)', expr)
            if not m:
                raise AlgorithmNotRecognized(expr)
            head = m.group(1)
        head = re.sub(r'_ANY\Z', r'', head)
        if re.match(r'ED[0-9]+PH\Z', head):
            head = 'EDDSA_PREHASH'
        return head

    CATEGORY_FROM_HEAD = {
        'AES_MMO_ZIGBEE': AlgorithmCategory.HASH,
        'ASCON_HASH': AlgorithmCategory.HASH,
        'SHA': AlgorithmCategory.HASH,
        'SHAKE256_512': AlgorithmCategory.HASH,
        'MD': AlgorithmCategory.HASH,
        'RIPEMD': AlgorithmCategory.HASH,
        'SM3': AlgorithmCategory.HASH,
        'ANY_HASH': AlgorithmCategory.HASH,
        'HMAC': AlgorithmCategory.MAC,
        'STREAM_CIPHER': AlgorithmCategory.CIPHER,
        'ASCON_AEAD': AlgorithmCategory.AEAD,
        'CHACHA20_POLY1305': AlgorithmCategory.AEAD,
        'XCHACHA20_POLY1305': AlgorithmCategory.AEAD,
        'DSA': AlgorithmCategory.SIGN,
        'ECDSA': AlgorithmCategory.SIGN,
        'EDDSA': AlgorithmCategory.SIGN,
        'HASH_ML_DSA': AlgorithmCategory.SIGN,
        'HASH_SLH_DSA': AlgorithmCategory.SIGN,
        'HSS': AlgorithmCategory.SIGN,
        'LMS': AlgorithmCategory.SIGN,
        'ML_DSA': AlgorithmCategory.SIGN,
        'PURE_EDDSA': AlgorithmCategory.SIGN,
        'SLH_DSA': AlgorithmCategory.SIGN,
        'RSA_PSS': AlgorithmCategory.SIGN,
        'RSA_PKCS1V15_SIGN': AlgorithmCategory.SIGN,
        'XMMS': AlgorithmCategory.SIGN,
        'XMMS_MT': AlgorithmCategory.SIGN,
        'RSA_PKCS1V15_CRYPT': AlgorithmCategory.ASYMMETRIC_ENCRYPTION,
        'RSA_OAEP': AlgorithmCategory.ASYMMETRIC_ENCRYPTION,
        'HKDF': AlgorithmCategory.KEY_DERIVATION,
        'TLS12_PRF': AlgorithmCategory.KEY_DERIVATION,
        'TLS12_PSK_TO_MS': AlgorithmCategory.KEY_DERIVATION,
        'TLS12_ECJPAKE_TO_PMS': AlgorithmCategory.KEY_DERIVATION,
        'SP800_108': AlgorithmCategory.KEY_DERIVATION,
        'PBKDF': AlgorithmCategory.KEY_DERIVATION,
        'ECDH': AlgorithmCategory.KEY_AGREEMENT,
        'FFDH': AlgorithmCategory.KEY_AGREEMENT,
        # KEY_AGREEMENT(...) is a key derivation with a key agreement component
        'KEY_AGREEMENT': AlgorithmCategory.KEY_DERIVATION,
        'JPAKE': AlgorithmCategory.PAKE,
        'SPAKE2P': AlgorithmCategory.PAKE,
        'KW': AlgorithmCategory.KEY_WRAP,
        'KWP': AlgorithmCategory.KEY_WRAP,
        'ECIES_SEC1': AlgorithmCategory.KEY_ENCAPSULATION,
        'ML_KEM': AlgorithmCategory.KEY_ENCAPSULATION,
        'ASCON_CXOF': AlgorithmCategory.XOF,
        'ASCON_XOF': AlgorithmCategory.XOF,
        'SHAKE': AlgorithmCategory.XOF,
    }
    for x in BLOCK_MAC_MODES:
        CATEGORY_FROM_HEAD[x] = AlgorithmCategory.MAC
    for x in BLOCK_CIPHER_MODES:
        CATEGORY_FROM_HEAD[x] = AlgorithmCategory.CIPHER
    for x in BLOCK_AEAD_MODES:
        CATEGORY_FROM_HEAD[x] = AlgorithmCategory.AEAD

    def determine_category(self, expr: str, head: str) -> AlgorithmCategory:
        """Return the category of the given algorithm expression.

        This function does not attempt to detect invalid inputs.
        """
        prefix = head
        while prefix:
            if prefix in self.CATEGORY_FROM_HEAD:
                return self.CATEGORY_FROM_HEAD[prefix]
            if re.match(r'.*[0-9]\Z', prefix):
                prefix = re.sub(r'_*[0-9]+\Z', r'', prefix)
            else:
                prefix = re.sub(r'_*[^_]*\Z', r'', prefix)
        raise AlgorithmNotRecognized(expr)

    @staticmethod
    def determine_wildcard(expr) -> bool:
        """Whether the given algorithm expression is a wildcard.

        This function does not attempt to detect invalid inputs.
        """
        if re.search(r'\bPSA_ALG_ANY_HASH\b', expr):
            return True
        if re.search(r'_AT_LEAST_', expr):
            return True
        return False

    def __init__(self, expr: str) -> None:
        """Analyze an algorithm value.

        The algorithm must be expressed as a C expression containing only
        calls to PSA algorithm constructor macros and numeric literals.

        This class is only programmed to handle valid expressions. Invalid
        expressions may result in exceptions or in nonsensical results.
        """
        self.expression = re.sub(r'\s+', r'', expr)
        self.base_expression = self.determine_base(self.expression)
        self.head = self.determine_head(self.base_expression)
        self.category = self.determine_category(self.base_expression, self.head)
        self.is_wildcard = self.determine_wildcard(self.expression)

    def get_key_agreement_derivation(self) -> Optional[str]:
        """For a combined key agreement and key derivation algorithm, get the derivation part.

        For anything else, return None.
        """
        if self.category != AlgorithmCategory.KEY_AGREEMENT:
            return None
        m = re.match(r'PSA_ALG_KEY_AGREEMENT\(\w+,\s*(.*)\)\Z', self.expression)
        if not m:
            return None
        kdf_alg = m.group(1)
        # Assume kdf_alg is either a valid KDF or 0.
        if re.match(r'(?:0[Xx])?0+\s*\Z', kdf_alg):
            return None
        return kdf_alg

    KEY_DERIVATIONS_INCOMPATIBLE_WITH_AGREEMENT = frozenset([
        'PSA_ALG_TLS12_ECJPAKE_TO_PMS', # secret input in specific format
    ])
    def is_valid_key_agreement_with_derivation(self) -> bool:
        """Whether this is a valid combined key agreement and key derivation algorithm."""
        kdf_alg = self.get_key_agreement_derivation()
        if kdf_alg is None:
            return False
        return kdf_alg not in self.KEY_DERIVATIONS_INCOMPATIBLE_WITH_AGREEMENT

    def is_invalid_key_agreement_with_derivation(self) -> bool:
        """Whether this is an invalid combined key agreement and key derivation algorithm."""
        kdf_alg = self.get_key_agreement_derivation()
        if kdf_alg is None:
            return False
        return kdf_alg in self.KEY_DERIVATIONS_INCOMPATIBLE_WITH_AGREEMENT

    def short_expression(self, level: int = 0) -> str:
        """Abbreviate the expression, keeping it human-readable.

        See `crypto_knowledge.short_expression`.
        """
        return short_expression(self.expression, level=level)

    HASH_LENGTH_BYTES = {
        'PSA_ALG_AES_MMO_ZIGBEE': 16,
        'PSA_ALG_MD2': 16,
        'PSA_ALG_MD4': 16,
        'PSA_ALG_MD5': 16,
        'PSA_ALG_SHA_1': 20,
        'PSA_ALG_SM3': 32,
    }
    HASH_LENGTH_BITS_RE = re.compile(r'([0-9]+)\Z')
    @classmethod
    def hash_length(cls, alg: str) -> int:
        """The length of the given hash algorithm, in bytes."""
        if alg in cls.HASH_LENGTH_BYTES:
            return cls.HASH_LENGTH_BYTES[alg]
        m = cls.HASH_LENGTH_BITS_RE.search(alg)
        if m:
            return int(m.group(1)) // 8
        raise ValueError('Unknown hash length for ' + alg)

    PERMITTED_TAG_LENGTHS = {
        'PSA_ALG_CCM': frozenset([4, 6, 8, 10, 12, 14, 16]),
        'PSA_ALG_CHACHA20_POLY1305': frozenset([16]),
        'PSA_ALG_GCM': frozenset([4, 8, 12, 13, 14, 15, 16]),
    }
    MAC_LENGTH = {
        'PSA_ALG_CBC_MAC': 16, # actually the block cipher length
        'PSA_ALG_CMAC': 16, # actually the block cipher length
    }
    HMAC_RE = re.compile(r'PSA_ALG_HMAC\((.*)\)\Z')
    @classmethod
    def permitted_truncations(cls, base: str) -> FrozenSet[int]:
        """Permitted output lengths for the given MAC or AEAD base algorithm.

        For a MAC algorithm, this is the set of truncation lengths that
        Mbed TLS supports.
        For an AEAD algorithm, this is the set of truncation lengths that
        are permitted by the algorithm specification.
        """
        if base in cls.PERMITTED_TAG_LENGTHS:
            return cls.PERMITTED_TAG_LENGTHS[base]
        max_length = cls.MAC_LENGTH.get(base, None)
        if max_length is None:
            m = cls.HMAC_RE.match(base)
            if m:
                max_length = cls.hash_length(m.group(1))
        if max_length is None:
            raise ValueError('Unknown permitted lengths for ' + base)
        return frozenset(range(4, max_length + 1))

    TRUNCATED_ALG_RE = re.compile(
        r'(?P<face>PSA_ALG_(?:AEAD_WITH_SHORTENED_TAG|TRUNCATED_MAC))'
        r'\((?P<base>.*),'
        r'(?P<length>0[Xx][0-9A-Fa-f]+|[1-9][0-9]*|0[0-7]*)[LUlu]*\)\Z')
    def is_invalid_truncation(self) -> bool:
        """False for a MAC or AEAD algorithm truncated to an invalid length.

        True for a MAC or AEAD algorithm truncated to a valid length or to
        a length that cannot be determined. True for anything other than
        a truncated MAC or AEAD.
        """
        m = self.TRUNCATED_ALG_RE.match(self.expression)
        if m:
            base = m.group('base')
            to_length = int(m.group('length'), 0)
            permitted_lengths = self.permitted_truncations(base)
            if to_length not in permitted_lengths:
                return True
        return False

    def is_valid_for_operation(self) -> bool:
        """Whether this algorithm construction is valid for an operation.

        This function assumes that the algorithm is constructed in a
        "grammatically" correct way, and only rejects semantically invalid
        combinations.
        """
        if self.is_wildcard:
            return False
        if self.is_invalid_truncation():
            return False
        return True

    def can_do(self, category: AlgorithmCategory) -> bool:
        """Whether this algorithm can perform operations in the given category.
        """
        if category == self.category:
            return True
        if category == AlgorithmCategory.KEY_DERIVATION and \
           self.is_valid_key_agreement_with_derivation():
            return True
        return False

    def usage_flags(self, public: bool = False) -> List[str]:
        """The list of usage flags describing operations that can perform this algorithm.

        If public is true, only return public-key operations, not private-key operations.
        """
        if self.category == AlgorithmCategory.HASH:
            flags = []
        elif self.category == AlgorithmCategory.MAC:
            flags = ['SIGN_HASH', 'SIGN_MESSAGE',
                     'VERIFY_HASH', 'VERIFY_MESSAGE']
        elif self.category == AlgorithmCategory.CIPHER or \
             self.category == AlgorithmCategory.AEAD:
            flags = ['DECRYPT', 'ENCRYPT']
        elif self.category == AlgorithmCategory.SIGN:
            flags = ['VERIFY_HASH', 'VERIFY_MESSAGE']
            if not public:
                flags += ['SIGN_HASH', 'SIGN_MESSAGE']
        elif self.category == AlgorithmCategory.ASYMMETRIC_ENCRYPTION:
            flags = ['ENCRYPT']
            if not public:
                flags += ['DECRYPT']
        elif self.category == AlgorithmCategory.KEY_DERIVATION or \
             self.category == AlgorithmCategory.KEY_AGREEMENT:
            flags = ['DERIVE']
        else:
            raise AlgorithmNotRecognized(self.expression)
        return ['PSA_KEY_USAGE_' + flag for flag in flags]
