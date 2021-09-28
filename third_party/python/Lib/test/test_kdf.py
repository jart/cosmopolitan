import hashlib
import unittest
import binascii

TRANS_5C = bytes((x ^ 0x5C) for x in range(256))
TRANS_36 = bytes((x ^ 0x36) for x in range(256))

class Pbkdf2Test(unittest.TestCase):
    def test_rfc6070_sha1_iter1(self):
      self.assertEqual(
          '0c60c80f961f0e71f3a9b524af6012062fe037a6',
          hashlib.pbkdf2_hmac(hash_name='sha1',
                              password=b'password',
                              salt=b'salt',
                              iterations=1,
                              dklen=20).hex())

    def test_rfc6070_sha1_iter2(self):
        self.assertEqual(
            'ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957',
            hashlib.pbkdf2_hmac(hash_name='sha1',
                                password=b'password',
                                salt=b'salt',
                                iterations=2,
                                dklen=20).hex())

def pbkdf2_hmac_golden(hash_name, password, salt, iterations, dklen=None):
    """Password based key derivation function 2 (PKCS #5 v2.0)

    This Python implementations based on the hmac module about as fast
    as Mbedtls's PKCS5_PBKDF2_HMAC for short passwords and much faster
    for long passwords.
    """
    if not isinstance(hash_name, str):
        raise TypeError(hash_name)

    if not isinstance(password, (bytes, bytearray)):
        password = bytes(memoryview(password))
    if not isinstance(salt, (bytes, bytearray)):
        salt = bytes(memoryview(salt))

    # Fast inline HMAC implementation
    inner = hashlib.new(hash_name)
    outer = hashlib.new(hash_name)
    blocksize = getattr(inner, 'block_size', 64)
    if len(password) > blocksize:
        password = hashlib.new(hash_name, password).digest()
    password = password + b'\x00' * (blocksize - len(password))
    inner.update(password.translate(TRANS_36))
    outer.update(password.translate(TRANS_5C))

    def prf(msg, inner=inner, outer=outer):
        # PBKDF2_HMAC uses the password as key. We can re-use the same
        # digest objects and just update copies to skip initialization.
        icpy = inner.copy()
        ocpy = outer.copy()
        icpy.update(msg)
        ocpy.update(icpy.digest())
        return ocpy.digest()

    if iterations < 1:
        raise ValueError(iterations)
    if dklen is None:
        dklen = outer.digest_size
    if dklen < 1:
        raise ValueError(dklen)

    dkey = b''
    loop = 1
    from_bytes = int.from_bytes
    while len(dkey) < dklen:
        prev = prf(salt + loop.to_bytes(4, 'big'))
        # endianess doesn't matter here as long to / from use the same
        rkey = int.from_bytes(prev, 'big')
        for i in range(iterations - 1):
            prev = prf(prev)
            # rkey = rkey ^ prev
            rkey ^= from_bytes(prev, 'big')
        loop += 1
        dkey += rkey.to_bytes(inner.digest_size, 'big')

    return dkey[:dklen]

