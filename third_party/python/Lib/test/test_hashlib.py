# Test hashlib module
#
# $Id$
#
#  Copyright (C) 2005-2010   Gregory P. Smith (greg@krypto.org)
#  Licensed to PSF under a Contributor Agreement.
#

import array
from binascii import unhexlify
import hashlib
import importlib
import itertools
import os
import sys
try:
    import _thread
    import threading
except ImportError:
    threading = None
import unittest
import warnings
from test import support
from test.support import _4G, bigmemtest, import_fresh_module
from http.client import HTTPException

# if __name__ == 'PYOBJ.COM':
#     import _sha3 # what a horror show

# Were we compiled --with-pydebug or with #define Py_DEBUG?
COMPILED_WITH_PYDEBUG = hasattr(sys, 'gettotalrefcount')

# [jart] wut
c_hashlib = import_fresh_module('hashlib', fresh=['_hashlib'])
py_hashlib = import_fresh_module('hashlib', blocked=['_hashlib'])

try:
    import _sha3
except ImportError:
    _sha3 = None

requires_sha3 = unittest.skipUnless(_sha3, 'requires _sha3')


def hexstr(s):
    assert isinstance(s, bytes), repr(s)
    h = "0123456789abcdef"
    r = ''
    for i in s:
        r += h[(i >> 4) & 0xF] + h[i & 0xF]
    return r


def read_vectors(hash_name):
    # [jart] modified to not phone home
    with open('/zip/.python/test/%s.txt' % (hash_name)) as testdata:
        for line in testdata:
            line = line.strip()
            if line.startswith('#') or not line:
                continue
            parts = line.split(',')
            parts[0] = bytes.fromhex(parts[0])
            yield parts


