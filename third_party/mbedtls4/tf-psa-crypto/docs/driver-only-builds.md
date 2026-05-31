This document explains how to create builds of Mbed TLS where some
cryptographic mechanisms are provided only by PSA drivers (that is, no
built-in implementation of those algorithms), from a user's perspective.

This is useful to save code size for people who are using either a hardware
accelerator, or an alternative software implementation that is more
aggressively optimized for code size than the default one in Mbed TLS.

General considerations
----------------------

This document assumes that you already have a working driver.
Otherwise, please see the [PSA driver example and
guide](psa-driver-example-and-guide.md) for information on writing a
driver.

For each mechanism you want provided only by your driver:

- Define the corresponding `PSA_WANT` macro in `psa/crypto_config.h` - this
  means the algorithm will be available in the PSA Crypto API.
- Define the corresponding `MBEDTLS_PSA_ACCEL` in your build. This could be
  defined in `psa/crypto_config.h` or your compiler's command line. This
informs the PSA code that an accelerator is available for this mechanism.
- Undefine / comment out the corresponding `MBEDTLS_xxx_C` macro in
  `mbedtls/mbedtls_config.h`. This ensures the built-in implementation is not
included in the build.

For example, if you want SHA-256 to be provided only by a driver, you'll want
`PSA_WANT_ALG_SHA_256` and `MBEDTLS_PSA_ACCEL_SHA_256` defined, and
`MBEDTLS_SHA256_C` undefined.

Recall that applications must call `psa_crypto_init()` prior to performing any
cryptographic or key management operation.

Mechanisms covered
------------------

For now, only the following (families of) mechanisms are supported:

- hashes: SHA-3, SHA-2, SHA-1, MD5, etc.
- elliptic-curve cryptography (ECC): ECDH, ECDSA, EC J-PAKE, ECC key types.
- finite-field Diffie-Hellman: FFDH algorithm, DH key types.
- RSA: PKCS#1 v1.5 and v2.1 signature and encryption algorithms, RSA key types
  (for now, only crypto, no X.509 or TLS support).
- AEADs:
  - GCM and CCM with AES, ARIA and Camellia key types
  - ChachaPoly with ChaCha20 Key type
- Unauthenticated ciphers:
  - key types: AES, ARIA, Camellia
  - modes: ECB, CBC, CTR, CFB, OFB, XTS

For each family listed above, all the mentioned alorithms/key types are also
all the mechanisms that exist in PSA API.

Supported means that when those are provided only by drivers, everything
(including PK, X.509 and TLS) should work in the same way as if the mechanisms
where built-in, except as documented in the "Limitations" sub-sections of the
sections dedicated to each family below.

Hashes
------

It is possible to have all hash operations provided only by a driver.

More precisely:

- you can enable `PSA_WANT_ALG_SHA_256` without `MBEDTLS_SHA256_C`, provided
  you have `MBEDTLS_PSA_ACCEL_ALG_SHA_256` enabled;
- and similarly for all supported hash algorithms: `MD5`, `RIPEMD160`,
  `SHA_1`, `SHA_224`, `SHA_256`, `SHA_384`, `SHA_512`, `SHA3_224`, `SHA3_256`,
`SHA3_384`, `SHA3_512`.

In such a build, all crypto operations (via the PSA Crypto API, or non-PSA
APIs), as well as X.509 and TLS, will work as usual, except that direct calls
to low-level hash APIs (`mbedtls_sha256()` etc.) are not possible for the
modules that are disabled.