class KDFTests(unittest.TestCase):

    pbkdf2_test_vectors = [
        (b'password', b'salt', 1, None),
        (b'password', b'salt', 2, None),
        (b'password', b'salt', 4096, None),
        # too slow, it takes over a minute on a fast CPU.
        #(b'password', b'salt', 16777216, None),
        (b'passwordPASSWORDpassword', b'saltSALTsaltSALTsaltSALTsaltSALTsalt',
         4096, -1),
        (b'pass\0word', b'sa\0lt', 4096, 16),
    ]

    scrypt_test_vectors = [
        (b'', b'', 16, 1, 1, binascii.unhexlify('77d6576238657b203b19ca42c18a0497f16b4844e3074ae8dfdffa3fede21442fcd0069ded0948f8326a753a0fc81f17e8d3e0fb2e0d3628cf35e20c38d18906')),
        (b'password', b'NaCl', 1024, 8, 16, binascii.unhexlify('fdbabe1c9d3472007856e7190d01e9fe7c6ad7cbc8237830e77376634b3731622eaf30d92e22a3886ff109279d9830dac727afb94a83ee6d8360cbdfa2cc0640')),
        (b'pleaseletmein', b'SodiumChloride', 16384, 8, 1, binascii.unhexlify('7023bdcb3afd7348461c06cd81fd38ebfda8fbba904f8e3ea9b543f6545da1f2d5432955613f0fcf62d49705242a9af9e61e85dc0d651e40dfcf017b45575887')),
   ]

    pbkdf2_results = {
        "sha1": [
            # official test vectors from RFC 6070
            (bytes.fromhex('0c60c80f961f0e71f3a9b524af6012062fe037a6'), None),
            (bytes.fromhex('ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957'), None),
            (bytes.fromhex('4b007901b765489abead49d926f721d065a429c1'), None),
            #(bytes.fromhex('eefe3d61cd4da4e4e9945b3d6ba2158c2634e984'), None),
            (bytes.fromhex('3d2eec4fe41c849b80c8d83662c0e44a8b291a964c'
                           'f2f07038'), 25),
            (bytes.fromhex('56fa6aa75548099dcc37d7f03425e0c3'), None),],
        "sha256": [
            (bytes.fromhex('120fb6cffcf8b32c43e7225256c4f837'
                           'a86548c92ccc35480805987cb70be17b'), None),
            (bytes.fromhex('ae4d0c95af6b46d32d0adff928f06dd0'
                           '2a303f8ef3c251dfd6e2d85a95474c43'), None),
            (bytes.fromhex('c5e478d59288c841aa530db6845c4c8d'
                           '962893a001ce4e11a4963873aa98134a'), None),
            #(bytes.fromhex('cf81c66fe8cfc04d1f31ecb65dab4089'
            #               'f7f179e89b3b0bcb17ad10e3ac6eba46'), None),
            (bytes.fromhex('348c89dbcbd32b2f32d814b8116e84cf2b17'
                           '347ebc1800181c4e2a1fb8dd53e1c635518c7dac47e9'), 40),
            (bytes.fromhex('89b69d0516f829893c696226650a8687'), None),],
        "sha512": [
            (bytes.fromhex('867f70cf1ade02cff3752599a3a53dc4af34c7a669815ae5'
                           'd513554e1c8cf252c02d470a285a0501bad999bfe943c08f'
                           '050235d7d68b1da55e63f73b60a57fce'), None),
            (bytes.fromhex('e1d9c16aa681708a45f5c7c4e215ceb66e011a2e9f004071'
                           '3f18aefdb866d53cf76cab2868a39b9f7840edce4fef5a82'
                           'be67335c77a6068e04112754f27ccf4e'), None),
            (bytes.fromhex('d197b1b33db0143e018b12f3d1d1479e6cdebdcc97c5c0f8'
                           '7f6902e072f457b5143f30602641b3d55cd335988cb36b84'
                           '376060ecd532e039b742a239434af2d5'), None),
            (bytes.fromhex('8c0511f4c6e597c6ac6315d8f0362e225f3c501495ba23b8'
                           '68c005174dc4ee71115b59f9e60cd9532fa33e0f75aefe30'
                           '225c583a186cd82bd4daea9724a3d3b8'), 64),
            (bytes.fromhex('9d9e9c4cd21fe4be24d5b8244c759665'), None),],
    }

    def _test_pbkdf2_hmac(self, pbkdf2):
        for digest_name, results in self.pbkdf2_results.items():
            for i, vector in enumerate(self.pbkdf2_test_vectors):
                password, salt, rounds, dklen = vector
                expected, overwrite_dklen = results[i]
                if overwrite_dklen:
                    dklen = overwrite_dklen
                out = pbkdf2(digest_name, password, salt, rounds, dklen)
                self.assertEqual(out, expected,
                                 (digest_name, password, salt, rounds, dklen))
                out = pbkdf2(digest_name, memoryview(password),
                             memoryview(salt), rounds, dklen)
                out = pbkdf2(digest_name, bytearray(password),
                             bytearray(salt), rounds, dklen)
                self.assertEqual(out, expected)
                if dklen is None:
                    out = pbkdf2(digest_name, password, salt, rounds)
                    self.assertEqual(out, expected,
                                     (digest_name, password, salt, rounds))
        self.assertRaises(TypeError, pbkdf2, b'sha1', b'pass', b'salt', 1)
        self.assertRaises(TypeError, pbkdf2, 'sha1', 'pass', 'salt', 1)
        self.assertRaises(ValueError, pbkdf2, 'sha1', b'pass', b'salt', 0)
        self.assertRaises(ValueError, pbkdf2, 'sha1', b'pass', b'salt', -1)
        self.assertRaises(ValueError, pbkdf2, 'sha1', b'pass', b'salt', 1, 0)
        self.assertRaises(ValueError, pbkdf2, 'sha1', b'pass', b'salt', 1, -1)
        with self.assertRaisesRegex(ValueError, 'unsupported hash type'):
            pbkdf2('unknown', b'pass', b'salt', 1)
        out = pbkdf2(hash_name='sha1', password=b'password', salt=b'salt',
            iterations=1, dklen=None)
        self.assertEqual(out, self.pbkdf2_results['sha1'][0][0])

    def test_pbkdf2_hmac_py(self):
        self._test_pbkdf2_hmac(pbkdf2_hmac_golden)

    def test_pbkdf2_hmac_c(self):
        self._test_pbkdf2_hmac(hashlib.pbkdf2_hmac)

    @unittest.skipUnless(hasattr(hashlib, 'scrypt'),
                         'Test requires OpenSSL > 1.1')
    def test_scrypt(self):
        for password, salt, n, r, p, expected in self.scrypt_test_vectors:
            result = hashlib.scrypt(password, salt=salt, n=n, r=r, p=p)
            self.assertEqual(result, expected)

        # this values should work
        hashlib.scrypt(b'password', salt=b'salt', n=2, r=8, p=1)
        # password and salt must be bytes-like
        with self.assertRaises(TypeError):
            hashlib.scrypt('password', salt=b'salt', n=2, r=8, p=1)
        with self.assertRaises(TypeError):
            hashlib.scrypt(b'password', salt='salt', n=2, r=8, p=1)
        # require keyword args
        with self.assertRaises(TypeError):
            hashlib.scrypt(b'password')
        with self.assertRaises(TypeError):
            hashlib.scrypt(b'password', b'salt')
        with self.assertRaises(TypeError):
            hashlib.scrypt(b'password', 2, 8, 1, salt=b'salt')
        for n in [-1, 0, 1, None]:
            with self.assertRaises((ValueError, OverflowError, TypeError)):
                hashlib.scrypt(b'password', salt=b'salt', n=n, r=8, p=1)
        for r in [-1, 0, None]:
            with self.assertRaises((ValueError, OverflowError, TypeError)):
                hashlib.scrypt(b'password', salt=b'salt', n=2, r=r, p=1)
        for p in [-1, 0, None]:
            with self.assertRaises((ValueError, OverflowError, TypeError)):
                hashlib.scrypt(b'password', salt=b'salt', n=2, r=8, p=p)
        for maxmem in [-1, None]:
            with self.assertRaises((ValueError, OverflowError, TypeError)):
                hashlib.scrypt(b'password', salt=b'salt', n=2, r=8, p=1,
                               maxmem=maxmem)
        for dklen in [-1, None]:
            with self.assertRaises((ValueError, OverflowError, TypeError)):
                hashlib.scrypt(b'password', salt=b'salt', n=2, r=8, p=1,
                               dklen=dklen)

if __name__ == '__main__':
    unittest.main()
