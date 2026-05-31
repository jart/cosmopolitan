This document lists current limitations of the PSA Crypto API (as of version
1.1) that may impact our ability to (1) use it for all crypto operations in
TLS and X.509 and (2) support isolation of all long-term secrets in TLS (that
is, goals G1 and G2 in
[strategy.md](https://github.com/Mbed-TLS/mbedtls/blob/mbedtls-3.6/docs/architecture/psa-migration/strategy.md)).

This is supposed to be a complete list, based on a exhaustive review of crypto
operations done in TLS and X.509 code, but of course it's still possible that
subtle-but-important issues have been missed. The only way to be really sure
is, of course, to actually do the migration work.

Limitations relevant for G1 (performing crypto operations)
==========================================================

Restartable (aka interruptible) ECC operations
----------------------------------------------

Support for interruptible ECDSA sign/verify was added to PSA in Mbed TLS 3.4.
However, support for interruptible ECDH is not present yet. Also, PK, X.509 and
TLS have not yet been adapted to take advantage of the new PSA APIs. See:
- <https://github.com/Mbed-TLS/mbedtls/issues/7292>;
- <https://github.com/Mbed-TLS/mbedtls/issues/7293>;
- <https://github.com/Mbed-TLS/mbedtls/issues/7294>.

Currently, when `MBEDTLS_ECP_RESTARTABLE` is enabled, some operations that
should be restartable are not (ECDH in TLS 1.2 clients using ECDHE-ECDSA), as
they are using PSA instead, and some operations that should use PSA do not
(signature generation & verification) as they use the legacy API instead, in
order to get restartable behaviour.

Things that are in the API but not implemented yet
--------------------------------------------------

PSA Crypto has an API for FFDH, but it's not implemented in Mbed TLS yet.
(Regarding FFDH, see the next section as well.) See issue [3261][ffdh] on
github.

[ffdh]: https://github.com/Mbed-TLS/mbedtls/issues/3261

Arbitrary parameters for FFDH
-----------------------------

(See also the first paragraph in the previous section.)

Currently, the PSA Crypto API can only perform FFDH with a limited set of
well-known parameters (some of them defined in the spec, but implementations
are free to extend that set).

TLS 1.2 (and earlier) on the other hand have the server send explicit
parameters (P and G) in its ServerKeyExchange message. This has been found to
be suboptimal for security, as it is prohibitively hard for the client to
verify the strength of these parameters. This led to the development of RFC
7919 which allows use of named groups in TLS 1.2 - however as this is only an
extension, servers can still send custom parameters if they don't support the
extension.

In TLS 1.3 the situation will be simpler: named groups are the only
option, so the current PSA Crypto API is a good match for that. (Not
coincidentally, all the groups used by RFC 7919 and TLS 1.3 are included
in the PSA specification.)

There are several options here:

1. Implement support for custom FFDH parameters in PSA Crypto: this would pose
   non-trivial API design problem, but most importantly seems backwards, as
the crypto community is moving away from custom FFDH parameters. (Could be
done any time.)
2. Drop the DHE-RSA and DHE-PSK key exchanges in TLS 1.2 when moving to PSA.
   (For people who want some algorithmic variety in case ECC collapses, FFDH
would still be available in TLS 1.3, just not in 1.2.) (Can only be done in
4.0 or another major version.)
3. Variant of the precedent: only drop client-side support. Server-side is
   easy to support in terms of API/protocol, as the server picks the
parameters: we just need remove the existing `mbedtls_ssl_conf_dh_param_xxx()`
APIs and tell people to use `mbedtls_ssl_conf_groups()` instead. (Can only be
done in 4.0 or another major version.)
4. Implement RFC 7919, support DHE-RSA and DHE-PSK only in conjunction with it
   when moving to PSA. Server-side would work as above; unfortunately
client-side the only option is to offer named groups and break the handshake
if the server didn't take on our offer. This is not fully satisfying, but is
perhaps the least unsatisfying option in terms of result; it's also probably
the one that requires the most work, but it would deliver value beyond PSA
migration by implementing RFC 7919. (Implementing RFC 7919 could be done any
time; making it mandatory can only be done in 4.0 or another major version.)

As of early 2023, the plan is to go with option 2 in Mbed TLS 4.0, which has
been announced on the mailing-list and got no push-back, see
<https://github.com/Mbed-TLS/mbedtls/issues/5278>.

RSA-PSS parameters
------------------

RSA-PSS signatures are defined by PKCS#1 v2, re-published as RFC 8017
(previously RFC 3447).

As standardized, the signature scheme takes several parameters, in addition to
the hash algorithm potentially used to hash the message being signed:
- a hash algorithm used for the encoding function
- a mask generation function
  - most commonly MGF1, which in turn is parametrized by a hash algorithm
- a salt length
- a trailer field - the value is fixed to 0xBC by PKCS#1 v2.1, but was left
  configurable in the original scheme; 0xBC is used everywhere in practice.

Both the existing `mbedtls_` API and the PSA API support only MGF1 as the
generation function (and only 0xBC as the trailer field), but there are
discrepancies in handling the salt length and which of the various hash
algorithms can differ from each other.

### API comparison

- RSA:
  - signature: `mbedtls_rsa_rsassa_pss_sign()`
    - message hashed externally
    - encoding hash = MGF1 hash (from context, or argument = message hash)
    - salt length: always using the maximum legal value
  - signature: `mbedtls_rsa_rsassa_pss_sign_ext()`
    - message hashed externally
    - encoding hash = MGF1 hash (from context, or argument = message hash)
    - salt length: specified explicitly
  - verification: `mbedtls_rsassa_pss_verify()`
    - message hashed externally
    - encoding hash = MGF1 hash (from context, or argument = message hash)
    - salt length: any valid length accepted
  - verification: `mbedtls_rsassa_pss_verify_ext()`
    - message hashed externally
    - encoding hash = MGF1 hash from dedicated argument
    - expected salt length: specified explicitly, can specify "ANY"
- PK:
  - signature: not supported
  - verification: `mbedtls_pk_verify_ext()`
    - message hashed externally
    - encoding hash = MGF1 hash, specified explicitly
    - expected salt length: specified explicitly, can specify "ANY"
- PSA:
  - algorithm specification:
    - hash alg used for message hashing, encoding and MGF1
    - salt length can be either "standard" (<= hashlen, see note) or "any"
  - signature generation:
    - salt length: always <= hashlen (see note) and random salt
  - verification:
    - salt length: either <= hashlen (see note), or any depending on algorithm

Note: above, "<= hashlen" means that hashlen is used if possible, but if it
doesn't fit because the key is too short, then the maximum length that fits is
used.

The RSA/PK API is in principle more flexible than the PSA Crypto API. The
following sub-sections study whether and how this matters in practice.

### Use in X.509

RFC 4055 Section 3.1 defines the encoding of RSA-PSS that's used in X.509.
It allows independently specifying the message hash (also used for encoding
hash), the MGF (and its hash if MGF1 is used), and the salt length (plus an
extra parameter "trailer field" that doesn't vary in practice"). These can be
encoded as part of the key, and of the signature. If both encoding are
presents, all values must match except possibly for the salt length, where the
value from the signature parameters is used.

In Mbed TLS, RSA-PSS parameters can be parsed and displayed for various
objects (certificates, CRLs, CSRs). During parsing, the following properties
are enforced:
- the extra "trailer field" parameter must have its default value
- the mask generation function is MGF1
- encoding hash = message hashing algorithm (may differ from MGF1 hash)

When it comes to cryptographic operations, only two things are supported:
- verifying the signature on a certificate from its parent;
- verifying the signature on a CRL from the issuing CA.

The verification is done using `mbedtls_pk_verify_ext()`.

Note: since X.509 parsing ensures that message hash = encoding hash, and
`mbedtls_pk_verify_ext()` uses encoding hash = mgf1 hash, it looks like all
three hash algorithms must be equal, which would be good news as it would
match a limitation of the PSA API.

It is unclear what parameters people use in practice. It looks like by default
OpenSSL picks saltlen = keylen - hashlen - 2 (tested with openssl 1.1.1f).
The `certtool` command provided by GnuTLS seems to be picking saltlen = hashlen
by default (tested with GnuTLS 3.6.13). FIPS 186-4 requires 0 <= saltlen <=
hashlen.

### Use in TLS

In TLS 1.2 (or lower), RSA-PSS signatures are never used, except via X.509.

In TLS 1.3, RSA-PSS signatures can be used directly in the protocol (in
addition to indirect use via X.509). It has two sets of three signature
algorithm identifiers (for SHA-256, SHA-384 and SHA-512), depending of what
the OID of the public key is (rsaEncryption or RSASSA-PSS).

In both cases, it specifies that:
- the mask generation function is MGF1
- all three hashes are equal
- the length of the salt MUST be equal to the length of the digest algorithm

When signing, the salt length picked by PSA is the one required by TLS 1.3
(unless the key is unreasonably small).

When verifying signatures, PSA will by default enforce the salt len is the one
required by TLS 1.3.

### Current testing - X509

All test files use the default trailer field of 0xBC, as enforced by our
parser. (There's a negative test for that using the
`x509_parse_rsassa_pss_params` test function and hex data.)

Files with "bad" in the name are expected to be invalid and rejected in tests.

**Test certificates:**

server9-bad-mgfhash.crt (announcing mgf1(sha224), signed with another mgf)
         Hash Algorithm: sha256
         Mask Algorithm: mgf1 with sha224
          Salt Length: 0xDE
server9-bad-saltlen.crt (announcing saltlen = 0xDE, signed with another len)
         Hash Algorithm: sha256
         Mask Algorithm: mgf1 with sha256
          Salt Length: 0xDE
server9-badsign.crt (one bit flipped in the signature)
         Hash Algorithm: sha1 (default)
         Mask Algorithm: mgf1 with sha1 (default)
          Salt Length: 0xEA
server9-defaults.crt
         Hash Algorithm: sha1 (default)
         Mask Algorithm: mgf1 with sha1 (default)
          Salt Length: 0x14 (default)
server9-sha224.crt
         Hash Algorithm: sha224
         Mask Algorithm: mgf1 with sha224
          Salt Length: 0xE2
server9-sha256.crt
         Hash Algorithm: sha256
         Mask Algorithm: mgf1 with sha256
          Salt Length: 0xDE
server9-sha384.crt
         Hash Algorithm: sha384
         Mask Algorithm: mgf1 with sha384
          Salt Length: 0xCE
server9-sha512.crt
         Hash Algorithm: sha512
         Mask Algorithm: mgf1 with sha512
          Salt Length: 0xBE
server9-with-ca.crt
         Hash Algorithm: sha1 (default)
         Mask Algorithm: mgf1 with sha1 (default)
          Salt Length: 0xEA
server9.crt
         Hash Algorithm: sha1 (default)
         Mask Algorithm: mgf1 with sha1 (default)
          Salt Length: 0xEA

These certificates are signed with a 2048-bit key. It appears that they are
all using saltlen = keylen - hashlen - 2, except for server9-defaults which is
using saltlen = hashlen.

**Test CRLs:**

crl-rsa-pss-sha1-badsign.pem
         Hash Algorithm: sha1 (default)
         Mask Algorithm: mgf1 with sha1 (default)
          Salt Length: 0xEA
crl-rsa-pss-sha1.pem
         Hash Algorithm: sha1 (default)
         Mask Algorithm: mgf1 with sha1 (default)
          Salt Length: 0xEA
crl-rsa-pss-sha224.pem
         Hash Algorithm: sha224
         Mask Algorithm: mgf1 with sha224
          Salt Length: 0xE2
crl-rsa-pss-sha256.pem
         Hash Algorithm: sha256
         Mask Algorithm: mgf1 with sha256
          Salt Length: 0xDE
crl-rsa-pss-sha384.pem
         Hash Algorithm: sha384
         Mask Algorithm: mgf1 with sha384
          Salt Length: 0xCE
crl-rsa-pss-sha512.pem
         Hash Algorithm: sha512
         Mask Algorithm: mgf1 with sha512
          Salt Length: 0xBE

These CRLs are signed with a 2048-bit key. It appears that they are
all using saltlen = keylen - hashlen - 2.

**Test CSRs:**

server9.req.sha1
         Hash Algorithm: sha1 (default)
         Mask Algorithm: mgf1 with sha1 (default)
          Salt Length: 0x6A
server9.req.sha224
         Hash Algorithm: sha224
         Mask Algorithm: mgf1 with sha224
          Salt Length: 0x62
server9.req.sha256
         Hash Algorithm: sha256
         Mask Algorithm: mgf1 with sha256
          Salt Length: 0x5E
server9.req.sha384
         Hash Algorithm: sha384
         Mask Algorithm: mgf1 with sha384
          Salt Length: 0x4E
server9.req.sha512
         Hash Algorithm: sha512
         Mask Algorithm: mgf1 with sha512
          Salt Length: 0x3E

These CSRs are signed with a 2048-bit key. It appears that they are
all using saltlen = keylen - hashlen - 2.

### Possible courses of action

There's no question about what to do with TLS (any version); the only question
is about X.509 signature verification. Options include:

1. Doing all verifications with `PSA_ALG_RSA_PSS_ANY_SALT` - while this
   wouldn't cause a concrete security issue, this would be non-compliant.
2. Doing verifications with `PSA_ALG_RSA_PSS` when we're lucky and the encoded
   saltlen happens to match hashlen, and falling back to `ANY_SALT` otherwise.
Same issue as with the previous point, except more contained.
3. Reject all certificates with saltlen != hashlen. This includes all
   certificates generated with OpenSSL using the default parameters, so it's
probably not acceptable.
4. Request an extension to the PSA Crypto API and use one of the above options
   in the meantime. Such an extension seems inconvenient and not motivated by
strong security arguments, so it's unclear whether it would be accepted.

Since Mbed TLS 3.4, option 1 is implemented.

Limitations relevant for G2 (isolation of long-term secrets)
============================================================

Currently none.