You need to call `psa_crypto_init()` before any crypto operation that uses
a hash algorithm that is provided only by a driver, as mentioned in [General
considerations](#general-considerations) above.

If you want to check at compile-time whether a certain hash algorithm is
available in the present build of Mbed TLS, regardless of whether it's
provided by a driver or built-in, you should use `PSA_WANT_ALG_xxx` from
`psa/crypto.h`.

### HMAC

In addition to accelerated hash operations, it is also possible to accelerate
HMAC by enabling and accelerating:
- HMAC algorithm and key type, i.e. `[PSA_WANT|MBEDTLS_PSA_ACCEL]_ALG_HMAC` and
  `[PSA_WANT|MBEDTLS_PSA_ACCEL]KEY_TYPE_HMAC`.
- Required hash algorithm(s) as explained in [Hashes](#hashes) section.

In such a build it is possible to disable legacy HMAC support by disabling
`MBEDTLS_MD_C` and still getting crypto operations, X.509 and TLS to work as
usual. Exceptions are:
- As mentioned in [Hashes](#hashes) direct calls to legacy lo-level hash APIs
  (`mbedtls_sha256()` etc.) will not be possible for the legacy modules that
  are disabled.
- Legacy HMAC support (`mbedtls_md_hmac_xxx()`) won't be possible.
- `MBEDTLS_PKCS[5|7]_C` and  `MBEDTLS_HMAC_DRBG_C` since they
  depend on the legacy implementation of HMAC.
  - disabling HMAC_DRBG_C cause deterministic ECDSA (i.e.
  `MBEDTLS_DETERMINISTIC_ECDSA` on the legacy side and 
  `PSA_WANT_ALG_DETERMINISTIC_ECDSA` on the PSA one) to be not available.

Elliptic-curve cryptography (ECC)
---------------------------------

It is possible to have most ECC operations provided only by a driver:

- the ECDH, ECDSA and EC J-PAKE algorithms;
- key import, export, and random generation.

More precisely, if:

- you have driver support for ECC public and using private keys (that is,
`MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY` and
`MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_BASIC` are enabled), and
- you have driver support for all ECC curves that are enabled (that is, for
  each `PSA_WANT_ECC_xxx` macro enabled, the corresponding
`MBEDTLS_PSA_ACCEL_ECC_xxx` macros is enabled as well);

then you can:

- enable `PSA_WANT_ALG_ECDH` without `MBEDTLS_ECDH_C`, provided
  `MBEDTLS_PSA_ACCEL_ALG_ECDH` is enabled
- enable `PSA_WANT_ALG_ECDSA` without `MBEDTLS_ECDSA_C`, provided
  `MBEDTLS_PSA_ACCEL_ALG_ECDSA` is enabled;
- enable `PSA_WANT_ALG_JPAKE` without `MBEDTLS_ECJPAKE_C`, provided
  `MBEDTLS_PSA_ACCEL_ALG_JPAKE` is enabled.

In addition, if:

- none of `MBEDTLS_ECDH_C`, `MBEDTLS_ECDSA_C`, `MBEDTLS_ECJPAKE_C` are enabled
  (see conditions above), and
- you have driver support for all enabled ECC key pair operations - that is,
  for each `PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_xxx` macro enabled, the
corresponding `MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_xxx` macros is also
enabled,

then you can also disable `MBEDTLS_ECP_C`. However, a small subset of it might
still be included in the build, see limitations sub-section below.

In addition, if:

- `MBEDTLS_ECP_C` is fully removed (see limitation sub-section below),
- and support for RSA key types and algorithms is either fully disabled or
  fully provided by a driver,
- and support for DH key types and the FFDH algorithm is either disabled or
  fully provided by a driver,

then you can also disable `MBEDTLS_BIGNUM_C`.

In such builds, all crypto operations via the PSA Crypto API will work as
usual, as well as the PK, X.509 and TLS modules, with the following exceptions:

- direct calls to APIs from the disabled modules are not possible;
- PK, X.509 and TLS will not support restartable ECC operations (see
  limitation sub-section below).

If you want to check at compile-time whether a certain curve is available in
the present build of Mbed TLS, regardless of whether ECC is provided by a
driver or built-in, you should use `PSA_WANT_ECC_xxx` from
  `psa/crypto.h`.

Note that for externally-provided drivers, the integrator is responsible for
ensuring the appropriate `MBEDTLS_PSA_ACCEL_xxx` macros are defined. However,
for the p256-m driver that's provided with the library, those macros are
automatically defined when enabling `MBEDTLS_PSA_P256M_DRIVER_ENABLED`.

### Limitations regarding fully removing `ecp.c`

A limited subset of `ecp.c` will still be automatically re-enabled if any of
the following is enabled:

- `MBEDTLS_PK_PARSE_EC_COMPRESSED` - support for parsing ECC keys where the
  public part is in compressed format;
- `MBEDTLS_PK_PARSE_EC_EXTENDED` - support for parsing ECC keys where the
  curve is identified not by name, but by explicit parameters;
- `PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE` - support for deterministic
  derivation of an ECC keypair with `psa_key_derivation_output_key()`.

Note: when any of the above options is enabled, a subset of `ecp.c` will
automatically be included in the build in order to support it. Therefore
you can still disable `MBEDTLS_ECP_C` in `mbedtls_config.h` and this will
result in some code size savings, but not as much as when none of the
above features are enabled.

We do have plans to support each of these with `ecp.c` fully removed in the
future, however there is no established timeline. If you're interested, please
let us know, so we can take it into consideration in our planning.

### Limitations regarding restartable / interruptible ECC operations

At the moment, there is no driver support for interruptible operations
(see `psa_sign_hash_start()` + `psa_sign_hash_complete()` etc.) so as a
consequence these are not supported in builds without `MBEDTLS_ECDSA_C`.

Similarly, there is no PSA support for interruptible ECDH operations so these
are not supported without `ECDH_C`. See also limitations regarding restartable
operations in [the documentation of
MBEDTLS_ECP_RESTARTABLE](include/psa/crypto_config.h).

Again, we have plans to support this in the future but not with an established
timeline, please let us know if you're interested.

### Limitations regarding "mixed" builds (driver and built-in)

In order for a build to be driver-only (no built-in implementation), all the
requested algorithms, key types (key operations) and curves must be
accelerated (plus a few other restrictions, see "Limitations regarding fully
removing `ecp.c`" above). However, what if you have an accelerator that only
supports some algorithms, some key types (key operations), or some curves, but
want to have more enabled in you build?

It is possible to have acceleration for only a subset of the requested
algorithms. In this case, the built-in implementation of the accelerated
algorithms will be disabled, provided all the requested curves and key types
that can be used with this algorithm are also declared as accelerated.

There is very limited support for having acceleration for only a subset of the
requested key type operations. The only configuration that's tested is that of
a driver accelerating `PUBLIC_KEY`, `KEY_PAIR_BASIC`, `KEY_PAIR_IMPORT`,
`KEY_PAIR_EXPORT` but not `KEY_PAIR_GENERATE`. (Note: currently the driver
interface does not support `KEY_PAIR_DERIVE`.)

There is limited support for having acceleration for only a subset of the
requested curves. In such builds, only the PSA API is currently tested and
working; there are known issues in PK, and X.509 and TLS are untested.

Finite-field Diffie-Hellman
---------------------------

Support is pretty similar to the "Elliptic-curve cryptography (ECC)" section
above.
Key management and usage can be enabled by means of the usual `PSA_WANT` +
`MBEDTLS_PSA_ACCEL` pairs:

- `[PSA_WANT|MBEDTLS_PSA_ACCEL]_KEY_TYPE_DH_PUBLIC_KEY`;
- `[PSA_WANT|MBEDTLS_PSA_ACCEL]_KEY_TYPE_DH_KEY_PAIR_BASIC`;
- `[PSA_WANT|MBEDTLS_PSA_ACCEL]_KEY_TYPE_DH_KEY_PAIR_IMPORT`;
- `[PSA_WANT|MBEDTLS_PSA_ACCEL]_KEY_TYPE_DH_KEY_PAIR_EXPORT`;
- `[PSA_WANT|MBEDTLS_PSA_ACCEL]_KEY_TYPE_DH_KEY_PAIR_GENERATE`;

The same holds for the associated algorithm:
`[PSA_WANT|MBEDTLS_PSA_ACCEL]_ALG_FFDH` allow builds accelerating FFDH and
removing builtin support.

Note that the PSA API only supports FFDH with RFC 7919 groups.
In theory TLS 1.2 allows custom FFDH groups to be used in DHE key exchanges, but
since DHE-RSA and DHE-PSK key exchanges support has been removed from Mbed TLS,
PSA API limitation to RFC 7919 groups is not a problem.
TLS 1.3 is also fine because the protocol itself does not allows custom FFDH
groups.

RSA
---

It is possible for all RSA operations to be provided only by a driver.

More precisely, if:

- all the RSA algorithms that are enabled (`PSA_WANT_ALG_RSA_*`) are also
  accelerated (`MBEDTLS_PSA_ACCEL_ALG_RSA_*`),
- and all the RSA key types that are enabled (`PSA_WANT_KEY_TYPE_RSA_*`) are
  also accelerated (`MBEDTLS_PSA_ACCEL_KEY_TYPE_RSA_*`),

then you can disable `MBEDTLS_RSA_C`, `MBEDTLS_PKCS1_V15` and
`MBEDTLS_PKCS1_V21`, and RSA will still work in PSA Crypto.

### Limitations on RSA acceleration

Unlike other mechanisms, for now in configurations with driver-only RSA, only
PSA Crypto works. In particular, PK, X.509 and TLS will _not_ work with
driver-only RSA.

Currently (early 2024) we don't have plans to extend this support. If you're
interested in wider driver-only support for RSA, please let us know.

Ciphers (unauthenticated and AEAD)
----------------------------------

It is possible to have all ciphers and AEAD operations provided only by a
driver. More precisely, for each desired combination of key type and
algorithm/mode you can:

- Enable desired PSA key type(s):
  - `PSA_WANT_KEY_TYPE_AES`,
  - `PSA_WANT_KEY_TYPE_ARIA`,
  - `PSA_WANT_KEY_TYPE_CAMELLIA`,
  - `PSA_WANT_KEY_TYPE_CHACHA20`,
- Enable desired PSA algorithm(s):
  - Unauthenticated ciphers modes:
    - `PSA_WANT_ALG_CBC_NO_PADDING`,
    - `PSA_WANT_ALG_CBC_PKCS7`,
    - `PSA_WANT_ALG_CCM_STAR_NO_TAG`,
    - `PSA_WANT_ALG_CFB`,
    - `PSA_WANT_ALG_CTR`,
    - `PSA_WANT_ALG_ECB_NO_PADDING`,
    - `PSA_WANT_ALG_OFB`,
    - `PSA_WANT_ALG_STREAM_CIPHER`.
  - AEADs:
    - `PSA_WANT_ALG_CCM`,
    - `PSA_WANT_ALG_GCM`,
    - `PSA_WANT_ALG_CHACHA20_POLY1305`.
- Enable `MBEDTLS_PSA_ACCEL_[KEY_TYPE_xxx|ALG_yyy]` symbol(s) which correspond
   to the `PSA_WANT_KEY_TYPE_xxx` and `PSA_WANT_ALG_yyy` of the previous steps.
- Disable builtin support of key types:
  - `MBEDTLS_AES_C`,
  - `MBEDTLS_ARIA_C`,
  - `MBEDTLS_CAMELLIA_C`,
  - `MBEDTLS_CHACHA20_C`.
  and algorithms/modes:
  - `MBEDTLS_CBC_C`,
  - `MBEDTLS_CFB_C`,
  - `MBEDTLS_CTR_C`,
  - `MBEDTLS_OFB_C`,
  - `MBEDTLS_XTS_C`,
  - `MBEDTLS_CCM_C`,
  - `MBEDTLS_GCM_C`,
  - `MBEDTLS_CHACHAPOLY_C`,
  - `MBEDTLS_NULL_CIPHER`.

Once a key type and related algorithm are accelerated, all the PSA Crypto APIs
will work, as well as X.509 and TLS but some non-PSA APIs will be absent or
have reduced functionality, see [Restrictions](#restrictions) for details.

### Restrictions

- If an algorithm other than CCM and GCM (see
  ["Partial acceleration for CCM/GCM"](#partial-acceleration-for-ccmgcm) below)
  is enabled but not accelerated, then all key types that can be used with it
  will need to be built-in.
- If a key type is enabled but not accelerated, then all algorithms that can be
  used with it will need to be built-in.

Some legacy modules can't take advantage of PSA drivers yet, and will either
need to be disabled, or have reduced features when the built-in implementations
of some ciphers are removed:

- `MBEDTLS_NIST_KW_C` needs built-in AES: it must be disabled when
  `MBEDTLS_AES_C` is disabled.
- `MBEDTLS_CMAC_C` needs built-in AES: it must be disabled when
  `MBEDTLS_AES_C` is disabled. (Note: if there is driver support for
  CMAC and all compatible key types, then `PSA_WANT_ALG_CMAC` can be enabled
  without `MBEDTLS_CMAC_C` and CMAC will be usable with `psa_max_xxx` APIs.)
- `MBEDTLS_CIPHER_C`: the `mbedtls_cipher_xxx()` APIs will only work with
  ciphers that are built-in - that is, both the underlying cipher
  (eg `MBEDTLS_AES_C`) and the mode (eg `MBEDTLS_CIPHER_MODE_CBC` or
  `MBEDTLS_GCM_C`).
- `MBEDTLS_PKCS5_C`: encryption/decryption (PBES2, PBE) will only work with
  ciphers that are built-in.
- PEM decryption will only work with ciphers that are built-in.
- PK parse will only be able to parse encrypted keys using built-in ciphers.

Note that if you also disable `MBEDTLS_CIPHER_C`, there will be additional
restrictions, see [Disabling `MBEDTLS_CIPHER_C`](#disabling-mbedtls_cipher_c).

### Legacy <-> PSA matching

Note that the relationship between legacy (i.e. `MBEDTLS_xxx_C`) and PSA
(i.e. `PSA_WANT_xxx`) symbols is not always 1:1. For example:

- ECB mode is always enabled in the legacy configuration for each key type that
  allows it (AES, ARIA, Camellia), whereas it must be explicitly enabled
  in PSA with `PSA_WANT_ALG_ECB_NO_PADDING`.
- In the legacy API, `MBEDTLS_CHACHA20_C` enables the ChaCha20 stream cipher, and
  enabling `MBEDTLS_CHACHAPOLY_C` also enables the ChaCha20-Poly1305 AEAD. In the
  PSA API, you need to enable `PSA_KEY_TYPE_CHACHA20` for both, plus
  `PSA_ALG_STREAM_CIPHER` or `PSA_ALG_CHACHA20_POLY1305` as desired.
- The legacy symbol `MBEDTLS_CCM_C` adds support for both cipher and AEAD,
  whereas in PSA there are 2 different symbols: `PSA_WANT_ALG_CCM_STAR_NO_TAG`
  and `PSA_WANT_ALG_CCM`, respectively.

### Partial acceleration for CCM/GCM

[This section depends on #8598 so it might be updated while that PR progresses.]

In case legacy CCM/GCM algorithms are enabled, it is still possible to benefit
from PSA acceleration of the underlying block cipher by enabling support for
ECB mode (`PSA_WANT_ALG_ECB_NO_PADDING` + `MBEDTLS_PSA_ACCEL_ALG_ECB_NO_PADDING`)
together with desired key type(s) (`PSA_WANT_KEY_TYPE_[AES|ARIA|CAMELLIA]` +
`MBEDTLS_PSA_ACCEL_KEY_TYPE_[AES|ARIA|CAMELLIA]`).

In such configurations it is possible to:

- Use CCM and GCM via the PSA Crypto APIs.
- Use CCM and GCM via legacy functions `mbedtls_[ccm|gcm]_xxx()` (but not the
  legacy functions `mbedtls_cipher_xxx()`).
- Disable legacy key types (`MBEDTLS_[AES|ARIA|CAMELLIA]_C`) if there is no
  other dependency requiring them.

ChaChaPoly has no such feature, so it requires full acceleration (key type +
algorithm) in order to work with a driver.

### CTR-DRBG

The legacy CTR-DRBG module (enabled by `MBEDTLS_CTR_DRBG_C`) can also benefit
from PSA acceleration if both of the following conditions are met:

- The legacy AES module (`MBEDTLS_AES_C`) is not enabled and
- AES is supported on the PSA side together with ECB mode, i.e.
  `PSA_WANT_KEY_TYPE_AES` + `PSA_WANT_ALG_ECB_NO_PADDING`.

### Disabling `MBEDTLS_CIPHER_C`

It is possible to save code size by disabling MBEDTLS_CIPHER_C when all of the
following conditions are met:

- The application is not using the `mbedtls_cipher_` API.
- In PSA, all unauthenticated (that is, non-AEAD) ciphers are either disabled or
  fully accelerated (that is, all compatible key types are accelerated too).
- `MBEDTLS_NIST_KW` is disabled.
- `MBEDTLS_CMAC_C` is disabled. (Note: support for CMAC in PSA can be provided by
  a driver.)

In such a build, everything will work as usual except for the following:

- Encryption/decryption functions from the PKCS5 and PKCS12 module will not be
  available (only key derivation functions).
- Parsing of PKCS5- or PKCS12-encrypted keys in PK parse will fail.

Note: AEAD ciphers (CCM, GCM, ChachaPoly) do not have a dependency on
MBEDTLS_CIPHER_C even when using the built-in implementations.

If you also have some ciphers fully accelerated and the built-ins removed, see
[Restrictions](#restrictions) for restrictions related to removing the built-ins.