class HashLibTestCase(unittest.TestCase):
    shakes = {'shake_128', 'shake_256'}
    supported_hash_names = ( 'md5', 'MD5', 'sha1', 'SHA1',
                             'sha224', 'SHA224', 'sha256', 'SHA256',
                             'sha384', 'SHA384', 'sha512', 'SHA512',
                             # 'sha3_224',  'sha3_256',  'sha3_384',
                             # 'sha3_512', 'shake_128', 'shake_256',
                             'blake2b256',
                            )

    # Issue #14693: fallback modules are always compiled under POSIX
    # [jart] don't care about sha3 don't care don't care
    _warn_on_extension_import = False # os.name == 'posix' or COMPILED_WITH_PYDEBUG

    def _conditional_import_module(self, module_name):
        """Import a module and return a reference to it or None on failure."""
        try:
            return importlib.import_module(module_name)
        except ModuleNotFoundError as error:
            if self._warn_on_extension_import:
                warnings.warn('Did a C extension fail to compile? %s' % error)
        return None

    def __init__(self, *args, **kwargs):
        algorithms = set()
        for algorithm in self.supported_hash_names:
            algorithms.add(algorithm.lower())

        self.constructors_to_test = {}
        for algorithm in algorithms:
            self.constructors_to_test[algorithm] = set()

        # For each algorithm, test the direct constructor and the use
        # of hashlib.new given the algorithm name.
        for algorithm, constructors in self.constructors_to_test.items():
            constructors.add(getattr(hashlib, algorithm))
            def _test_algorithm_via_hashlib_new(data=None, _alg=algorithm, **kwargs):
                if data is None:
                    return hashlib.new(_alg, **kwargs)
                return hashlib.new(_alg, data, **kwargs)
            constructors.add(_test_algorithm_via_hashlib_new)

        _hashlib = self._conditional_import_module('_hashlib')
        if _hashlib:
            # These two algorithms should always be present when this module
            # is compiled.  If not, something was compiled wrong.
            self.assertTrue(hasattr(_hashlib, 'mbedtls_md5'))
            self.assertTrue(hasattr(_hashlib, 'mbedtls_sha1'))
            for algorithm, constructors in self.constructors_to_test.items():
                constructor = getattr(_hashlib, 'mbedtls_'+algorithm, None)
                if constructor:
                    constructors.add(constructor)

        def add_builtin_constructor(name):
            constructor = getattr(hashlib, "__get_builtin_constructor")(name)
            self.constructors_to_test[name].add(constructor)

        _md5 = self._conditional_import_module('_md5')
        if _md5:
            add_builtin_constructor('md5')
        _sha1 = self._conditional_import_module('_sha1')
        if _sha1:
            add_builtin_constructor('sha1')
        _sha256 = self._conditional_import_module('_sha256')
        if _sha256:
            add_builtin_constructor('sha224')
            add_builtin_constructor('sha256')
        _sha512 = self._conditional_import_module('_sha512')
        if _sha512:
            add_builtin_constructor('sha384')
            add_builtin_constructor('sha512')
        _sha3 = self._conditional_import_module('_sha3')
        if _sha3:
            add_builtin_constructor('sha3_224')
            add_builtin_constructor('sha3_256')
            add_builtin_constructor('sha3_384')
            add_builtin_constructor('sha3_512')
            add_builtin_constructor('shake_128')
            add_builtin_constructor('shake_256')

        super(HashLibTestCase, self).__init__(*args, **kwargs)

    @property
    def hash_constructors(self):
        constructors = self.constructors_to_test.values()
        return itertools.chain.from_iterable(constructors)

    @support.refcount_test
    @unittest.skipIf(c_hashlib is None, 'Require _hashlib module')
    def test_refleaks_in_hash___init__(self):
        gettotalrefcount = support.get_attribute(sys, 'gettotalrefcount')
        sha1_hash = c_hashlib.new('sha1')
        refs_before = gettotalrefcount()
        for i in range(100):
            sha1_hash.__init__('sha1')
        self.assertAlmostEqual(gettotalrefcount() - refs_before, 0, delta=10)

    @unittest.skip('[jart] what')
    def test_hash_array(self):
        a = array.array("b", range(10))
        for cons in self.hash_constructors:
            c = cons(a)
            if c.name in self.shakes:
                c.hexdigest(16)
            else:
                c.hexdigest()

    def test_algorithms_guaranteed(self):
        self.assertEqual(hashlib.algorithms_guaranteed,
            set(_algo for _algo in self.supported_hash_names
                  if _algo.islower()))

    def test_algorithms_available(self):
        self.assertTrue(set(hashlib.algorithms_guaranteed).
                            issubset(hashlib.algorithms_available))

    @unittest.skip('[jart] dont care about sha3 dont care dont care')
    def test_unknown_hash(self):
        self.assertRaises(ValueError, hashlib.new, 'spam spam spam spam spam')
        self.assertRaises(TypeError, hashlib.new, 1)

    def test_get_builtin_constructor(self):
        get_builtin_constructor = getattr(hashlib,
                                          '__get_builtin_constructor')
        builtin_constructor_cache = getattr(hashlib,
                                            '__builtin_constructor_cache')
        self.assertRaises(ValueError, get_builtin_constructor, 'test')
        try:
            import _md5
        except ImportError:
            self.skipTest("_md5 module not available")
        # This forces an ImportError for "import _md5" statements
        sys.modules['_md5'] = None
        # clear the cache
        builtin_constructor_cache.clear()
        try:
            self.assertRaises(ValueError, get_builtin_constructor, 'md5')
        finally:
            if '_md5' in locals():
                sys.modules['_md5'] = _md5
            else:
                del sys.modules['_md5']
        self.assertRaises(TypeError, get_builtin_constructor, 3)
        constructor = get_builtin_constructor('md5')
        self.assertIs(constructor, _md5.md5)
        self.assertEqual(sorted(builtin_constructor_cache), ['MD5', 'md5'])

    def test_hexdigest(self):
        for cons in self.hash_constructors:
            h = cons()
            if h.name in self.shakes:
                self.assertIsInstance(h.digest(16), bytes)
                self.assertEqual(hexstr(h.digest(16)), h.hexdigest(16))
            else:
                self.assertIsInstance(h.digest(), bytes)
                self.assertEqual(hexstr(h.digest()), h.hexdigest())

    def test_digest_length_overflow(self):
        # See issue #34922
        large_sizes = (2**29, 2**32-10, 2**32+10, 2**61, 2**64-10, 2**64+10)
        for cons in self.hash_constructors:
            h = cons()
            if h.name not in self.shakes:
                continue
            for digest in h.digest, h.hexdigest:
                with self.assertRaises((ValueError, OverflowError)):
                    digest(-10)
                for length in large_sizes:
                    with self.assertRaises((ValueError, OverflowError)):
                        digest(length)

    def test_name_attribute(self):
        for cons in self.hash_constructors:
            h = cons()
            self.assertIsInstance(h.name, str)
            if h.name in self.supported_hash_names:
                self.assertIn(h.name, self.supported_hash_names)
            else:
                self.assertNotIn(h.name, self.supported_hash_names)
            self.assertEqual(h.name, hashlib.new(h.name).name)

    def test_large_update(self):
        aas = b'a' * 128
        bees = b'b' * 127
        cees = b'c' * 126
        dees = b'd' * 2048 #  HASHLIB_GIL_MINSIZE

        for cons in self.hash_constructors:
            m1 = cons()
            m1.update(aas)
            m1.update(bees)
            m1.update(cees)
            m1.update(dees)
            if m1.name in self.shakes:
                args = (16,)
            else:
                args = ()

            m2 = cons()
            m2.update(aas + bees + cees + dees)
            self.assertEqual(m1.digest(*args), m2.digest(*args))

            m3 = cons(aas + bees + cees + dees)
            self.assertEqual(m1.digest(*args), m3.digest(*args))

            # verify copy() doesn't touch original
            m4 = cons(aas + bees + cees)
            m4_digest = m4.digest(*args)
            m4_copy = m4.copy()
            m4_copy.update(dees)
            self.assertEqual(m1.digest(*args), m4_copy.digest(*args))
            self.assertEqual(m4.digest(*args), m4_digest)

    def check(self, name, data, hexdigest, shake=False, **kwargs):
        length = len(hexdigest)//2
        hexdigest = hexdigest.lower()
        constructors = self.constructors_to_test[name]
        # 2 is for hashlib.name(...) and hashlib.new(name, ...)
        self.assertGreaterEqual(len(constructors), 2)
        for hash_object_constructor in constructors:
            m = hash_object_constructor(data, **kwargs)
            computed = m.hexdigest() if not shake else m.hexdigest(length)
            self.assertEqual(
                    computed, hexdigest,
                    "Hash algorithm %s constructed using %s returned hexdigest"
                    " %r for %d byte input data that should have hashed to %r."
                    % (name, hash_object_constructor,
                       computed, len(data), hexdigest))
            computed = m.digest() if not shake else m.digest(length)
            digest = bytes.fromhex(hexdigest)
            self.assertEqual(computed, digest)
            if not shake:
                self.assertEqual(len(digest), m.digest_size)

    def check_no_unicode(self, algorithm_name):
        # Unicode objects are not allowed as input.
        constructors = self.constructors_to_test[algorithm_name]
        for hash_object_constructor in constructors:
            self.assertRaises(TypeError, hash_object_constructor, 'spam')

    def test_no_unicode(self):
        self.check_no_unicode('md5')
        self.check_no_unicode('sha1')
        self.check_no_unicode('sha224')
        self.check_no_unicode('sha256')
        self.check_no_unicode('sha384')
        self.check_no_unicode('sha512')

    @requires_sha3
    def test_no_unicode_sha3(self):
        self.check_no_unicode('sha3_224')
        self.check_no_unicode('sha3_256')
        self.check_no_unicode('sha3_384')
        self.check_no_unicode('sha3_512')
        self.check_no_unicode('shake_128')
        self.check_no_unicode('shake_256')

    def check_blocksize_name(self, name, block_size=0, digest_size=0,
                             digest_length=None):
        constructors = self.constructors_to_test[name]
        for hash_object_constructor in constructors:
            m = hash_object_constructor()
            self.assertEqual(m.block_size, block_size)
            self.assertEqual(m.digest_size, digest_size)
            if digest_length:
                self.assertEqual(len(m.digest(digest_length)),
                                 digest_length)
                self.assertEqual(len(m.hexdigest(digest_length)),
                                 2*digest_length)
            else:
                self.assertEqual(len(m.digest()), digest_size)
                self.assertEqual(len(m.hexdigest()), 2*digest_size)
            self.assertEqual(m.name, name)
            # split for sha3_512 / _sha3.sha3 object
            self.assertIn(name.split("_")[0], repr(m))

    @unittest.skip('[jart] bad test')
    def test_blocksize_name(self):
        self.check_blocksize_name('md5', 64, 16)
        self.check_blocksize_name('sha1', 64, 20)
        self.check_blocksize_name('sha224', 64, 28)
        self.check_blocksize_name('sha256', 64, 32)
        self.check_blocksize_name('sha384', 128, 48)
        self.check_blocksize_name('sha512', 128, 64)

    @requires_sha3
    def test_blocksize_name_sha3(self):
        self.check_blocksize_name('sha3_224', 144, 28)
        self.check_blocksize_name('sha3_256', 136, 32)
        self.check_blocksize_name('sha3_384', 104, 48)
        self.check_blocksize_name('sha3_512', 72, 64)
        self.check_blocksize_name('shake_128', 168, 0, 32)
        self.check_blocksize_name('shake_256', 136, 0, 64)

    def check_sha3(self, name, capacity, rate, suffix):
        constructors = self.constructors_to_test[name]
        for hash_object_constructor in constructors:
            m = hash_object_constructor()
            self.assertEqual(capacity + rate, 1600)
            self.assertEqual(m._capacity_bits, capacity)
            self.assertEqual(m._rate_bits, rate)
            self.assertEqual(m._suffix, suffix)

    @requires_sha3
    def test_extra_sha3(self):
        self.check_sha3('sha3_224', 448, 1152, b'\x06')
        self.check_sha3('sha3_256', 512, 1088, b'\x06')
        self.check_sha3('sha3_384', 768, 832, b'\x06')
        self.check_sha3('sha3_512', 1024, 576, b'\x06')
        self.check_sha3('shake_128', 256, 1344, b'\x1f')
        self.check_sha3('shake_256', 512, 1088, b'\x1f')

    def test_case_md5_0(self):
        self.check('md5', b'', 'd41d8cd98f00b204e9800998ecf8427e')

    def test_case_md5_1(self):
        self.check('md5', b'abc', '900150983cd24fb0d6963f7d28e17f72')

    def test_case_md5_2(self):
        self.check('md5',
                   b'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789',
                   'd174ab98d277d9f5a5611c2c9f419d9f')

    @unittest.skipIf(sys.maxsize < _4G + 5, 'test cannot run on 32-bit systems')
    @bigmemtest(size=_4G + 5, memuse=1, dry_run=False)
    def test_case_md5_huge(self, size):
        self.check('md5', b'A'*size, 'c9af2dff37468ce5dfee8f2cfc0a9c6d')

    @unittest.skipIf(sys.maxsize < _4G - 1, 'test cannot run on 32-bit systems')
    @bigmemtest(size=_4G - 1, memuse=1, dry_run=False)
    def test_case_md5_uintmax(self, size):
        self.check('md5', b'A'*size, '28138d306ff1b8281f1a9067e1a1a2b3')

    # use the three examples from Federal Information Processing Standards
    # Publication 180-1, Secure Hash Standard,  1995 April 17
    # http://www.itl.nist.gov/div897/pubs/fip180-1.htm

    def test_case_sha1_0(self):
        self.check('sha1', b"",
                   "da39a3ee5e6b4b0d3255bfef95601890afd80709")

    def test_case_sha1_1(self):
        self.check('sha1', b"abc",
                   "a9993e364706816aba3e25717850c26c9cd0d89d")

    def test_case_sha1_2(self):
        self.check('sha1',
                   b"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
                   "84983e441c3bd26ebaae4aa1f95129e5e54670f1")

    def test_case_sha1_3(self):
        self.check('sha1', b"a" * 1000000,
                   "34aa973cd4c4daa4f61eeb2bdbad27316534016f")


    # use the examples from Federal Information Processing Standards
    # Publication 180-2, Secure Hash Standard,  2002 August 1
    # http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf

    def test_case_sha224_0(self):
        self.check('sha224', b"",
          "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f")

    def test_case_sha224_1(self):
        self.check('sha224', b"abc",
          "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7")

    def test_case_sha224_2(self):
        self.check('sha224',
          b"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
          "75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525")

    def test_case_sha224_3(self):
        self.check('sha224', b"a" * 1000000,
          "20794655980c91d8bbb4c1ea97618a4bf03f42581948b2ee4ee7ad67")


    def test_case_sha256_0(self):
        self.check('sha256', b"",
          "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")

    def test_case_sha256_1(self):
        self.check('sha256', b"abc",
          "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")

    def test_case_sha256_2(self):
        self.check('sha256',
          b"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
          "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1")

    def test_case_sha256_3(self):
        self.check('sha256', b"a" * 1000000,
          "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0")


    def test_case_sha384_0(self):
        self.check('sha384', b"",
          "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da"+
          "274edebfe76f65fbd51ad2f14898b95b")

    def test_case_sha384_1(self):
        self.check('sha384', b"abc",
          "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed"+
          "8086072ba1e7cc2358baeca134c825a7")

    def test_case_sha384_2(self):
        self.check('sha384',
                   b"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"+
                   b"hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
          "09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712"+
          "fcc7c71a557e2db966c3e9fa91746039")

    def test_case_sha384_3(self):
        self.check('sha384', b"a" * 1000000,
          "9d0e1809716474cb086e834e310a4a1ced149e9c00f248527972cec5704c2a5b"+
          "07b8b3dc38ecc4ebae97ddd87f3d8985")


    def test_case_sha512_0(self):
        self.check('sha512', b"",
          "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"+
          "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e")

    def test_case_sha512_1(self):
        self.check('sha512', b"abc",
          "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"+
          "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f")

    def test_case_sha512_2(self):
        self.check('sha512',
                   b"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"+
                   b"hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
          "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018"+
          "501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909")

    def test_case_sha512_3(self):
        self.check('sha512', b"a" * 1000000,
          "e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973eb"+
          "de0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b")

    @requires_sha3
    def test_case_sha3_224_0(self):
        self.check('sha3_224', b"",
          "6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7")

    @requires_sha3
    def test_case_sha3_224_vector(self):
        for msg, md in read_vectors('sha3_224'):
            self.check('sha3_224', msg, md)

    @requires_sha3
    def test_case_sha3_256_0(self):
        self.check('sha3_256', b"",
          "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a")

    @requires_sha3
    def test_case_sha3_256_vector(self):
        for msg, md in read_vectors('sha3_256'):
            self.check('sha3_256', msg, md)

    @requires_sha3
    def test_case_sha3_384_0(self):
        self.check('sha3_384', b"",
          "0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2a"+
          "c3713831264adb47fb6bd1e058d5f004")

    @requires_sha3
    def test_case_sha3_384_vector(self):
        for msg, md in read_vectors('sha3_384'):
            self.check('sha3_384', msg, md)

    @requires_sha3
    def test_case_sha3_512_0(self):
        self.check('sha3_512', b"",
          "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a6"+
          "15b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26")

    @requires_sha3
    def test_case_sha3_512_vector(self):
        for msg, md in read_vectors('sha3_512'):
            self.check('sha3_512', msg, md)

    @requires_sha3
    def test_case_shake_128_0(self):
        self.check('shake_128', b"",
          "7f9c2ba4e88f827d616045507605853ed73b8093f6efbc88eb1a6eacfa66ef26",
          True)
        self.check('shake_128', b"", "7f9c", True)

    @requires_sha3
    def test_case_shake128_vector(self):
        for msg, md in read_vectors('shake_128'):
            self.check('shake_128', msg, md, True)

    @requires_sha3
    def test_case_shake_256_0(self):
        self.check('shake_256', b"",
          "46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762f",
          True)
        self.check('shake_256', b"", "46b9", True)

    @requires_sha3
    def test_case_shake256_vector(self):
        for msg, md in read_vectors('shake_256'):
            self.check('shake_256', msg, md, True)

    def test_gil(self):
        # Check things work fine with an input larger than the size required
        # for multithreaded operation (which is hardwired to 2048).
        gil_minsize = 2048

        for cons in self.hash_constructors:
            m = cons()
            m.update(b'1')
            m.update(b'#' * gil_minsize)
            m.update(b'1')

            m = cons(b'x' * gil_minsize)
            m.update(b'1')

        m = hashlib.md5()
        m.update(b'1')
        m.update(b'#' * gil_minsize)
        m.update(b'1')
        self.assertEqual(m.hexdigest(), 'cb1e1a2cbc80be75e19935d621fb9b21')

        m = hashlib.md5(b'x' * gil_minsize)
        self.assertEqual(m.hexdigest(), 'cfb767f225d58469c5de3632a8803958')

    @unittest.skipUnless(threading, 'Threading required for this test.')
    @support.reap_threads
    def test_threaded_hashing(self):
        # Updating the same hash object from several threads at once
        # using data chunk sizes containing the same byte sequences.
        #
        # If the internal locks are working to prevent multiple
        # updates on the same object from running at once, the resulting
        # hash will be the same as doing it single threaded upfront.
        hasher = hashlib.sha1()
        num_threads = 5
        smallest_data = b'swineflu'
        data = smallest_data * 200000
        expected_hash = hashlib.sha1(data*num_threads).hexdigest()

        def hash_in_chunks(chunk_size):
            index = 0
            while index < len(data):
                hasher.update(data[index:index + chunk_size])
                index += chunk_size

        threads = []
        for threadnum in range(num_threads):
            chunk_size = len(data) // (10 ** threadnum)
            self.assertGreater(chunk_size, 0)
            self.assertEqual(chunk_size % len(smallest_data), 0)
            thread = threading.Thread(target=hash_in_chunks,
                                      args=(chunk_size,))
            threads.append(thread)

        for thread in threads:
            thread.start()
        for thread in threads:
            thread.join()

        self.assertEqual(expected_hash, hasher.hexdigest())


if __name__ == "__main__":
    unittest.main()
