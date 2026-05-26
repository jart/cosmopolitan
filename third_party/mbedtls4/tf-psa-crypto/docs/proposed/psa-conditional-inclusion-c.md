Conditional inclusion of cryptographic mechanism through the PSA API in Mbed TLS
================================================================================

This document is a proposed interface for deciding at build time which cryptographic mechanisms to include in the PSA Cryptography interface.

This is currently a proposal for Mbed TLS. It is not currently on track for standardization in PSA.

## Introduction

### Purpose of this specification

The [PSA Cryptography API specification](https://armmbed.github.io/mbed-crypto/psa/#application-programming-interface) specifies the interface between a PSA Cryptography implementation and an application. The interface defines a number of categories of cryptographic algorithms (hashes, MAC, signatures, etc.). In each category, a typical implementation offers many algorithms (e.g. for signatures: RSA-PKCS#1v1.5, RSA-PSS, ECDSA). When building the implementation for a specific use case, it is often desirable to include only a subset of the available cryptographic mechanisms, primarily in order to reduce the code footprint of the compiled system.

The present document proposes a way for an application using the PSA cryptography interface to declare which mechanisms it requires.

### Conditional inclusion of legacy cryptography modules

Mbed TLS offers a way to select which cryptographic mechanisms are included in a build through its configuration file (`mbedtls_config.h`). This mechanism is based on two main sets of symbols: `MBEDTLS_xxx_C` controls the availability of the mechanism to the application, and `MBEDTLS_xxx_ALT` controls the availability of an alternative implementation, so the software implementation is only included if `MBEDTLS_xxx_C` is defined but not `MBEDTLS_xxx_ALT`.

### PSA evolution

In the PSA cryptography interface, the **core** (built-in implementations of cryptographic mechanisms) can be augmented with drivers. **Transparent drivers** replace the built-in implementation of a cryptographic mechanism (or, with **fallback**, the built-in implementation is tried if the driver only has partial support for the mechanism). **Opaque drivers** implement cryptographic mechanisms on keys which are stored in a separate domain such as a secure element, for which the core only does key management and dispatch using wrapped key blobs or key identifiers.

The current model is difficult to adapt to the PSA interface for several reasons. The `MBEDTLS_xxx_ALT` symbols are somewhat inconsistent, and in particular do not work well for asymmetric cryptography. For example, many parts of the ECC code have no `MBEDTLS_xxx_ALT` symbol, so a platform with ECC acceleration that can perform all ECDSA and ECDH operations in the accelerator would still embark the `bignum` module and large parts of the `ecp_curves`, `ecp` and `ecdsa` modules. Also the availability of a transparent driver for a mechanism does not translate directly to `MBEDTLS_xxx` symbols.

### Requirements

[Req.interface] The application can declare which cryptographic mechanisms it needs.

[Req.inclusion] If the application does not require a mechanism, a suitably configured Mbed TLS build must not include it. The granularity of mechanisms must work for typical use cases and has [acceptable limitations](#acceptable-limitations).

[Req.drivers] If a PSA driver is available in the build, a suitably configured Mbed TLS build must not include the corresponding software code (unless a software fallback is needed).

[Req.c] The configuration mechanism consists of C preprocessor definitions, and the build does not require tools other than a C compiler. This is necessary to allow building an application and Mbed TLS in development environments that do not allow third-party tools.

[Req.adaptability] The implementation of the mechanism must be adaptable with future evolution of the PSA cryptography specifications and Mbed TLS. Therefore the interface must remain sufficiently simple and abstract.

### Acceptable limitations

[Limitation.matrix] If a mechanism is defined by a combination of algorithms and key types, for example a block cipher mode (CBC, CTR, CFB, …) and a block permutation (AES, CAMELLIA, ARIA, …), there is no requirement to include only specific combinations.

[Limitation.direction] For mechanisms that have multiple directions (for example encrypt/decrypt, sign/verify), there is no requirement to include only one direction.

[Limitation.size] There is no requirement to include only support for certain key sizes.

[Limitation.multipart] Where there are multiple ways to perform an operation, for example single-part and multi-part, there is no mechanism to select only one or a subset of the possible ways.

## Interface

### PSA Crypto configuration file

The PSA Crypto configuration file `psa/crypto_config.h` defines a series of symbols of the form `PSA_WANT_xxx` where `xxx` describes the feature that the symbol enables. The symbols are documented in the section [“PSA Crypto configuration symbols”](#psa-crypto-configuration-symbols) below.
The necessary software implementations of cryptographic algorithms are included based on the content of the PSA Crypto configuration file. For example, the code in `aes.c` is enabled if `psa/crypto_config.h` contains `PSA_WANT_KEY_TYPE_AES`.

### PSA Crypto configuration symbols

#### Configuration symbol syntax

A PSA Crypto configuration symbol is a C preprocessor symbol whose name starts with `PSA_WANT_`.

* If the symbol is not defined, the corresponding feature is not included.
* If the symbol is defined to a preprocessor expression with the value `1`, the corresponding feature is included.
* If the symbol is defined with a different value, the behavior is currently undefined and reserved for future use.

#### Configuration symbol usage

The presence of a symbol `PSA_WANT_xxx` in the Mbed TLS configuration determines whether a feature is available through the PSA API. These symbols should be used in any place that requires conditional compilation based on the availability of a cryptographic mechanism through the PSA API, including:

* In Mbed TLS test code.
* In Mbed TLS library code, for example in TLS to determine which cipher suites to enable.
* In application code that provides additional features based on cryptographic capabilities, for example additional key parsing and formatting functions, or cipher suite availability for network protocols.

#### Configuration symbol semantics

If a feature is not requested for inclusion in the PSA Crypto configuration file, it may still be included in the build, either because the feature has been requested in some other way, or because the library does not support the exclusion of this feature. Mbed TLS should make a best effort to support the exclusion of all features, but in some cases this may be judged too much effort for too little benefit.

#### Configuration symbols for key types

For most constant or constructor macros of the form `PSA_KEY_TYPE_xxx`, the symbol **`PSA_WANT_KEY_TYPE_xxx`** indicates that support for this key type is desired.

As an exception, starting in Mbed TLS 3.5.0, for `KEY_PAIR` types (that is, private keys for asymmetric cryptography), the feature selection is more fine-grained, with an additional suffix:
* `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_BASIC` enables basic support for the key type, and in particular support for operations with a key of that type for enabled algorithms. This is automatically enabled if any of the other `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_yyy` options is enabled.
* `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_IMPORT` enables support for `psa_import_key` to import a key of that type.
* `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_GENERATE` enables support for `psa_generate_key` to randomly generate a key of that type.
* `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_DERIVE` enables support for `psa_key_derivation_output_key` to deterministically derive a key of that type.
* `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_EXPORT` enables support for `psa_export_key` to export a key of that type.

For asymmetric cryptography, `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_BASIC` determines whether private-key operations are desired, and `PSA_WANT_KEY_TYPE_xxx_PUBLIC_KEY` determines whether public-key operations are desired. `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_BASIC` implicitly enables `PSA_WANT_KEY_TYPE_xxx_PUBLIC_KEY`, as well as support for `psa_export_public_key` on the private key: there is no way to only include private-key operations (which typically saves little code).

Note: the implementation is always free to include support for more than what was explicitly requested. (For example, as of Mbed TLS 3.5.0, `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_BASIC` implicitly enables import and export support for that key type, but this may not be the case in future versions.) Applications should always request support for all operations they need, rather than rely on them being implicitly enabled by the implementation. The only thing that is documented and guaranteed in the future is as follows: `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_yyy` -> `PSA_WANT_KEY_TYPE_xxx_KEY_PAIR_BASIC` -> `PSA_WANT_KEY_TYPE_xxx_PUBLIC_KEY`.

#### Configuration symbols for elliptic curves

For elliptic curve key types, only the specified curves are included. To include a curve, include a symbol of the form **`PSA_WANT_ECC_family_size`**. For example: `PSA_WANT_ECC_SECP_R1_256` for secp256r1, `PSA_WANT_ECC_MONTGOMERY_255` for Curve25519. It is an error to require an ECC key type but no curve, and Mbed TLS will reject this at compile time.

Rationale: this is a deviation of the general principle that `PSA_ECC_FAMILY_xxx` would have a corresponding symbol `PSA_WANT_ECC_FAMILY_xxx`. This deviation is justified by the fact that it is very common to wish to include only certain curves in a family, and that can lead to a significant gain in code size.

#### Configuration symbols for Diffie-Hellman groups

There are no configuration symbols for Diffie-Hellman groups (`PSA_DH_GROUP_xxx`).

Rationale: Finite-field Diffie-Hellman code is usually not specialized for any particular group, so reducing the number of available groups at compile time only saves a little code space. Constrained implementations tend to omit FFDH anyway, so the small code size gain is not important.

#### Configuration symbols for algorithms

For each constant or constructor macro of the form `PSA_ALG_xxx`, the symbol **`PSA_WANT_ALG_xxx`** indicates that support for this algorithm is desired.

For parametrized algorithms, the `PSA_WANT_ALG_xxx` symbol indicates whether the base mechanism is supported. Parameters must themselves be included through their own `PSA_WANT_ALG_xxx` symbols. It is an error to include a base mechanism without at least one possible parameter, and Mbed TLS will reject this at compile time. For example, `PSA_WANT_ALG_ECDSA` requires the inclusion of randomized ECDSA for all hash algorithms whose corresponding symbol `PSA_WANT_ALG_xxx` is enabled.

## Implementation

### Additional non-public symbols

#### Accounting for transparent drivers

In addition to the [configuration symbols](#psa-crypto-configuration-symbols), we need two parallel or mostly parallel sets of symbols:

* **`MBEDTLS_PSA_ACCEL_xxx`** indicates whether a fully-featured, fallback-free transparent driver is available.
* **`MBEDTLS_PSA_BUILTIN_xxx`** indicates whether the software implementation is needed.

`MBEDTLS_PSA_ACCEL_xxx` is one of the outputs of the transpilation of a driver description, alongside the glue code for calling the drivers.

`MBEDTLS_PSA_BUILTIN_xxx` is enabled when `PSA_WANT_xxx` is enabled and `MBEDTLS_PSA_ACCEL_xxx` is disabled.

These symbols are not part of the public interface of Mbed TLS towards applications or to drivers, regardless of whether the symbols are actually visible.

### Architecture of symbol definitions

#### Definition of configuration symbols

The header file `mbedtls/mbedtls_config.h` defines all the `MBEDTLS_xxx_C` configuration symbols, including the ones deduced from the PSA Crypto configuration. It does this by including the new header file **`mbedtls/config_psa.h`**, which defines the `MBEDTLS_PSA_BUILTIN_xxx` symbols and deduces the corresponding `MBEDTLS_xxx_C` (and other) symbols.

`mbedtls/config_psa.h` includes `psa/crypto_config.h`, the user-editable file that defines application requirements.

#### Summary of definitions of configuration symbols

`mbedtls/config_psa.h` includes `mbedtls/crypto_drivers.h`, a header file generated by the transpilation of the driver descriptions. It defines `MBEDTLS_PSA_ACCEL_xxx` symbols according to the availability of transparent drivers without fallback.

The following table summarizes where symbols are defined depending on the configuration mode.

* (U) indicates a symbol that is defined by the user (application).
* (D) indicates a symbol that is deduced from other symbols by code that ships with Mbed TLS.
* (G) indicates a symbol that is generated from driver descriptions.

| Symbols                   |                                   |
| ------------------------- | --------------------------------- |
| `MBEDTLS_xxx_C`           | `mbedtls/mbedtls_config.h` (U) or |
|                           | `mbedtls/config_psa.h` (D)        |
| `PSA_WANT_xxx`            | `psa/crypto_config.h` (U)         |
| `MBEDTLS_PSA_BUILTIN_xxx` | `mbedtls/config_psa.h` (D)        |
| `MBEDTLS_PSA_ACCEL_xxx`   | `mbedtls/crypto_drivers.h` (G)    |

#### Visibility of internal symbols

Ideally, the `MBEDTLS_PSA_ACCEL_xxx` and `MBEDTLS_PSA_BUILTIN_xxx` symbols should not be visible to application code or driver code, since they are not part of the public interface of the library. However these symbols are needed to deduce whether to include library modules (for example `MBEDTLS_AES_C` has to be enabled if `MBEDTLS_PSA_BUILTIN_KEY_TYPE_AES` is enabled), which makes it difficult to keep them private.

#### Compile-time checks

The header file **`library/psa_check_config.h`** applies sanity checks to the configuration, throwing `#error` if something is wrong.

A mechanism similar to `mbedtls/check_config.h` detects errors such as enabling ECDSA but no curve.

Since configuration symbols must be undefined or 1, any other value should trigger an `#error`.

#### Automatic generation of preprocessor symbol manipulations

A lot of the preprocessor symbol manipulation is systematic calculations that analyze the configuration. `mbedtls/config_psa.h` and `library/psa_check_config.h` should be generated automatically, in the same manner as `version_features.c`.

### Structure of PSA Crypto library code

#### Conditional inclusion of library entry points

An entry point can be eliminated entirely if no algorithm requires it.

#### Conditional inclusion of mechanism-specific code

Code that is specific to certain key types or to certain algorithms must be guarded by the applicable symbols: `PSA_WANT_xxx` for code that is independent of the application, and `MBEDTLS_PSA_BUILTIN_xxx` for code that calls an Mbed TLS software implementation.

## PSA standardization

### JSON configuration mechanism

At the time of writing, the preferred configuration mechanism for a PSA service is in JSON syntax. The translation from JSON to build instructions is not specified by PSA.

For PSA Crypto, the preferred configuration mechanism would be similar to capability specifications of transparent drivers. The same JSON properties that are used to mean “this driver can perform that mechanism” in a driver description would be used to mean “the application wants to perform that mechanism” in the application configuration.

### From JSON to C

The JSON capability language allows a more fine-grained selection than the C mechanism proposed here. For example, it allows requesting only single-part mechanisms, only certain key sizes, or only certain combinations of algorithms and key types.

The JSON capability language can be translated approximately to the boolean symbol mechanism proposed here. The approximation considers a feature to be enabled if any part of it is enabled. For example, if there is a capability for AES-CTR and one for CAMELLIA-GCM, the translation to boolean symbols will also include AES-GCM and CAMELLIA-CTR. If there is a capability for AES-128, the translation will also include AES-192 and AES-256.

The boolean symbol mechanism proposed here can be translated to a list of JSON capabilities: for each included algorithm, include a capability with that algorithm, the key types that apply to that algorithm, no size restriction, and all the entry points that apply to that algorithm.

## Open questions

### Open questions about the interface

#### Naming of symbols

The names of [elliptic curve symbols](#configuration-symbols-for-elliptic-curves) are a bit weird: `SECP_R1_256` instead of `SECP256R1`, `MONTGOMERY_255` instead of `CURVE25519`. Should we make them more classical, but less systematic?

#### Impossible combinations

What does it mean to have `PSA_WANT_ALG_ECDSA` enabled but with only Curve25519? Is it a mandatory error?

#### Diffie-Hellman

Way to request only specific groups? Not a priority: constrained devices don't do FFDH. Specify it as may change in future versions.

#### Coexistence with the current Mbed TLS configuration

The two mechanisms have very different designs. Is there serious potential for confusion? Do we understand how the combinations work?

### Open questions about the design

#### Algorithms without a key type or vice versa

Is it realistic to mandate a compile-time error if a key type is required, but no matching algorithm, or vice versa? Is it always the right thing, for example if there is an opaque driver that manipulates this key type?

#### Opaque-only mechanisms

If a mechanism should only be supported in an opaque driver, what does the core need to know about it? Do we have all the information we need?

This is especially relevant to suppress a mechanism completely if there is no matching algorithm. For example, if there is no transparent implementation of RSA or ECDSA, `psa_sign_hash` and `psa_verify_hash` may still be needed if there is an opaque signature driver.

### Open questions about the implementation

#### Testability

Is this proposal decently testable? There are a lot of combinations. What combinations should we test?

<!--
Local Variables:
time-stamp-line-limit: 40
time-stamp-start: "Time-stamp: *\""
time-stamp-end: "\""
time-stamp-format: "%04Y/%02m/%02d %02H:%02M:%02S %Z"
time-stamp-time-zone: "GMT"
End:
-->
