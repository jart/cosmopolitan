# Transitioning to the PSA API

> I have code written for `mbedtls_` cryptography APIs. How do I migrate to `psa_` APIs?

## Introduction

TF-PSA-Crypto, the successor of Mbed TLS 3.x for cryptography, replaces most `mbedtls_xxx` cryptography APIs with newer `psa_xxx` APIs. This guide is intended to help migrate existing applications that used Mbed TLS for cryptography. It aims to cover common use cases, but cannot cover all possible scenarios.

### Suggested reading

This document is long, but you probably don't need to read all of it. You should start with the following sections:

1. [Where can I find documentation?](#where-can-i-find-documentation)
2. [General considerations](#general-considerations)

Then use the [summary of API modules](#summary-of-api-modules), the table of contents or a text search to locate the sections that interest you, based on what legacy interfaces your code is currently using.

### Where can I find documentation?

**Tutorial**: See the [getting started guide](https://mbed-tls.readthedocs.io/en/latest/getting_started/psa/).

**Reference**: The [PSA Crypto API specification](https://arm-software.github.io/psa-api/crypto/) is available online. TF-PSA-Crypto implements a large subset of the specification which is documented in the [`psa/crypto*.h` headers](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto_8h/).

### Additional resources

* [TF-PSA-Crypto open issues](https://github.com/Mbed-TLS/TF-PSA-Crypto/issues) (some [Mbed TLS issues](https://github.com/Mbed-TLS/mbedtls/issues) may also still be relevant)
* [PSA API open issues](https://github.com/ARM-software/psa-api/issues) (not just cryptography APIs)
* [Mbed TLS mailing list](https://lists.trustedfirmware.org/mailman3/lists/mbed-tls.lists.trustedfirmware.org/) (also covers TF-PSA-Crypto)

### Why change the API?

* Mbed TLS APIs are traditionally very transparent: the caller can access internal fields of operations. This is less true in the 3.x major version than before, but still the case to some extent. This offers applications some flexibility, but it removes flexibility from the implementation. For example, it is hard to support hardware acceleration, because the API constrains how the data must be represented. PSA APIs were designed to be more opaque, giving more freedom to the implementation.
* Mbed TLS legacy APIs require key material to be present in the application memory. The PSA Crypto API natively supports operations on keys stored in an external [location](https://arm-software.github.io/psa-api/crypto/1.1/api/keys/lifetimes.html#c.psa_key_location_t) (secure enclave, secure element, HSM, etc.).
* PSA APIs have [consistent conventions](https://arm-software.github.io/psa-api/crypto/1.1/overview/conventions.html#parameter-conventions) which many legacy APIs in Mbed TLS do not follow. For example, many legacy cryptography functions require the caller to know how large an output buffer needs to be based on the selected algorithm, whereas in the PSA API, all buffer arguments have a well-defined size and those sizes are checked.
* Mbed TLS legacy APIs require passing around a random generator argument where needed. This has historically been problematic with functions that were created without an RNG argument but later needed one as part of a security countermeasure. The PSA crypto subsystem maintains a global random generator, resolving this problem.

### Migration timeline

* Mbed TLS 2.15.0 (Nov 2018): first release with a draft implementation of the PSA API.
* Mbed TLS 2.18.0 (Jun 2019): The PSA API is available in the default build.
* Mbed TLS 3.1.0 (Dec 2021): TLS 1.3 support is the first major feature that requires the PSA API.
* TF-PSA-Crypto 1.0.0 (Oct 2025): Removal of most legacy crypto APIs. Drop support for builds without the PSA subsystem and for legacy configuration of cryptographic mechanisms.
  The corresponding release Mbed TLS 4.0.0 drops support for cryptography calls that bypass PSA.
* TF-PSA-Crypto 2.0.0 (??): Removal of the remaining non-PSA crypto APIs.

### Compatibility with Mbed TLS 3.6

Most of the new APIs suggested in this document are already present in Mbed TLS in the [3.6 long-time support branch](https://github.com/Mbed-TLS/mbedtls/tree/mbedtls-3.6). This document should indicate which features were added in TF-PSA-Crypto 1.x. However, if your application needs to be compatible with both Mbed TLS 3.6 and TF-PSA-Crypto 1.x, we invite you to consult the [PSA transition guide for Mbed TLS 3.6](https://github.com/Mbed-TLS/mbedtls/blob/mbedtls-3.6/docs/psa-transition.md).

## General considerations

### Configuration of the PSA subsystem

To make the PSA cryptography API available, make sure that the configuration option [`MBEDTLS_PSA_CRYPTO_C`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/mbedtls__config_8h/#c.MBEDTLS_PSA_CRYPTO_C) is enabled. (It is enabled in the default configuration.)

PSA uses a different set of configuration symbols to express which cryptographic mechanisms are enabled in the build. See the section “[Cryptographic mechanism availability](#cryptographic-mechanism-availability)”. If you are migrating from a configuration where `MBEDTLS_PSA_CRYPTO_CONFIG` is enabled, or if you are mostly relying on the default configuration for cryptography, there will be few or no changes between Mbed TLS 3.6 and TF-PSA-Crypto 1.0. But if you were explicitly specifying a set of cryptographic mechanisms using `MBEDTLS_xxx` options, you will need to migrate your configuration to the new mechanism.

Note that in Mbed TLS 3.x, the whole library configuration could be in `mbedtls/mbedtls_config.h` (or `MBEDTLS_CONFIG_FILE`) plus optionally `MBEDTLS_USER_CONFIG_FILE`. In TF-PSA-Crypto, the configuration must be in `psa/crypto_config.h` (or `TF_PSA_CRYPTO_CONFIG_FILE`) plus optionally `TF_PSA_CRYPTO_USER_CONFIG_FILE`.

The Mbed TLS 4.x configuration file should not try to modify any option consumed by TF-PSA-Crypto: this would make Mbed TLS's use of cryptography inconsistent with what the crypto library provides. However, it is ok to redundantly set a TF-PSA-Crypto option in the Mbed TLS configuration if it was already set to the same value in the TF-PSA-Crypto configuration; this allows using the same file as `TF_PSA_CRYPTO_CONFIG_FILE` and `MBEDTLS_CONFIG_FILE`.

### Header files

Applications only need to include a single header file for all cryptographic mechanisms:
```
#include <psa/crypto.h>
```

Note that you may want to keep other crypto header files for functionality that is not provided by the PSA API, such as `<mbedtls/nist_kw.h>` for NIST KW/KWP (not yet exposed through PSA), `<mbedtls/pk.h>` for key pair and public key parsing and formatting, `<mbedtls/asn1.h>` for ASN.1 parsing, etc.

### General application layout

Before any cryptographic operation, call [`psa_crypto_init`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__initialization/#group__initialization_1ga2de150803fc2f7dc6101d5af7e921dd9) and check that it succeeds. (A failure indicates an abnormal system state from which most applications cannot recover.)

If you wish to free all resources associated with PSA cryptography, call [`mbedtls_psa_crypto_free`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__extra_8h/#_CPPv423mbedtls_psa_crypto_freev).

The PSA subsystem has an internal random generator. As a consequence, you do not need to instantiate one manually (no need to create an `mbedtls_entropy_context` and an `mbedtls_xxx_drbg_context`).

### Error codes

Mbed TLS functions return a status of type `int`: 0 for success (or occasionally a positive value which is the output length), or a negative value, traditionally `MBEDTLS_ERR_xxx`, indicating an error.

PSA functions return a status of type [`psa_status_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__error/#group__error_1ga05676e70ba5c6a7565aff3c36677c1f9): `PSA_SUCCESS == 0` for success, or a negative value [`PSA_ERROR_xxx`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__error/) indicating an error.

In Mbed TLS 3.x, there is no overlap between `MBEDTLS_ERR_xxx` values and `PSA_ERROR_xxx` values. Thus applications that propagate error codes do not need to keep track of whether a value was initially reported as `psa_status_t` (`PSA_ERROR_xxx`) or `int` (Mbed TLS error codes).

Since TF-PSA-Crypto 1.0, the error sets have been fully merged. Many former `MBEDTLS_ERR_xxx` error codes have been merged with `PSA_ERROR_xxx` error codes with similar semantics. Functions returning `int` can return `PSA_ERROR_xxx` values, and some `MBEDTLS_ERR_xxx` constants are now aliases to a `PSA_ERROR_xxx` value. This also applies to Mbed TLS 4.0 onwards.

### Memory management

Apart from keys, as described in “[Key management](#key-management)” below, APIs that need to preserve state between function calls store this state in a structure allocated by the calling code. For example, multipart operations store state in a multipart operation object.

All PSA operation objects must be zero-initialized (or equivalently, initialized with the provided `PSA_XXX_INIT` macro or `psa_xxx_init()` function) before calling any API function.

Functions that output data require an output buffer of sufficient size. For all PSA crypto API functions that have an output buffer, there is a corresponding macro, generally called `PSA_XXX_OUTPUT_SIZE`, that calculates a sufficient size for the output buffer, given the relevant parameters. In some cases, there may be macros with less precision which can be resolved at compile time. For example, for the size of a buffer containing a hash, you can use `PSA_HASH_LENGTH(hash_alg)` where `hash_alg` is a specific hash algorithm, or `PSA_HASH_MAX_SIZE` for a buffer that is long enough for any supported hash. See the relevant sections of this document and of the reference documentation for more details.

#### Key management

One of the major differences between the legacy API and the PSA API is that in the PSA API, access to keys is indirect. Operations that require a key take a parameter of type [`psa_key_id_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__types_8h/#_CPPv412psa_key_id_t), which is an identifier for the key. This allows the API to be used with keys that are not directly accessible to the application, for example because they are stored in a secure environment that does not allow the key material to be exported.

To use a key:

1. First create a key object with a key creation function. The two most common ones are [`psa_import_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga0336ea76bf30587ab204a8296462327b) if you have the key material available and [`psa_generate_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__random/#group__random_1ga1985eae417dfbccedf50d5fff54ea8c5) to create a random key. The key creation function has the key identifier as an output parameter.
2. Use the key as desired, passing the key identifier obtained during the key creation.
3. Finally destroy the key object with [`psa_destroy_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__management/#group__key__management_1ga5f52644312291335682fbc0292c43cd2).

See “[Cipher key management](#cipher-key-management)”, “[MAC key management](#mac-key-management)”, “[Key lifecycle for asymmetric cryptography](#key-lifecycle-for-asymmetric-cryptography)”, “[Creating keys for asymmetric cryptography](#creating-keys-for-asymmetric-cryptography)” and “[Diffie-Hellman key pair management](#diffie-hellman-key-pair-management)” for more details about key management in specific workflows, including information about choosing the key's attributes.

If you need access to the key material, call [`psa_export_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga668e35be8d2852ad3feeef74ac6f75bf). If you need the public key corresponding to a key pair object, call [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062).

Note that a key consumes a key store entry, which is distinct from heap memory, until it is destroyed or the application exits. (This is not true for persistent keys, which instead consume disk space. Since persistent keys have no analog in the legacy API, we will not discuss them further in this document.)

## Summary of API modules

| Header | Function prefix | PSA equivalent |
| ------ | --------------- | -------------- |
| `aes.h` | `mbedtls_aes_` | [Symmetric encryption](#symmetric-encryption) |
| `aria.h` | `mbedtls_aria_` | [Symmetric encryption](#symmetric-encryption) |
| `asn1.h` | `mbedtls_asn1_` | No change ([PK format support interfaces](#pk-format-support-interfaces)) |
| `asn1write.h` | `mbedtls_asn1_write_` | No change ([PK format support interfaces](#pk-format-support-interfaces)) |
| `base64.h` | `mbedtls_base64_` | No change ([PK format support interfaces](#pk-format-support-interfaces)) |
| `bignum.h` | `mbedtls_mpi_` | None (no low-level arithmetic) |
| `build_info.h` | `MBEDTLS_` | Belongs in Mbed TLS, but see also `tf-psa-crypto/build_info.h` |
| `camellia.h` | `mbedtls_camellia_` | [Symmetric encryption](#symmetric-encryption) |
| `ccm.h` | `mbedtls_ccm_` | [Symmetric encryption](#symmetric-encryption), [Authenticated cipher operations](#authenticated-cipher-operations) |
| `chacha20.h` | `mbedtls_chacha20_` | [Symmetric encryption](#symmetric-encryption) |
| `chachapoly.h` | `mbedtls_chachapoly_` | [Symmetric encryption](#symmetric-encryption), [Authenticated cipher operations](#authenticated-cipher-operations) |
| `check_config.h` | N/A | No public APIs (internal support header) |
| `cipher.h` | `mbedtls_cipher_` | [Symmetric encryption](#symmetric-encryption) |
| `cmac.h` | `mbedtls_cipher_cmac_` | [Hashes and MAC](#hashes-and-mac), [MAC calculation](#mac-calculation) |
| `compat-2.x.h` | various | None (transitional APIs) |
| `config_psa.h` | N/A | No public APIs (internal support header) |
| `constant_time.h` | `mbedtls_ct_` | [Constant-time functions](#constant-time-functions) |
| `ctr_drbg.h` | `mbedtls_ctr_drbg_` | [Random generation interface](#random-generation-interface), [Deterministic pseudorandom generation](#deterministic-pseudorandom-generation) |
| `des.h` | `mbedtls_des_` | [Symmetric encryption](#symmetric-encryption) |
| `dhm.h` | `mbedtls_dhm_` | [Asymmetric cryptography](#asymmetric-cryptography) |
| `ecdh.h` | `mbedtls_ecdh_` | [Asymmetric cryptography](#asymmetric-cryptography) |
| `ecdsa.h` | `mbedtls_ecdsa_` | [Asymmetric cryptography](#asymmetric-cryptography) |
| `ecjpake.h` | `mbedtls_ecjpake_` | [EC-JPAKE](#ec-jpake) |
| `ecp.h` | `mbedtls_ecp_` | [Asymmetric cryptography](#asymmetric-cryptography) |
| `entropy.h` | `mbedtls_entropy_` | [Random generation interface](#random-generation-interface), [Entropy sources](#entropy-sources) |
| `error.h` | `mbedtls_*err*` | [Error messages](#error-messages) |
| `gcm.h` | `mbedtls_gcm_` | [Symmetric encryption](#symmetric-encryption), [Authenticated cipher operations](#authenticated-cipher-operations) |
| `hkdf.h` | `mbedtls_hkdf_` | [HKDF](#hkdf) |
| `hmac_drbg.h` | `mbedtls_hmac_drbg_` | [Random generation interface](#random-generation-interface), [Deterministic pseudorandom generation](#deterministic-pseudorandom-generation) |
| `lms.h` | `mbedtls_lms_` | No change ([LMS signatures](#lms-signatures)) |
| `mbedtls_config.h` | `MBEDTLS_` | [Compile-time configuration](#compile-time-configuration) |
| `md.h` | `mbedtls_md_` | [Hashes and MAC](#hashes-and-mac) |
| `md5.h` | `mbedtls_md5_` | [Hashes and MAC](#hashes-and-mac) |
| `memory_buffer_alloc.h` | `mbedtls_memory_buffer_alloc_` | No change (not a crypto API) |
| `nist_kw.h` | `mbedtls_nist_kw_` | [NIST KW and KWP](#nist-kw-and-kwp) |
| `oid.h` | `mbedtls_oid_` | [PK format support interfaces](#pk-format-support-interfaces) |
| `pem.h` | `mbedtls_pem_` | No change ([PK format support interfaces](#pk-format-support-interfaces)) |
| `pk.h` | `mbedtls_pk_` | [Asymmetric cryptography](#asymmetric-cryptography) |
| `pkcs5.h` | `mbedtls_pkcs5_` | [PKCS#5 module](#pkcs5-module) |
| `pkcs12.h` | `mbedtls_pkcs12_` | [PKCS#12 module](#pkcs12-module) |
| `platform.h` | `mbedtls_platform_` | No change (not a crypto API) |
| `platform_time.h` | `mbedtls_*time*` | No change (not a crypto API) |
| `platform_util.h` | `mbedtls_platform_` | No change (not a crypto API) |
| `poly1305.h` | `mbedtls_poly1305_` | None (but there is Chacha20-Poly1305 [AEAD](#symmetric-encryption)) |
| `private_access.h` | N/A | No public APIs (internal support header) |
| `psa_util.h` | various | No change (functions specific to removed legacy APIs have been removed in TF-PSA-Crypto 1.0) |
| `ripemd160.h` | `mbedtls_ripemd160_` | [Hashes and MAC](#hashes-and-mac) |
| `rsa.h` | `mbedtls_rsa_` | [Asymmetric cryptography](#asymmetric-cryptography) |
| `sha1.h` | `mbedtls_sha1_` | [Hashes and MAC](#hashes-and-mac) |
| `sha3.h` | `mbedtls_sha3_` | [Hashes and MAC](#hashes-and-mac) |
| `sha256.h` | `mbedtls_sha256_` | [Hashes and MAC](#hashes-and-mac) |
| `sha512.h` | `mbedtls_sha512_` | [Hashes and MAC](#hashes-and-mac) |
| `threading.h` | `mbedtls_threading_` | No change (not a crypto API) |

## Compile-time configuration

### Cryptographic mechanism availability

#### Crypto configuration file

The cryptographic mechanisms available through the PSA API are determined by the contents of the header file `"psa/crypto_config.h"`. You can override the file location with the macro [`TF_PSA_CRYPTO_CONFIG_FILE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/mbedtls__config_8h/#mbedtls__config_8h_1a25f7e358caa101570cb9519705c2b873), and you can set [`TF_PSA_CRYPTO_USER_CONFIG_FILE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/mbedtls__config_8h/#mbedtls__config_8h_1abd1870cc0d2681183a3018a7247cb137) to the path of an additional file (similar to `MBEDTLS_CONFIG_FILE` and `MBEDTLS_USER_CONFIG_FILE` for legacy configuration symbols).

3.x note: `TF_PSA_CRYPTO_CONFIG_FILE` and `TF_PSA_CRYPTO_USER_CONFIG_FILE` were called `MBEDTLS_PSA_CRYPTO_CONFIG_FILE` and `MBEDTLS_PSA_CRYPTO_USER_CONFIG_FILE` respectively in Mbed TLS 3.x.

#### General rules for `PSA_WANT_xxx`

The availability of cryptographic mechanisms in the PSA API is based on a systematic pattern:

* To make `PSA_ALG_aaa` available, enable `PSA_WANT_ALG_aaa`.
  For parametrized algorithms, there is a `PSA_WANT_` symbol both for the main macro and for each argument. For example, to make `PSA_ALG_HMAC(PSA_ALG_SHA_256)` available, enable both `PSA_WANT_ALG_HMAC` and `PSA_WANT_ALG_SHA_256`.

* To make `PSA_KEY_TYPE_ttt` available, enable `PSA_WANT_KEY_TYPE_ttt`.

    As an exception, starting in Mbed TLS 3.5.0, for key pair types, the feature selection is more fine-grained, with an additional suffix:
    * `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_BASIC` enables basic support for the key type, and in particular support for operations with a key of that type for enabled algorithms. This is automatically enabled if any of the other `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_yyy` options are enabled.
    * `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_IMPORT` enables support for `psa_import_key` to import a key of that type.
    * `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_GENERATE` enables support for `psa_generate_key` to randomly generate a key of that type.
    * `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_DERIVE` enables support for `psa_key_derivation_output_key` to deterministically derive a key of that type.
    * `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_EXPORT` enables support for `psa_export_key` to export a key of that type.

    Enabling any support for a key pair type automatically enables support for the corresponding public key type, as well as support for `psa_export_public_key` on the private key.

* To make `PSA_ECC_FAMILY_fff` available for size sss, enable `PSA_WANT_ECC_fff_sss`.

Note that all `PSA_WANT_xxx` symbols must be set to a non-zero value. In particular, setting `PSA_WANT_xxx` to an empty value may not be handled consistently.

#### A small `PSA_WANT` configuration example

For example, the following configuration enables hashing with SHA-256, AEAD with AES-GCM, signature with deterministic ECDSA using SHA-256 on the curve secp256r1 using a randomly generated key as well as the corresponding verification, and ECDH key exchange on secp256r1 and Curve25519.

```
#define PSA_WANT_ALG_SHA_256 1

#define PSA_WANT_KEY_TYPE_AES 1
#define PSA_WANT_ALG_GCM 1

#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
// ^^ implicitly enables PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC, PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY
#define PSA_WANT_ECC_SECP_R1_256 1 // secp256r1 (suitable for ECDSA and ECDH)
#define PSA_WANT_ECC_MONTGOMERY_255 1 // Curve25519 (suitable for ECDH)
#define PSA_WANT_ALG_DETERMINISTIC_ECDSA 1
#define PSA_WANT_ALG_ECDH
```

#### Automatically translating legacy configurations

If you have a working configuration file with legacy configuration options (i.e. an Mbed TLS 3.x configuration that did not enable `MBEDTLS_PSA_CRYPTO_CONFIG`), build **Mbed TLS 3.6** and run the program

```
programs/test/query_compile_time_config -l
```

(Note that you cannot use TF-PSA-Crypto for this: it would not recognize the legacy configuration options.)

The lines with `PSA_WANT_...=1` should constitute a PSA configuration that is similar to your legacy configuration. That is, for every line `PSA_WANT_XXX=1` in the output of `query_compile_time_config -l`, make sure the line `#define PSA_WANT_XXX 1` is enabled in `include/psa/crypto_config.h` (or alternate `TF_PSA_CRYPTO_CONFIG_FILE`). You use the following bash/Linux/macOS shell snippet to automate this translation:

```
programs/test/query_compile_time_config -l | sed -n 's/^\(PSA_WANT_.*\)=1/#define \1/p'
```

Please review the result as the configuration may not be fully equivalent in all cases. It will generally provide at least the same features, but sometimes this translation results in more than desired.

Note that this only generates the new selection of cryptographic mechanisms. You will also need to remove config lines that set legacy crypto options. Note also that TF-PSA-Crypto 1.0 has changed a few other options; see the [1.0 migration guide](1.0-migration-guide.md#configuration-of-tf-psa-crypto) for more information.

#### Implicit activation of crypto features

If a mechanism is not enabled by `PSA_WANT_xxx`, TF-PSA-Crypto will normally not include it. This allows builds that use few features to have a small code size. However, this is not guaranteed: a mechanism that is not explicitly requested can be enabled because it is a dependency of another configuration option, because it is used internally, or because the granularity is not fine enough to distinguish between it and another mechanism that is requested.

These automatic enablement rules may change in future versions of TF-PSA-Crypto, so you should not rely on them. Declare what you use.

### Optimization options

When PSA Crypto mechanisms are implemented by the built-in code from TF-PSA-Crypto, the legacy optimization options (e.g. `MBEDTLS_SHA256_SMALLER`, `MBEDTLS_ECP_WINDOW_SIZE`, etc.) apply to the PSA implementation as well (they invoke the same code under the hood).

The PSA Crypto API may use accelerator drivers. In this case any options controlling the driver behavior are driver-specific.

### Alternative implementations (`MBEDTLS_xxx_ALT` options)

In the Mbed TLS legacy interface, you can replace some cryptographic primitives and modes by an alternative implementation, by enabling configuration options of the form `MBEDTLS_xxx_ALT` and linking with your own implementation of the affected function or module. This feature does not exist in TF-PSA-Crypto.

The corresponding PSA feature is accelerator drivers. To implement an accelerator driver, see the [PSA cryptoprocessor driver example and guide](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/docs/psa-driver-example-and-guide.md). See the [Guide to driver-only builds](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/docs/driver-only-builds.md) for more information.

### Self-tests

There is currently [no PSA equivalent to the self-tests](https://github.com/Mbed-TLS/mbedtls/issues/7781) enabled by `MBEDTLS_SELF_TEST`.

## Miscellaneous support modules

### Error messages

As of TF-PSA-Crypto 1.0, there is no equivalent to the error messages provided by `mbedtls_strerror`. However, you can use the companion program `programs/psa/psa_constant_names` to convert various numbers (`psa_status_t`, `psa_algorithm_t`, `psa_key_type_t`, `psa_ecc_family_t`, `psa_dh_family_t`, `psa_key_usage_t`) to a programmer-friendly representation. The conversion doesn't depend on the library configuration or the target platform, so you can use a native build of this program even if you cross-compile your application.

```
$ programs/psa/psa_constant_names error -138
PSA_ERROR_BUFFER_TOO_SMALL
$ programs/psa/psa_constant_names type 0x7112
PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)
$ programs/psa/psa_constant_names alg 0x06000609
PSA_ALG_ECDSA(PSA_ALG_SHA_256)
```

Since TF-PSA-Crypto 1.0, legacy functions always return an `MBEDTLS_ERR_xxx` constant or a `psa_status_t` constant (`PSA_SUCCESS`, `PSA_OPERATION_INCOMPLETE` or `PSA_ERROR_xxx`). Functions no longer return values that are the sum of a “low-level” and a “high-level” error code.

### Constant-time functions

The PSA API does not have an equivalent to the timing-side-channel-resistance utility functions in `mbedtls/constant_time.h`. Continue using `mbedtls/constant_time.h` as needed.

Note that the PSA API does include features that reduce the need for `mbedtls_ct_memcmp`:

* To compare a MAC with a reference value, use `psa_mac_verify` rather than `psa_mac_compute` followed by `mbedtls_ct_memcmp`, or use `psa_mac_verify_setup` and `psa_mac_verify_finish` in the multi-part case. See “[MAC calculation](#mac-calculation)”.
* The AEAD decryption functions take care of verifying the tag. See “[Authenticated cipher operations](#authenticated-cipher-operations)”.

## Symmetric encryption

All PSA APIs have algorithm agility, where the functions depend only on the nature of the operation and the choice of a specific algorithm comes from an argument. There is no special API for a particular block cipher (`aes.h`, `aria.h`, `camellia.h`, `des.h`), a particular block cipher mode (`ccm.h`, `gcm.h`) or a particular stream cipher (`chacha20.h`, `chachapoly.h`). To migrate code using those low-level modules, please follow the recommendations in the following sections, using the same principles as the corresponding `cipher.h` API.

### Cipher mechanism selection

Instead of `mbedtls_cipher_id_t` (`MBEDTLS_CIPHER_ID_xxx` constants), `mbedtls_cipher_type_t` (`MBEDTLS_CIPHER_base_size_mode` constants), `mbedtls_cipher_mode_t` (`MBEDTLS_CIPHER_MODE_xxx` constants) and `mbedtls_cipher_padding_t` (`MBEDTLS_CIPHER_PADDING_xxx` constants), use the [`PSA_KEY_TYPE_xxx` and `PSA_ALG_xxx` constants](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/).

For modes that are based on a block cipher, the key type encodes the choice of block cipher:
[`PSA_KEY_TYPE_AES`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga6ee54579dcf278c677eda4bb1a29575e),
[`PSA_KEY_TYPE_ARIA`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#c.PSA_KEY_TYPE_ARIA),
[`PSA_KEY_TYPE_CAMELLIA`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gad8e5da742343fd5519f9d8a630c2ed81),
[`PSA_KEY_TYPE_DES`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga577562bfbbc691c820d55ec308333138).
The algorithm encodes the mode and if relevant the padding type:

* Unauthenticated cipher modes:
  [`PSA_ALG_CTR`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gad318309706a769cffdc64e4c7e06b2e9),
  [`PSA_ALG_CFB`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga0088c933e01d671f263a9a1f177cb5bc),
  [`PSA_ALG_OFB`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gae96bb421fa634c6fa8f571f0112f1ddb),
  [`PSA_ALG_XTS`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gaa722c0e426a797fd6d99623f59748125),
  [`PSA_ALG_ECB_NO_PADDING`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gab8f0609cd0f12cccc9c950fd5a81a0e3),
  [`PSA_ALG_CBC_NO_PADDING`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gacb332d72716958880ee7f97d8365ae66),
  [`PSA_ALG_CBC_PKCS7`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gaef50d2e9716eb6d476046608e4e0c78c),
  [`PSA_ALG_CCM_STAR_NO_TAG`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga89627bb27ec3ce642853ab8554a88572).
* Other padding modes, which are obsolete, are not available in the PSA API. If you need them, handle the padding in your application code and use the `NO_PADDING` algorithm.
* AEAD modes:
  [`PSA_ALG_CCM`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gac2c0e7d21f1b2df5e76bcb4a8f84273c),
  [`PSA_ALG_GCM`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga0d7d02b15aaae490d38277d99f1c637c).
* KW/KWP modes are not available in the PSA API at the time of writing.

For the ChaCha20 unauthenticated cipher, use [`PSA_KEY_TYPE_CHACHA20`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga901548883b3bce56cc21c3a22cf8d93c) with [`PSA_ALG_STREAM_CIPHER`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gad98c105198f7428f7d1dffcb2cd398cd).
For the Chacha20+Poly1305 AEAD, use [`PSA_KEY_TYPE_CHACHA20`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga901548883b3bce56cc21c3a22cf8d93c) with [`PSA_ALG_CHACHA20_POLY1305`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga1fec55093541640a71bdd022d4adfb9c)

### Cipher mechanism availability

For each key type value `PSA_KEY_TYPE_xxx`, the symbol `PSA_WANT_KEY_TYPE_xxx` is defined with a non-zero value if the library is built with support for that key type. For each algorithm value `PSA_ALG_yyy`, the symbol `PSA_WANT_ALG_yyy` is defined with a non-zero value if the library is built with support for that algorithm. Note that for a mechanism to be supported, both the key type and the algorithm must be supported.

For example, to test if AES-CBC-PKCS7 is supported, in the legacy API, you could write:
```
#if defined(MBEDTLS_AES_C) && \
    defined(MBEDTLS_CIPHER_MODE_CBC) && defined(MBEDTLS_CIPHER_PADDING_PKCS7)
```
The equivalent in the PSA API is
```
#if PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_CBC_PKCS7
```

### Cipher metadata

Both APIs express key sizes in bits. Note however that in the PSA API, the size of a _buffer_ is always expressed in bytes, even if that buffer contains a key.

The following table lists corresponding PSA macros for maximum-size macros that take all supported algorithms into account.

| Legacy macro | PSA macro |
| ------------ | --------- |
| `MBEDTLS_MAX_IV_LENGTH` | [`PSA_CIPHER_IV_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_IV_MAX_SIZE), [`PSA_AEAD_NONCE_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#crypto__sizes_8h_1ac2a332765ba4ccfc24935d6f7f48fcc7) |
| `MBEDTLS_MAX_BLOCK_LENGTH` | [`PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE) |
| `MBEDTLS_MAX_KEY_LENGTH` | no equivalent|

There is no equivalent to the type `mbedtls_cipher_info_t` and the functions `mbedtls_cipher_info_from_type` and `mbedtls_cipher_info_from_values` in the PSA API because it is unnecessary. All macros and functions operate directly on key type values (`psa_key_type_t`, `PSA_KEY_TYPE_xxx` constants) and algorithm values (`psa_algorithm_t`, `PSA_ALG_xxx` constants).

| Legacy function | PSA macro |
| --------------- | --------- |
| `mbedtls_cipher_info_get_iv_size` | [`PSA_CIPHER_IV_LENGTH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_IV_LENGTH), [`PSA_AEAD_NONCE_LENGTH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_AEAD_NONCE_LENGTH) |
| `mbedtls_cipher_info_get_block_size` | not available (use specific macros for the IV, nonce or tag length) |

The following features have no PSA equivalent:

* `mbedtls_cipher_list`: the PSA API does not currently have a discovery mechanism for cryptographic mechanisms, but one may be added in the future.
* `mbedtls_cipher_info_has_variable_key_bitlen`, `mbedtls_cipher_info_has_variable_iv_size`: the PSA API does not currently have such mechanism for high-level metadata information.
* `mbedtls_cipher_info_from_string`: there is no equivalent of Mbed TLS's lookup based on a (nonstandard) name.

### Cipher key management

The legacy API and the PSA API have a different organization of operations in several respects:

* In the legacy API, each operation object contains the necessary key material. In the PSA API, an operation object contains a reference to a key object. To perform a cryptographic operation, you must create a key object first. However, for a one-shot operation, you do not need an operation object, just a single function call.
* The legacy API uses the same interface for authenticated and non-authenticated ciphers, while the PSA API has separate functions.
* The legacy API uses the same functions for encryption and decryption, while the PSA API has separate functions where applicable.

Here is an overview of the lifecycle of a key object.

1. First define the attributes of the key by filling a [`psa_key_attributes_t` structure](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga0ec645e1fdafe59d591104451ebf5680). You need to set the following parameters:
    * Call [`psa_set_key_type`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga6857ef0ecb3fa844d4536939d9c64025) to set the key type to the desired `PSA_KEY_TYPE_xxx` value (see “[Cipher mechanism selection](#cipher-mechanism-selection)”).
    * Call [`psa_set_key_bits`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaf61683ac87f87687a40262b5afbfa018) to set the key's size in bits. This is optional with `psa_import_key`, which determines the key size from the length of the key material.
    * Call [`psa_set_key_algorithm`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaeb8341ca52baa0279475ea3fd3bcdc98) to set the algorithm to the desired `PSA_ALG_xxx` value (see “[Cipher mechanism selection](#cipher-mechanism-selection)”). By design, the same key cannot be used with multiple algorithms.
    * Call [`psa_set_key_usage_flags`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga42a65b3c4522ce9b67ea5ea7720e17de) to enable at least [`PSA_KEY_USAGE_ENCRYPT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#c.PSA_KEY_USAGE_ENCRYPT) or [`PSA_KEY_USAGE_DECRYPT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#c.PSA_KEY_USAGE_DECRYPT), depending on which direction you want to use the key in. To allow both directions, use the flag mask `PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_ENCRYPT`. The same policy flags cover authenticated and non-authenticated encryption/decryption.
2. Call one of the key creation functions, passing the attributes defined in the previous step, to get an identifier of type [`psa_key_id_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__types_8h/#_CPPv412psa_key_id_t) to the key object.
    * Use [`psa_import_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga0336ea76bf30587ab204a8296462327b) to directly import key material.
    * If the key is randomly generated, use [`psa_generate_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__random/#group__random_1ga1985eae417dfbccedf50d5fff54ea8c5).
    * If the key is derived from other material (for example from a key exchange), use the [key derivation interface](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/) and create the key with [`psa_key_derivation_output_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gada7a6e17222ea9e7a6be6864a00316e1).
3. Call the functions in the following sections to perform operations on the key. The same key object can be used in multiple operations.
4. To free the resources used by the key object, call [`psa_destroy_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__management/#group__key__management_1ga5f52644312291335682fbc0292c43cd2) after all operations with that key are finished.

### Unauthenticated cipher operations

Recall the workflow of an unauthenticated cipher operation in the legacy Mbed TLS cipher API:

1. Create a cipher context of type `mbedtls_cipher_context_t` and initialize it with `mbedtls_cipher_init`.
2. Establish the operation parameters (algorithm, key, mode) with `mbedtls_cipher_setup`, `mbedtls_cipher_setkey` (or `mbedtls_cipher_setup_psa`), `mbedtls_cipher_set_padding_mode` if applicable.
3. Set the IV with `mbedtls_cipher_set_iv` (except for ECB which does not use an IV).
4. For a one-shot operation, call `mbedtls_cipher_crypt`. To pass the input in multiple parts, call `mbedtls_cipher_update` as many times as necessary followed by `mbedtls_cipher_finish`.
5. Finally free the resources associated with the operation object by calling `mbedtls_cipher_free`.

For a one-shot operation (where the whole plaintext or ciphertext is passed as a single input), the equivalent workflow with the PSA API is to call a single function:

* [`psa_cipher_encrypt`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1ga61f02fbfa681c2659546eca52277dbf1) to perform encryption with a random IV of the default size (indicated by [`PSA_CIPHER_IV_LENGTH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_IV_LENGTH)). (To encrypt with a specified IV, use the multi-part API described below.) You can use the macro [`PSA_CIPHER_ENCRYPT_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_ENCRYPT_OUTPUT_SIZE) or [`PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE) to determine a sufficient size for the output buffer.
* [`psa_cipher_decrypt`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1gab3593f5f14d8c0431dd306d80929215e) to perform decryption with a specified IV. You can use the macro [`PSA_CIPHER_DECRYPT_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_DECRYPT_OUTPUT_SIZE) or [`PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE) to determine a sufficient size for the output buffer.

For a multi-part operation, the equivalent workflow with the PSA API is as follows:

1. Create an operation object of type [`psa_cipher_operation_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1ga1399de29db657e3737bb09927aae51fa) and zero-initialize it (or use the corresponding `INIT` macro).
2. Select the key and algorithm with [`psa_cipher_encrypt_setup`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1ga587374c0eb8137a572f8e2fc409bb2b4) or [`psa_cipher_decrypt_setup`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1gaa4ba3a167066eaef2ea49abc5dcd1d4b) depending on the desired direction.
3. When encrypting with a random IV, use [`psa_cipher_generate_iv`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1ga29fd7d32a5729226a2f73e7b6487bd8a). When encrypting with a chosen IV, or when decrypting, set the IV with [`psa_cipher_set_iv`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1ga9caddac1a429a5032d6d4a907fb70ba1). Skip this step with ECB since it does not use an IV.
4. Call [`psa_cipher_update`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1gac3ca27ac6682917c48247d01fd96cd0f) as many times as needed. You can use [`PSA_CIPHER_UPDATE_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_UPDATE_OUTPUT_SIZE) or [`PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#crypto__sizes_8h_1ab1f6598efd6a7dc56e7ad7e34719eb32) to determine a sufficient size for the output buffer.
5. Call [`psa_cipher_finish`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1ga1dcb58b8befe23f8a4d7a1d49c99249b) to obtain the last part of the output. You can use [`PSA_CIPHER_FINISH_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_FINISH_OUTPUT_SIZE) or [`PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE) to determine a sufficient size for the output buffer.

If you need to interrupt the operation after calling the setup function without calling the finish function, call [`psa_cipher_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1gaad482cdca2098bca0620596aaa02eaa4).

### Authenticated cipher operations

Recall the workflow of an authenticated cipher operation in the legacy Mbed TLS cipher API (or similar workflows in the `chachapoly`, `ccm` and `gcm` modules):

1. Create a cipher context of type `mbedtls_cipher_context_t` and initialize it with `mbedtls_cipher_init`.
2. Establish the operation parameters (algorithm, key, mode) with `mbedtls_cipher_setup`, `mbedtls_cipher_setkey` (or `mbedtls_cipher_setup_psa`), `mbedtls_cipher_set_padding_mode` if applicable.
3. Set the nonce with `mbedtls_cipher_set_iv` (or the `starts` function for low-level modules). For CCM, which requires direct use of the `ccm` module, also call `mbedtls_ccm_set_lengths` to set the length of the additional data and of the plaintext.
4. Call `mbedtls_cipher_update_ad` to pass the unencrypted additional data.
5. Call `mbedtls_cipher_update` as many times as necessary to pass the input plaintext or ciphertext.
6. Call `mbedtls_cipher_finish` to obtain the last part of the output. Then call `mbedtls_cipher_write_tag` (when encrypting) or `mbedtls_cipher_check_tag` (when decrypting) to process the authentication tag.
7. Finally free the resources associated with the operation object by calling `mbedtls_cipher_free`.

Steps 3–6 can be replaced by a single call to `mbedtls_cipher_auth_encrypt_ext` or `mbedtls_cipher_auth_decrypt_ext` for a one-shot operation (where the whole plaintext or ciphertext is passed as a single input).

For a one-shot operation, the PSA API allows you to call a single function:

* [`psa_aead_encrypt`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1gae72e1eb3c2da3ebd843bb9c8db8df509) to perform authenticated encryption with a random nonce of the default size (indicated by [`PSA_AEAD_NONCE_LENGTH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_AEAD_NONCE_LENGTH)), with the authentication tag written at the end of the output. (To encrypt with a specified nonce, or to separate the tag from the rest of the ciphertext, use the multi-part API described below.)  You can use the macro [`PSA_AEAD_ENCRYPT_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_AEAD_ENCRYPT_OUTPUT_SIZE) or [`PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE) to determine a sufficient size for the output buffer.
* [`psa_aead_decrypt`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1gae799f6196a22d50c216c947e0320d3ba) to perform authenticated decryption of a ciphertext with the authentication tag at the end. (If the tag is separate, use the multi-part API described below.) You can use the macro [`PSA_AEAD_DECRYPT_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_DECRYPT_OUTPUT_SIZE) or [`PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE) to determine a sufficient size for the output buffer.

For a multi-part operation, the equivalent workflow with the PSA API is as follows:

1. Create an operation object of type [`psa_aead_operation_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1ga14f6a01afbaa8c5b3d8c5d345cbaa3ed) and zero-initialize it (or use the corresponding `INIT` macro).
2. Select the key and algorithm with [`psa_aead_encrypt_setup`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1ga2732c40ce8f3619d41359a329e9b46c4) or [`psa_aead_decrypt_setup`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1gaaa5c5018e67a7a6514b7e76b9a14de26) depending on the desired direction.
3. When encrypting with a random nonce, use [`psa_aead_generate_nonce`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1ga5799df1c555efd35970b65be51cb07d1). When encrypting with a chosen nonce, or when decrypting, set the nonce with [`psa_aead_set_nonce`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1ga59132751a6f843d038924cb217b5e13b). If the algorithm is CCM, you must also call [`psa_aead_set_lengths`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1gad3431e28d05002c2a7b0760610176050) before or after setting the nonce (for other algorithms, this is permitted but not needed).
4. Call [`psa_aead_update_ad`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1ga6d0eed03f832e5c9c91cb8adf2882569) as many times as needed.
5. Call [`psa_aead_update`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1gaf6d49864951ca42136b4a9b71ea26e5c) as many times as needed. You can use [`PSA_AEAD_UPDATE_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_AEAD_UPDATE_OUTPUT_SIZE) or [`PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE) to determine a sufficient size for the output buffer.
6. Finally:
    * When encrypting, call [`psa_aead_finish`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1ga759791bbe1763b377c3b5447641f1fc8) to obtain the last part of the ciphertext and the authentication tag. You can use [`PSA_AEAD_FINISH_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_AEAD_FINISH_OUTPUT_SIZE) or [`PSA_AEAD_FINISH_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_AEAD_FINISH_OUTPUT_MAX_SIZE) to determine a sufficient size for the output buffer.
    * When decrypting, call [`psa_aead_verify`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1gae0280e2e61a185b893c36d858453f0d0) to obtain the last part of the plaintext and check the authentication tag. You can use [`PSA_AEAD_VERIFY_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_AEAD_VERIFY_OUTPUT_SIZE) or [`PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE) to determine a sufficient size for the output buffer.

If you need to interrupt the operation after calling the setup function without calling the finish or verify function, call [`psa_aead_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1gae8a5f93d92318c8f592ee9fbb9d36ba0).

### Miscellaneous cipher operation management

The equivalent of `mbedtls_cipher_reset` is to call [`psa_cipher_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__cipher/#group__cipher_1gaad482cdca2098bca0620596aaa02eaa4) or [`psa_aead_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__aead/#group__aead_1gae8a5f93d92318c8f592ee9fbb9d36ba0). Note that you must set the key again with a setup function: the PSA API does not have a special way to reuse an operation object with the same key.

There is no equivalent for the `mbedtls_cipher_get_xxx` functions to extract information from an ongoing PSA cipher or AEAD operation. Applications that need this information will need to save it from the key and operation parameters.

### NIST KW and KWP

TF-PSA-Crypto 1.0 does not yet have a PSA API for key wrapping. We plan to implement the upcoming PSA API in a future minor version. In the meantime, you can continue using the legacy module `<mbedtls/nist_kw.h>`. The API of this module was tweaked in TF-PSA-Crypto 1.0, compared to Mbed TLS 3.6, to take the wrapping key as a PSA key identifier as input instead of a custom context. See the [1.0/4.0 migration guide](1.0-migration-guide.md#changes-to-nist_kw) for more information.

## Hashes and MAC

The PSA API groups functions by purpose rather than by underlying primitive: there is a MAC API (equivalent to `md.h` for HMAC, and `cmac.h` for CMAC) and a hash API (equivalent to `md.h` for hashing). There is no special API for a particular hash algorithm (`md5.h`, `sha1.h`, `sha256.h`, `sha512.h`, `sha3.h`). To migrate code using those low-level modules, please follow the recommendations in the following section, using the same principles as the corresponding `md.h` API.

Note that `<mbedtls/md.h>` is still available in TF-PSA-Crypto 1.x, for hash operations only. It is a thin wrapper over the corresponding PSA function. In TF-PSA-Crypto, `<mbedtls/md.h>` does not support HMAC, so applications using HMAC will need to migrate to PSA.

The PSA API does not have a direct interface for the AES-CMAC-PRF-128 algorithm from RFC 4615 calculated by `mbedtls_aes_cmac_prf_128` at the time of writing. You can implement it using the MAC interface with an AES key and the CMAC algorithm.

### Hash mechanism selection

The equivalent to `mbedtls_md_type_t` and `MBEDTLS_MD_XXX` constants is the type `psa_algorithm_t` and `PSA_ALG_xxx` constants (the type encompasses all categories of cryptographic algorithms, not just hashes). PSA offers a similar selection of algorithms, but note that SHA-1 and SHA-2 are spelled slightly differently.

| Mbed TLS constant      | PSA constant        |
| ---------------------- | ------------------- |
| `MBEDTLS_MD_MD5`       | `PSA_ALG_MD5`       |
| `MBEDTLS_MD_SHA1`      | `PSA_ALG_SHA_1`     |
| `MBEDTLS_MD_SHA224`    | `PSA_ALG_SHA_224`   |
| `MBEDTLS_MD_SHA256`    | `PSA_ALG_SHA_256`   |
| `MBEDTLS_MD_SHA384`    | `PSA_ALG_SHA_384`   |
| `MBEDTLS_MD_SHA512`    | `PSA_ALG_SHA_512`   |
| `MBEDTLS_MD_RIPEMD160` | `PSA_ALG_RIPEMD160` |
| `MBEDTLS_MD_SHA3_224`  | `PSA_ALG_SHA3_224`  |
| `MBEDTLS_MD_SHA3_256`  | `PSA_ALG_SHA3_256`  |
| `MBEDTLS_MD_SHA3_384`  | `PSA_ALG_SHA3_384`  |
| `MBEDTLS_MD_SHA3_512`  | `PSA_ALG_SHA3_512`  |

The following helper functions from `<mbedtls/psa_util.h>` can be used to convert between the two types:

- `mbedtls_md_psa_alg_from_type()` converts from legacy `mbedtls_md_type_t` to PSA's `psa_algorithm_t`.
- `mbedtls_md_type_from_psa_alg()` converts from PSA's `psa_algorithm_t` to legacy `mbedtls_md_type_t`.

### MAC mechanism selection

PSA Crypto has a generic API with the same functions for all MAC mechanisms. The mechanism is determined by a combination of an algorithm value of type [`psa_algorithm_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gac2e4d47f1300d73c2f829a6d99252d69) and a key type value of type [`psa_key_type_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga63fce6880ca5933b5d6baa257febf1f6).

* For HMAC, the algorithm is [`PSA_ALG_HMAC`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga70f397425684b3efcde1e0e34c28261f)`(hash)` where `hash` is the underlying hash algorithm (see “[Hash mechanism selection](#hash-mechanism-selection)”),
  for example `PSA_ALG_HMAC(PSA_ALG_SHA_256)` for HMAC-SHA-256.
  The key type is [`PSA_KEY_TYPE_HMAC`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__values_8h/#c.PSA_KEY_TYPE_HMAC) regardless of the hash algorithm.
* For CMAC, the algorithm is [`PSA_ALG_CMAC`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__values_8h/#c.PSA_ALG_CMAC) regardless of the underlying block cipher. The key type determines the block cipher:
  [`PSA_KEY_TYPE_AES`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga6ee54579dcf278c677eda4bb1a29575e),
  [`PSA_KEY_TYPE_ARIA`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#c.PSA_KEY_TYPE_ARIA),
  [`PSA_KEY_TYPE_CAMELLIA`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gad8e5da742343fd5519f9d8a630c2ed81) or
  [`PSA_KEY_TYPE_DES`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga577562bfbbc691c820d55ec308333138).

### Hash and MAC mechanism availability

For each key type value `PSA_KEY_TYPE_xxx`, the symbol `PSA_WANT_KEY_TYPE_xxx` is defined with a non-zero value if the library is built with support for that key type. For each algorithm value `PSA_ALG_yyy`, the symbol `PSA_WANT_ALG_yyy` is defined with a non-zero value if the library is built with support for that algorithm. For a compound mechanism, all parts must be supported. In particular, for HMAC, all three of `PSA_WANT_KEY_TYPE_HMAC`, `PSA_WANT_ALG_HMAC` and the underlying hash must be enabled. (A configuration with only one of `PSA_WANT_KEY_TYPE_HMAC` and `PSA_WANT_ALG_HMAC` is technically possible but not useful.)

For example, to test if HMAC-SHA-256 is supported, in the legacy API, you could write:
```
#if defined(MBEDTLS_MD_C) && defined(MBEDTLS_SHA256_C)
```
The equivalent in the PSA API is
```
#if PSA_WANT_KEY_TYPE_HMAC && PSA_WANT_ALG_HMAC && PSA_WANT_ALG_SHA_256
```

To test if AES-CMAC is supported, in the legacy API, you could write:
```
if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CMAC_C)
```
The equivalent in the PSA API is
```
#if PSA_WANT_KEY_TYPE_AES && PSA_WANT_ALG_CMAC
```

### Hash algorithm metadata

There is no equivalent to the type `mbedtls_md_info_t` and the functions `mbedtls_md_info_from_type` and `mbedtls_md_get_type` in the PSA API because it is unnecessary. All macros and functions operate directly on algorithm (`psa_algorithm_t`, `PSA_ALG_xxx` constants).

| Legacy macro | PSA macro |
| ------------ | --------- |
| `MBEDTLS_MD_MAX_SIZE` | [`PSA_HASH_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_HASH_MAX_SIZE) |
| `MBEDTLS_MD_MAX_BLOCK_SIZE` | [`PSA_HMAC_MAX_HASH_BLOCK_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_HMAC_MAX_HASH_BLOCK_SIZE) |
| `mbedtls_md_get_size` | [`PSA_HASH_LENGTH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_HASH_LENGTH) |
| `mbedtls_md_get_size_from_type` | [`PSA_HASH_LENGTH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_HASH_LENGTH) |

The following features have no PSA equivalent:

* `mbedtls_md_list`: the PSA API does not currently have a discovery mechanism for cryptographic mechanisms, but one may be added in the future.
* `mbedtls_md_info_from_ctx`
* `mbedtls_cipher_info_from_string`, `mbedtls_md_get_name`: there is no equivalent of Mbed TLS's lookup based on a (nonstandard) name.

### Hash calculation

TF-PSA-Crypto will still support the `<mbedtls/md.h>` interface for hash calculations throughout the 1.x version range, so migrating to PSA for this use case is optional.

The equivalent of `mbedtls_md` for a one-shot hash calculation is [`psa_hash_compute`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__hash/#group__hash_1gac69f7f19d96a56c28cf3799d11b12156). In addition, to compare the hash of a message with an expected value, you can call [`psa_hash_compare`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__hash/#group__hash_1ga0c08f4797bec96b886c8c8d7acc2a553) instead of `mbedtls_md` followed by `memcmp` or a constant-time equivalent.

For a multi-part hash calculation, the legacy process is as follows:

1. Create a digest context of type `mbedtls_md_context_t` and initialize it with `mbedtls_md_init`.
2. Call `mbedtls_md_setup` to select the hash algorithm, with `hmac=0`. Then call `mbedtls_md_starts` to start the hash operation.
3. Call `mbedtls_md_update` as many times as necessary.
4. Call `mbedtls_md_finish`. If verifying the hash against an expected value, compare the result with the expected value.
5. Finally free the resources associated with the operation object by calling `mbedtls_md_free`.

The equivalent process in the PSA API is as follows:

1. Create an operation object of type [`psa_hash_operation_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__hash/#group__hash_1ga3c4205d2ce66c4095fc5c78c25273fab) and zero-initialize it (or use the corresponding `INIT` macro).
2. Call [`psa_hash_setup`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__hash/#group__hash_1ga8d72896cf70fc4d514c5c6b978912515) to specify the algorithm.
3. Call [`psa_hash_update`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__hash/#group__hash_1ga65b16ef97d7f650899b7db4b7d1112ff) as many times as necessary.
4. To obtain the hash, call [`psa_hash_finish`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__hash/#group__hash_1ga4795fd06a0067b0adcd92e9627b8c97e). Alternatively, to verify the hash against an expected value, call [`psa_hash_verify`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__hash/#group__hash_1ga7be923c5700c9c70ef77ee9b76d1a5c0).

If you need to interrupt the operation after calling the setup function without calling the finish or verify function, call [`psa_hash_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__hash/#group__hash_1gab0b4d5f9912a615559497a467b532928).

There is no equivalent to `mbedtls_md_file` in the PSA API. Load the file data and calculate its hash.

### MAC key management

The legacy API and the PSA API have a different organization of operations in several respects:

* In the legacy API, each operation object contains the necessary key material. In the PSA API, an operation object contains a reference to a key object. To perform a cryptographic operation, you must create a key object first. However, for a one-shot operation, you do not need an operation object, just a single function call.
* The legacy API uses the same interface for authenticated and non-authenticated ciphers, while the PSA API has separate functions.
* The legacy API uses the same functions for encryption and decryption, while the PSA API has separate functions where applicable.

Here is an overview of the lifecycle of a key object.

1. First define the attributes of the key by filling a [`psa_key_attributes_t` structure](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga0ec645e1fdafe59d591104451ebf5680). You need to set the following parameters:
    * Call [`psa_set_key_type`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga6857ef0ecb3fa844d4536939d9c64025) to set the key type to the desired `PSA_KEY_TYPE_xxx` value (see “[Cipher mechanism selection](#cipher-mechanism-selection)”).
    * Call [`psa_set_key_bits`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaf61683ac87f87687a40262b5afbfa018) to set the key's size in bits. This is optional with `psa_import_key`, which determines the key size from the length of the key material.
    * Call [`psa_set_key_algorithm`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaeb8341ca52baa0279475ea3fd3bcdc98) to set the algorithm to the desired `PSA_ALG_xxx` value (see “[Cipher mechanism selection](#cipher-mechanism-selection)”). By design, the same key cannot be used with multiple algorithms.
    * Call [`psa_set_key_usage_flags`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga42a65b3c4522ce9b67ea5ea7720e17de) to enable at least [`PSA_KEY_USAGE_SIGN_MESSAGE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#c.PSA_KEY_USAGE_SIGN_MESSAGE) to calculate a MAC or [`PSA_KEY_USAGE_VERIFY_MESSAGE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#c.PSA_KEY_USAGE_VERIFY_MESSAGE) to verify the MAC of a message. To allow both directions, use the flag mask `PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE`.
2. Call one of the key creation functions, passing the attributes defined in the previous step, to get an identifier of type [`psa_key_id_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__types_8h/#_CPPv412psa_key_id_t) to the key object.
    * Use [`psa_import_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga0336ea76bf30587ab204a8296462327b) to directly import key material.
    * If the key is randomly generated, use [`psa_generate_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__random/#group__random_1ga1985eae417dfbccedf50d5fff54ea8c5).
    * If the key is derived from other material (for example from a key exchange), use the [key derivation interface](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/) and create the key with [`psa_key_derivation_output_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gada7a6e17222ea9e7a6be6864a00316e1).
3. Call the functions in the following sections to perform operations on the key. The same key object can be used in multiple operations.
4. To free the resources used by the key object, call [`psa_destroy_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__management/#group__key__management_1ga5f52644312291335682fbc0292c43cd2) after all operations with that key are finished.

### MAC calculation

The process for a HMAC operation in the legacy API is as follows:

1. Create a digest context of type `mbedtls_md_context_t` and initialize it with `mbedtls_md_init`.
2. Call `mbedtls_md_setup` to select the hash algorithm, with `hmac=1`. Then call `mbedtls_md_hmac_starts` to set the key.
3. Call `mbedtls_md_hmac_update` as many times as necessary.
4. Call `mbedtls_md_hmac_finish`. If verifying the MAC against an expected value, compare the result with the expected value. Note that this comparison should be in constant time to avoid a side channel vulnerability, for example using `mbedtls_ct_memcmp`.
5. Finally free the resources associated with the operation object by calling `mbedtls_md_free`.

The process for a CMAC operation in the legacy API is as follows:

1. Create a cipher context of type `mbedtls_cipher_context_t` and initialize it with `mbedtls_cipher_init`.
2. Call `mbedtls_cipher_setup` to select the block cipher. Then call `mbedtls_md_cmac_starts` to set the key.
3. Call `mbedtls_cipher_cmac_update` as many times as necessary.
4. Call `mbedtls_cipher_cmac_finish`. If verifying the MAC against an expected value, compare the result with the expected value. Note that this comparison should be in constant time to avoid a side channel vulnerability, for example using `mbedtls_ct_memcmp`.
5. Finally free the resources associated with the operation object by calling `mbedtls_cipher_free`.

The process in the PSA API to calculate a MAC is as follows:

1. Create an operation object of type [`psa_mac_operation_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1ga78f0838b0c4e3db28b26355624d4bd37) and zero-initialize it (or use the corresponding `INIT` macro).
2. Call [`psa_mac_sign_setup`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1ga03bc3e3c0b7e55b20d2a238e418d46cd) to specify the algorithm and the key. See “[MAC key management](#mac-key-management)” for how to obtain a key identifier.
3. Call [`psa_mac_update`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1ga5560af371497babefe03c9da4e8a1c05) as many times as necessary.
4. To obtain the MAC, call [`psa_mac_sign_finish`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1gac22bc0125580c96724a09226cfbc97f2).

To verify a MAC against an expected value, use the following process instead:

1. Create an operation object of type [`psa_mac_operation_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1ga78f0838b0c4e3db28b26355624d4bd37) and zero-initialize it (or use the corresponding `INIT` macro).
2. Call [`psa_mac_verify_setup`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1ga08ae327fcbc5f8e201172fe11e536984) to specify the algorithm and the key. See “[MAC key management](#mac-key-management)” for how to obtain a key identifier.
3. Call [`psa_mac_update`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1ga5560af371497babefe03c9da4e8a1c05) as many times as necessary.
4. To verify the MAC against an expected value, call [`psa_mac_verify_finish`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1gac92b2930d6728e1be4d011c05d485822).

If you need to interrupt the operation after calling the setup function without calling the finish function, call [`psa_mac_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1gacd8dd54855ba1bc0a03f104f252884fd).

The PSA API also offers functions for a one-shot MAC calculation, similar to `mbedtls_cipher_cmac` and `mbedtls_md_hmac`:

* [`psa_mac_compute`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1gabf02ebd3595ea15436967092b5d52878) to calculate the MAC of a buffer in memory.
* [`psa_mac_verify`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1gaf6988545df5d5e2466c34d753443b15a) to verify the MAC of a buffer in memory against an expected value.

In both cases, see “[MAC key management](#mac-key-management)” for how to obtain a key identifier.

### Miscellaneous hash or MAC operation management

The equivalent of `mbedtls_md_reset`, `mbedtls_md_hmac_reset` or `mbedtls_cmac_reset` is to call [`psa_hash_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__hash/#group__hash_1gab0b4d5f9912a615559497a467b532928) or [`psa_mac_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group___m_a_c/#group___m_a_c_1gacd8dd54855ba1bc0a03f104f252884fd). Note that you must call a setup function to specify the algorithm and the key (for MAC) again, and they can be different ones.

The equivalent of `mbedtls_md_clone` to clone a hash operation is [`psa_hash_clone`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__hash/#group__hash_1ga39673348f3302b4646bd780034a5aeda). A PSA MAC operation cannot be cloned.

## Key derivation

### HKDF

PSA Crypto provides access to HKDF, HKDF-Extract and HKDF-Expand via its [key derivation interface](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/). This is a generic interface using an operation object with one function call for each input and one function call for each output.

1. Create an operation object of type [`psa_key_derivation_operation_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga5f099b63799a0959c3d46718c86c2609) and zero-initialize it (or use the corresponding `INIT` macro).
2. Call [`psa_key_derivation_setup`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gac0b6a76e45cceb1862752bf041701859) to select the algorithm, which is a value of type [`psa_algorithm_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gac2e4d47f1300d73c2f829a6d99252d69). For HKDF and variants, use one of the macros [`PSA_ALG_HKDF`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__values_8h/#c.PSA_ALG_HKDF), [`PSA_ALG_HKDF_EXTRACT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__values_8h/#c.PSA_ALG_HKDF_EXTRACT) or [`PSA_ALG_HKDF_EXPAND`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__values_8h/#c.PSA_ALG_HKDF_EXPAND) with the [hash algorithm](#hash-mechanism-selection) passed as an argument. For example `PSA_ALG_HKDF(PSA_ALG_SHA_256)` selects HKDF-SHA-256.
3. Call [`psa_key_derivation_input_bytes`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga8fd934dfb0ca45cbf89542ef2a5494c2) on each of the inputs in the order listed below. (Use [`psa_key_derivation_input_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gab2d7ce8705dd8e4a093f4b8a21a0c15a) instead for an input that is a PSA key object.) The input step value for each step is as follows:
    1. [`PSA_KEY_DERIVATION_INPUT_SALT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__derivation/#group__derivation_1gab62757fb125243562c3947a752470d4a) for the salt used during the extraction step. Omit this step for HKDF-Expand. For HKDF, you may omit this step if the salt is empty.
    2. [`PSA_KEY_DERIVATION_INPUT_SECRET`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__derivation/#group__derivation_1ga0ddfbe764baba995c402b1b0ef59392e) for the secret input.
    3. [`PSA_KEY_DERIVATION_INPUT_INFO`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__derivation/#group__derivation_1gacef8df989e09c769233f4b779acb5b7d) for the info string used during the expansion step. Omit this step for HKDF-Extract.
4. Call [`psa_key_derivation_output_bytes`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga06b7eb34a2fa88965f68e3d023fa12b9) to obtain the output of the derivation. You may call this function more than once to retrieve the output in successive chunks. Use [`psa_key_derivation_output_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gada7a6e17222ea9e7a6be6864a00316e1) instead if you want to use a chunk as a PSA key.
5. Call [`psa_key_derivation_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga90fdd2716124d0bd258826184824675f) to free the resources associated with the key derivation object.

### PKCS#5 module

Applications currently using `mbedtls_pkcs5_pbkdf2_hmac` or `mbedtls_pkcs5_pbkdf2_hmac_ext` can switch to the PSA key derivation API for PBKDF2. This is a generic interface using an operation object with one function call for each input and one function call for each output.

1. Create an operation object of type [`psa_key_derivation_operation_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga5f099b63799a0959c3d46718c86c2609) and zero-initialize it (or use the corresponding `INIT` macro).
2. Call [`psa_key_derivation_setup`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gac0b6a76e45cceb1862752bf041701859) to select the algorithm, which is a value of type [`psa_algorithm_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gac2e4d47f1300d73c2f829a6d99252d69). For PBKDF2-HMAC, select `PSA_ALG_PBKDF2_HMAC(hash)` where `hash` is the underlying hash algorithm (see “[Hash mechanism selection](#hash-mechanism-selection)”).
3. Call `psa_key_derivation_input_cost` with the step `PSA_KEY_DERIVATION_INPUT_COST` to select the iteration count.
4. Call [`psa_key_derivation_input_bytes`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga8fd934dfb0ca45cbf89542ef2a5494c2) on each of the inputs in the order listed below. (Use [`psa_key_derivation_input_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gab2d7ce8705dd8e4a093f4b8a21a0c15a) instead for an input that is a PSA key object.) The input step value for each step is as follows:
    1. [`PSA_KEY_DERIVATION_INPUT_SALT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__derivation/#group__derivation_1gab62757fb125243562c3947a752470d4a) for the salt used during the extraction step. You may repeat this step to pass the salt in pieces (for example a salt and a pepper).
    2. [`PSA_KEY_DERIVATION_INPUT_SECRET`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__derivation/#group__derivation_1ga0ddfbe764baba995c402b1b0ef59392e) for the password.
5. Call [`psa_key_derivation_output_bytes`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga06b7eb34a2fa88965f68e3d023fa12b9) to obtain the output of the derivation. You may call this function more than once to retrieve the output in successive chunks.
  Use [`psa_key_derivation_output_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gada7a6e17222ea9e7a6be6864a00316e1) instead if you want to use a chunk as a PSA key.  
  If you want to verify the output against an expected value (for authentication, rather than to derive key material), call [`psa_key_derivation_verify_bytes`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gaf01520beb7ba932143ffe733b0795b08) or [`psa_key_derivation_verify_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gac041714e34a94742e8ee006ac7dfea5a) instead of `psa_key_derivation_output_bytes`. (Available since Mbed TLS 3.6.0.)
6. Call [`psa_key_derivation_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga90fdd2716124d0bd258826184824675f) to free the resources associated with the key derivation object.

The function `mbedtls_pkcs5_pbes2` was only intended as a support function to parse encrypted private keys in the PK module. It has no PSA equivalent.

### PKCS#12 module

The functions `mbedtls_pkcs12_derivation` and `mbedtls_pkcs12_pbe` were only intended as support functions to parse encrypted private keys in the PK module. They have no PSA equivalent.

## Random generation

### Random generation interface

The PSA subsystem has an internal random generator. As a consequence, you do not need to instantiate one manually, so most applications using PSA crypto do not need the interfaces from `entropy.h`, `ctr_drbg.h` and `hmac_drbg.h`. See the next sections for remaining use cases for [entropy](#entropy-sources) and [DRBG](#deterministic-pseudorandom-generation).

The PSA API uses its internal random generator to generate keys (`psa_generate_key`), nonces for encryption (`psa_cipher_generate_iv`, `psa_cipher_encrypt`, `psa_aead_generate_nonce`, `psa_aead_encrypt`, `psa_asymmetric_encrypt`), and other random material as needed. If you need random data for some other purposes, call [`psa_generate_random`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__random/#group__random_1ga1985eae417dfbccedf50d5fff54ea8c5) instead of `mbedtls_ctr_drbg_random` or `mbedtls_hmac_drbg_random`.

You will need to remove the Mbed TLS RNG boilerplate: calls to `mbedtls_entropy_init`, `mbedtls_ctr_drbg_init`, `mbedtls_ctr_drbg_seed`, `mbedtls_ctr_drbg_random`, `mbedtls_ctr_drbg_free` and `mbedtls_entropy_free` (or `hmac_drbg` equivalents of the `ctr_drbg` functions).

TF-PSA-Crypto 1.x and Mbed TLS 4.x functions do not take RNG callbacks. If your code includes calls to internal functions or third-party functions that take RNG callbacks as parameters, include the header file `<mbedtls/psa_util.h>` and use:

* [`mbedtls_psa_get_random`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/psa__util_8h/#_CPPv422mbedtls_psa_get_randomPvPh6size_t) as the `f_rng` argument;
* [`MBEDTLS_PSA_RANDOM_STATE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/psa__util_8h/#c.MBEDTLS_PSA_RANDOM_STATE) as the `p_rng` argument.

### Entropy sources

The PSA random generator always uses the entropy source(s) configured at compile time. TF-PSA-Crypto 1.0.0 made major changes to how entropy sources are configured, compared with Mbed TLS 3.x. In TF-PSA-Crypto 1.0.0, PSA uses exactly one of three possible entropy sources, depending on which compilation option is enabled:

* `MBEDTLS_PSA_BUILTIN_GET_ENTROPY` (default): built-in platform entropy sources (Linux, other Unix-like systems, Windows).
* `#MBEDTLS_PSA_DRIVER_GET_ENTROPY`: the user-provided callback `mbedtls_platform_get_entropy()`, querying a low-rate entropy source.
* `MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG`: the user-provided callback `mbedtls_psa_external_get_random()`, a high-rate cryptographic-quality random source.

For more details, see the [1.0/4.0 migration guide](1.0-migration-guide.md#entropy-configuration) and the documentation of these options.

### Deterministic pseudorandom generation

The PSA API does not currently have a dedicated interface for pseudorandom generation. The [key derivation interface](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/) can serve a similar purpose in some applications, but it does not offer CTR\_DRBG or HMAC\_DRBG.

We expect to add PSA APIs to DRBG mechanisms in a future version of TF-PSA-Crypto.

## Asymmetric cryptography

The PSA API supports the same algorithm families for asymmetric cryptography as the legacy API: RSA (see “[RSA mechanism selection](#rsa-mechanism-selection)”), elliptic curve cryptography (see “[ECC mechanism selection](#elliptic-curve-mechanism-selection)” and “[EC-JPAKE](#ec-jpake)”) and finite-field Diffie-Hellman (see “[Diffie-Hellman mechanism selection](#diffie-hellman-mechanism-selection)”).

### Key lifecycle for asymmetric cryptography

In the PSA API, keys are referenced by an identifier of type [`psa_key_id_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__types_8h/#_CPPv412psa_key_id_t).
(Some documentation references [`mbedtls_svc_key_id_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__types_8h/#_CPPv420mbedtls_svc_key_id_t); the two types are identical except when the library is configured for use in a multi-client cryptography service.)
The PSA key identifier tends to play the same role as an `mbedtls_pk_context`, `mbedtls_rsa_context` or `mbedtls_ecp_keypair` structure in the legacy API. However, there are major differences in the way the two APIs can be used to create keys or to obtain information about a key.

Here is an overview of the lifecycle of a PSA key object.

1. First define the attributes of the key by filling a [`psa_key_attributes_t` structure](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga0ec645e1fdafe59d591104451ebf5680). You need to set the following parameters:
    * Call [`psa_set_key_type`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga6857ef0ecb3fa844d4536939d9c64025) to set the key type to the desired `PSA_KEY_TYPE_xxx` value (see “[RSA mechanism selection](#rsa-mechanism-selection)”, “[Elliptic curve mechanism selection](#elliptic-curve-mechanism-selection)” and “[Diffie-Hellman mechanism selection](#diffie-hellman-mechanism-selection)”).
    * Call [`psa_set_key_bits`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaf61683ac87f87687a40262b5afbfa018) to set the key's conceptual size in bits. This is optional with `psa_import_key`, which determines the key size from the length of the key material.
    * Call [`psa_set_key_algorithm`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaeb8341ca52baa0279475ea3fd3bcdc98) to set the permitted algorithm to the desired `PSA_ALG_xxx` value (see “[RSA mechanism selection](#rsa-mechanism-selection)”, “[Elliptic curve mechanism selection](#elliptic-curve-mechanism-selection)” and “[Diffie-Hellman mechanism selection](#diffie-hellman-mechanism-selection)” as well as “[Public-key cryptography policies](#public-key-cryptography-policies)”).
    * Call [`psa_set_key_usage_flags`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga42a65b3c4522ce9b67ea5ea7720e17de) to enable the desired usage types (see “[Public-key cryptography policies](#public-key-cryptography-policies)”).
2. Call one of the key creation functions, passing the attributes defined in the previous step, to get an identifier of type [`psa_key_id_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__types_8h/#_CPPv412psa_key_id_t) to the key object.
    * Use [`psa_import_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga0336ea76bf30587ab204a8296462327b) to directly import key material.
    * If the key is randomly generated, use [`psa_generate_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__random/#group__random_1ga1985eae417dfbccedf50d5fff54ea8c5).
    * If the key is derived from other material (for example from a key exchange), use the [key derivation interface](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/) and create the key with [`psa_key_derivation_output_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gada7a6e17222ea9e7a6be6864a00316e1).
3. Call the functions in the following sections to perform operations on the key. The same key object can be used in multiple operations.
4. To free the resources used by the key object, call [`psa_destroy_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__management/#group__key__management_1ga5f52644312291335682fbc0292c43cd2) after all operations with that key are finished.

### Public-key cryptography policies

A key's policy indicates what algorithm(s) it can be used with (usage algorithm policy) and what operations are permitted (usage flags).

The following table lists the relevant usage flags for asymmetric cryptography. You can pass those flags (combined with bitwise-or) to [`psa_set_key_usage_flags`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga42a65b3c4522ce9b67ea5ea7720e17de).

| Usage | Flag |
| ----- | ---- |
| export public key | 0 (always permitted) |
| export private key | [`PSA_KEY_USAGE_EXPORT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1ga7dddccdd1303176e87a4d20c87b589ed) |
| Sign a message directly | [`PSA_KEY_USAGE_SIGN_MESSAGE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1ga552117ac92b79500cae87d4e65a85c54) |
| Sign an already-calculated hash | at least one of [`PSA_KEY_USAGE_SIGN_MESSAGE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1ga552117ac92b79500cae87d4e65a85c54) or [`PSA_KEY_USAGE_SIGN_HASH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1ga552117ac92b79500cae87d4e65a85c54) |
| Verify a message directly | [`PSA_KEY_USAGE_VERIFY_MESSAGE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1gabea7ec4173f4f943110329ac2953b2b1) |
| Verify an already-calculated hash | at least one of [`PSA_KEY_USAGE_VERIFY_MESSAGE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1gabea7ec4173f4f943110329ac2953b2b1) or [`PSA_KEY_USAGE_VERIFY_HASH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1gafadf131ef2182045e3483d03aadaa1bd) |
| Encryption | [`PSA_KEY_USAGE_ENCRYPT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1ga75153b296d045d529d97203a6a995dad) |
| Decryption | [`PSA_KEY_USAGE_DECRYPT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1gac3f2d2e5983db1edde9f142ca9bf8e6a) |
| Key agreement | [`PSA_KEY_USAGE_DERIVE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1gaf19022acc5ef23cf12477f632b48a0b2) |

The sections “[RSA mechanism selection](#rsa-mechanism-selection)”, “[Elliptic curve mechanism selection](#elliptic-curve-mechanism-selection)” and “[Diffie-Hellman mechanism selection](#diffie-hellman-mechanism-selection)” cover the available algorithm values for each key type. Normally, a key can only be used with a single algorithm, following standard good practice. However, there are two ways to relax this requirement.

* Many signature algorithms encode a hash algorithm. Sometimes the same key may need to be used to sign messages with multiple different hashes. In an algorithm policy, you can use [`PSA_ALG_ANY_HASH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__values_8h/#c.PSA_ALG_ANY_HASH) instead of a hash algorithm value to allow the key to be used with any hash. For example, `psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PSS(PSA_ALG_ANY_HASH))` allows the key to be used with RSASSA-PSS, with different hash algorithms in each operation.
* In addition to the algorithm (or wildcard) selected with [`psa_set_key_algorithm`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaeb8341ca52baa0279475ea3fd3bcdc98), you can use [`psa_set_key_enrollment_algorithm`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaffa134b74aa52aa3ed9397fcab4005aa) to permit a second algorithm (or wildcard). This is intended for scenarios where a key is normally used with a single algorithm, but needs to be used with a different algorithm for enrollment (such as an ECDH key for which an ECDSA proof-of-possession is also required).

### Asymmetric cryptographic mechanisms

#### RSA mechanism selection

The former PK types `MBEDTLS_PK_RSA`, `MBEDTLS_PK_RSASSA_PSS` and `MBEDTLS_PK_RSA_ALT` correspond to RSA key types in the PSA API. In the PSA API, key pairs and public keys are separate object types.
See “[RSA-ALT interface](#rsa-alt-interface)” for more information about `MBEDTLS_PK_RSA_ALT`.

The PSA API uses policies and algorithm parameters rather than key types to distinguish between RSA-based mechanisms. The PSA algorithm selection corresponds to the `mbedtls_pk_sigalg_type_t` (formerly `mbedtls_pk_type_t`) value passed to `mbedtls_pk_{sign,verify}_ext`. It also replaces the use of `mbedtls_rsa_set_padding` on an `mbedtls_rsa_context` object. See the list of algorithms below and the signature and encryption sections for more information.

An RSA public key has the type [`PSA_KEY_TYPE_RSA_PUBLIC_KEY`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga9ba0878f56c8bcd1995ac017a74f513b).

An RSA key pair has the type [`PSA_KEY_TYPE_RSA_KEY_PAIR`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga11745b110166e927e2abeabc7d532051). A key with this type can be used both for private-key and public-key operations (there is no separate key type for a private key without the corresponding public key).
You can always use a private key for operations on the corresponding public key (as long as the policy permits it).

The following cryptographic algorithms work with RSA keys:

* PKCS#1v1.5 RSA signature: [`PSA_ALG_RSA_PKCS1V15_SIGN`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga702ff75385a6ae7d4247033f479439af), [`PSA_ALG_RSA_PKCS1V15_SIGN_RAW`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga4215e2a78dcf834e9a625927faa2a817).
* PKCS#1v1.5 RSA encryption: [`PSA_ALG_RSA_PKCS1V15_CRYPT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga4c540d3abe43fb9abcb94f2bc51acef9).
* PKCS#1 RSASSA-PSS signature: [`PSA_ALG_RSA_PSS`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga62152bf4cb4bf6aace5e1be8f143564d), [`PSA_ALG_RSA_PSS_ANY_SALT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga9b7355a2cd6bde88177634d539127f2b).
* PKCS#1 RSAES-OAEP encryption: [`PSA_ALG_RSA_OAEP`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gaa1235dc3fdd9839c6c1b1a9857344c76).

#### Elliptic curve mechanism selection

The former PK types `MBEDTLS_PK_ECKEY`, `MBEDTLS_PK_ECKEY_DH` and `MBEDTLS_PK_ECDSA` correspond to elliptic-curve key types in the PSA API. In the PSA API, key pairs and public keys are separate object types. The PSA API uses policies and algorithm parameters rather than key types to distinguish between the PK EC types.

An ECC public key has the type [`PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gad54c03d3b47020e571a72cd01d978cf2) where `curve` is a curve family identifier.

An ECC key pair has the type [`PSA_KEY_TYPE_ECC_KEY_PAIR(curve)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga0b6f5d4d5037c54ffa850d8059c32df0) where `curve` is a curve family identifier. A key with this type can be used both for private-key and public-key operations (there is no separate key type for a private key without the corresponding public key).
You can always use a private key for operations on the corresponding public key (as long as the policy permits it).

A curve is fully determined by a curve family identifier and the private key size in bits. The following table gives the correspondence between legacy and PSA elliptic curve designations (when TF-PSA-Crypto 1.0 implements the given curve).

| Mbed TLS legacy curve identifier | PSA curve family | Curve bit-size |
| -------------------------------- | ---------------- | -------------- |
| `MBEDTLS_ECP_DP_SECP192R1` | not supported | N/A |
| `MBEDTLS_ECP_DP_SECP224R1` | not supported | N/A |
| `MBEDTLS_ECP_DP_SECP256R1` | [`PSA_ECC_FAMILY_SECP_R1`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga48bb340b5544ba617b0f5b89542665a7) | 256 |
| `MBEDTLS_ECP_DP_SECP384R1` | [`PSA_ECC_FAMILY_SECP_R1`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga48bb340b5544ba617b0f5b89542665a7) | 384 |
| `MBEDTLS_ECP_DP_SECP521R1` | [`PSA_ECC_FAMILY_SECP_R1`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga48bb340b5544ba617b0f5b89542665a7) | 521 |
| `MBEDTLS_ECP_DP_BP256R1` | [`PSA_ECC_FAMILY_BRAINPOOL_P_R1`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gac1643f1baf38b30d07c20a6eac697f15) | 256 |
| `MBEDTLS_ECP_DP_BP384R1` | [`PSA_ECC_FAMILY_BRAINPOOL_P_R1`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gac1643f1baf38b30d07c20a6eac697f15) | 384 |
| `MBEDTLS_ECP_DP_BP512R1` | [`PSA_ECC_FAMILY_BRAINPOOL_P_R1`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gac1643f1baf38b30d07c20a6eac697f15) | 512 |
| `MBEDTLS_ECP_DP_CURVE25519` | [`PSA_ECC_FAMILY_MONTGOMERY`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga1f624c5cdaf25b21287af33024e1aff8) | 255 |
| `MBEDTLS_ECP_DP_SECP192K1` | not supported | N/A |
| `MBEDTLS_ECP_DP_SECP224K1` | not supported | N/A |
| `MBEDTLS_ECP_DP_SECP256K1` | [`PSA_ECC_FAMILY_SECP_K1`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga48bb340b5544ba617b0f5b89542665a7) | 256 |
| `MBEDTLS_ECP_DP_CURVE448` | [`PSA_ECC_FAMILY_MONTGOMERY`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga1f624c5cdaf25b21287af33024e1aff8) | 448 |

The following cryptographic algorithms work with ECC keys:

* ECDH key agreement (including X25519 and X448): [`PSA_ALG_ECDH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gab2dbcf71b63785e7dd7b54a100edee43).
* ECDSA: [`PSA_ALG_ECDSA`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga7e3ce9f514a227d5ba5d8318870452e3), [`PSA_ALG_ECDSA_ANY`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga51d6b6044a62e33cae0cf64bfc3b22a4), [`PSA_ALG_DETERMINISTIC_ECDSA`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga11da566bcd341661c8de921e2ca5ed03).
* EC-JPAKE (see “[EC-JPAKE](#ec-jpake)”.

#### Diffie-Hellman mechanism selection

A finite-field Diffie-Hellman key pair has the type [`PSA_KEY_TYPE_DH_KEY_PAIR(group)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gab4f857c4cd56f5fe65ded421e61bcc8c) where `group` is a group family as explained below.

A finite-field Diffie-Hellman public key has the type [`PSA_KEY_TYPE_DH_PUBLIC_KEY(group)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gaa22f0f2ea89b929f2fadc19890cc5d5c) where `group` is a group family as explained below. Due to the design of the API, there is rarely a need to use Diffie-Hellman public key objects.

The PSA API only supports Diffie-Hellman with predefined groups. A group is fully determined by a group family identifier and the public key size in bits.

| Mbed TLS DH group P value | PSA DH group family | Bit-size |
| ------------------------- | ------------------- | -------- |
| `MBEDTLS_DHM_RFC7919_FFDHE2048_P_BIN` | [`PSA_DH_FAMILY_RFC7919`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga7be917e67fe4a567fb36864035822ff7) | 2048 |
| `MBEDTLS_DHM_RFC7919_FFDHE3072_P_BIN` | [`PSA_DH_FAMILY_RFC7919`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga7be917e67fe4a567fb36864035822ff7) | 3072 |
| `MBEDTLS_DHM_RFC7919_FFDHE4096_P_BIN` | [`PSA_DH_FAMILY_RFC7919`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga7be917e67fe4a567fb36864035822ff7) | 4096 |
| `MBEDTLS_DHM_RFC7919_FFDHE6144_P_BIN` | [`PSA_DH_FAMILY_RFC7919`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga7be917e67fe4a567fb36864035822ff7) | 6144 |
| `MBEDTLS_DHM_RFC7919_FFDHE8192_P_BIN` | [`PSA_DH_FAMILY_RFC7919`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga7be917e67fe4a567fb36864035822ff7) | 8192 |

A finite-field Diffie-Hellman key can be used for key agreement with the algorithm [`PSA_ALG_FFDH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga0ebbb6f93a05b6511e6f108ffd2d1eb4).

### Creating keys for asymmetric cryptography

To generate a random key pair, call [`psa_generate_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__random/#group__random_1ga1985eae417dfbccedf50d5fff54ea8c5). Compared with the low-level functions from the legacy API (`mbedtls_rsa_gen_key`, `mbedtls_ecp_gen_privkey`, `mbedtls_ecp_gen_keypair`, `mbedtls_ecp_gen_keypair_base`, `mbedtls_ecdsa_genkey`), this directly creates an object that can be used with high-level APIs, but removes some of the flexibility. Note that if you want to export the generated private key, you must pass the flag [`PSA_KEY_USAGE_EXPORT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#group__policy_1ga7dddccdd1303176e87a4d20c87b589ed) to [`psa_set_key_usage_flags`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga42a65b3c4522ce9b67ea5ea7720e17de). Exporting the public key with [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062) is always permitted.

For RSA keys, `psa_generate_key` uses 65537 as the public exponent. You can use [`psa_generate_key_custom`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__random/#ga0415617443afe42a712027bbb8ad89f0) to select a different public exponent. As of TF-PSA-Crypto 1.0.0, selecting a different public exponent is only supported with the built-in RSA implementation, not with PSA drivers.

To create a key object from existing material, use [`psa_import_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga0336ea76bf30587ab204a8296462327b). This function has the same basic goal as the PK parse functions (`mbedtls_pk_parse_key`, `mbedtls_pk_parse_public_key`, `mbedtls_pk_parse_subpubkey`), but only supports a single format that just contains the number(s) that make up the key, with very little metadata. The table below summarizes the PSA import/export format for key pairs and public keys; see the documentation of [`psa_export_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga668e35be8d2852ad3feeef74ac6f75bf) and [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062) for more details.

| Key type | PSA import/export format |
| -------- | ------------------------ |
| RSA key pair | PKCS#1 RSAPrivateKey DER encoding (including both private exponent and CRT parameters) |
| RSA public key | PKCS#1 RSAPublicKey DER encoding |
| ECC key pair | Fixed-length private value (not containing the public key) |
| ECC public key (Weierstrass curve) | Fixed-length uncompressed point |
| ECC public key (Montgomery curve) | Fixed-length public value |
| FFDH key pair | Fixed-length private value (not containing the public key) |
| FFDH public key | Fixed-length public value |

There is no equivalent of `mbedtls_pk_parse_keyfile` and `mbedtls_pk_parse_public_keyfile`. Either call the legacy function or load the file data manually.

A future extension of the PSA API will support other import formats. Until those are implemented, see the following subsection for how to use the PK module for key parsing and construct a PSA key object from the PK object.

### Creating a PSA key via PK

You can use the PK module as an intermediate step to create an RSA or ECC key for use with PSA. This is useful for parsing a key in a format that the PK module supports, but `psa_import_key` doesn't. An advantage of the formats supported by PK is that they contain enough metadata to determine the key type, thus PK's parsing functions do not take a key type as input.

To construct a PSA key by parsing it with a PK function:

1. First create a PK object with the desired key material, using `mbedtls_pk_parse_key()` or `mbedtls_pk_parse_keyfile()`, `mbedtls_pk_parse_public_key()` or `mbedtls_pk_parse_public_keyfile()`.
2. Call [`mbedtls_pk_get_psa_attributes`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/pk_8h/#pk_8h_1a7aa7b33cffb6981d95d1632631de9244) to fill PSA attributes corresponding to the PK key. Pass one of the following values as the `usage` parameter:
    * `PSA_KEY_USAGE_SIGN_HASH` or `PSA_KEY_USAGE_SIGN_MESSAGE` for a key pair used for signing.
    * `PSA_KEY_USAGE_DECRYPT` for a key pair used for decryption.
    * `PSA_KEY_USAGE_DERIVE` for a key pair used for key agreement.
    * `PSA_KEY_USAGE_VERIFY_HASH` or `PSA_KEY_USAGE_VERIFY_MESSAGE` for a public key pair used for signature verification.
    * `PSA_KEY_USAGE_ENCRYPT` for a key pair used for encryption.
3. Optionally, tweak the attributes (this is rarely necessary). For example:
    * Call [`psa_set_key_usage_flags`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga42a65b3c4522ce9b67ea5ea7720e17de), [`psa_set_key_algorithm`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaeb8341ca52baa0279475ea3fd3bcdc98) and/or [`psa_set_key_enrollment_algorithm`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__extra_8h/#group__attributes_1gaffa134b74aa52aa3ed9397fcab4005aa) to change the key's policy (by default, it allows what can be done through the PK module).
    · Call [`psa_set_key_id`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gae48fcfdc72a23e7499957d7f54ff5a64) and perhaps [`psa_set_key_lifetime`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gac03ccf09ca6d36cc3d5b43f8303db6f7) if you wish to create a PSA persistent key.
4. Call [`mbedtls_pk_import_into_psa`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/pk_8h/#pk_8h_1ad59835d14832daf0f4b4bd0a4555abb9) to import the key into the PSA key store.
5. You can now free the PK object with `mbedtls_pk_free`.

Here is some sample code illustrating the above process, with error checking omitted.

```
mbedtls_pk_context pk;
mbedtls_pk_init(&pk);
mbedtls_pk_parse_key(&pk, key_buffer, key_buffer_length, NULL, 0,
                     mbedtls_psa_get_random, MBEDTLS_PSA_RANDOM_STATE);
psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
mbedtls_pk_get_psa_attributes(&pk, PSA_KEY_USAGE_SIGN_HASH, &attributes);
psa_key_id_t key_id;
mbedtls_pk_import_into_psa(&pk, &attributes, &key_id);
mbedtls_pk_free(&pk);
psa_sign_hash(key_id, ...);
```

### Key pair and public key metadata

There is no equivalent to the type `mbedtls_pk_info_t` and the functions `mbedtls_pk_info_from_type` in the PSA API because it is unnecessary. All macros and functions operate directly on key type values (`psa_key_type_t`, `PSA_KEY_TYPE_xxx` constants) and algorithm values (`psa_algorithm_t`, `PSA_ALG_xxx` constants).

You can call [`psa_get_key_attributes`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gacbbf5c11eac6cd70c87ffb936e1b9be2) to populate a structure with the attributes of a key, then functions such as [`psa_get_key_type`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gae4fb812af4f57aa1ad85e335a865b918) and [`psa_get_key_bits`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga5bee85c2164ad3d4c0d42501241eeb06) to obtain a key's type (`PSA_KEY_TYPE_xxx` value) and size (nominal size in bits).

The bit-size from `psa_get_key_bits` is the same as the one from `mbedtls_pk_get_bitlen`. To convert to bytes as `mbedtls_pk_get_len` or `mbedtls_rsa_get_len` do, you can use the macro `PSA_BITS_TO_BYTES`. However, note that the PSA API has generic macros for each related buffer size (export, signature size, etc.), so you should generally use those instead. The present document lists those macros where it explains the usage of the corresponding function.

Most code that calls `mbedtls_pk_get_type` or `mbedtls_pk_can_do` only requires the key's type as reported by [`psa_get_key_type`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gae4fb812af4f57aa1ad85e335a865b918). If you need more information, you can call `mbedtls_pk_can_do_psa()`, which checks the compatibility between a key object and a mechanism. This function is new in TF-PSA-Crypto 1.0, and generalizes the function [`mbedtls_pk_can_do_ext`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/pk_8h/#pk_8h_1a256d3e8d4323a45aafa7d2b6c59a36f6) from Mbed TLS 3.x. If needed, you can also access a key's policy from its attributes with [`psa_get_key_usage_flags`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaa1af20f142ca722222c6d98678a0c448), [`psa_get_key_algorithm`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gac255da850a00bbed925390044f016b34) and [`psa_get_key_enrollment_algorithm`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga39803b62a97198cf630854db9b53c588). The algorithm policy also conveys the padding and hash information provided by the legacy functions `mbedtls_rsa_get_padding_mode` and `mbedtls_rsa_get_md_alg`.

### Exporting a public key or a key pair

To export a PSA key pair or public key, call [`psa_export_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga668e35be8d2852ad3feeef74ac6f75bf). If the key is a key pair, its policy must allow `PSA_KEY_USAGE_EXPORT` (see “[Public-key cryptography policies](#public-key-cryptography-policies)”).

To export a PSA public key or to export the public key of a PSA key pair object, call [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062). This is always permitted regardless of the key's policy.

The export format is the same format used for `psa_import_key`, described in “[Creating keys for asymmetric cryptography](#creating-keys-for-asymmetric-cryptography)” above.

A future extension of the PSA API will support other export formats. Until those are implemented, see “[Exposing a PSA key via PK](#exposing-a-psa-key-via-pk)” for ways to use the PK module to format a PSA key.

#### Exposing a PSA key via PK

This section discusses how to use a PSA key in a context that requires a PK object, such as PK formatting functions (`mbedtls_pk_write_key_der`, `mbedtls_pk_write_pubkey_der`, `mbedtls_pk_write_pubkey_pem`, `mbedtls_pk_write_key_pem` or `mbedtls_pk_write_pubkey`), Mbed TLS X.509 functions, Mbed TLS SSL functions, or another API that involves `mbedtls_pk_context` objects. The PSA key must be an RSA or ECC key since the PK module does not support DH keys. Three functions from `pk.h` help with that:

* [`mbedtls_pk_copy_from_psa`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/pk_8h/#pk_8h_1ab8e88836fd9ee344ffe630c40447bd08) copies a PSA key into a PK object. The PSA key must be exportable. The PK object remains valid even if the PSA key is destroyed.
* [`mbedtls_pk_copy_public_from_psa`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/pk_8h/#pk_8h_1a2a50247a528889c12ea0ddddb8b15a4e) copies the public part of a PSA key into a PK object. The PK object remains valid even if the PSA key is destroyed.
* `mbedtls_pk_wrap_psa` (new under this name in TF-PSA-Crypto 1.0, corresponding to [`mbedtls_pk_setup_opaque`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/pk_8h/#pk_8h_1a4c04ac22ab9c1ae09cc29438c308bf05) in Mbed TLS 3.x) sets up a PK object that wraps the PSA key. This PK object can only be used as permitted by the PSA key's policy. The PK object contains a reference to the PSA key identifier, therefore the PSA key must not be destroyed as long as the PK object remains alive.

Here is some sample code illustrating how to use the PK module to format a PSA public key or the public key of a PSA key pair.
```
int write_psa_pubkey(psa_key_id_t key_id,
                     unsigned char *buf, size_t size, size_t *len) {
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);
    int ret = mbedtls_pk_copy_public_from_psa(key_id, &pk);
    if (ret != 0) goto exit;
    ret = mbedtls_pk_write_pubkey_der(&pk, buf, size);
    if (ret < 0) goto exit;
    *len = ret;
    memmove(buf, buf + size - ret, ret);
    ret = 0;
exit:
    mbedtls_pk_free(&pk);
}
```

#### Restartable public key export

Code that uses restartable ECC (see “[Restartable ECDSA signature](#restartable-ecdsa-signature)) may require an interruptible way to calculate the public key from an object containing only the private key. The legacy API did not provide an explicit way to do it, but this could be done by invoking `mbedtls_ecp_mul_restart()` to do the computation manually.

As of TF-PSA-Crypto 1.0, PSA ECC key pair objects in fact only contain the private key, and  `psa_export_public_key()` calculates the public key on each call. If needed, you can use the interruptible variant of this function, first released in TF-PSA-Crypto 1.0.

1. Call [`psa_export_public_key_iop_setup()`](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/include/psa/crypto.h) to start the interruptible key agreement operation.
2. Call [`psa_export_public_key_iop_complete()`](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/include/psa/crypto.h) repeatedly until it returns a status other than `PSA_OPERATION_INCOMPLETE`.

If you need to cancel the operation after calling the start function without waiting for the loop calling the complete function to finish, call `psa_export_public_key_iop_abort()`.

Call [`psa_interruptible_set_max_ops`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga6d86790b31657c13705214f373af869e) to set the number of basic operations per call. This is the same unit as `mbedtls_ecp_set_max_ops`. You can retrieve the current value with [`psa_interruptible_get_max_ops`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga73e66a6d93f2690b626fcea20ada62b2). The value is [`PSA_INTERRUPTIBLE_MAX_OPS_UNLIMITED`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible/#group__interruptible_1gad19c1da7f6b7d59d5873d5b68eb943d4) if operations are not restartable, which corresponds to `mbedtls_ecp_restart_is_enabled()` being false.


### Signature operations

The equivalent of `mbedtls_pk_sign` or `mbedtls_pk_sign_ext` to sign an already calculated hash is [`psa_sign_hash`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__asymmetric/#group__asymmetric_1ga785e746a31a7b2a35ae5175c5ace3c5c).
The key must be a key pair allowing the usage `PSA_KEY_USAGE_SIGN_HASH` (see “[Public-key cryptography policies](#public-key-cryptography-policies)”).
Use [`PSA_SIGN_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_SIGN_OUTPUT_SIZE) or [`PSA_SIGNATURE_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_SIGNATURE_MAX_SIZE) (similar to `MBEDTLS_PK_SIGNATURE_MAX_SIZE`) to determine a sufficient size for the output buffer.
This is also the equivalent of the type-specific functions `mbedtls_rsa_pkcs1_sign`, `mbedtls_rsa_rsassa_pkcs1_v15_sign`, `mbedtls_rsa_rsassa_pss_sign`, `mbedtls_rsa_rsassa_pss_sign_ext`, `mbedtls_ecdsa_sign`, `mbedtls_ecdsa_sign_det_ext` and `mbedtls_ecdsa_write_signature`. Note that the PSA API uses the raw format for ECDSA signatures, not the ASN.1 format; see “[ECDSA signature](#ecdsa-signature)” for more details.

The equivalent of `mbedtls_pk_verify` or `mbedtls_pk_verify_ext` to verify an already calculated hash is [`psa_verify_hash`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__asymmetric/#group__asymmetric_1gae2ffbf01e5266391aff22b101a49f5f5).
The key must be a public key (or a key pair) allowing the usage `PSA_KEY_USAGE_VERIFY_HASH` (see “[Public-key cryptography policies](#public-key-cryptography-policies)”).
This is also the equivalent of the type-specific functions `mbedtls_rsa_pkcs1_verify`, `mbedtls_rsa_rsassa_pkcs1_v15_verify`, `mbedtls_rsa_rsassa_pss_verify`, `mbedtls_rsa_rsassa_pss_verify_ext`, `mbedtls_ecdsa_verify` and `mbedtls_ecdsa_read_signature`. Note that the PSA API uses the raw format for ECDSA signatures, not the ASN.1 format; see “[ECDSA signature](#ecdsa-signature)” for more details.

Generally, `psa_sign_hash` and `psa_verify_hash` require the input to have the correct length for the hash (this has historically not always been enforced in the corresponding legacy APIs).

See also “[Restartable ECDSA signature](#restartable-ecdsa-signature)” for a restartable variant of this API.

The PSA API also has functions [`psa_sign_message`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__asymmetric/#group__asymmetric_1ga963ecadae9c38c85826f9a13cf1529b9) and [`psa_verify_message`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__asymmetric/#group__asymmetric_1ga01c11f480b185a4268bebd013df7c14c). These functions combine the hash calculation with the signature calculation or verification.
For `psa_sign_message`, either the usage flag `PSA_KEY_USAGE_SIGN_MESSAGE` or `PSA_KEY_USAGE_SIGN_HASH` is sufficient.
For `psa_verify_message`, either the usage flag `PSA_KEY_USAGE_VERIFY_MESSAGE` or `PSA_KEY_USAGE_VERIFY_HASH` is sufficient.

Most signature algorithms involve a hash algorithm. See “[Hash mechanism selection](#hash-mechanism-selection)”.

The following subsections describe the PSA signature mechanisms that correspond to legacy Mbed TLS mechanisms.

#### ECDSA signature

**Note: in the PSA API, the format of an ECDSA signature is the raw fixed-size format. This is different from the legacy API** which uses the ASN.1 DER format for ECDSA signatures. To convert between the two formats, use [`mbedtls_ecdsa_raw_to_der`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/psa__util_8h/#group__psa__tls__helpers_1ga9295799b5437bdff8ce8abd524c5ef2e) or [`mbedtls_ecdsa_der_to_raw`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/psa__util_8h/#group__psa__tls__helpers_1ga33b3cf65d5992ccc724b7ee00186ae61) from `<mbedtls/psa_util.h>`.

<!-- The following are specific to the DER format and therefore have no PSA equivalent: MBEDTLS_ECDSA_MAX_SIG_LEN, MBEDTLS_ECDSA_MAX_LEN -->

ECDSA is the mechanism provided by `mbedtls_pk_sign` and `mbedtls_pk_verify` for ECDSA keys, as well as by `mbedtls_ecdsa_sign`, `mbedtls_ecdsa_sign_det_ext`, `mbedtls_ecdsa_write_signature`, `mbedtls_ecdsa_verify` and `mbedtls_ecdsa_read_signature`.

The PSA API offers three algorithm constructors for ECDSA. They differ only for signature, and have exactly the same behavior for verification.

* [`PSA_ALG_ECDSA(hash)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga7e3ce9f514a227d5ba5d8318870452e3) is a randomized ECDSA signature of a hash calculated with the algorithm `hash`.
* [`PSA_ALG_ECDSA_ANY`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga51d6b6044a62e33cae0cf64bfc3b22a4) is equivalent to `PSA_ALG_ECDSA`, but does not require specifying a hash as part of the algorithm. It can only be used with `psa_sign_hash` and `psa_verify_hash`, with no constraint on the length of the hash.
* [`PSA_ALG_DETERMINISTIC_ECDSA(hash)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga11da566bcd341661c8de921e2ca5ed03) is a deterministic ECDSA signature of a hash calculated with the algorithm `hash`. This is the same as the functionality offered by `MBEDTLS_ECDSA_DETERMINISTIC` in the legacy API.
    * For `psa_sign_message` with `PSA_ALG_DETERMINISTIC_ECDSA`, the same hash algorithm is used to hash the message and to parametrize the deterministic signature generation.

Unlike the legacy API, where `mbedtls_pk_sign` and `mbedtls_ecdsa_write_signature` automatically select deterministic ECDSA if both are available, the PSA API requires the application to select the preferred variant. ECDSA verification cannot distinguish between randomized and deterministic ECDSA (except in so far as if the same message is signed twice and the signatures are different, then at least one of the signatures is not the determinstic variant), so in most cases switching between the two is a compatible change.

Since TF-PSA-Crypto 1.0, the PK module provides the macro `MBEDTLS_PK_ALG_ECDSA(hash)`, which is equivalent to either `PSA_ALG_ECDSA(hash)` or `PSA_ALG_DETERMINISTIC_ECDSA(hash)`. This macro indicates which ECDSA variant the function `mbedtls_pk_sign()` uses (except when constrained by the key's policy).

#### Restartable ECDSA signature

The legacy API includes an API for “restartable” ECC operations: the operation returns after doing partial computation, and can be resumed. This is intended for highly constrained devices where long cryptographic calculations need to be broken up to poll some inputs, where interrupt-based scheduling is not desired. The legacy API for signature consists of the functions `mbedtls_pk_sign_restartable`, `mbedtls_pk_verify_restartable`, `mbedtls_ecdsa_sign_restartable`, `mbedtls_ecdsa_verify_restartable`, `mbedtls_ecdsa_write_signature_restartable`, `mbedtls_ecdsa_read_signature_restartable`, as well as several configuration and data manipulation functions.

The PSA API offers similar functionality via “interruptible” public-key operations. As of TF-PSA-Crypto 1.0, it is only implemented for ECDSA and ECDH (see “[Restartable key agreement](#restartable-key-agreement)”), for the same curves as the legacy API. (Mbed TLS 3.6 only has interruptible ECDSA signature, not ECDH key agrement.) At the time of writing, no extension is planned to other curves or other algorithms.

The flow of operations for an interruptible signature operation is as follows:

1. Create an operation object of type [`psa_sign_hash_interruptible_operation_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga6948d4653175b1b530a265540066a7e7) and zero-initialize it (or use the corresponding `INIT` macro).
2. Call [`psa_sign_hash_start`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga441988da830205182b3e791352537fac) with the private key object and the hash to verify.
3. Call [`psa_sign_hash_complete`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga79849aaa7004a85d2ffbc4b658a333dd) repeatedly until it returns a status other than `PSA_OPERATION_INCOMPLETE`.

The flow of operations for an interruptible signature verification operation is as follows:

1. Create an operation object of type [`psa_verify_hash_interruptible_operation_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga537054cf4909ad1426331ae4ce7148bb) and zero-initialize it (or use the corresponding `INIT` macro).
2. Call [`psa_verify_hash_start`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga912eb51fb94056858f451f276ee289cb) with the private key object and the hash and signature to verify.
3. Call [`psa_verify_hash_complete`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga67fe82352bc2f8c0343e231a70a5bc7d) repeatedly until it returns a status other than `PSA_OPERATION_INCOMPLETE`.

If you need to cancel the operation after calling the start function without waiting for the loop calling the complete function to finish, call [`psa_sign_hash_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1gae893a4813aa8e03bd201fe4f1bbbb403) or [`psa_verify_hash_abort`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga18dc9c0cc27d590c5e3b186094d90f88).

Call [`psa_interruptible_set_max_ops`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga6d86790b31657c13705214f373af869e) to set the number of basic operations per call. This is the same unit as `mbedtls_ecp_set_max_ops`. You can retrieve the current value with [`psa_interruptible_get_max_ops`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga73e66a6d93f2690b626fcea20ada62b2). The value is [`PSA_INTERRUPTIBLE_MAX_OPS_UNLIMITED`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible/#group__interruptible_1gad19c1da7f6b7d59d5873d5b68eb943d4) if operations are not restartable, which corresponds to `mbedtls_ecp_restart_is_enabled()` being false.

#### PKCS#1 v1.5 RSA signature

This mechanism corresponds to `mbedtls_pk_sign`, `mbedtls_pk_verify`, `mbedtls_rsa_pkcs1_sign` and `mbedtls_rsa_pkcs1_verify` for an RSA key, unless PSS has been selected with `mbedtls_rsa_set_padding` on the underlying RSA key context. This mechanism also corresponds to `mbedtls_rsa_rsassa_pkcs1_v15_sign` and `mbedtls_rsa_rsassa_pkcs1_v15_verify`.

The PSA API has two algorithm constructors:

* [`PSA_ALG_RSA_PKCS1V15_SIGN(hash)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga702ff75385a6ae7d4247033f479439af) formats the hash as specified in PKCS#1. The hash algorithm corresponds to the `md_alg` parameter of the legacy functions.
* [`PSA_ALG_RSA_PKCS1V15_SIGN_RAW`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga4215e2a78dcf834e9a625927faa2a817) uses the “hash” input in lieu of a DigestInfo structure. This is the same as calling the legacy functions with `md_alg=MBEDTLS_MD_NONE`.

#### PKCS#1 RSASSA-PSS signature

This mechanism corresponds to `mbedtls_pk_sign_ext` and `mbedtls_pk_verify_ext` for an RSA key, as well as `mbedtls_pk_sign`, `mbedtls_pk_verify`, `mbedtls_rsa_pkcs1_sign` and `mbedtls_rsa_pkcs1_verify` if PSS has been selected on the underlying RSA context with `mbedlts_rsa_set_padding`.
It also corresponds to `mbedtls_rsa_rsassa_pss_sign` and `mbedtls_rsa_rsassa_pss_sign_ext`, `mbedtls_rsa_rsassa_pss_verify` and `mbedtls_rsa_rsassa_pss_verify_ext`.

The PSA API has two algorithm constructors: [`PSA_ALG_RSA_PSS(hash)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga62152bf4cb4bf6aace5e1be8f143564d) and [`PSA_ALG_RSA_PSS_ANY_SALT(hash)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga9b7355a2cd6bde88177634d539127f2b). They differ only for verification, and have exactly the same behavior for signature. The hash algorithm `hash` corresponds to the `md_alg` parameter passed to the legacy API. It is used to hash the message, to create the salted hash, and for the mask generation with MGF1. The PSA API does not support using different hash algorithms for these different purposes.

With respect to the salt length:

* When signing, the salt is random, and the salt length is the largest possible salt length up to the hash length. This is the same as passing `MBEDTLS_RSA_SALT_LEN_ANY` as the salt length to `xxx_ext` legacy functions or using a legacy function that does not have a `saltlen` argument.
* When verifying, `PSA_ALG_RSA_PSS` requires the the salt length to the largest possible salt length up to the hash length (i.e. the same that would be used for signing).
* When verifying, `PSA_ALG_RSA_PSS_ANY_SALT` accepts any salt length. This is the same as passing `MBEDTLS_RSA_SALT_LEN_ANY` as the salt length to `xxx_ext` legacy functions or using a legacy function that does not have a `saltlen` argument.

### Asymmetric encryption and decryption

The equivalent of `mbedtls_pk_encrypt`, `mbedtls_rsa_pkcs1_encrypt`, `mbedtls_rsa_rsaes_pkcs1_v15_encrypt` or `mbedtls_rsa_rsaes_oaep_encrypt` to encrypt a short message (typically a symmetric key) is [`psa_asymmetric_encrypt`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__asymmetric/#group__asymmetric_1gaa17f61e4ddafd1823d2c834b3706c290).
The key must be a public key (or a key pair) allowing the usage `PSA_KEY_USAGE_ENCRYPT` (see “[Public-key cryptography policies](#public-key-cryptography-policies)”).
Use the macro [`PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#crypto__sizes_8h_1a66ba3bd93e5ec52870ccc3848778bad8) or [`PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE) to determine the output buffer size.

The equivalent of `mbedtls_pk_decrypt`, `mbedtls_rsa_pkcs1_decrypt`, `mbedtls_rsa_rsaes_pkcs1_v15_decrypt` or `mbedtls_rsa_rsaes_oaep_decrypt` to decrypt a short message (typically a symmetric key) is [`psa_asymmetric_decrypt`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__asymmetric/#group__asymmetric_1ga4f968756f6b22aab362b598b202d83d7).
The key must be a key pair allowing the usage `PSA_KEY_USAGE_DECRYPT` (see “[Public-key cryptography policies](#public-key-cryptography-policies)”).
Use the macro [`PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#crypto__sizes_8h_1a61a246f3eac41989821d982e56fea6c1) or [`PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE) to determine the output buffer size.

The following subsections describe the PSA asymmetric encryption mechanisms that correspond to legacy Mbed TLS mechanisms.

#### RSA PKCS#1v1.5 encryption

This is the mechanism used by the PK functions and by `mbedtls_rsa_pkcs1_{encrypt,decrypt}` unless `mbedtls_rsa_set_padding` has been called on the underlying RSA key context.
This is also the mechanism used by `mbedtls_rsa_rsaes_pkcs1_v15_{encrypt,decrypt}`.

The PSA algorithm is [`PSA_ALG_RSA_PKCS1V15_CRYPT`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga4c540d3abe43fb9abcb94f2bc51acef9).

Beware that PKCS#1v1.5 decryption is subject to padding oracle attacks. Revealing when `psa_asymmetric_decrypt` returns `PSA_ERROR_INVALID_PADDING` may allow an adversary to decrypt arbitrary ciphertexts.

#### RSA RSAES-OAEP

This is the mechanism used by `mbedtls_rsa_rsaes_oaep_{encrypt,decrypt}`.

The PSA algorithm is [`PSA_ALG_RSA_OAEP(hash)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gaa1235dc3fdd9839c6c1b1a9857344c76) where `hash` is a hash algorithm value (`PSA_ALG_xxx`, see “[Hash mechanism selection](#hash-mechanism-selection)”).

As with the PK API, the mask generation is MGF1, the label is empty, and the same hash algorithm is used for MGF1 and to hash the label. The PSA API does not offer a way to choose a different label or a different hash algorithm for the label.

### Private-public key consistency

There is no direct equivalent of the functions `mbedtls_rsa_check_privkey`, `mbedtls_rsa_check_pubkey`,`mbedtls_ecp_check_privkey`, `mbedtls_ecp_check_pubkey`. The PSA API performs some basic checks when it imports a key, and may perform additional checks before performing an operation if needed, so it will never perform an operation on a key that does not satisfy these checks, but the details of when the check is performed may change between versions of the library.

The legacy API provides the function `mbedtls_pk_check_pair`, which can be used to check the consistency between a private key and a public key. To perform such a check with the PSA API, you can export the public keys; this works because the PSA representation of public keys is canonical.

* Prepare a key object containing the private key, for example with [`psa_import_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga0336ea76bf30587ab204a8296462327b).
* Prepare a key object containing the public key, for example with [`psa_import_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga0336ea76bf30587ab204a8296462327b).
* Export both public keys with [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062) (this is possible regardless of the usage policies on the keys) and compare the output.
    ```
    // Error checking omitted
    unsigned char pub1[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
    unsigned char pub2[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
    size_t length1, length2;
    psa_export_public_key(key1, pub1, sizeof(pub1), &length1);
    psa_export_public_key(key2, pub2, sizeof(pub2), &length2);
    if (length1 == length2 && !memcmp(pub1, pub2, length1))
        puts("The keys match");
    else
        puts("The keys do not match");
    ```

### PK functionality with no PSA equivalent

There is no PSA equivalent of the debug functionality provided by `mbedtls_pk_debug`. Use `psa_export_key` to export the key if desired.

There is no PSA equivalent to Mbed TLS's custom key type names exposed by `mbedtls_pk_get_name`.

### Key agreement

The PSA API has a generic interface for key agreement, covering the main use of both `ecdh.h` and `dhm.h`.

<!-- TODO: static FFDH/ECDH (including `mbedtls_ecdh_get_params`)
 https://github.com/Mbed-TLS/mbedtls/pull/7766#discussion_r1410568541
 -->

#### Diffie-Hellman key pair management

The PSA API manipulates keys as such, rather than via an operation context. Thus, to use Diffie-Hellman, you need to create a key object, then perform the key exchange, then destroy the key. There is no equivalent to the types `mbedtls_ecdh_context` and `mbedtls_dhm_context`.

Here is an overview of the lifecycle of a key object.

1. First define the attributes of the key by filling a [`psa_key_attributes_t` structure](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga0ec645e1fdafe59d591104451ebf5680). You need to set the following parameters:
    * Call [`psa_set_key_type`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga6857ef0ecb3fa844d4536939d9c64025) to set the key type to the desired `PSA_KEY_TYPE_xxx` value:
        * [`PSA_KEY_TYPE_DH_KEY_PAIR(group)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gab4f857c4cd56f5fe65ded421e61bcc8c) for finite-field Diffie-Hellman (see “[Diffie-Hellman mechanism selection](#diffie-hellman-mechanism-selection)”).
    * [`PSA_KEY_TYPE_ECC_KEY_PAIR(curve)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga0b6f5d4d5037c54ffa850d8059c32df0) for elliptic-curve Diffie-Hellman (see “[Elliptic curve mechanism selection](#elliptic-curve-mechanism-selection)”).
    * Call [`psa_set_key_bits`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaf61683ac87f87687a40262b5afbfa018) to set the private key size in bits. This is optional with `psa_import_key`, which determines the key size from the length of the key material.
    * Call [`psa_set_key_algorithm`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gaeb8341ca52baa0279475ea3fd3bcdc98) to select the appropriate algorithm:
    * [`PSA_ALG_ECDH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1gab2dbcf71b63785e7dd7b54a100edee43) or [`PSA_ALG_FFDH`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga0ebbb6f93a05b6511e6f108ffd2d1eb4) for a raw key agreement.
    * [`PSA_ALG_KEY_AGREEMENT(ka, kdf)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__crypto__types/#group__crypto__types_1ga78bb81cffb87a635c247725eeb2a2682) if the key will be used as part of a key derivation, where:
        * `ka` is either `PSA_ALG_ECDH` or `PSA_ALG_FFDH`.
        * `kdf` is a key derivation algorithm.
    * Call [`psa_set_key_usage_flags`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga42a65b3c4522ce9b67ea5ea7720e17de) to enable at least [`PSA_KEY_USAGE_DERIVE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__policy/#c.PSA_KEY_USAGE_DERIVE). See “[Public-key cryptography policies](#public-key-cryptography-policies)” for more information.
2. Call one of the key creation functions, passing the attributes defined in the previous step, to get an identifier of type [`psa_key_id_t`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__types_8h/#_CPPv412psa_key_id_t) to the key object.
    * Use [`psa_generate_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__random/#group__random_1ga1985eae417dfbccedf50d5fff54ea8c5) to generate a random key. This is normally the case for a Diffie-Hellman key.
    * Use [`psa_import_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga0336ea76bf30587ab204a8296462327b) to directly import key material.
    * If the key is derived deterministically from other material, use the [key derivation interface](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/) and create the key with [`psa_key_derivation_output_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1gada7a6e17222ea9e7a6be6864a00316e1).
3. Call the functions in the following sections to perform operations on the key. The same key object can be used in multiple operations.
4. To free the resources used by the key object, call [`psa_destroy_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__management/#group__key__management_1ga5f52644312291335682fbc0292c43cd2) after all operations with that key are finished.

#### Performing a key agreement

Call [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062) to obtain the public key that needs to be sent to the other party.
Use the macros [`PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE) or [`PSA_EXPORT_PUBLIC_KEY_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_EXPORT_PUBLIC_KEY_MAX_SIZE) to determine a sufficient size for the output buffer.

Call [`psa_raw_key_agreement`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga90fdd2716124d0bd258826184824675f) to calculate the shared secret from your private key and the other party's public key.
Use the macros [`PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE) or [`PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE) to determine a sufficient size for the output buffer.

Call [`psa_key_derivation_key_agreement`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga2cd5a8ac906747d3204ec442db78745f) instead of `psa_raw_key_agreement` to use the resulting shared secret as the secret input to a key derivation. See “[HKDF](#hkdf)” for an example of the key derivation interface.

#### Translating a legacy key agreement contextless workflow

A typical workflow for ECDH using the legacy API without a context object is:

1. Initialize objects:
    * `mbedtls_ecp_group grp` for the curve;
    * `mbedtls_mpi our_priv` for our private key;
    * `mbedtls_ecp_point our_pub` for our public key;
    * `mbedtls_ecp_point their_pub` for their public key (this may be the same variable as `our_pub` if the application does not need to hold both at the same time);
    * `mbedtls_mpi z` for the shared secret (this may be the same variable as `our_priv` when doing ephemeral ECDH).
2. Call `mbedtls_ecp_group_load` on `grp` to select the curve.
3. Call `mbedtls_ecdh_gen_public` on `grp`, `our_priv` (output) and `our_pub` (output) to generate a key pair and retrieve the corresponding public key.
4. Send `our_pub` to the peer. Retrieve the peer's public key and import it into `their_pub`. These two actions may be performed in either order.
5. Call `mbedtls_ecdh_compute_shared` on `grp`, `z` (output), `their_pub` and `our_priv`. Use the raw shared secret `z`, typically, to construct a shared key.
6. Free `grp`, `our_priv`, `our_pub`, `their_pub` and `z`.

The corresponding workflow with the PSA API is as follows:

1. Initialize objects:
    * `psa_key_id_t our_key`: a handle to our key pair;
    * `psa_key_attributes_t attributes`: key attributes used in steps 2–3;;
    * `our_pub`: a buffer of size [`PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, bits)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE) (where `key_type` is the value passed to `psa_set_key_size` in step 2) or [`PSA_EXPORT_PUBLIC_KEY_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_EXPORT_PUBLIC_KEY_MAX_SIZE) to hold our key.
    * `their_pub`: a buffer of the same size, to hold the peer's key. This can be the same as `our_pub` if the application does not need to hold both at the same time;
    * `shared_secret`: a buffer of size [`PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(key_type, bits)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE) or [`PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE) (if not using a key derivation operation).
2. Prepare an attribute structure as described in “[Diffie-Hellman key pair management](#diffie-hellman-key-pair-management)”, in particular selecting the curve with `psa_set_key_type`.
3. Call [`psa_generate_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__random/#group__random_1ga1985eae417dfbccedf50d5fff54ea8c5) on `attributes` and `our_key` (output) to generate a key pair, then [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062) on `our_key` and `our_pub` (output) to obtain our public key.
4. Send `our_pub` to the peer. Retrieve the peer's public key and import it into `their_pub`. These two actions may be performed in either order.
5. Call [`psa_raw_key_agreement`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga90fdd2716124d0bd258826184824675f) on `our_key`, `their_pub` and `shared_secret` (output).  
   Alternatively, call `psa_key_derivation_key_agreement` to use the shared secret directly in a key derivation operation (see “[Performing a key agreement](#performing-a-key-agreement)”).
6. Call [`psa_destroy_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__management/#group__key__management_1ga5f52644312291335682fbc0292c43cd2) on `key_id`, and free the memory buffers.

Steps 4–6 are only performed once for a "true" ephemeral Diffie-Hellman. They may be repeated multiple times for a "fake ephemeral" Diffie-Hellman where the same private key is used for multiple key exchanges, but it not saved.

#### Translating a legacy ephemeral key agreement TLS server workflow

The legacy API offers the following workflow for an ephemeral Diffie-Hellman key agreement in a TLS 1.2 server. The PSA version of this workflow can also be used with other protocols, on the side of the party that selects the curve or group and sends its public key first.

1. Setup phase:
    1. Initialize a context of type `mbedtls_ecdh_context` or `mbedtls_dhm_context` with `mbedtls_ecdh_init` or `mbedtls_dhm_init`.
    2. Call `mbedtls_ecdh_setup` or `mbedtls_dhm_set_group` to select the curve or group.
    3. Call `mbedtls_ecdh_make_params` or `mbedtls_dhm_make_params` to generate our key pair and obtain a TLS ServerKeyExchange message encoding the selected curve/group and our public key.
2. Send the ServerKeyExchange message to the peer.
3. Retrieve the peer's public key.
4. Call `mbedtls_ecdh_read_public` or `mbedtls_dhm_read_public` on the peer's public key, then call `mbedtls_ecdh_calc_secret` or `mbedtls_dhm_calc_secret` to calculate the shared secret.
5. Free the context with `mbedtls_ecdh_free` or `mbedtls_dhm_free`.

The corresponding workflow with the PSA API is as follows:

1. Setup phase:
    1. Generate an ECDH or DHM key pair with `psa_generate_key` as described in “[Diffie-Hellman key pair management](#diffie-hellman-key-pair-management)”.
    2. Call [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062) to obtain our public key.
    3. Format a ServerKeyExchange message containing the curve/group selection and our public key.
2. Send the ServerKeyExchange message to the peer.
3. Retrieve the peer's public key.
4. Call [`psa_raw_key_agreement`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga90fdd2716124d0bd258826184824675f) on `our_key`, `their_pub` and `shared_secret` (output).  
   Alternatively, call `psa_key_derivation_key_agreement` to use the shared secret directly in a key derivation operation (see “[Performing a key agreement](#performing-a-key-agreement)”).
5. Call [`psa_destroy_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__management/#group__key__management_1ga5f52644312291335682fbc0292c43cd2) to free the resources associated with our key pair.

#### Translating a legacy ephemeral key agreement TLS client workflow

The legacy API offers the following workflow for an ephemeral Diffie-Hellman key agreement in a TLS 1.2 client. The PSA version of this workflow can also be used with other protocols, on the side of the party that receives a message indicating both the choice of curve or group, and the peer's public key.

1. Upon reception of a TLS ServerKeyExchange message received from the peer, which encodes the selected curve/group and the peer's public key:
    1. Initialize a context of type `mbedtls_ecdh_context` or `mbedtls_dhm_context` with `mbedtls_ecdh_init` or `mbedtls_dhm_init`.
    2. Call `mbedtls_ecdh_read_params` or `mbedtls_dhm_read_params` to input the data from the ServerKeyExchange message.
2. Call `mbedtls_ecdh_make_public` or `mbedtls_dh_make_public` to generate our private key and export our public key.
3. Send our public key to the peer.
4. Call `mbedtls_ecdh_calc_secret` or `mbedtls_dhm_calc_secret` to calculate the shared secret.
5. Free the context with `mbedtls_ecdh_free` or `mbedtls_dhm_free`.

The corresponding workflow with the PSA API is as follows:

1. Upon reception of a TLS ServerKeyExchange message received from the peer, which encodes the selected curve/group and the peer's public key:
    1. Decode the selected curve/group and use this to determine a PSA key type (`PSA_KEY_TYPE_ECC_KEY_PAIR(curve)` or `PSA_KEY_TYPE_DH_KEY_PAIR(group)`), a key size and an algorithm.
2. Generate an ECDH or DHM key pair with `psa_generate_key` as described in “[Diffie-Hellman key pair management](#diffie-hellman-key-pair-management)”.
   Call [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062) to obtain our public key.
3. Send our public key to the peer.
4. Call [`psa_raw_key_agreement`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__derivation/#group__key__derivation_1ga90fdd2716124d0bd258826184824675f) on `our_key`, `their_pub` and `shared_secret` (output).  
   Alternatively, call `psa_key_derivation_key_agreement` to use the shared secret directly in a key derivation operation (see “[Performing a key agreement](#performing-a-key-agreement)”).
5. Call [`psa_destroy_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__key__management/#group__key__management_1ga5f52644312291335682fbc0292c43cd2) to free the resources associated with our key pair.

#### ECDH and DHM metadata functions

You can obtain data and metadata from an ECDH key agreement through the PSA API as follows:

* With either side, accessing the group: call [`psa_get_key_attributes`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gacbbf5c11eac6cd70c87ffb936e1b9be2) on the key identifier, then [`psa_get_key_type`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gae4fb812af4f57aa1ad85e335a865b918) and [`psa_get_key_bits`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga5bee85c2164ad3d4c0d42501241eeb06) to obtain metadata about the key.
* Accessing our public key: call [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062) on the PSA key identifier.
* Accessing our private key: call [`psa_export_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga668e35be8d2852ad3feeef74ac6f75bf) on the key identifier. Note that the key policy must allow `PSA_KEY_USAGE_EXPORT` (see “[Public-key cryptography policies](#public-key-cryptography-policies)”).
* Accessing the peer's public key: there is no PSA equivalent since the PSA API only uses the peer's public key to immediately calculate the shared secret. If your application needs the peer's public key for some other purpose, store it separately.

The functions `mbedtls_dhm_get_bitlen`, `mbedtls_dhm_get_len` and `mbedtls_dhm_get_value` allow the caller to obtain metadata about the keys used for the key exchange. The PSA equivalents access the key identifier:

* `mbedtls_dhm_get_bitlen`, `mbedtls_dhm_get_len`: call [`psa_get_key_attributes`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gacbbf5c11eac6cd70c87ffb936e1b9be2) on the PSA key identifier, then [`psa_get_key_bits`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1ga5bee85c2164ad3d4c0d42501241eeb06).
* `mbedtls_dhm_get_value` for `MBEDTLS_DHM_PARAM_X` (our private key): call [`psa_export_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga668e35be8d2852ad3feeef74ac6f75bf) on the key identifier. Note that the key policy must allow `PSA_KEY_USAGE_EXPORT` (see “[Public-key cryptography policies](#public-key-cryptography-policies)”).
* `mbedtls_dhm_get_value` for `MBEDTLS_DHM_PARAM_GX` (our public key): call [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062) on the PSA key identifier.
* `mbedtls_dhm_get_value` for `MBEDTLS_DHM_PARAM_GY` (peer's public key): the there is no PSA equivalent since the PSA API only uses the peer's public key to immediately calculate the shared secret. If your application needs the peer's public key for some other purpose, store it separately.
* `mbedtls_dhm_get_value` for `MBEDTLS_DHM_PARAM_K` (shared secret): this is the value calculated by `psa_raw_key_agreement` or `psa_key_derivation_key_agreement`. If you need to use it multiple times (for example to derive multiple values independently), call `psa_raw_key_agreement` and make a copy.
* `mbedtls_dhm_get_value` for `MBEDTLS_DHM_PARAM_P` or `MBEDTLS_DHM_PARAM_G` (group parameters): [there is no PSA API to retrieve these values](https://github.com/Mbed-TLS/mbedtls/issues/7780).

The PSA API for finite-field Diffie-Hellman only supports predefined groups. Therefore there is no equivalent to `mbedtls_dhm_parse_dhm`, `mbedtls_dhm_parse_dhmfile`, and the `MBEDTLS_DHM_xxx_BIN` macros.

#### Restartable key agreement

The legacy API includes an API for “restartable” ECC operations: the operation returns after doing partial computation, and can be resumed. This is intended for highly constrained devices where long cryptographic calculations need to be broken up to poll some inputs, where interrupt-based scheduling is not desired. The legacy API for ECDH is the same as the ordinary ECDH API, with an extra call to `mbedtls_ecdh_enable_restart()` on the ECDH context. The PSA API uses a different set of functions, available since TF-PSA-Crypto 1.0.

1. Call [`psa_key_agreement_iop_setup()`](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/include/psa/crypto.h) to start the interruptible key agreement operation.
2. Call [`psa_key_agreement_iop_complete()`](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/include/psa/crypto.h) repeatedly until it returns a status other than `PSA_OPERATION_INCOMPLETE`.

If you need to cancel the operation after calling the start function without waiting for the loop calling the complete function to finish, call `psa_key_agreement_iop_abort()`.

Call [`psa_interruptible_set_max_ops`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga6d86790b31657c13705214f373af869e) to set the number of basic operations per call. This is the same unit as `mbedtls_ecp_set_max_ops`. You can retrieve the current value with [`psa_interruptible_get_max_ops`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible__hash/#group__interruptible__hash_1ga73e66a6d93f2690b626fcea20ada62b2). The value is [`PSA_INTERRUPTIBLE_MAX_OPS_UNLIMITED`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__interruptible/#group__interruptible_1gad19c1da7f6b7d59d5873d5b68eb943d4) if operations are not restartable, which corresponds to `mbedtls_ecp_restart_is_enabled()` being false.

### Additional information about Elliptic-curve cryptography

#### Information about a curve

The legacy API identifies a curve by an `MBEDTLS_ECP_DP_xxx` value of type `mbedtls_ecp_group_id`. The PSA API identifies a curve by a `PSA_ECC_FAMILY_xxx` value and the private value's bit-size. See “[Elliptic curve mechanism selection](#elliptic-curve-mechanism-selection)” for the correspondence between the two sets of values.

There is no PSA equivalent of the `mbedtls_ecp_group` data structure (and so no equivalent to `mbedtls_ecp_group_init`, `mbedtls_ecp_group_load`, `mbedtls_ecp_group_copy` and `mbedtls_ecp_group_free`) or of the `mbedtls_ecp_curve_info` data structure (and so no equivalent to `mbedtls_ecp_curve_info_from_grp_id`) because they are not needed. All API elements identify the curve directly by its family and size.

The bit-size used by the PSA API is the size of the private key. For most curves, the PSA bit-size, the `bit_size` field in `mbedtls_ecp_curve_info`, the `nbits` field in `mbedtls_ecp_group` and the `pbits` field in `mbedtls_ecp_group` are the same. The following table lists curves for which they are different.

| Curve | `grp->nbits` | `grp->pbits` | `curve_info->bit_size` | PSA bit-size |
| ----- | ------------ | ------------ | ---------------------- | ------------ |
| Curve25519 | 253 | 255 | 256 | 255 |
| Curve448 | 446 | 448 | 448 | 448 |

There is no exact PSA equivalent of the type `mbedtls_ecp_curve_type` and the function `mbedtls_ecp_get_type`, but the curve family encodes the same information. `PSA_ECC_FAMILY_MONTGOMERY` is the only Montgomery family. All other families supported in TF-PSA-Crypto 1.0 are short Weierstrass families.

There is no PSA equivalent for the following functionality:

* The `name` field of `mbedtls_ecp_curve_info`, and the function `mbedtls_ecp_curve_info_from_name`. There is no equivalent of Mbed TLS's lookup based on the name used for the curve in TLS specifications.
* The `tls_id` field of `mbedtls_ecp_curve_info`, the constant `MBEDTLS_ECP_TLS_NAMED_CURVE`, and the functions `mbedtls_ecp_curve_info_from_tls_id`, `mbedtls_ecp_tls_read_group`, `mbedtls_ecp_tls_read_group_id` and `mbedtls_ecp_tls_write_group`. The PSA crypto API does not have this dedicated support for the TLS protocol.
* Retrieving the parameters of a curve from the fields of an `mbedtls_ecp_group` structure.

#### Information about supported curves

The PSA API does not currently have a discovery mechanism for cryptographic mechanisms (although one may be added in the future). Thus there is no equivalent for `MBEDTLS_ECP_DP_MAX` and the functions `mbedtls_ecp_curve_list` and `mbedtls_ecp_grp_id_list`.

The API provides macros that give the maximum supported sizes for various kinds of objects. The following table lists equivalents for `MBEDTLS_ECP_MAX_xxx` macros.

| Legacy macro | PSA equivalent |
| ------------ | -------------- |
| `MBEDTLS_ECP_MAX_BITS` | [`PSA_VENDOR_ECC_MAX_CURVE_BITS`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_VENDOR_ECC_MAX_CURVE_BITS) |
| `MBEDTLS_ECP_MAX_BYTES` | `PSA_BITS_TO_BYTES(PSA_VENDOR_ECC_MAX_CURVE_BITS)` |
| `MBEDTLS_ECP_MAX_PT_LEN` | [`PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_VENDOR_ECC_MAX_CURVE_BITS)`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__sizes_8h/#c.PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE) |

#### Restartable ECC

The PSA API supports the equivalent of restartable operations, but only for signatures at the time of writing. See “[Restartable ECDSA signature](#restartable-ecdsa-signature)”.

There is no PSA API for elliptic curve arithmetic as such, and therefore no equivalent of `mbedtls_ecp_restart_ctx` and functions that operate on it.

There is PSA no equivalent of the `MBEDTLS_ECP_OPS_xxx` constants.

#### ECC functionality with no PSA equivalent

There is no PSA equivalent of `mbedtls_ecdsa_can_do` and `mbedtls_ecdh_can_do` to query the capabilities of a curve at runtime. Check the documentation of each curve family to see what algorithms it supports.

There is no PSA equivalent to the types `mbedtls_ecdsa_context` and `mbedtls_ecdsa_restart_ctx`, and to basic ECDSA context manipulation functions including `mbedtls_ecdsa_from_keypair`, because they are not needed: the PSA API does not have ECDSA-specific context types.

#### No curve arithmetic

The PSA API is a cryptography API, not an arithmetic API. As a consequence, there is no PSA equivalent for the ECC arithmetic functionality exposed by `ecp.h`:

* Manipulation of point objects and input-output: the type `mbedtls_ecp_point` and functions operating on it (`mbedtls_ecp_point_xxx`, `mbedtls_ecp_copy`, `mbedtls_ecp_{set,is}_zero`, `mbedtls_ecp_tls_{read,write}_point`). Note that the PSA export format for public keys corresponds to the uncompressed point format (`MBEDTLS_ECP_PF_UNCOMPRESSED`), so [`psa_import_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga0336ea76bf30587ab204a8296462327b), [`psa_export_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga668e35be8d2852ad3feeef74ac6f75bf) and [`psa_export_public_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1gaf22ae73312217aaede2ea02cdebb6062) are equivalent to `mbedtls_ecp_point_read_binary` and `mbedtls_ecp_point_write_binary` for uncompressed points. The PSA API does not currently support compressed points, but it is likely that such support will be added in the future.
* Manipulation of key pairs as such, with a bridge to bignum arithmetic (`mbedtls_ecp_keypair` type, `mbedtls_ecp_export`). However, the PSA export format for ECC private keys used by [`psa_import_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga0336ea76bf30587ab204a8296462327b), [`psa_export_key`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__import__export/#group__import__export_1ga668e35be8d2852ad3feeef74ac6f75bf) is the same as the format used by `mbedtls_ecp_read_key` and `mbedtls_ecp_write_key_ext`.
* Elliptic curve arithmetic (`mbedtls_ecp_mul`, `mbedtls_ecp_muladd` and their restartable variants).

### Additional information about RSA

#### RSA-ALT interface

Implementers of the RSA-ALT interface (`MBEDTLS_PK_RSA_ALT` pk type, `mbedtls_pk_setup_rsa_alt` setup function) must migrate to the [PSA cryptoprocessor driver interface](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/docs/psa-driver-example-and-guide.md).

* If the purpose of the ALT interface is acceleration only: use the accelerator driver interface. This is fully transparent to application code.
* If the purpose of the ALT interface is to isolate the private key in a high-security environment: use the opaque driver interface. This is mostly transparent to user code. Code that uses a key via its key identifier does not need to know whether the key is transparent (equivalent of `MBEDTLS_PK_RSA`) or opaque (equivalent of `MBEDTLS_PK_RSA_ALT`). When creating a key, it will be transparent by default; to create an opaque key, call [`psa_set_key_lifetime`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__attributes/#group__attributes_1gac03ccf09ca6d36cc3d5b43f8303db6f7) to set the key's location to the chosen location value for the driver, e.g.
    ```
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
        PSA_KEY_PERSISTENCE_VOLATILE, MY_RSA_DRIVER_LOCATION));
    ```

The PSA subsystem uses its internal random generator both for randomized algorithms and to generate blinding values. As a consequence, none of the API functions take an RNG parameter.

#### RSA functionality with no PSA equivalent

The PSA API does not provide direct access to the exponentiation primitive as with `mbedtls_rsa_public` and `mbedtls_rsa_private`. If you need an RSA-based mechanism that is not supported by the PSA API, please [submit an issue on GitHub](https://github.com/ARM-software/psa-api/issues) so that we can extend the API to support it.

The PSA API does not support constructing RSA keys progressively from numbers, like the legacy functions `mbedtls_rsa_import`, `mbedtls_rsa_import_raw` and `mbedtls_rsa_complete`. You will need to construct the standard ASN.1 representation and call `psa_import_key()` (see “[Creating keys for asymmetric cryptography](#creating-keys-for-asymmetric-cryptography)”).

There is no direct equivalent of `mbedtls_rsa_export`, `mbedtls_rsa_export_raw` and `mbedtls_rsa_export_crt` to export some of the numbers in a key. You can export the whole key with `psa_export_key`, or with `psa_export_public_key` to export the public key from a key pair object. See also “[Exporting a public key or a key pair](#exporting-a-public-key-or-a-key-pair)”.

A PSA key object is immutable, so there is no need for an equivalent of `mbedtls_rsa_copy`. (There is a function `psa_copy_key`, but it is only useful to make a copy of a key with a different policy of ownership; both concepts are out of scope of this document since they have no equivalent in the legacy API.)

### LMS signatures

A future version of TF-PSA-Crypto will likely support LMS keys and signatures through the PSA API (`psa_generate_key`, `psa_export_public_key`, `psa_import_key`, `psa_sign_hash`, `psa_verify_hash`, etc.). For the time being, `mbedtls/lms.h` is the only interface to LMS.

### PK format support interfaces

The interfaces in `base64.h`, `asn1.h`, `asn1write.h` and `pem.h` are intended to support X.509 and key file formats. Since these APIs are not directly about cryptography, there is no PSA replacement in Mbed TLS 3.6 or in TF-PSA-Crypto 1.0.

TF-PSA-Crypto 1.0 removes direct access to OID values and functions formerly in `mbedtls/oid.h`. OID lookup is only used internally to parse and write keys and other objects.

In the ASN.1 modules, the functions `mbedtls_asn1_get_mpi` and `mbedtls_asn1_write_mpi` have been replaced by `mbedtls_asn1_get_integer` and `mbedtls_asn1_write_integer`. See the [TF-PSA-Crypto 1.0 migration guide](1.0-migration-guide.md#changes-to-asn-1-functions) for more information.

## EC-JPAKE

The PSA API exposes EC-JPAKE via the algorithm [`PSA_ALG_JPAKE`](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__extra_8h/#c.PSA_ALG_JPAKE) and the PAKE API functions.

Please note a few differences between the two APIs: the legacy API is geared towards the use of EC-JPAKE in TLS 1.2, whereas the PSA API is protocol-agnostic.

* The PSA API is finer-grained and offers more flexibility in message ordering. Where the legacy API makes a single function call, the PSA API may require multiple calls.
* The legacy API uses the TLS 1.2 wire format in the input or output format of several functions. In particular, one of the messages embeds the curve identifier in the TLS protocol. The PSA API uses protocol-agnostic formats.
* The legacy API always applies the key derivation specified by TLS 1.2 to the shared secret. With the PSA API, use a key derivation with `PSA_ALG_TLS12_ECJPAKE_TO_PMS` for the same calculation.

TF-PSA-Crypto implements the official PSA Crypto PAKE API 1.2. Note that Mbed TLS 3.6 implements a beta version of the PAKE API, which is not fully compatible.
