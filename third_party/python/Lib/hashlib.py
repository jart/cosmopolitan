#  Copyright (C) 2005-2010   Gregory P. Smith (greg@krypto.org)
#  Licensed to PSF under a Contributor Agreement.
#

__doc__ = """hashlib module - A common interface to many hash functions.

new(name, data=b'', **kwargs) - returns a new hash object implementing
                                the given hash function; initializing
                                the hash using the given binary data.

Named constructor functions are also available, these are faster
than using new(name):

md5(), sha1(), sha224(), sha256(), sha384(), sha512(), sha3_224(),
sha3_256(), sha3_384(), sha3_512(), shake_128(), shake_256(), and
finally blake2b256() which is an Actually Portable Python feature

 - zlib.crc32 n=22851              46 ps/byte             20 GB/s
 - hashlib.md5 n=22851              1 ns/byte            676 mb/s
 - hashlib.sha1 n=22851           516 ps/byte          1,892 mb/s
 - hashlib.sha256 n=22851         537 ps/byte          1,818 mb/s
 - hashlib.sha384 n=22851           1 ns/byte            800 mb/s
 - hashlib.sha512 n=22851           1 ns/byte            802 mb/s
 - hashlib.blake2b256 n=22851       1 ns/byte            712 mb/s

More algorithms may be available on your platform but the above are
guaranteed to exist. See algorithms_guaranteed/algorithms_available
to find out what algorithm names can be passed to new().

NOTE: If you want the adler32 or crc32 hash functions they are available
      in the zlib module.

Hash objects have these methods:

 - update(data): Update the hash object with the bytes in data. Repeated
                 calls are equivalent to a single call with the
                 concatenation of all the arguments.
 - digest():     Return the digest of the bytes passed to the update()
                 method so far as a bytes object.
 - hexdigest():  Like digest() except the digest is returned as a string
                 of double length, containing only hexadecimal digits.
 - copy():       Return a copy (clone) of the hash object. This can be
                 used to efficiently compute the digests of datas that
                 share a common initial substring.

For example, to obtain the digest of the byte string 'Nobody inspects the
spammish repetition':

    >>> import hashlib
    >>> m = hashlib.blake2b256()
    >>> m.update(b"Science is what we understand well enough to explain ")
    >>> m.update(b"to a computer; art is everything else. -D.E. Knuth")
    >>> m.digest().hex()
    'e246f77a8c37bd2f601a47273846f085ec3000e1c1a692b82e76921410386e56'

More condensed:

    >>> hashlib.sha224(b"Nobody inspects the spammish repetition").digest().hex()
    'a4337bc45a8fc544c03f52dc550cd6e1e87021bc896588bd79e901e2'

"""

import _hashlib as _prevent_recursive_loading
del _prevent_recursive_loading

# This tuple and __get_builtin_constructor() must be modified if a new
# always available algorithm is added.
__always_supported = (
    'md5',
    'sha1',
    'sha224', 'sha256', 'sha384', 'sha512',
    # 'sha3_224',  'sha3_256',  'sha3_384',
    # 'sha3_512', 'shake_128', 'shake_256',
    'blake2b256',
)

algorithms_guaranteed = set(__always_supported)
algorithms_available = set(__always_supported)

__all__ = __always_supported + ('new', 'algorithms_guaranteed',
                                'algorithms_available', 'pbkdf2_hmac')

__builtin_constructor_cache = {}

def __get_builtin_constructor(name):
    cache = __builtin_constructor_cache
    constructor = cache.get(name)
    if constructor is not None:
        return constructor
    try:
        if name in ('SHA1', 'sha1'):
            import _sha1
            cache['SHA1'] = cache['sha1'] = _sha1.sha1
        elif name in ('MD5', 'md5'):
            import _md5
            cache['MD5'] = cache['md5'] = _md5.md5
        elif name in ('SHA256', 'sha256', 'SHA224', 'sha224'):
            import _sha256
            cache['SHA224'] = cache['sha224'] = _sha256.sha224
            cache['SHA256'] = cache['sha256'] = _sha256.sha256
        elif name in ('SHA512', 'sha512', 'SHA384', 'sha384'):
            import _sha512
            cache['SHA384'] = cache['sha384'] = _sha512.sha384
            cache['SHA512'] = cache['sha512'] = _sha512.sha512
        elif name in {'sha3_224', 'sha3_256', 'sha3_384', 'sha3_512',
                      'shake_128', 'shake_256'}:
            import _sha3
            cache['sha3_224'] = _sha3.sha3_224
            cache['sha3_256'] = _sha3.sha3_256
            cache['sha3_384'] = _sha3.sha3_384
            cache['sha3_512'] = _sha3.sha3_512
            cache['shake_128'] = _sha3.shake_128
            cache['shake_256'] = _sha3.shake_256
    except ImportError:
        pass  # no extension module, this hash is unsupported.
    constructor = cache.get(name)
    if constructor is not None:
        return constructor
    raise ValueError('unsupported hash type ' + name)


def __get_mbedtls_constructor(name):
    try:
        f = getattr(_hashlib, 'mbedtls_' + name)
        # Allow the C module to raise ValueError.  The function will be
        # defined but the hash not actually available thanks to Mbedtls.
        f()
        # Use the C function directly (very fast)
        return f
    except (AttributeError, ValueError):
        return __get_builtin_constructor(name)


def __py_new(name, data=b'', **kwargs):
    """new(name, data=b'', **kwargs) - Return a new hashing object using the
    named algorithm; optionally initialized with data (which must be
    a bytes-like object).
    """
    return __get_builtin_constructor(name)(data, **kwargs)


def __hash_new(name, data=b'', **kwargs):
    """new(name, data=b'') - Return a new hashing object using the named algorithm;
    optionally initialized with data (which must be a bytes-like object).
    """
    try:
        return _hashlib.new(name, data)
    except ValueError:
        # If the _hashlib module (Mbedtls) doesn't support the named
        # hash, try using our builtin implementations.
        # This allows for SHA224/256 and SHA384/512 support even though
        # the Mbedtls library prior to 0.9.8 doesn't provide them.
        return __get_builtin_constructor(name)(data)


try:
    import _hashlib
    new = __hash_new
    __get_hash = __get_mbedtls_constructor
    algorithms_available = algorithms_available.union(
            _hashlib.mbedtls_md_meth_names)
except ImportError as e:
    new = __py_new
    __get_hash = __get_builtin_constructor

try:
    # Mbedtls's scrypt requires Mbedtls 1.1+
    from _hashlib import scrypt
except ImportError:
    pass

md5 = _hashlib.mbedtls_md5
sha1 = _hashlib.mbedtls_sha1
sha224 = _hashlib.mbedtls_sha224
sha256 = _hashlib.mbedtls_sha256
sha384 = _hashlib.mbedtls_sha384
sha512 = _hashlib.mbedtls_sha512
blake2b256 = _hashlib.mbedtls_blake2b256
pbkdf2_hmac = _hashlib.pbkdf2_hmac

try:
    sha3_224 = __get_builtin_constructor('sha3_224')
    sha3_256 = __get_builtin_constructor('sha3_256')
    sha3_384 = __get_builtin_constructor('sha3_384')
    sha3_512 = __get_builtin_constructor('sha3_512')
    shake_128 = __get_builtin_constructor('shake_128')
    shake_256 = __get_builtin_constructor('shake_256')
except (ImportError, ValueError):
    pass  # [jart] modified to not force using sha3

# Cleanup locals()
del __always_supported, __get_hash
del __py_new, __hash_new, __get_mbedtls_constructor
