TF-PSA-Crypto minimal interfaces
================================

## Introduction

This document describes plans for a minimalist TF-PSA-Crypto, tentatively called TF-PSA-Crypto 0ε, and a matching Mbed TLS consuming it. The general idea is to start from PSA interfaces, and expose additional interfaces to provide functionality that PSA does not currently offer. Most additional interfaces will be existing ones, but they can be new interfaces if it makes more sense.

Mbed TLS will continue using internal interfaces of TF-PSA-Crypto, even after the release of TF-PSA-Crypto 1.0 and Mbed TLS 4.0. However, it is an objective that some 4.x version will no longer need to use any internal interface of TF-PSA-Crypto. As a result, we will remove some functionality from Mbed TLS. In the 0ε version, it is an objective that no public interface of Mbed TLS will expose an internal interface of TF-PSA-Crypto.

Note that the name “TF-PSA-Crypto 0ε” is for convenience only. At this time, we are not planning a release of this version. We are planning a beta release of TF-PSA-Crypto before the 1.0 release, but this may come before or after the 0ε described in this document.

This document is about C-level interfaces for applications and integration. Other aspects of the project, such as build and test scripts, are out of scope.

## Glossary

This document uses a few unusual terms, and a few generic terms with a specific meaning. This section explains those terms.

**0ε**: a version of TF-PSA-Crypto that minimally meets the formal requirements for TF-PSA-Crypto 1.0. This may not be something we want to release, for example because it lacks critical features, is insufficiently tested, has insufficient documentation, or would be too much of a burden to maintain. It comes before the TF-PSA-Crypto **MVP**.

**1.0**: The first ever stable release of TF-PSA-Crypto. Its publicly documented APIs will remain supported throughout the lifecycle of TF-PSA-Crypto 1.x.

**3ε**: A version of Mbed TLS corresponding to TF-PSA-Crypto 0ε.

**4.0**: The first stable release of Mbed TLS consuming TF-PSA-Crypto. Its publicly documented APIs will remain supported throughout the lifecycle of Mbed TLS 4.x.

**Alpha**: A release of TF-PSA-Crypto or Mbed TLS that is not widely advertized and that may have known major gaps that make it unsuitable for **MVP** or even **0ε/3ε**. There are no commitments to API stability.

**Beta**: A release of TF-PSA-Crypto or Mbed TLS that is advertized to integrators and application writers and that may have known minor gaps that make it unsuitable for **MVP** or even **0ε/3ε**. There are no commitments to API stability.

**Exposed** (interface): An interface (such as a header, type, function or configuration option) that is not publicly documented and not part of the stable API, but that is visible to the compiler when building application code. A typical example is types that are used in fields of structs whose content is not stable, but that it must be possible to allocate on the stack or statically.

**Internal** (interface): an interface of a part of TF-PSA-Crypto or Mbed TLS that is only used by other parts of the same project, and is not visible outside of the project. In particular, Mbed TLS cannot use internal interfaces of TF-PSA-Crypto.

**MVP** (minimum viable product): A version of TF-PSA-Crypto and a corresponding version of Mbed TLS that meet the requirements for TF-PSA-Crypto 1.0, and that would be acceptable for a 1.0/4.0 release.

**Private** (interface): An interface (such as a header, type, function or macro) that is not publicly documented, and that may change or be removed without warning. This is the opposite of **public**. To the extent that it is practical, the library should prevent applications from accidentally relying on private interfaces, but some private interfaces have to be **exposed**.

**Public** (interface): An interface (such as a header, type, function or configuration option) that is publicly documented, and that is covered by API stability guarantees within a major version of TF-PSA-Crypto or Mbed TLS. This is the opposite of **private**, and does not include **exposed** interfaces.

The **Split** (repository split): the split between TF-PSA-Crypto and Mbed TLS, where `tf-psa-crypto` became a submodule instead of a subdirectory. This happened on 2024-12-16.

## Project goals

### Starting point

Our starting point is TF-PSA-Crypto and Mbed TLS as of the repository split, or equivalently as of 2025-01-01. Compared with Mbed TLS 3.6:

* `MBEDTLS_USE_PSA_CRYPTO` is always enabled: PK, X.509 and TLS always use PSA APIs for cryptography, except for a few limitations documented in [`use-psa-crypto.md`](https://github.com/Mbed-TLS/mbedtls/blob/mbedtls-3.6/docs/use-psa-crypto.md).
* `MBEDTLS_PSA_CRYPTO_CONFIG` is always enabled: only `PSA_WANT_xxx` symbols can be used to configure which cryptographic mechanisms are enabled, not `MBEDTLS_xxx`.
* The only public interfaces in TF-PSA-Crypto are the ones in `<psa/crypto.h>` (including interfaces in `include/psa/crypto_*.h` which are exposed indirectly via `crypto.h`). The crypto interfaces in `<mbedtls/*.h>` are considered private, but not marked as such, at the start of the project.

### Feature goals

Ideally, TF-PSA-Crypto 1.0 and Mbed TLS 4.0 would have the same features as Mbed TLS 3.6, except for a small set of features that we have decided to remove (e.g. obsolescent cryptographic mechanisms). This would include creating PSA interfaces where 3.6 only has non-PSA (Mbed TLS legacy) interfaces.

By removing legacy crypto interfaces, we are removing many features for which no corresponding PSA interface exists. For example:

* Configuring an entropy source relies on `<mbedtls/entropy.h>`
* Parsing or formatting a key in common formats relies on `<mbedtls/pk.h>`.
* X.509 fundamentally relies on some features that are not strictly speaking cryptography, but are implemented in TF-PSA-Crypto which needs them for its own use as well: ASN.1 (`<mbedtls/asn1.h>` and `<mbedtls/asn1write.h>`), PEM (`<mbedtls/pem.h>`).
* Parsing or constructing X.509 extensions tends to require ASN.1 functions.

Given the available time and resources, we cannot ensure that 1.0/4.0 will be suitable for all the same use cases as 3.6. However, compared to the project starting point, we will bring back essential use cases. The use cases listed just above are examples of essential features that we want to provide in 0ε. More use cases can be brought back later by exposing more internal legacy interfaces or designing new PSA interfaces.

### Maintainability goals

We don't want to keep maintaining the legacy crypto APIs. They are too low-level, and we want the ability to fully migrate to PSA APIs for cryptography.

This goal will mainly act as a moderator on bringing back features by just exposing legacy interfaces.

For example, we do not want the type `mbedtls_mpi` to be part of the 0ε or 1.0/4.0 API, because it leaks the internal representation of integers, and this limits us when doing optimizations and security improvements.

### Quality goals

TF-PSA-Crypto and Mbed TLS must continue to be high-quality products. Given our limited bandwidth, 0ε and possibly even 1.0/4.0 may compromise on some aspects of quality. However, this needs to be balanced carefully, and not all aspects of quality are up for compromise. As an extreme example, security shall not be compromised at any point.

Some considerations on quality have come up:

* Documentation in the 1.0/4.0 release must be good enough for users and should match our standards for 3.6. We may skip some documentation tasks in 0ε, but the remaining tasks should be clearly identified.
* For users who migrate from 3.6, there should be a clear upgrade path, and there should be an easy way for users to ensure that they have finished migrating, i.e. that they are no longer accidentally using an interface that is now private. This aspect may be compromised in 0ε, although it is not clear whether this is desirable, since having a clear way to flag uses of legacy APIs is a convenient way of evaluating whether 0ε is fit for purpose.
* We have few sample programs for the PSA API. It is unfortunately likely that this will continue past the TF-PSA-Crypto 1.0 release.
* At this point, the TF-PSA-Crypto project does not have self-reliant testing. As outlined in the introduction above, this is out of scope of this document.

### Project 0ε goals

1. In TF-PSA-Crypto, ensure a clear separation between public interfaces (part of the API stability promise) and private interfaces (which may change at any time, even if they are used in Mbed TLS code, or embedded in private structure types).
2. Remove selected features of Mbed TLS. This is mainly driven by the desire to stop maintaining some features that cannot be easily provided without legacy crypto APIs.
3. Expose selected legacy interfaces to fill some functionality gaps. In a few cases, this can also include new interfaces, generally tweaks on the legacy interface (e.g. replacing an `mbedtls_mpi*` argument of a public function by a byte array).

## Hiding low-level cryptography interfaces

### Deciding what to hide

#### Degrees of public interfaces

There are several potential criteria for whether an interface is part of the library's stable API. Roughly from the most permissive to the most restrictive:

* Declared in a public header.
* Declared in a public header, without a comment indicating that it's private or unstable.
* Present in the rendered documentation.
* Present in the rendered documentation, and not documented as unstable.

In [`BRANCHES.md`](https://github.com/Mbed-TLS/mbedtls/blob/development/BRANCHES.md#backwards-compatibility-for-application-code), we promise backward compatibility for “code that's working and secure with Mbed TLS x.y.z and does not rely on undocumented features”. That roughly corresponds to the most restrictive criteria.

#### Exposed interfaces

There are two main reasons why interfaces are declared in public headers, but not part of the stable API of the library:

* Historically, all non-static functions were declared in public headers. This started changing during the Mbed TLS 2.x era, and Mbed TLS 3.6.0 removed most of the remaining functions that were declared in a public header with a comment stating that they were not part of the API. However, some functions may have been missed, or may become non-desired in the API in 4.0/1.0.
* Several elements have to be visible to the compiler, even though they are not part of the public interface. In particular:
    * The definition of types, where we only promise the stability of the existence of the type, but not of how it is implemented. These are **opaque types**.
    * The existence of types whose purpose is to define opaque types. These have to be in a header that is visible to the compiler, but we don't want to make any stability commitment about them. For example, `mbedtls_aes_context` is needed to define the opaque type `psa_cipher_operation_t`. Let us call such types **exposed types**.
    * The implementation of static inline functions. This is generally not a problem since you can't do anything other than call the function anyway.
    * **Intermediate macros** which were only intended to define other macros and not intended to be stable. There are many such macros in `psa/crypto_values.h` and `psa/crypto_sizes.h`, for example. They are identified by not having Doxygen documentation, but this is discreet.

For the 0ε target, and likely even the 1.0/4.0 release, we aim to make the situation not worse than it is in 3.6. We don't plan to go on a hunt for ambiguous declarations in headers, but we should make sure we don't create new ambiguities. In particular, if an interface was part of the public API of Mbed TLS 3.6, but we don't consider it part of the public API of 1.0/4.0, we should make this clear, preferably by ensuring that applications won't compile if they try to use such interfaces.

#### Quasi-private interfaces of TF-PSA-Crypto

The separation between TF-PSA-Crypto and Mbed TLS adds another layer of complexity: some interfaces are considered private in TF-PSA-Crypto, but are still used in Mbed TLS. This includes code used in the library itself, or in test code, or in sample programs. This excludes code guarded by `!defined(MBEDTLS_USE_PSA_CRYPTO)` which is still present but never built. Given the engineering bandwidth available for the Mbed TLS 4.0 release, we know that we will not be able to eliminate such interfaces.

Note that private interfaces of TF-PSA-Crypto may only be used internally in Mbed TLS. They may not leak though the public interface of Mbed TLS. This is an objective for 0ε as well as for the 4.0 release. (Exposed interfaces of TF-PSA-Crypto may be exposed in Mbed TLS as well.)

It is a common use case that TF-PSA-Crypto is integrated on a platform as part of the basic board support package (BSP). It may be a lightweight integration with just a default configuration file and some platform customization, or a more elaborate integration such as TF-M which runs the bulk of the crypto library in a separate runtime environment. In the latter case, the source code of the customized crypto library might even not be present when compiling Mbed TLS.

To allow such builds, the interfaces of TF-PSA-Crypto that are used internally in Mbed TLS will be declared in public headers, but in such a way that applications cannot use them without going out of their way.

#### Categories of crypto headers

At the start of the 0ε work, all legacy headers of TF-PSA-Crypto are located under `drivers/*/include`. This includes the definitions of many exposed types, as well as many TF-PSA-Crypto interfaces used by Mbed TLS. Thus it is impossible to build TF-PSA-Crypto or Mbed TLS without having these headers in the include path (except maybe some configurations that exclusively use third-party PSA drivers).

For the 0ε target, we will separate headers that are only needed to compile TF-PSA-Crypto itself from headers that are needed when compiling applications or Mbed TLS. We distinguish three categories of headers:

* Public legacy headers: they contain public interfaces of TF-PSA-Crypto. For example, `"mbedtls/platform.h"` and `"mbedtls/asn1.h"`. Just move them wholesale.
* Exposed headers: these headers do not define any public interface, but they define exposed interfaces, typically types. For example, `"mbedtls/aes.h"` (where the type `mbedtls_aes_context` needs to be exposed, but the functions `mbedtls_aes_xxx()` are only meant to be used inside TF-PSA-Crypto). We will move these headers to a public directory, but ensure that exposed interfaces are clearly documented as such and that private interfaces are not casually usable by applications. See [“Hiding functions in an exposed header”](#hiding-functions-in-an-exposed-header).
* Purely private headers: these headers only define interfaces used to compile TF-PSA-Crypto itself, not interfaces that are exposed or that are used by Mbed TLS. In the long term, most mixed-use headers should be split into an exposed part (typically defining only types and perhaps macros) and a private part.

### Analysis of legacy crypto headers

#### Table of legacy crypto headers

The following table lists the headers that, as of the repository split, are located in `tf-psa-crypto/drivers/builtin/include/mbedtls/`. This is essentially the crypto or platform headers formerly in `include/mbedtls/` in Mbed TLS.

| Header | Function prefix | Fate | Notes |
| ------ | --------------- | ---- | ----- |
| `aes.h` | `mbedtls_aes_` | Expose | [context types](#headers-with-context-types) |
| `aria.h` | `mbedtls_aria_` | Expose | [context types](#headers-with-context-types) |
| `asn1.h` | `mbedtls_asn1_` | Public | [cryptography-adjacent](#cryptography-adjacent-headers) |
| `asn1write.h` | `mbedtls_asn1_write_` | Public | [cryptography-adjacent](#cryptography-adjacent-headers) |
| `base64.h` | `mbedtls_base64_` | TBD | [Base64 and PEM](#base64-and-pem) |
| `bignum.h` | `mbedtls_mpi_` | Expose | [context types](#headers-with-context-types) |
| `block_ciper.h` | `mbedtls_block_cipher_` | Expose | [context types](#headers-with-context-types) |
| `build_info.h` | `MBEDTLS_` | Exposed | [can be made fully private](#headers-that-can-be-made-fully-private) |
| `camellia.h` | `mbedtls_camellia_` | Expose | [context types](#headers-with-context-types) |
| `ccm.h` | `mbedtls_ccm_` | Expose | [context types](#headers-with-context-types) |
| `chacha20.h` | `mbedtls_chacha20_` | Expose | [context types](#headers-with-context-types) |
| `chachapoly.h` | `mbedtls_chachapoly_` | Expose | [context types](#headers-with-context-types) |
| `cipher.h` | `mbedtls_cipher_` | Expose | [context types](#headers-with-context-types) |
| `cmac.h` | `mbedtls_cipher_cmac_` | Expose | [context types](#headers-with-context-types) |
| `config_adjust_*.h` | N/A | Exposed | [Only for exposed macros ](#headers-that-remain-exposed-for-exposed-macros) |
| `config_psa.h` | N/A | Exposed | [Only for exposed macros ](#headers-that-remain-exposed-for-exposed-macros) |
| `constant_time.h` | `mbedtls_ct_` | Public | [cryptography-adjacent](#cryptography-adjacent-headers) |
| `ctr_drbg.h` | `mbedtls_ctr_drbg_` | Private | [Internal eventually](#headers-that-will-become-internal-eventually) |
| `des.h` | `mbedtls_des_` | Expose | [context types](#headers-with-context-types) |
| `dhm.h` | `mbedtls_dhm_` | Private | [can be made fully private](#headers-that-can-be-made-fully-private) |
| `ecdh.h` | `mbedtls_ecdh_` | Expose | [context types](#headers-with-context-types) |
| `ecdsa.h` | `mbedtls_ecdsa_` | Expose | [context types](#headers-with-context-types) |
| `ecjpake.h` | `mbedtls_ecjpake_` | Expose | [context types](#headers-with-context-types) |
| `ecp.h` | `mbedtls_ecp_` | Expose | [context types](#headers-with-context-types) |
| `entropy.h` | `mbedtls_entropy_` | Private | [Internal eventually](#headers-that-will-become-internal-eventually) |
| `error_common.h` | `mbedtls_*err*` | Private | [Internal eventually](#headers-that-will-become-internal-eventually) |
| `gcm.h` | `mbedtls_gcm_` | Expose | [context types](#headers-with-context-types) |
| `hkdf.h` | `mbedtls_hkdf_` | Delete | [Mbed-TLS/mbedtls#9150](https://github.com/Mbed-TLS/mbedtls/issues/9150) |
| `hmac_drbg.h` | `mbedtls_hmac_drbg_` | Private | [can be made fully private](#headers-that-can-be-made-fully-private) with a little work |
| `lms.h` | `mbedtls_lms_` | Public | [no PSA equivalent](#cryptographic-mechanisms-with-no-psa-equivalent) |
| `md.h` | `mbedtls_md_` | Expose | [context types](#headers-with-context-types), but likely [Public hash-only `md.h`](#public-hash-only-mdh) |
| `md5.h` | `mbedtls_md5_` | Expose | [context types](#headers-with-context-types) |
| `memory_buffer_alloc.h` | `mbedtls_memory_buffer_alloc_` | Public | [Platform headers](#platform-headers) |
| `nist_kw.h` | `mbedtls_nist_kw_` | Public | [no PSA equivalent](#cryptographic-mechanisms-with-no-psa-equivalent) |
| `oid.h` | `mbedtls_oid_` | Private | [OID interface](#oid-interface) |
| `pem.h` | `mbedtls_pem_` | TBD | [Base64 and PEM](#base64-and-pem) |
| `pk.h` | `mbedtls_pk_` | Public | [cryptography-adjacent](#cryptography-adjacent-headers) |
| `pkcs12.h` | `mbedtls_pkcs12_` | Private | [can be made fully private](#headers-that-can-be-made-fully-private) |
| `pkcs5.h` | `mbedtls_pkcs5_` | Private | [can be made fully private](#headers-that-can-be-made-fully-private) |
| `platform.h` | `mbedtls_platform_` | Public | [Platform headers](#platform-headers) |
| `platform_time.h` | `mbedtls_*time*` | Public | [Platform headers](#platform-headers) |
| `platform_util.h` | `mbedtls_platform_` | Public | [Platform headers](#platform-headers) |
| `poly1305.h` | `mbedtls_poly1305_` | Expose | [context types](#headers-with-context-types) |
| `private_access.h` | N/A | Exposed | [Only for exposed macros ](#headers-that-remain-exposed-for-exposed-macros) |
| `psa_util.h` | N/A | Public | [remains public](#headers-that-remain-public) but see [Private types in `psa_util.h`](#private-types-in-psa_utilh) |
| `ripemd160.h` | `mbedtls_ripemd160_` | Expose | [context types](#headers-with-context-types) |
| `rsa.h` | `mbedtls_rsa_` | Private | [can be made fully private](#headers-that-can-be-made-fully-private) with a little work |
| `sha1.h` | `mbedtls_sha1_` | Expose | [context types](#headers-with-context-types) |
| `sha256.h` | `mbedtls_sha256_` | Expose | [context types](#headers-with-context-types) |
| `sha3.h` | `mbedtls_sha3_` | Expose | [context types](#headers-with-context-types) |
| `sha512.h` | `mbedtls_sha512_` | Expose | [context types](#headers-with-context-types) |
| `threading.h` | `mbedtls_threading_` | Public | [Platform headers](#platform-headers) |
| `timing.h` | `mbedtls_timing_` | Delete | Move to mbedtls (done) |

For contributed drivers, see:

* Everest: Expose [Everest](#privatization-of-everest-headers).
* p256-m: [fully private](#headers-that-can-be-made-fully-private).

#### Cryptographic mechanisms with no PSA equivalent

The header files listed in this section define cryptographic mechanisms which do not currently fit well in the PSA API, are useful, and have an acceptable ad hoc interface. We will therefore keep this interface in TF-PSA-Crypto 1.x, possibly with minor tweaks to make them fit a PSA-only API. They may evolve later in the life of TF-PSA-Crypto 1.x.

* `lms.h`: The PSA API does not support stateful signatures yet. This is planned, but the API design is still under discussion. This is a critical feature in TF-A, hence considered necessary in the TF-PSA-Crypto MVP.
* `nist_kw.h`: The PSA API does not have an encoding for KW. It is under discussion, but it is mostly used to wrap key material or blobs containing key material, rather than to directly manipulate text, which complicates the API design. This is a request from many silicon vendors, hence considered necessary in the TF-PSA-Crypto MVP.

#### Cryptography-adjacent headers

The following header files define cryptography-adjacent interfaces which we have no plans to replace.

* `asn1.h`, `asn1write.h`: ASN.1, needed for key parsing/writing as well as for X.509.
* `constant_time.h`: This header defines `mbedtls_ct_memcmp()` which is in the public API because it is useful to application code (including but not limited to the TLS layer in Mbed TLS).
* `pk.h`: There is no equivalent PSA API. (One is planned, but the design won't be ready until after 1.0.) This is critical for parsing and writing keys. We plan to keep parts of the existing `pk.h` for parsing, writing and signature, and to remove `mbedtls_pk_type_t`, encrypt/decrypt and a few other bits. For 0ε, `pk.h` goes into the public category, and we will remove parts of it. Continued in https://github.com/Mbed-TLS/mbedtls/issues/8452 .

We will therefore keep those headers public in TF-PSA-Crypto 0ε and 1.x.

#### Base64 and PEM

Base64 and PEM are cryptography-adjacent interfaces which we have no plans to replace. In particular, they are outside the scope of PSA APIs.

PEM is used:

* Inside the crypto library, to parse and write keys.
* Inside the Mbed TLS library, to parse and write X.509 objects.
* In application code, very occasionally. (Examples: [Fire-evacuation-guidance-system-IoT](https://github.com/2nd-Chance/Fire-evacuation-guidance-system-IoT/blob/33031a8255fe1ae516ddd58f1baa808801cd3abf/iotivity/resource/csdk/security/src/credresource.c#L3185) (dead project), [SiLabs Bluetooth attestation server](https://github.com/SiliconLabs/bluetooth_applications/blob/3eb0f3c9e234ada1f10714fb9376fcbc8e95807f/bluetooth_secure_attestation/bt_secure_attestation_server/src/ecdh_util3.c#L375))

The use of PEM inside the Mbed TLS library is intrinsic. It doesn't leak through the API of Mbed TLS, but Mbed TLS cannot be implemented without PEM. This is different from other private modules that Mbed TLS currently calls internally, but will no longer need to call once Mbed TLS has fully migrated to PSA. Thus, in the long term, TF-PSA-Crypto needs to expose its PEM API to Mbed TLS. (We reject the hypotheses of independent PEM implementations, or of making PEM its own library, as too much maintenance work.)

The current PEM interface is unsatisfactory. We would like to improve it (https://github.com/Mbed-TLS/mbedtls/issues/9374) but it is unlikely that we will have enough bandwidth to do so before the 1.0 release. We need to make the PEM interface public to reach the milestone where Mbed TLS stops relying on private interfaces of TF-PSA-Crypto. We can choose to make it public now, or wait until later. Waiting is only advantageous if we believe that we will have enough bandwidth to actually clean up the PEM interface.

Base64 is used:

* Inside the crypto library, to implement PEM.
* In the `pem2der` sample program.
* In Mbed TLS SSL test programs, for context serialization. (It's not clear to me why we encode in Base64 rather than binary.)
* In the Mbed TLS sample program `ssl_mail_client`, for a tiny bit of SMTP that requires Base64.
* In application code, sometimes, for miscellaneous things, often not directly related to cryptography. On the one hand, many of these uses are out of scope. On the other hand, since TF-PSA-Crypto has a Base64 implementation anyway, users who like TF-PSA-Crypto for its small code size would be justifiably disappointed not to have a Base64 interface.

Conclusion: we will keep the existing `pem.h` and `base64.h` as public interfaces in TF-PSA-Crypto 0ε. If we have time before the 1.0 release, we will improve the APIs.

#### OID interface

`oid.h` and `MBEDTLS_OID_C` are in charge of defining all the OIDs used internally or by application code. This is a known problem which can result in applications wasting code size on many OIDs that they don't care about. We are planning a redesign: https://github.com/Mbed-TLS/mbedtls/issues/9380 .

At this point, we do not consider this critical for TF-PSA-Crypto 1.0, and we are likely to work on it during the lifetime of TF-PSA-Crypto 1.x. This may change if we reevaluate the priority based on concrete use cases. In the meantime, `oid.h` will become private, but will remain used in Mbed TLS.

#### Platform headers

Due to a lack of bandwidth, we are not planning any major changes to most platform interfaces. As of the 0ε target, the following platform-related headers remain public.

* `memory_buffer_alloc.h`: a feature used by some applications, with close to 0 maintenance cost for us.
* `platform.h`, `platform_time.h`, `platform_util.h`: platform abstractions. We would like to adapt them, but we do not think we will have time in before 1.0, so this will have to wait until the next major version.
* `threading.h`: We want to change the threading abstraction, but this is out of scope of the 0ε project. https://github.com/Mbed-TLS/mbedtls/issues/8455

### Plans for hiding

In an exposed header, the minimum work for 0ε is:

* Ensure that exposed interfaces are not listed in the rendered Doxygen documentation.
* Ensure that applications cannot inadvertently call private functions that are declared in exposed headers.

Note that due to a lack of bandwidth, sample programs may keep using private interfaces.

#### TF-PSA-Crypto header locations

We distinguish between four categories of headers:

* Public headers define public, stable APIs.
* Exposed headers define exposed interfaces, as well as private interfaces used by Mbed TLS. They need to be present when building Mbed TLS, but their content must clearly be excluded from the stable API.
* Private headers do not define any exposed interfaces, but they are used by Mbed TLS for the time being. They need to be present when building Mbed TLS, but their content must clearly be excluded from the stable API.
* Internal headers are only needed when building TF-PSA-Crypto itself. Their content must clearly be excluded from the stable API.

The following table summarizes the characteristics of each category and their location in TF-PSA-Crypto 0ε.

| Category | Location | Installed? | Doxygen? | Visible to Mbed TLS? |
| -------- | -------- | ---------------- |
| Public   | `include/mbedtls` | yes | yes | yes |
| Exposed  | `drivers/*/include/mbedtls/private` | yes | no | yes |
| Private  | `drivers/*/include/mbedtls/private` | yes for now | no | yes |
| Internal | anything except `**/include/mbedtls/*.h` | no | no | no |

We will arrange so that:

* Public interfaces are available to consuming projects and visible in the rendered documentation.
* Exposed and private interfaces are available to consuming projects, but not visible in the rendered documentation and clearly not public for someone who reads the header files themselves. It would be convenient to have separate locations for permanently exposed interfaces (e.g. types appearing in operation contexts) and temporarily exposed interfaces (legacy functions that Mbed TLS is still using), but this is not necessry.
* Internal interfaces are available only inside TF-PSA-Crypto, not to consuming projects. Like exposed interfaces, the documentation (rendered or by direct header reading) clearly conveys that they are not part of the API.

#### Hiding functions in an exposed header

For TF-PSA-Crypto 0ε, all private functions defined in private headers will be guarded by `defined(MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS)`. We define this macro when building TF-PSA-Crypto and Mbed TLS, but applications should not define it, and our Doxygen build will not define it.

This is similar to `MBEDTLS_ALLOW_PRIVATE_ACCESS` to “bless” access to structure fields.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/219): in TF-PSA-Crypto, in `private_access.h`, define `MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS` when `MBEDTLS_ALLOW_PRIVATE_ACCESS` is defined. Add  `#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS` just before the first `#include` in every program under `programs` that calls `mbedtls_xxx` functions (just `benchmark.c` at the time of writing).

Since `MBEDTLS_ALLOW_PRIVATE_ACCESS` implies `MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS`, and `tf_psa_crypto_common.h` enables `MBEDTLS_ALLOW_PRIVATE_ACCESS`, parts of the code base that already allow access to private structure fields (library, unit tests) will automatically have access to private functions.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10082): in Mbed TLS, add `#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS` just before the first `#include` in every program under `programs`, except the ones that already have `MBEDTLS_ALLOW_PRIVATE_ACCESS`. The reason to add it program by program is that later we can validate that an individual program no longer requires private identifiers by removing `#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS` from that particular program. This is a prerequisite for any privatization in TF-PSA-Crypto that privatizes something used in Mbed TLS.

Prototype: https://github.com/Mbed-TLS/TF-PSA-Crypto/pull/132

Thus, for the 0ε milestone, in each affected header, we just need to place `#ifdef` guards around the function declarations.

Rationale:

* This is a small amount of work.
* This requires few changes to the code, and they are very localized, so it will not disrupt other work happening in parallel.

Additionally, each header may itself be public or not: do we still want applications to `#include <mbedtls/foo.h>`? A header that contains exposed declarations must be exposed, but does not need to be public. See [Move private headers](#move-private-headers).

#### Separating private interfaces from exposed interfaces

If a private interface of TF-PSA-Crypto is declared in an exposed header, in the medium term, we should move it to a private header. Note that we can only do that if the interface is not called from Mbed TLS code.

At some point, perhaps after the 1.0 release, we expect that all the functions declared in an exposed header will be private and will not be called by Mbed TLS. That point may be reached at different times for different headers. When we reach that point for a header, we can run a script to move the declarations guarded by `MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS` to an internal header file and adjust `#include` directives accordingly.

### Analysis of privatization by header

#### Headers that remain public

The headers listed below declare functionality that has no PSA equivalent and that is desirable in 1.0/4.0. We will just move these headers to the public include directory. We may make further changes to some of these headers, but it is out of scope of this chapter.

```
asn1.h
asn1write.h
base64.h
constant_time.h
lms.h
memory_buffer_alloc.h
nist_kw.h
pem.h
pk.h
platform.h
platform_time.h
platform_util.h
psa_util.h
threading.h
```

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/223): Move TF-PSA-Crypto public headers to `/include/mbedtls`.

#### Headers that remain exposed for exposed macros

The following headers solely define exposed macros, and must remain exposed. They can be excluded from Doxygen parsing, but they shouldn't contain Doxygen comments anyway, and currently mostly don't apart from `\file` comments.

```
config_adjust_legacy_from_psa.h
config_adjust_psa_superset_legacy.h
config_adjust_test_accelerators.h
config_psa.h
private_access.h
```

#### Headers with context types

The headers listed below are used in operation context types. The types that they define must remain exposed, and possibly some macros as well. The functions that they declare will be made private by guarding them with `MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS`.

```
aes.h
aria.h
bignum.h
block_cipher.h
camellia.h
ccm.h
chacha20.h
chachapoly.h
cipher.h
cmac.h
des.h
ecdh.h
ecdsa.h
ecjpake.h
ecp.h
gcm.h
md.h
md5.h
poly1305.h
ripemd160.h
sha1.h
sha256.h
sha3.h
sha512.h
```

Main loss of functionality:

* Self-test functions. See [PSA self-test interface](#psa-self-test-interface)
* Access to bignum and ECC arithmetic. We've decided that this is acceptable.

Note: see also [Everest](#privatization-of-everest-headers).

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/224): Privatize functions in low-level hash headers.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/225: Privatize functions in cipher primitive headers.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/226): Privatize functions in cipher mode headers.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/227): Privatize functions in low-level asymmetric crypto headers.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/220): Privatize functions in `cipher.h`.

See [Public hash-only `md.h`](#public-hash-only-mdh) regarding `md.h`.

#### Headers that can be made fully private

The headers listed below are not used in Mbed TLS, except in places that should be removed and can be removed easily.

```
build_info.h
dhm.h
hmac_drbg.h
pkcs12.h
pkcs5.h
rsa.h
```

Places where some of these headers are used:

* `library/ssl_*.c` (for DHM in TLS 1.2, which is going to be removed: https://github.com/Mbed-TLS/mbedtls/issues/9685)
* `programs/fuzz/fuzz_*key.c` (to fuzz RSA functions that are now private)
* `programs/ssl/ssl_test_lib.c` (HMAC\_DRBG; can move to only using the PSA RNG)
* `programs/test/benchmark.c` (HMAC\_DRBG)
* `programs/test/selftest.c`
* `scripts/data_files/query_config.fmt`

Main loss of functionality:

* Finite-field Diffie-Hellman with arbitrary groups. We've decided that this is acceptable. See “[Removal of low-level DHM](#removal-of-low-level-dhm)”.
* Custom RSA mechanisms. We've decided that this is acceptable.
* PKCS5 and PKCS12 mechanisms except as exposed by the pk module. We've decided that this is acceptable.
* HMAC\_DRBG in itself (i.e. outside of deterministic ECDSA and for the PSA Crypto RNG instance). We intend to restore this functionality through a PSA API, but the API isn't designed yet, so this will happen after 1.0 and not with the existing API.

`drivers/builtin/include/mbedtls/build_info.h` is a special case that exists only as a transition for the sake of our source files contains `#include <mbedtls/build_info.h>` and that must be buildable against either TF-PSA-Crypto or Mbed TLS. It should be removed: https://github.com/Mbed-TLS/mbedtls/issues/9862 .

We will make these headers internal after 0ε. For 0ε, they can remain private.

#### Headers that will become internal eventually

The headers listed below should be private, but are currently used in Mbed TLS to an extent that makes it hard to remove before the 1.0/4.0 release. As a result, they need to remain exposed to Mbed TLS, but should be clearly indicated as not part of the stable API.

```
ctr_drbg.h
entropy.h
error_common.h
oid.h
```

Main loss of functionality:

* CTR\_DRBG in itself (i.e. other than for the PSA Crypto RNG instance). We intend to restore this functionality through a PSA API, but the API isn't designed yet, so this will happen after 1.0 and not with the existing API.
* Direct access to entropy sources. We've decided that this is acceptable.
* The ability to configure entropy sources on a platform. This is not an acceptable loss. In the long term (likely after 1.0), this will be resolved by the PSA crypto random driver API. In the short term, we will expose a modified `mbedtls_hardware_poll()` (https://github.com/Mbed-TLS/mbedtls/issues/9618) through `mbedtls/platform.h`.
* `MBEDTLS_ERROR_ADD` will no longer be used after https://github.com/Mbed-TLS/mbedtls/pull/9926 .

We will make these headers internal after 0ε. For 0ε, they can remain private.

#### Move private headers

Move private and exposed headers so that they are always under a subdirectory called `.../private`. This is a simple way of conveying that their content is not part of the public API both to humans and to programs. In particular, this makes it clear to human readers that the header's content is not part of the public API, even after the headers are installed. (Merely relying on the layout of the source tree does not help after installation.)

Note that public headers can include private headers, since some private headers define exposed types and macros.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10087): Move TF-PSA-Crypto private headers from `drivers/builtin/include/mbedtls` to `drivers/builtin/include/mbedtls/private`. Move mbedtls private headers from `include/mbedtls` to `include/mbedtls/private`. This requires coordinated action between the three repositories.

OPEN: is this really necessary? If we don't do this, attempts to call exposed functions will fail thanks to `MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS` (see “[Hiding functions in an exposed header](#hiding-functions-in-an-exposed-header)”). But this preprocessor guard is discrete and users are likely to miss it, leading to frustration when their code mysteriously doesn't compile.

#### Privatization of Everest headers

Everest headers (`drivers/everest/include/everest/include/**/*.h`) contain some exposed types: they are exposed via `mbedtls_ecdh_context` from `mbedtls/ecdh.h` which is exposed via `mbedtls_psa_key_agreement_interruptible_operation_t` indirectly from `psa/crypto.h`. The rest of their content is private (to be consumed only by `ecdh.c`) or internal (to be consumed only by `everest/**/*.c`) definitions.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/230): For 0ε, guard everything that isn't an exposed type (or necessary macros, if any) by `MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS`.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/229): For 0ε, move Everest headers so that they are installed under `$PREFIX/include/tf-psa-crypto/private`.

#### Privatization of 256-m headers

P256-m headers only declare private functions (called by PSA driver wrappers). They do not expose anything. So they can be made internal to TF-PSA-Crypto.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/228): do not install `p256/p256-m.h`.

### Renaming mbedtls interfaces that remain public

Some existing `mbedtls_xxx` interfaces declared in `<mbedtls/*.h>` headers will remain public in TF-PSA-Crypto 1.0 (e.g. `mbedtls_asn1_xxx` from `<mbedtls/asn1*.h`, `mbedtls_pk_xxx` from `<mbedtls/pk.h>`). Arguably, since they are no longer in Mbed TLS but in TF-PSA-Crypto, the interfaces should be renamed to `tfc_xxx` in `<tf-psa-crypto/*.h>`.

Prioritization:

* Feature: irrelevant.
* Maintainability: negative, since it would complicate backports.
* Quality: dubious. It's better for new users to have API names that reflect the product names. But it's worse for existing users who would have more code to rewrite.

At the moment, renaming existing interfaces (headers files, types, functions, macros, etc.) is out of scope of 0ε.

## Private interfaces leaking through public interfaces

Public interface elements must be usable without requiring private interfaces. Concretely, a public function must not have an argument type or a return type that uses a private type.

### Legacy type report

Output of `scripts/legacy_report.py -DMBEDTLS_USER_CONFIG_FILE='<../tests/configs/user_config_no_deprecated.h>' -I include -I tf-psa-crypto/include -I tf-psa-crypto/drivers/builtin/include include/mbedtls/*.h tf-psa-crypto/drivers/builtin/include/mbedtls/{asn1.h,asn1write.h,base64.h,constant_time.h,lms.h,memory_buffer_alloc.h,nist_kw.h,pem.h,pk.h,platform.h,platform_time.h,platform_util.h,psa_util.h,threading.h}` from https://github.com/gilles-peskine-arm/mbedtls/tree/legacy-unstable-headers-detect

```
tf-psa-crypto/drivers/builtin/include/mbedtls/pk.h:1035:36: mbedtls_pk_rsa#return: mbedtls_rsa_context *
tf-psa-crypto/drivers/builtin/include/mbedtls/pk.h:1058:36: mbedtls_pk_ec#return: mbedtls_ecp_keypair *
tf-psa-crypto/drivers/builtin/include/mbedtls/asn1.h:543:39: mbedtls_asn1_get_mpi#3=X: mbedtls_mpi *
include/mbedtls/ssl_ticket.h:126:52: mbedtls_ssl_ticket_setup#4=cipher: mbedtls_cipher_type_t
tf-psa-crypto/drivers/builtin/include/mbedtls/asn1write.h:104:47: mbedtls_asn1_write_mpi#3=X: const mbedtls_mpi *
tf-psa-crypto/drivers/builtin/include/mbedtls/nist_kw.h:78:48: mbedtls_nist_kw_setkey#2=cipher: mbedtls_cipher_id_t
tf-psa-crypto/drivers/builtin/include/mbedtls/psa_util.h:87:64: mbedtls_ecc_group_to_psa#1=grpid: mbedtls_ecp_group_id
tf-psa-crypto/drivers/builtin/include/mbedtls/psa_util.h:102:22: mbedtls_ecc_group_from_psa#return: mbedtls_ecp_group_id
```

### Private types in `pk.h`

`pk.h` only exposes private types through deprecated functions which will be removed from the [Shrunk-down `pk.h`](#shrunk-down-pkh).

### Private types in `asn1.h` and `asn1write.h`

The ASN.1 interfaces use `mbedtls_mpi` for INTEGER parsing/writing. This must change to a byte array. This is filed as https://github.com/Mbed-TLS/mbedtls/issues/9373 and https://github.com/Mbed-TLS/mbedtls/issues/9372 .

### Private types in `nist_kw.h`

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/9382): `nist_kw.h` must switch from a legacy cipher ID to a PSA key type.

### Private types in `psa_util.h`

The functions `mbedtls_ecc_group_to_psa()` and `mbedtls_ecc_group_from_psa()` are no longer relevant for public use since the legacy side of the conversion is no longer a public interface. They are not used in Mbed TLS. They should be moved to an internal header.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/231): Move the declarations of `mbedtls_ecc_group_to_psa()` and `mbedtls_ecc_group_from_psa()` to `pk_internal.h`.

### Private types in `ssl_ticket.h`

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/9874): `ssl_ticket.h` uses a legacy cipher type to specify the AEAD mechanism to use for tickets. Switch to a PSA key type and algorithm.

### Leaking error codes

#### About leaking error codes

Non-PSA functions signal errors by returning an `MBEDTLS_ERR_xxx` error code. They return the type `int`, so this does not appear in type-based analyses.

In Mbed TLS 3, all error codes are declared and documented in public headers. Since all error codes are part of the official API, functions cannot return undocumented error codes, by construction. (This assumes that we use `MBEDTLS_ERR_xxx` constants when we decide which error code to return, which we do.)

Many of the error codes in TF-PSA-Crypto are declared in headers that are becoming private, either exposed or not. These error codes are generally intended for functions declared in the same module, however they may also be used in other modules, in particular when module A calls functions from module B and forwards error codes from module B. This can cause three problems:

* Mbed TLS code checks for specific error codes from crypto functions. These error codes must remain exposed.
* Specific error codes appear in TF-PSA-Crypto or Mbed TLS documentation. These error codes are thus effectively public, and the declaration of the `MBEDTLS_ERR_xxx` constant should remain public.
* Public functions return error codes that are now private, thus undocumented. This is a quality problem for users. It is not critical to resolve this problem before the 1.0 release, because if the specific error code is not documented, a future minor release can either start documenting the error code or change the function's behavior to make it return a documented error code instead.

#### Assumptions on error codes

In Mbed TLS up to 3.x, all error codes are values between -32767 and -1, with 0 meaning success. There are two types of status codes:

* `psa_status_t`, returned by PSA functions (`psa_xxx()` or `mbedtls_psa_xxx()`). Success is `PSA_SUCCESS == 0` and errors are `PSA_ERROR_xxx` constants.
* `int`, returned by Mbed TLS functions (`mbedtls_xxx()` except `mbedtls_psa_xxx()`). Success is 0. Mbed TLS error codes (legacy errors) have the form `low + high` where `-127 <= low <= 0` and `high` is either 0 or of the form `-128 * n` for `16 <= n <= 255` (this is an unambiguous decomposition) (`low == high == 0` is the success value).

Having to convert between the two error code spaces costs significant code size. It also adds complexity and we occasionally have bugs where we forget to convert. Thus it would be desirable not to have this distinction.

It is already the case in Mbed TLS 3.x that a PSA error value cannot be equal to a legacy error, because we avoid assigning values in a range that would overlap. (PSA error codes are in the range [-255, -128], while legacy error codes are either in the range [-127, -1] (low) or [-32767, -4096] (high or high+low).)

In many places, a high-level module adds a constant to the error code returned by a low-level module. This requires the low-level module to return a low-level error (where the high part is 0). If the low-level module changes to return a PSA error, the addition will not result in an unambiguous value.

#### Unified error code space

To avoid constraining the refactoring of low-level interfaces and low-level modules, as well as simplify the rework of high-level modules, we will do a little upfront work to unify the error code space: https://github.com/Mbed-TLS/mbedtls/issues/9619#issuecomment-2612664288

Once that is done:

* `MBEDTLS_ERROR_ADD` is a no-op (it just returns one of its arguments — currently the low-level code). New code does not need to bother.
* Any legacy error code can be removed from the public documentation and made an alias of a PSA error code.
* New code can just pick PSA error codes when convenient.
* All conversions between error codes can be removed. (This can, but probably shouldn't, be done all at once. If done piecewise, this needs to be divided adequately in the sense that when a function stops converting its error codes, its callers must stop expecting converted error codes.)

#### Publicly documented private error codes

Output of `grep -P 'MBEDTLS_ERR_(?!ASN1_|BASE64_|LMS_|NET_|NIST_KW_|PEM_|PKCS7_|PK_|PLATFORM_|SSL_|THREADING_|X509_|XXX)' include/mbedtls/*.h tf-psa-crypto/drivers/builtin/include/mbedtls/{asn1.h,asn1write.h,base64.h,constant_time.h,lms.h,memory_buffer_alloc.h,nist_kw.h,pem.h,pk.h,platform.h,platform_time.h,platform_util.h,psa_util.h,threading.h}`:

```
include/mbedtls/x509.h: *                  MBEDTLS_ERR_OID_BUF_TOO_SMALL in case of error
include/mbedtls/x509_crt.h: * \return         #MBEDTLS_ERR_ECP_IN_PROGRESS if maximum number of
tf-psa-crypto/drivers/builtin/include/mbedtls/nist_kw.h: * \return          \c MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA for any invalid input.
tf-psa-crypto/drivers/builtin/include/mbedtls/nist_kw.h: * \return          \c MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE for 128-bit block ciphers
tf-psa-crypto/drivers/builtin/include/mbedtls/nist_kw.h: * \return          \c MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA for invalid input length.
tf-psa-crypto/drivers/builtin/include/mbedtls/nist_kw.h: * \return          \c MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA for invalid input length.
tf-psa-crypto/drivers/builtin/include/mbedtls/nist_kw.h: * \return          \c MBEDTLS_ERR_CIPHER_AUTH_FAILED for verification failure of the ciphertext.
tf-psa-crypto/drivers/builtin/include/mbedtls/pk.h: * \return          #MBEDTLS_ERR_ECP_IN_PROGRESS if maximum number of
tf-psa-crypto/drivers/builtin/include/mbedtls/pk.h: * \return          #MBEDTLS_ERR_ECP_IN_PROGRESS if maximum number of
tf-psa-crypto/drivers/builtin/include/mbedtls/psa_util.h: * \return              An `MBEDTLS_ERR_ENTROPY_xxx`,
tf-psa-crypto/drivers/builtin/include/mbedtls/psa_util.h: *                      `MBEDTLS_ERR_CTR_DRBG_xxx` or
tf-psa-crypto/drivers/builtin/include/mbedtls/psa_util.h: *                      `MBEDTLS_ERR_HMAC_DRBG_xxx` on error.
```

This tells us which error codes are documented in public headers but defined in private headers. Note that there are likely to be many error codes that are not specifically documented (or only vaguely, e.g. “a specific X509 or PEM error code”), but in such cases we can change which error is returned in a minor release.

To fix these cases:

* Make `MBEDTLS_ERR_OID_BUF_TOO_SMALL` an alias of `PSA_ERROR_BUFFER_TOO_SMALL`.
* Make `MBEDTLS_ERR_ECP_IN_PROGRESS` an alias of `PSA_OPERATION_INCOMPLETE`.
* When moving NIST\_KW to the PSA API (https://github.com/Mbed-TLS/mbedtls/issues/9382), for error conditions that are discovered inside `nist_kw.c`, make it return PSA error codes. Errors from lower-level functions can just be propagated without conversion.
* In `psa_util.h`, the documented legacy error code categories are for `mbedtls_psa_get_random`, which is no longer needed. See [Shrunk-down `psa_util.h`](#shrunk-down-psa_utilh).

#### Unifying error codes

Ideally we should unify legacy error codes with PSA error codes that have the same meaning. Note that this includes unifying legacy error codes since these tend to be per-module. For example, ideally, all `MBEDTLS_ERR_xxx_ALLOC_FAILED` errors would change to `PSA_ERROR_INSUFFICIENT_MEMORY`.

A legacy error name (`MBEDTLS_ERR_xxx`) can be made an alias for a PSA error: just change `#define MBEDTLS_ERR_xxx -0xNNN` to `#define MBEDTLS_ERR_xxx PSA_ERROR_yyy`. https://github.com/Mbed-TLS/mbedtls/pull/9926 does a few to show that it works.

See https://github.com/Mbed-TLS/mbedtls/issues/8501 . This is beyond the code of 0ε and 4ε, but we may do some of it on an ad hoc basis.

Note that if we want to be able to remove all error code translations (to save code size and complexity), we need to use PSA errors in all low-level modules, including ones that remain public such as ASN.1. In a 1.x minor release, we can declare that `MBEDTLS_ERR_xxx` is now a PSA error by giving it a new name `PSA_ERROR_xxx`, but not by making it an alias of an existing `PSA_ERROR_xxx`, since aliasing errors is an API break.

## Changes to compilation options

### Strategy for removing a compilation option

#### Types of compilation option removals

There are ways in which a compilation option can be removed from the public interface:

* The option is no longer referenced in the code base. Setting it is harmless.
* The option is still present in the code base, but now implied by other options. This is notably the case for legacy crypto options that are now always controlled via `PSA_WANT_xxx` (`MBEDTLS_PSA_CRYPTO_CONFIG` always on). Setting it may cause the configuration to be inconsistent, so we should complain if the option is set through a configuration file (`mbedtls/mbedtls_config.h`, `psa/crypto_config.h` or similar) rather than internally (in `*/config_adjust*.h`).
* The option is still present in the code base and not set automatically, but it is not part of the public interface, and will be removed in 1.x/4.x. This happens when we remove a feature that tests still rely on (for example, directly removing some small elliptic curves ([#8136](https://github.com/Mbed-TLS/mbedtls/issues/8136)) would remove some test coverage). We should complain if the option is set, unless it is set from our own test scripts.

#### How to remove a legacy configuration option

Goal: complain if a user tries to set an option in the configuration file (`mbedtls/mbedtls_config.h`, `psa/crypto_config.h` or similar), for options that existed in 3.x but have become internal in 1.0/4.0.

This concerns, in particular, legacy crypto options that are now always controlled via `PSA_WANT_xxx` (`MBEDTLS_PSA_CRYPTO_CONFIG` always on).

Note that users may be setting crypto options in the Mbed TLS configuration, which is included after the config adjustments in TF-PSA-Crypto have set some legacy symbols. We need a complaint if TF-PSA-Crypto's `build_info.h` does not set some legacy option and the user's `mbedtls_config.h` then sets it. How can we do this?

* Remember the state of all implied options before including the Mbed TLS configuration, then check if it has changed. This requires a lot of boilerplate. This doesn't cause a complaint if the Mbed TLS configuration sets an implied option redundantly, only if it sets an implied option that should no longer be set.
* Set implied options to an expansion that is a nonzero integer (e.g. 0xdeadc0f1) when doing config adjustments. After reading the Mbed TLS configuration, reject implied options that are defined but false (which is the case for `#if FOO` when `FOO` is defined with an empty expansion). This causes a complaint if the Mbed TLS configuration sets an option redundantly, but the reason for the complaint is not clear from the error message (macro redefinition warning).
* Rename all implied options internally. This would be ideal, but it's a lot of work, including work in the framework, so it is not achievable for 0ε.
* Temporarily rename implied options around the inclusion of the Mbed TLS configuration (e.g. `#ifdef FOO #define REMEMBER_FOO #undef FOO #endif` before, and `#ifdef FOO #error #endif #ifdef REMEMBER_FOO #define FOO #endif` after). This requires a lot of boilerplate.
* Split `build_info.h` differently so that we can have all user configuration, then the removed-option checks, then the config adjustments. This doesn't seem doable since the application code may have included a crypto header before any mbedtls header.

Requiring a lot of boilerplate is not much of a problem. The code follows a systematic pattern and can be generated easily. This code is unlikely to change throughout 1.x/4.x, so we can generate it and check it in as part of the 0ε work.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10147): in `build_info.h`, after including the user configuration file but before the configuration adjustments, include a header that has `#error` directives for each preprocessor symbol that was a configuration option in 3.6 but no longer is in 1.0/4.0. Suggested header names:

* `mbedtls/check_config_removed.h`
* `tf-psa-crypto/check_config_removed.h`

#### How to remove a feature option from the interface

Goal: have a way for an option to be forbidden to users, but allowed in our test scripts.

This concerns options controlling features that we want to remove, but that we still rely on. For example DES to decrypt some test keys, or small elliptic curves used in some curve-agnostic testing.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10147): define a new option in `psa/crypto_config.h` called `TF_PSA_CRYPTO_ALLOW_REMOVED_FEATURES`, disabled by default. Let it be enabled in the `full` configuration. Add a section in `psa/crypto_config.h` for removed options with a comment explaining that the options in this section are not part of the interface. In `config.py`, exclude this whole section from the `realfull` configuration.

Then, to remove a feature option `FOO` from the interface without removing it from the code base:

* Keep `//#define FOO` in `psa/crypto_config.h`, but remove its documentation and move the line to the removed features section.
* In any `all.sh` component that enables the option from the default configuration rather than from the `full` configuration, explicitly enable `TF_PSA_CRYPTO_ALLOW_REMOVED_FEATURES`.
* We should not do this for any option that is enabled in a sample configuration.

### Analysis of legacy crypto options

#### List of boolean legacy crypto options

Legacy crypto boolean options: `perl -l -ne 'print $1 if /#define +(MBEDTLS_\w+)($| *\/)/' tf-psa-crypto/include/psa/crypto_config.h | sort`

```
MBEDTLS_AESCE_C
MBEDTLS_AESNI_C
MBEDTLS_AES_C
MBEDTLS_AES_FEWER_TABLES
MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH
MBEDTLS_AES_ROM_TABLES
MBEDTLS_AES_USE_HARDWARE_ONLY
MBEDTLS_ARIA_C
MBEDTLS_ASN1_PARSE_C
MBEDTLS_ASN1_WRITE_C
MBEDTLS_BASE64_C
MBEDTLS_BIGNUM_C
MBEDTLS_BLOCK_CIPHER_NO_DECRYPT
MBEDTLS_CAMELLIA_C
MBEDTLS_CAMELLIA_SMALL_MEMORY
MBEDTLS_CCM_C
MBEDTLS_CHACHA20_C
MBEDTLS_CHACHAPOLY_C
MBEDTLS_CHECK_RETURN_WARNING
MBEDTLS_CIPHER_C
MBEDTLS_CIPHER_MODE_CBC
MBEDTLS_CIPHER_MODE_CFB
MBEDTLS_CIPHER_MODE_CTR
MBEDTLS_CIPHER_MODE_OFB
MBEDTLS_CIPHER_MODE_XTS
MBEDTLS_CIPHER_NULL_CIPHER
MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
MBEDTLS_CIPHER_PADDING_PKCS7
MBEDTLS_CIPHER_PADDING_ZEROS
MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
MBEDTLS_CMAC_C
MBEDTLS_CTR_DRBG_C
MBEDTLS_CTR_DRBG_USE_128_BIT_KEY
MBEDTLS_DEPRECATED_REMOVED
MBEDTLS_DEPRECATED_WARNING
MBEDTLS_DES_C
MBEDTLS_DHM_C
MBEDTLS_ECDH_C
MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED
MBEDTLS_ECDSA_C
MBEDTLS_ECDSA_DETERMINISTIC
MBEDTLS_ECJPAKE_C
MBEDTLS_ECP_C
MBEDTLS_ECP_DP_BP256R1_ENABLED
MBEDTLS_ECP_DP_BP384R1_ENABLED
MBEDTLS_ECP_DP_BP512R1_ENABLED
MBEDTLS_ECP_DP_CURVE25519_ENABLED
MBEDTLS_ECP_DP_CURVE448_ENABLED
MBEDTLS_ECP_DP_SECP192K1_ENABLED
MBEDTLS_ECP_DP_SECP192R1_ENABLED
MBEDTLS_ECP_DP_SECP224K1_ENABLED
MBEDTLS_ECP_DP_SECP224R1_ENABLED
MBEDTLS_ECP_DP_SECP256K1_ENABLED
MBEDTLS_ECP_DP_SECP256R1_ENABLED
MBEDTLS_ECP_DP_SECP384R1_ENABLED
MBEDTLS_ECP_DP_SECP521R1_ENABLED
MBEDTLS_ECP_NIST_OPTIM
MBEDTLS_ECP_RESTARTABLE
MBEDTLS_ECP_WITH_MPI_UINT
MBEDTLS_ENTROPY_C
MBEDTLS_ENTROPY_FORCE_SHA256
MBEDTLS_ENTROPY_HARDWARE_ALT
MBEDTLS_ENTROPY_NV_SEED
MBEDTLS_FS_IO
MBEDTLS_GCM_C
MBEDTLS_GCM_LARGE_TABLE
MBEDTLS_GENPRIME
MBEDTLS_HAVE_ASM
MBEDTLS_HAVE_SSE2
MBEDTLS_HAVE_TIME
MBEDTLS_HAVE_TIME_DATE
MBEDTLS_HKDF_C
MBEDTLS_HMAC_DRBG_C
MBEDTLS_LMS_C
MBEDTLS_LMS_PRIVATE
MBEDTLS_MD5_C
MBEDTLS_MD_C
MBEDTLS_MEMORY_BACKTRACE
MBEDTLS_MEMORY_BUFFER_ALLOC_C
MBEDTLS_MEMORY_DEBUG
MBEDTLS_NIST_KW_C
MBEDTLS_NO_64BIT_MULTIPLICATION
MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
MBEDTLS_NO_PLATFORM_ENTROPY
MBEDTLS_NO_UDBL_DIVISION
MBEDTLS_OID_C
MBEDTLS_PEM_PARSE_C
MBEDTLS_PEM_WRITE_C
MBEDTLS_PKCS12_C
MBEDTLS_PKCS1_V15
MBEDTLS_PKCS1_V21
MBEDTLS_PKCS5_C
MBEDTLS_PK_C
MBEDTLS_PK_PARSE_C
MBEDTLS_PK_PARSE_EC_COMPRESSED
MBEDTLS_PK_PARSE_EC_EXTENDED
MBEDTLS_PK_RSA_ALT_SUPPORT
MBEDTLS_PK_WRITE_C
MBEDTLS_PLATFORM_C
MBEDTLS_PLATFORM_EXIT_ALT
MBEDTLS_PLATFORM_FPRINTF_ALT
MBEDTLS_PLATFORM_GMTIME_R_ALT
MBEDTLS_PLATFORM_MEMORY
MBEDTLS_PLATFORM_MS_TIME_ALT
MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
MBEDTLS_PLATFORM_NV_SEED_ALT
MBEDTLS_PLATFORM_PRINTF_ALT
MBEDTLS_PLATFORM_SETBUF_ALT
MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT
MBEDTLS_PLATFORM_SNPRINTF_ALT
MBEDTLS_PLATFORM_TIME_ALT
MBEDTLS_PLATFORM_VSNPRINTF_ALT
MBEDTLS_PLATFORM_ZEROIZE_ALT
MBEDTLS_POLY1305_C
MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS
MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS
MBEDTLS_PSA_CRYPTO_C
MBEDTLS_PSA_CRYPTO_CLIENT
MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
MBEDTLS_PSA_CRYPTO_SE_C
MBEDTLS_PSA_CRYPTO_SPM
MBEDTLS_PSA_CRYPTO_STORAGE_C
MBEDTLS_PSA_INJECT_ENTROPY
MBEDTLS_PSA_ITS_FILE_C
MBEDTLS_PSA_KEY_STORE_DYNAMIC
MBEDTLS_PSA_P256M_DRIVER_ENABLED
MBEDTLS_PSA_STATIC_KEY_SLOTS
MBEDTLS_RIPEMD160_C
MBEDTLS_RSA_C
MBEDTLS_RSA_NO_CRT
MBEDTLS_SELF_TEST
MBEDTLS_SHA1_C
MBEDTLS_SHA224_C
MBEDTLS_SHA256_C
MBEDTLS_SHA256_SMALLER
MBEDTLS_SHA256_USE_A64_CRYPTO_IF_PRESENT
MBEDTLS_SHA256_USE_A64_CRYPTO_ONLY
MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT
MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY
MBEDTLS_SHA384_C
MBEDTLS_SHA3_C
MBEDTLS_SHA512_C
MBEDTLS_SHA512_SMALLER
MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT
MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY
MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN
MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND
MBEDTLS_TEST_HOOKS
MBEDTLS_THREADING_ALT
MBEDTLS_THREADING_C
MBEDTLS_THREADING_PTHREAD
```

#### Boolean options that can be set indirectly

Options that can be set via adjustment, and can currently also be set directly. Such options are likely to be redundant, however not all of them are.

`perl -l -ne 'print $1 if /#define +(MBEDTLS_\w+)($| *\/)/' tf-psa-crypto/include/psa/crypto_adjust_* tf-psa-crypto/drivers/builtin/include/mbedtls/*adjust* | sort -u | comm -12 - <(perl -l -ne 'print $1 if /#define +(MBEDTLS_\w+)($| *\/)/' tf-psa-crypto/include/psa/crypto_config.h | sort)`

| Option | Replacement | Notes |
| ------ | ----------- | ----- |
| `MBEDTLS_AES_C` | `PSA_WANT_KEY_TYPE_AES` |  |
| `MBEDTLS_ARIA_C` | `PSA_WANT_KEY_TYPE_ARIA` |  |
| `MBEDTLS_ASN1_PARSE_C` | Keep | [Changes to ASN.1 enablement](#changes-to-asn1-enablement) |
| `MBEDTLS_ASN1_WRITE_C` | Keep | [Changes to ASN.1 enablement](#changes-to-asn1-enablement) |
| `MBEDTLS_BIGNUM_C` | Remove | Enabled as needed by asymmetric crypto |
| `MBEDTLS_CAMELLIA_C` | `PSA_WANT_KEY_TYPE_CAMELLIA` |  |
| `MBEDTLS_CCM_C` | `PSA_WANT_ALG_CCM` |  |
| `MBEDTLS_CHACHA20_C` | `PSA_WANT_KEY_TYPE_CHACHA20` |  |
| `MBEDTLS_CHACHAPOLY_C` | `PSA_WANT_ALG_CHACHA20_POLY1305` |  |
| `MBEDTLS_CIPHER_C` | Remove | Enabled as needed by cipher modes |
| `MBEDTLS_CIPHER_MODE_CBC` | `PSA_WANT_ALG_CBC_PKCS7`, `PSA_WANT_ALG_CBC_NO_PADDING` |  |
| `MBEDTLS_CIPHER_MODE_CFB` | `PSA_WANT_ALG_CFB` |  |
| `MBEDTLS_CIPHER_MODE_CTR` | `PSA_WANT_ALG_CTR` |  |
| `MBEDTLS_CIPHER_MODE_OFB` | `PSA_WANT_ALG_OFB` |  |
| `MBEDTLS_CIPHER_PADDING_PKCS7` | `PSA_WANT_ALG_CBC_PKCS7` |  |
| `MBEDTLS_CMAC_C` | `PSA_WANT_ALG_CMAC` |  |
| `MBEDTLS_DES_C` | `PSA_WANT_KEY_TYPE_DES` | May be removed: [Mbed-TLS/mbedtls#9164](https://github.com/Mbed-TLS/mbedtls/issues/9164) |
| `MBEDTLS_ECDH_C` | `PSA_WANT_ALG_ECDH` |  |
| `MBEDTLS_ECDSA_C` | `PSA_WANT_ALG_ECDSA`, `PSA_WANT_ALG_DETERMINISTIC_ECDSA` |  |
| `MBEDTLS_ECDSA_DETERMINISTIC` | `PSA_WANT_ALG_DETERMINISTIC_ECDSA` |  |
| `MBEDTLS_ECJPAKE_C` | `PSA_WANT_ALG_JPAKE` + `PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE` |  |
| `MBEDTLS_ECP_C` | Remove | Enabled as needed by ECC mechanisms |
| `MBEDTLS_ECP_DP_BP256R1_ENABLED` | `PSA_WANT_ECC_BRAINPOOL_P_R1_256` |  |
| `MBEDTLS_ECP_DP_BP384R1_ENABLED` | `PSA_WANT_ECC_BRAINPOOL_P_R1_384` |  |
| `MBEDTLS_ECP_DP_BP512R1_ENABLED` | `PSA_WANT_ECC_BRAINPOOL_P_R1_512` |  |
| `MBEDTLS_ECP_DP_CURVE25519_ENABLED` | `PSA_WANT_ECC_MONTGOMERY_255` |  |
| `MBEDTLS_ECP_DP_CURVE448_ENABLED` | `PSA_WANT_ECC_MONTGOMERY_448` |  |
| `MBEDTLS_ECP_DP_SECP192K1_ENABLED` | `PSA_WANT_ECC_SECP_K1_192` | May be removed: [Mbed-TLS/mbedtls#8136](https://github.com/Mbed-TLS/mbedtls/issues/8136) |
| `MBEDTLS_ECP_DP_SECP192R1_ENABLED` | `PSA_WANT_ECC_SECP_R1_192` | May be removed: [Mbed-TLS/mbedtls#8136](https://github.com/Mbed-TLS/mbedtls/issues/8136) |
| `MBEDTLS_ECP_DP_SECP224R1_ENABLED` | `PSA_WANT_ECC_SECP_R1_224` | May be removed: [Mbed-TLS/mbedtls#8136](https://github.com/Mbed-TLS/mbedtls/issues/8136) |
| `MBEDTLS_ECP_DP_SECP256K1_ENABLED` | `PSA_WANT_ECC_SECP_K1_256` |  |
| `MBEDTLS_ECP_DP_SECP256R1_ENABLED` | `PSA_WANT_ECC_SECP_R1_256` |  |
| `MBEDTLS_ECP_DP_SECP384R1_ENABLED` | `PSA_WANT_ECC_SECP_R1_384` |  |
| `MBEDTLS_ECP_DP_SECP521R1_ENABLED` | `PSA_WANT_ECC_SECP_R1_521` |  |
| `MBEDTLS_GCM_C` | `PSA_WANT_ALG_GCM` |  |
| `MBEDTLS_GENPRIME` | `PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE` |  |
| `MBEDTLS_HMAC_DRBG_C` | TBD | [Changes to RNG options](#changes-to-rng-options) |
| `MBEDTLS_MD5_C` | `PSA_WANT_ALG_MD5` |  |
| `MBEDTLS_MD_C` | Keep | Reduced to what was `MBEDTLS_MD_LIGHT`: [Public hash-only `md.h`](#public-hash-only-mdh) |
| `MBEDTLS_OID_C` | Keep | [OID conditional compilation](#oid-conditional-compilation) |
| `MBEDTLS_PKCS1_V15` | `PSA_WANT_ALG_RSA_PKCS1V15_CRYPT`, `PSA_WANT_ALG_RSA_PKCS1V15_SIGN` |  |
| `MBEDTLS_PKCS1_V21` | `PSA_WANT_ALG_RSA_OAEP`, `PSA_WANT_ALG_RSA_PSS` |  |
| `MBEDTLS_PK_PARSE_EC_COMPRESSED` | Keep |  |
| `MBEDTLS_PLATFORM_SNPRINTF_ALT` | Keep |  |
| `MBEDTLS_PLATFORM_VSNPRINTF_ALT` | Keep |  |
| `MBEDTLS_POLY1305_C` | `PSA_WANT_ALG_CHACHA20_POLY1305` |  |
| `MBEDTLS_PSA_CRYPTO_CLIENT` | Keep | maybe [revise client/server inclusion](#revise-clientserver-inclusion) |
| `MBEDTLS_RIPEMD160_C` | `PSA_WANT_ALG_RIPEMD160` |  |
| `MBEDTLS_RSA_C` | `PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY` |  |
| `MBEDTLS_SHA1_C` | `PSA_WANT_ALG_SHA_1` |  |
| `MBEDTLS_SHA224_C` | `PSA_WANT_ALG_SHA_224` |  |
| `MBEDTLS_SHA256_C` | `PSA_WANT_ALG_SHA_256` |  |
| `MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT` | Keep | Only currently auto-enabled from a deprecated synonym |
| `MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY` | Keep | Only currently auto-enabled from a deprecated synonym |
| `MBEDTLS_SHA384_C` | `PSA_WANT_ALG_SHA_384` |  |
| `MBEDTLS_SHA3_C` | `PSA_WANT_ALG_SHA_3` |  |
| `MBEDTLS_SHA512_C` | `PSA_WANT_ALG_SHA_512` |  |

#### Boolean options that cannot be set indirectly

Options that can currently be set directly, and cannot be set via adjustment. Such options are likely to be still relevant. However, some may have become irrelevant, for example if they configure a module that is being removed.

`perl -l -ne 'print $1 if /#define +(MBEDTLS_\w+)($| *\/)/' tf-psa-crypto/include/psa/crypto_adjust_* tf-psa-crypto/drivers/builtin/include/mbedtls/*adjust* | sort -u | comm -13 - <(perl -l -ne 'print $1 if /#define +(MBEDTLS_\w+)($| *\/)/' tf-psa-crypto/include/psa/crypto_config.h | sort)`

| Option | Fate | Notes |
| ------ | ---- | ----- |
| `MBEDTLS_AESCE_C` | Keep | Tune built-in crypto |
| `MBEDTLS_AESNI_C` | Keep | Tune built-in crypto |
| `MBEDTLS_AES_FEWER_TABLES` | Keep | Tune built-in crypto |
| `MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH` | Keep | Feature selection not available through `PSA_WANT` |
| `MBEDTLS_AES_ROM_TABLES` | Keep | Tune built-in crypto |
| `MBEDTLS_AES_USE_HARDWARE_ONLY` | Keep | Tune built-in crypto |
| `MBEDTLS_BASE64_C` | Keep | Crypto-adjacent feature, [remains public](#headers-that-remain-public) |
| `MBEDTLS_BLOCK_CIPHER_NO_DECRYPT` | Keep in 0ε | Should be deduced from `PSA_WANT`: [Mbed-TLS/mbedtls#9163](https://github.com/Mbed-TLS/mbedtls/issues/9163) |
| `MBEDTLS_CAMELLIA_SMALL_MEMORY` | Keep | Tune built-in crypto |
| `MBEDTLS_CHECK_RETURN_WARNING` | Keep | Platform feature |
| `MBEDTLS_CIPHER_MODE_XTS` | Remove (keep the code) | [XTS migration](#xts-migration) |
| `MBEDTLS_CIPHER_NULL_CIPHER` | Remove | No longer used by TLS |
| `MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS` | Remove | [Obsolete cipher padding modes](#obsolete-cipher-padding-modes) |
| `MBEDTLS_CIPHER_PADDING_ZEROS` | Remove | [Obsolete cipher padding modes](#obsolete-cipher-padding-modes) |
| `MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN` | Remove | [Obsolete cipher padding modes](#obsolete-cipher-padding-modes) |
| `MBEDTLS_CTR_DRBG_C` | TBD | [Changes to RNG options](#changes-to-rng-options) |
| `MBEDTLS_CTR_DRBG_USE_128_BIT_KEY` | TBD | [Changes to RNG options](#changes-to-rng-options) |
| `MBEDTLS_DEPRECATED_REMOVED` | Keep | Generic feature support |
| `MBEDTLS_DEPRECATED_WARNING` | Keep | Generic feature support |
| `MBEDTLS_DHM_C` | Remove | Dead code: [removal of low-level DHM](#removal-of-low-level-dhm) |
| `MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED` | Keep | Tune built-in crypto |
| `MBEDTLS_ECP_DP_SECP224K1_ENABLED` | Remove | [Removal of small elliptic curves](#removal-of-small-elliptic-curves) |
| `MBEDTLS_ECP_NIST_OPTIM` | Keep | Tune built-in crypto |
| `MBEDTLS_ECP_RESTARTABLE` | Keep | No corresponding PSA option yet |
| `MBEDTLS_ECP_WITH_MPI_UINT` | Keep | Tune built-in crypto |
| `MBEDTLS_ENTROPY_C` | Remove (now implied) | See [Builds without entropy](#builds-without-entropy) |
| `MBEDTLS_ENTROPY_FORCE_SHA256` | TBD | [Changes to RNG options](#changes-to-rng-options) |
| `MBEDTLS_ENTROPY_HARDWARE_ALT` | Keep | Now enables a slightly different callback: [Mbed-TLS/mbedtls#9618](https://github.com/Mbed-TLS/mbedtls/issues/9618) |
| `MBEDTLS_ENTROPY_NV_SEED` | Keep | Platform feature |
| `MBEDTLS_FS_IO` | Keep | Platform configuration |
| `MBEDTLS_GCM_LARGE_TABLE` | Keep | Tune built-in crypto |
| `MBEDTLS_HAVE_ASM` | Keep | Tune built-in crypto |
| `MBEDTLS_HAVE_SSE2` | Keep | Tune built-in crypto |
| `MBEDTLS_HAVE_TIME` | Move | [Move time options back to Mbed TLS](#move-time-options-back-to-mbed-tls) |
| `MBEDTLS_HAVE_TIME_DATE` | Move | [Move time options back to Mbed TLS](#move-time-options-back-to-mbed-tls) |
| `MBEDTLS_HKDF_C` | Remove | Not used by PSA: [Mbed-TLS/mbedtls#9150](https://github.com/Mbed-TLS/mbedtls/issues/9150) |
| `MBEDTLS_LMS_C` | Keep | Crypto not yet in PSA |
| `MBEDTLS_LMS_PRIVATE` | Keep | Crypto not yet in PSA |
| `MBEDTLS_MEMORY_BACKTRACE` | Keep | Platform feature |
| `MBEDTLS_MEMORY_BUFFER_ALLOC_C` | Keep | Platform feature |
| `MBEDTLS_MEMORY_DEBUG` | Keep | Platform feature |
| `MBEDTLS_NIST_KW_C` | Keep | Crypto not yet in PSA |
| `MBEDTLS_NO_64BIT_MULTIPLICATION` | Keep | Tune built-in crypto |
| `MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES` | TBD | [Changes to RNG options](#changes-to-rng-options) |
| `MBEDTLS_NO_PLATFORM_ENTROPY` | TBD | [Changes to RNG options](#changes-to-rng-options) |
| `MBEDTLS_NO_UDBL_DIVISION` | Keep | Tune built-in crypto |
| `MBEDTLS_PEM_PARSE_C` | Keep | Crypto-adjacent feature, [remains public](#headers-that-remain-public) |
| `MBEDTLS_PEM_WRITE_C` | Keep | Crypto-adjacent feature, [remains public](#headers-that-remain-public) |
| `MBEDTLS_PKCS12_C` | Keep | No longer an exposed API, but functionality accessed through PK. Need doc update. |
| `MBEDTLS_PKCS5_C` | Keep | No longer an exposed API, but functionality accessed through PK. Need doc update. |
| `MBEDTLS_PK_C` | Keep | No PSA equivalent yet |
| `MBEDTLS_PK_PARSE_C` | Keep | No PSA equivalent yet |
| `MBEDTLS_PK_PARSE_EC_EXTENDED` | Keep | No PSA equivalent yet |
| `MBEDTLS_PK_RSA_ALT_SUPPORT` | Remove | Feature subsumed by PSA drivers |
| `MBEDTLS_PK_WRITE_C` | Keep | No PSA equivalent yet |
| `MBEDTLS_PLATFORM_C` | Keep | Platform configuration |
| `MBEDTLS_PLATFORM_EXIT_ALT` | Keep | Platform configuration |
| `MBEDTLS_PLATFORM_FPRINTF_ALT` | Keep | Platform configuration |
| `MBEDTLS_PLATFORM_GMTIME_R_ALT` | Move | [Move time options back to Mbed TLS](#move-time-options-back-to-mbed-tls) |
| `MBEDTLS_PLATFORM_MEMORY` | Keep | Platform configuration |
| `MBEDTLS_PLATFORM_MS_TIME_ALT` | Move | [Move time options back to Mbed TLS](#move-time-options-back-to-mbed-tls) |
| `MBEDTLS_PLATFORM_NO_STD_FUNCTIONS` | Keep | Platform configuration |
| `MBEDTLS_PLATFORM_NV_SEED_ALT` | Keep | Platform configuration |
| `MBEDTLS_PLATFORM_PRINTF_ALT` | Keep | Platform configuration |
| `MBEDTLS_PLATFORM_SETBUF_ALT` | Keep | Platform configuration |
| `MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT` | Keep | Platform configuration |
| `MBEDTLS_PLATFORM_TIME_ALT` | Move | [Move time options back to Mbed TLS](#move-time-options-back-to-mbed-tls) |
| `MBEDTLS_PLATFORM_ZEROIZE_ALT` | Keep | Platform configuration |
| `MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS` | Keep | Tune built-in crypto |
| `MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS` | Keep | Core feature |
| `MBEDTLS_PSA_CRYPTO_C` | Keep | Core feature; maybe [revise client/server inclusion](#revise-clientserver-inclusion) |
| `MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG` | Keep | Platform configuration |
| `MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER` | Keep | Platform configuration |
| `MBEDTLS_PSA_CRYPTO_SE_C` | Remove | [Mbed-TLS/mbedtls#8151](https://github.com/Mbed-TLS/mbedtls/issues/8151) |
| `MBEDTLS_PSA_CRYPTO_SPM` | Keep | Core feature; maybe [revise client/server inclusion](#revise-clientserver-inclusion) |
| `MBEDTLS_PSA_CRYPTO_STORAGE_C` | Keep | Core feature |
| `MBEDTLS_PSA_INJECT_ENTROPY` | Remove | [Mbed-TLS/mbedtls#9707](https://github.com/Mbed-TLS/mbedtls/issues/9707) |
| `MBEDTLS_PSA_ITS_FILE_C` | Keep | Platform configuration |
| `MBEDTLS_PSA_KEY_STORE_DYNAMIC` | Keep | Core feature; maybe [revise key store selection](#revise-key-store-selection) |
| `MBEDTLS_PSA_P256M_DRIVER_ENABLED` | Keep | Tune built-in crypto |
| `MBEDTLS_PSA_STATIC_KEY_SLOTS` | Keep | Core feature; maybe [revise key store selection](#revise-key-store-selection) |
| `MBEDTLS_RSA_NO_CRT` | Remove | [Mbed-TLS/TF-PSA-Crypto#114](https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/114); not in 0ε because it can become a no-op after 1.0 |
| `MBEDTLS_SELF_TEST` | Remove | [PSA self-test interface](#psa-self-test-interface) |
| `MBEDTLS_SHA256_SMALLER` | Keep | Tune built-in crypto |
| `MBEDTLS_SHA256_USE_A64_CRYPTO_IF_PRESENT` | Keep | Tune built-in crypto |
| `MBEDTLS_SHA256_USE_A64_CRYPTO_ONLY` | Keep | Tune built-in crypto |
| `MBEDTLS_SHA512_SMALLER` | Keep | Tune built-in crypto |
| `MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT` | Keep | Tune built-in crypto |
| `MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY` | Keep | Tune built-in crypto |
| `MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN` | Remove (for test only) | [Remove test-related platform options](#remove-test-related-platform-options) |
| `MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND` | Remove (for test only) | [Remove test-related platform options](#remove-test-related-platform-options) |
| `MBEDTLS_TEST_HOOKS` | Keep | Test injection feature (discussed in [Remove test-related platform options](#remove-test-related-platform-options)) |
| `MBEDTLS_THREADING_ALT` | Keep | as `MBEDTLS_THREADING_C` |
| `MBEDTLS_THREADING_C` | Keep | We plan to revise the threading abstraction after 0ε: [Mbed-TLS/mbedtls#8455](https://github.com/Mbed-TLS/mbedtls/issues/8455) |
| `MBEDTLS_THREADING_PTHREAD` | Keep | as `MBEDTLS_THREADING_C` |

#### Legacy options in `all.sh`

We have finished migrating `all.sh` to `MBEDTLS_PSA_CRYPTO_CONFIG`, so in principle, it should not rely on setting legacy options. There are still many occurrences of [boolean options that can be set indirectly](#boolean-options-that-can-be-set-indirectly) (and thus are likely to be removed). Almost all are `config.py unset` that are only present because we're disabling a mechanism through PSA and we want to avoid it coming back through the legacy API where it's enabled by default or by `full`, hence we can remove the `unset` statements once the option is no longer present in `crypto_config.h`.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10148): validate this. By making the legacy option default-off and excluded from `full`, then comparing the outcome files? (Removing the option may affect [`depends.py`](#legacy-options-in-dependspy) until the ongoing work there is finished.)

#### Legacy options in `depends.py`

At the time of writing, `depends.py` still works with legacy options. A migration to PSA options is in progress ([“TF-PSA-Crypto all.sh components” epic](https://github.com/orgs/Mbed-TLS/projects/18)). For the 0ε work, we assume that this migration is finished.

#### Non-boolean options

Non-boolean options are likely to remain relevant since PSA has no equivalent. However, some may have become irrelevant, for example if they configure a module that is being removed.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10149): check non-boolean options, other than RNG options, to see if any should be removed or adapted.

### Changes to RNG options

#### Impacted RNG modules

The RNG used when `MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG` is disabled consists of three modules:

* Entropy (`entropy.c` and `entropy_poll.c`): gather entropy which then needs to be fed to a DRBG (deterministic random bit generator, also called a PRNG for pseudo-random number generator).
* CTR\_DRBG, a NIST-approved DRBG relying on the AES block cipher.
* HMAC\_DRBG, a NIST-approved DRBG relying on a hash algorithm.

These modules remain relevant internally, but they are becoming fully private. (See “[Headers that can be made fully private](#headers-that-can-be-made-fully-private)” and “[Headers that will become internal eventually](#headers-that-will-become-internal-eventually)”.) We will likely expose CTR\_DRBG and HMAC\_DRBG as programming interfaces in a later minor version of TF-PSA-Crypto 1.x, but this is out of scope for TF-PSA-Crypto 1.0, and will likely wait until there is an official PSA interface.

Entropy and CTR\_DRBG are only used to instantiate the PSA RNG. HMAC\_DRBG is also used internally for deterministic ECDSA.

#### Impacted RNG options

The following boolean options inherited from Mbed TLS 3.x control the random generator used by PSA, and are not tied to any function or callback exposed by TF-PSA-Crypto 0ε or 1.0.

```
MBEDTLS_CTR_DRBG_C
MBEDTLS_CTR_DRBG_USE_128_BIT_KEY
MBEDTLS_ENTROPY_FORCE_SHA256
MBEDTLS_HMAC_DRBG_C
MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
MBEDTLS_NO_PLATFORM_ENTROPY
```

They are complemented by the following non-boolean options:

```
MBEDTLS_CTR_DRBG_ENTROPY_LEN
MBEDTLS_CTR_DRBG_MAX_INPUT
MBEDTLS_CTR_DRBG_MAX_REQUEST
MBEDTLS_CTR_DRBG_MAX_SEED_INPUT
MBEDTLS_CTR_DRBG_RESEED_INTERVAL
MBEDTLS_ENTROPY_MAX_GATHER
MBEDTLS_ENTROPY_MAX_SOURCES
MBEDTLS_ENTROPY_MIN_HARDWARE
MBEDTLS_HMAC_DRBG_MAX_INPUT
MBEDTLS_HMAC_DRBG_MAX_REQUEST
MBEDTLS_HMAC_DRBG_MAX_SEED_INPUT
MBEDTLS_HMAC_DRBG_RESEED_INTERVAL
MBEDTLS_PSA_HMAC_DRBG_MD_TYPE
```

#### High-level view of RNG interfaces

The random generator subsystem is not directly exposed to applications. It exists only to feed `psa_generate_random()` and other APIs that require randomness (either for their result or for blinding).

The random generator subsystem has an interface with drivers, which is not implemented yet and will likely not be implemented until after TF-PSA-Crypto 1.0. This interface is described in the [PSA Cryptoprocessor Driver Interface draft specification](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/docs/proposed/psa-driver-interface.md): [entropy collection entry point](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/docs/proposed/psa-driver-interface.md#entropy-collection-entry-point) and [random generation entry points](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/docs/proposed/psa-driver-interface.md#random-generation-entry-points).

Beyond the entry points, the random generator also has a configuration interface which tunes its security parameters and performance/code-size/memory-usage compromises:

* Which DRBG to use (we offer CTR\_DRBG and HMAC\_DRBG). [security, code size, performance]
* Which low-level algorithms and key sizes to use (AES size for CTR\_DRBG, hash used in HMAC\_DRBG, hash used to accumulate entropy). [security, performance]
* How much entropy is considered good enough — in total and for each source. [security, performance]
* Whether to use entropy sources, a seed file, or both.
* Various sizes of intermediate buffers. [performance, memory usage, security]

#### Towards a minimal set of RNG options

This section is about the parameters of the DRBG (including how it communicates with the entropy module) and the internal workings of the entropy module. Entropy sources, and builds with `MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG` enabled, are out of scope here.

Many of the [RNG options](#impacted-rng-options) are mostly meaningless now that the DRBG modules and the entropy module are no longer public. In this section, we determine which options are still relevant, either because they are algorithm choices or because they are a security/resources compromise.

For notions of strength, we follow [NIST SP 800-90A r1](https://doi.org/10.6028/NIST.SP.800-90Ar1) (especially table 2 p. 38 for HMAC\_DRBG) and table 3 p. 49 for CTR\_DRBG), and [NIST SP 800-57 part 1 r1](https://doi.org/10.6028/NIST.SP.800-57pt1r5) §5.6.1.

The RNG uses the following algorithms:

* A hash function in the entropy module, to update the accumulator. The size of the hash gives the strength (the inputs are not chosen by an adversary unless they have complete control, so collisions are irrelevant) (except for hashes smaller than 32 bits, for which SP 800-57 specifies a smaller strength). Mbed TLS picks between SHA-256 and SHA-512 based on availability and on `MBEDTLS_ENTROPY_FORCE_SHA256`.
* A hash function in HMAC\_DRBG. The size of the hash gives the strength. Mbed TLS 3.6 allows any supported hash function, configured with `MBEDTLS_PSA_HMAC_DRBG_MD_TYPE`.
* A block cipher in CTR\_DRBG. The key size gives the strength. The library only supports AES, and defaults to 256 bits unless overridden by `MBEDTLS_CTR_DRBG_USE_128_BIT_KEY`.

When using HMAC\_DRBG, there is no particular reason to use a different hash algorithm for the DRBG and for the entropy accumulator.

For the choice of DRBG, we preserve the existing behavior: pick CTR\_DRBG if enabled in the build, otherwise HMAC\_DRBG. There is no particular reason to change at this point.

#### New RNG options

We can deduce the sizes used in entropy and for DRBG internals from just two settings:

* `MBEDTLS_PSA_CRYPTO_RNG_STRENGTH` indicating the minimum strength of the RNG. Only 128 and 256 are meant to be useful values. Default to 256.
* `MBEDTLS_PSA_CRYPTO_RNG_HASH` indicating which hash algorithm to use for the entropy module, and for HMAC\_DRBG if configured. Default to SHA-256.

For CTR\_DRBG, use AES-256 if `MBEDTLS_PSA_CRYPTO_RNG_STRENGTH > 128` and AES-128 otherwise.

In addition, we unify some DRBG options which currently exist separately for CTR\_DRBG and HMAC\_DRBG:

* `MBEDTLS_PSA_RNG_RESEED_INTERVAL`: inteeger, becomes the value of `MBEDTLS_CTR_DRBG_RESEED_INTERVAL` and `MBEDTLS_HMAC_DRBG_RESEED_INTERVAL`.

#### Investigation of `MBEDTLS_ENTROPY_BLOCK_SIZE`

The size of the hash used by the entropy module becomes the value of `MBEDTLS_ENTROPY_BLOCK_SIZE`. This size is used in several places in the entropy module:

* Size of the NV seed.
* Size collected from each entropy source.
* Size of the internal entropy accumulator that mixes all the sources (including the NV seed).
* Maximum size returned by `mbedtls_entropy_func()` (the function that DRBG modules call).

The length requested by a DRBG is `ctx->entropy_len`, which, for default instantiations (the only ones that matter in TF-PSA-Crypto 1.0), is:

* For CTR\_DRBG: `MBEDTLS_CTR_DRBG_ENTROPY_LEN`. In Mbed TLS 3.6, and since the PolarSSL days, this is 48 if entropy uses SHA-512 and 32 if entropy uses SHA-256. Per NIST strength specifications (SP 800-90Ar1 table 3), `MBEDTLS_PSA_CRYPTO_RNG_STRENGTH` would be sufficient.
* For HMAC\_DRBG: 32 for all hashes of 256 bits and above.

In conclusion, if we don't worry about hashes that are less than 256 bits, then we can systematically have the DRBG request 32 bytes, and the entropy module will always deliver.

#### Removed DRBG length options

Remove the following as configuration options (they may still be used internally):

* `MBEDTLS_ENTROPY_C`: now an internal detail. Enable it automatically if `MBEDTLS_PSA_CRYPTO_C` is enabled (so not in client-only builds) and `MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG` is disabled.
* `MBEDTLS_CTR_DRBG_USE_128_BIT_KEY`: deduced from `MBEDTLS_PSA_CRYPTO_RNG_STRENGTH`.
* `MBEDTLS_ENTROPY_FORCE_SHA256`: superseded by `MBEDTLS_PSA_CRYPTO_RNG_HASH`.
* `MBEDTLS_CTR_DRBG_ENTROPY_LEN`: 32 per “[Investigation of `MBEDTLS_ENTROPY_BLOCK_SIZE`](#investigation-of-mbedtls_entropy_block_size)”.
* `MBEDTLS_CTR_DRBG_MAX_INPUT`: only relevant for custom instantiations which are no longer possible. Do not touch in 1.0, other than hiding it.
* `MBEDTLS_CTR_DRBG_MAX_REQUEST`: this is the maximum size of a request, reflected through `MBEDTLS_PSA_RANDOM_MAX_REQUEST`. The current default is 1024 bytes. It's a minor compromise of stack usage vs performance. We may consider exposing `MBEDTLS_PSA_RANDOM_MAX_REQUEST` later if needs be.
* `MBEDTLS_CTR_DRBG_MAX_SEED_INPUT`: the default value 384 is always fine (we can reduce it later). This needs to be at least `entropy_len + nonce_len + additional_len` where, in the default instantiation, `entropy_len` is `MBEDTLS_CTR_DRBG_ENTROPY_LEN`, `nonce_len` is at most `(MBEDTLS_CTR_DRBG_ENTROPY_LEN + 1) / 2`  and `additional_len` is 0.
* `MBEDTLS_CTR_DRBG_RESEED_INTERVAL`: now `MBEDTLS_PSA_RNG_RESEED_INTERVAL`.
* `MBEDTLS_ENTROPY_MAX_GATHER`: no longer relevant. Can remain the default (128 bytes) which is sufficient for the maximum possible strength of the RNG subsystem (512 bits).
* `MBEDTLS_ENTROPY_MAX_SOURCES`: no longer relevant since there can only be two entropy sources at most (platform or custom entropy source, and NV seed). Can be hard-coded to 2.
* `MBEDTLS_ENTROPY_MIN_HARDWARE`: not relevant since there is only one source which must provide the whole entropy. Removed in [#212](https://github.com/Mbed-TLS/TF-PSA-Crypto/pull/212) resolving [Mbed-TLS/mbedtls#9618](https://github.com/Mbed-TLS/mbedtls/issues/9618).
* `MBEDTLS_HMAC_DRBG_MAX_INPUT`: same as `MBEDTLS_CTR_DRBG_MAX_INPUT` (not relevant for deterministic ECDSA). ECDSA only needs the size of the private key (so up to 66 bytes, for secp521r1).
* `MBEDTLS_HMAC_DRBG_MAX_REQUEST`: same as `MBEDTLS_CTR_DRBG_MAX_REQUEST`.
* `MBEDTLS_HMAC_DRBG_MAX_SEED_INPUT`: same as `MBEDTLS_CTR_DRBG_MAX_SEED_INPUT`. ECDSA passes `2*n` bits where `n` is the size of the private key in bits (so up to 131 bytes, for secp521r1).
* `MBEDTLS_HMAC_DRBG_RESEED_INTERVAL`: now `MBEDTLS_PSA_RNG_RESEED_INTERVAL`.
* `MBEDTLS_PSA_HMAC_DRBG_MD_TYPE`: deduced from `MBEDTLS_PSA_CRYPTO_RNG_HASH`.

#### RNG algorithm and length options: summary

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/328): implement the new configuration options described in “[New RNG options](#new-rng-options)”. Remove the options described in “[Removed DRBG length options](#removed-drbg-length-options)” from `psa/crypto_config.h`, keeping the macros set from remaining options as described. Replace the current configuration checks in `check_config.h` as described below (the new checks may be in `check_config.h` or in other, possibly internal headers). Also, remove the warnings about `MBEDTLS_CTR_DRBG_USE_128_BIT_KEY` in the makefiles. They are no longer relevant: now, if you select a 128-bit RNG strength, that's a very explicit choice.

The new configuration checks ensure that the RNG configuration options achieve the strength in bits specified as `MBEDTLS_PSA_CRYPTO_RNG_STRENGTH`. Overall we should have the following checks:

* At least one of CTR\_DRBG or HMAC\_DRBG must be enabled (already enforced in `psa_crypto_random_impl.h`).
* If CTR\_DRBG is used, the AES key size is chosen based on the strength. A strength of more than 256 is an error.
* If HMAC\_DRBG is used, the size of the hash must be at least `MBEDTLS_PSA_CRYPTO_RNG_STRENGTH`.
* The size of the hash `MBEDTLS_PSA_CRYPTO_RNG_HASH` must be at least 256 bits (32 bytes). We could in principle support smaller hashes, but we would need more complex strength calculations, and nobody needs this in 2025.

#### Builds without entropy

TF-PSA-Crypto 0ε and in all likelihood TF-PSA-Crypto 1.0 cannot be built without either an entropy source, or entropy stored in a file (NV seed). Builds without entropy are useful, for example, to only perform signature verification, so this is something we plan to support in the future.

For the time being, any build of the PSA core will continue to require `MBEDTLS_ENTROPY_C` which requires either an entropy source or an NV seed.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/9618): Require entropy. Change `MBEDTLS_ENTROPY_HARDWARE_ALT` to be simpler and more suitable for TF-PSA-Crypto:

* Add a parameter to the entropy callback to convey how much entropy is present in the output. For the time being, the output must have full entropy, but in the future TF-PSA-Crypto will allow it to be less.
* Rename the entropy callback.
* Simplify RNG options by only giving the choice between the platform default source and a custom source. We don't need both.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/307): [Mbed-TLS/mbedtls#9618](https://github.com/Mbed-TLS/mbedtls/issues/9618) removes the possibility of builds with only an NV seed. This is critical for some of our users, including some TF-M builds. Restore this ability. Test it on the CI (done for 3.6 in https://github.com/Mbed-TLS/mbedtls/pull/10210).

### Changes to platform support options

Due to a lack of bandwidth, we do not plan any significant changes in the platform support layer.

#### Remove test-related platform options

For historical reasons, a few test-specific options are present in `mbedtls_config.h` in Mbed TLS 3.6, and have made their way into `crypto_config.h` in TF-PSA-Crypto. These options have no effect on the build of the library, only on test framework code and unit tests.

Task: Remove the following options from `crypto_config.h` and adjust the corresponding test code to pass `-D` to the compiler instead of calling `crypto_config.h`:

```
MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN
MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND
```

Note: `MBEDTLS_TEST_HOOKS` is different because it does influence the build of the library. Going by our invasive testing guidelines, all invasive testing is gated by this single option plus a runtime activation mechanism. Since `MBEDTLS_TEST_HOOKS` is enabled by `config.py full`, it needs to remain in the official configuration file (or else we would need to add nontrivial code to take care of it). Thus we are not changing anything related to `MBEDTLS_TEST_HOOKS`. It will gate the test hooks both in TF-PSA-Crypto and Mbed TLS since there is no reason to separate the two.

#### Move time options back to Mbed TLS

The options `MBEDTLS_HAVE_TIME`, `MBEDTLS_HAVE_TIME_DATE` are not used in crypto, only in X.509 and TLS.

`MBEDTLS_HAVE_TIME` is used in the timing module which was historically used in the crypto library, but only for the sake of being a weak entropy source (removed in Mbed TLS 3.0) and for the benchmark program. The benchmark program is still relevant, but that doesn't justify a timing abstraction in TF-PSA-Crypto, and the timing module went into the Mbed TLS repository.

Task:

* Move `MBEDTLS_HAVE_TIME` and `MBEDTLS_HAVE_TIME_DATE` back to Mbed TLS.
* Move time-related platform function options (`MBEDTLS_PLATFORM_TIME_ALT`, `MBEDTLS_PLATFORM_MS_TIME_ALT`, `MBEDTLS_PLATFORM_MS_TIME_TYPE_MACRO`, `MBEDTLS_PLATFORM_MS_TIME_ALT`, `MBEDTLS_PLATFORM_GMTIME_R_ALT`)  back to Mbed TLS.
* Move the declaration of `mbedtls_platform_gmtime_r` to `platform_time.h`.
* Move `platform_time.h` back to Mbed TLS.
* Move the corresponding function definitions from `platform*.c` to Mbed TLS.
* Move the corresponding tests in `test_suite_platform` to Mbed TLS.

Rationale: saves maintenance complexity and simplifies the user interface, compared to the current situation where time-related interfaces are split between the two repositories.

### Compilation options for non-cryptographic features

#### Counter inclusions in ASN.1

The functions `mbedtls_asn1_get_len` and `mbedtls_asn1_get_tag` are enabled when `MBEDTLS_ASN1_WRITE_C` is enabled, even if `MBEDTLS_ASN1_PARSE_C` is disabled. The functions `mbedtls_asn1_write_len` and `mbedtls_asn1_write_tag` are enabled when `MBEDTLS_ASN1_PARSE_C` is enabled, even if `MBEDTLS_ASN1_WRITE_C` is disabled. This is for the sake of X.509 code, and until [Mbed-TLS/TF-PSA-Crypto#143](https://github.com/Mbed-TLS/TF-PSA-Crypto/pull/143), the dependencies were `MBEDTLS_X509_CREATE_C` for the get functions and `MBEDTLS_X509_USE_C` for the write functions.

The counter-direction uses of these functions in X.509 arose from hexstring support in distinguished names which was introduced in [Mbed-TLS/mbedtls#8025](https://github.com/Mbed-TLS/mbedtls/pull/8025). At this point, it is not clear to me whether counter-direction functions are intrinsically needed to support this aspect of X.509, or if it's a logical consequence of a plausible design decision we made on data representation, or if it's just an implementation convenience that is no longer so convenient when ASN.1 and X.509 are in separate projects. The counter-direction uses are:

* In `mbedtls_x509_dn_gets`, which is used to inspect X.509 parsing results, we use `mbedtls_asn1_write_len` and `mbedtls_asn1_write_tag` to format a name containing special characters as a hexstring. `mbedtls_asn1_write_tag` is trivial and could be made a simple assignment. `mbedtls_asn1_write_len` is less trivial because it needs logic to encode lengths (different encoding methods under and over 128, per DER rules), but it could reasonably be reimplemented. Using ASN.1 functions there adds overhead to conform to their interface, and so doesn't make the code smaller or simpler.
* In `mbedtls_x509_string_to_names`, which is used to prepare data for X.509 writing, we use `mbedtls_asn1_get_len` to parse a hex-encoded name. We don't use `mbedtls_asn1_get_tag` because several tags are permitted. We could reimplement the length parsing logic here, but it's nontrivial and would make the code here slightly more complex.

Although names are typically shorter than 128 bytes, there is no official limit. Per https://stackoverflow.com/questions/1253575/does-the-ldap-protocol-limit-the-length-of-a-dn there is no limit for X.509 or LDAP, and ActiveDirectory allows 255 characters, so we should at least support up to 65535 octets (the next length encoding threshold after 128).

In the short term, we have an undocumented interface: `MBEDTLS_ASN1_PARSE_C` enables two write functions and `MBEDTLS_ASN1_WRITE_C` enables two parse functions.

In the longer term, plausible solutions include:

* Making the short-term workaround of enabling counter-direction functions official.
* Having `MBEDTLS_X509_USE_C` require `MBEDTLS_ASN1_WRITE_C` in addition to `MBEDTLS_ASN1_PARSE_C`, and having `MBEDTLS_X509_CREATE_C` require `MBEDTLS_ASN1_PARSE_C` in addition to `MBEDTLS_ASN1_WRITE_C`.
* Changing the X.509 code to reimplement the logic it needs, rather than use ASN.1 functions which don't fit the calling code well.
* One of the above solutions for USE and a different one for CREATE.

#### OID conditional compilation

The compilation option `MBEDTLS_OID_C` guards the whole OID module (`oid.c`). Some OID table and support functions are further guarded according to the cryptographic mechanisms and library interfaces that are enabled. However, some OIDs that are generically useful in X.509 and not used directly by any other crypto code (e.g. `MBEDTLS_OID_X509_EXT_xxx`) are always included when `MBEDTLS_OID_C` is enabled. Furthermore, many OIDs for X.509 are guarded by `!defined(MBEDTLS_X509_REMOVE_INFO)`, which is a not a crypto option and therefore cannot appear in TF-PSA-Crypto (since we want to make it possible to build TF-PSA-Crypto independently of Mbed TLS).

From the perspective of the TF-PSA-Crypto build, there are three levels of OID inclusion:

1. Only as needed in crypto.
2. (1) plus the core set for X.509 (subject to available cryptographic mechanisms where relevant).
3. (2) plus the extra data for `mbedtls_x509_crt_info()` and friends.

We either need public compilation options with this level of inclusion, or to move the X.509 parts of `oid.c` to the X.509 library (obviously preferable, but requiring more upfront work).

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10124): move the X.509 parts of `oid.c` to the X.509 library.

### Changes to option implications

#### Changes to ASN.1 enablement

Currently, `MBEDTLS_ASN1_PARSE_C` and `MBEDTLS_ASN1_WRITE_C` are automatically enabled:

* If `MBEDTLS_RSA_C` is enabled. This is needed for RSA key import and export. We don't try to optimize code size when import or export of a key type is disabled, so this can stay. As part of internal refactoring, it will change to `PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY` eventually.
* If `PSA_WANT_ALG_ECDSA` or `PSA_WANT_ALG_DETERMINISTIC_ECDSA` is built in. This used to be required to convert between ASN.1 and raw signature formats internally back when PSA was built on top of the legacy API, but today I think it isn't needed. However, it is needed for `MBEDTLS_ECDSA_C`. We do expose the signature conversion functions `mbedtls_ecdsa_raw_to_der` and `mbedtls_ecdsa_der_to_raw` when an ECDSA signature is enabled through PSA (built-in or not), but we automatically enable the necessary ASN.1 functions regardless of the ASN.1 configuration.

So we have an optimization here to not auto-enable all of ASN.1 for built-in ECDSA. This will require removing or changing the guards on `mbedtls_ecdsa_read_signature()` and `mbedtls_ecdsa_write_signature()` and friends, which are now private functions.

### Non-critical changes to compilation options

The changes discussed in this section would make for a more user-friendly API, but would not reduce our maintenance burden. As a consequence, we are treating them as low priority. They may not be fully fleshed out.

#### Revise client/server inclusion

Now that PSA is “always on”, what does it mean for `MBEDTLS_PSA_CRYPTO_C`, `MBEDTLS_PSA_CRYPTO_CLIENT` and `MBEDTLS_PSA_CRYPTO_SPM`? We no longer officially support builds without any PSA parts, but we do support server-only or client-only builds, so there is no part of the code that is actually mandatory.

* A library build (the most common case) defines `MBEDTLS_PSA_CRYPTO_C` (which implies `MBEDTLS_PSA_CRYPTO_CLIENT`).
* A client-only build defines `MBEDTLS_PSA_CRYPTO_CLIENT` and little to no else.
* A server-only build defines `MBEDTLS_PSA_CRYPTO_C` and `MBEDTLS_PSA_CRYPTO_SPM`.

Strongly related: https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/112

#### Revise key store selection

For historical reasons, we have three key store architectures:

* Purely static, when `MBEDTLS_PSA_STATIC_KEY_SLOTS` is enabled. Key slots are allocated statically, and the core does not use `malloc()`. This only allows a small number of keys determined at compile time, and keys have a size limit that is determined at compile time. The memory usage is determined at compile time, regardless of how full the key store is. This achieves the smallest code size, but has major limitations.
* Hybrid, when neither `MBEDTLS_PSA_KEY_STORE_DYNAMIC` nor `MBEDTLS_PSA_STATIC_KEY_SLOTS` is enabled. This is the original design, with a static array of key slots (for simplicity) and dynamic allocation of key data. Compared to the fully dynamic key store, this only allows a small number of keys determined at compile time, but achieves a small code size (almost on par with the static key store if `malloc()` is present anyway). Compared to the static key store, it wastes less memory when only a fraction of the key store is used, and has no specific limit on the size of keys.
* Fully dynamic, when `MBEDTLS_PSA_KEY_STORE_DYNAMIC` is enabled (default). Key slots and key data are both allocated dynamically. This has no practical limitations, but larger code size.

Should we keep all three? Since `MBEDTLS_PSA_KEY_STORE_DYNAMIC` is the default (because that's what you need e.g. in a Linux distribution package), should we change to defining a symbol for the hybrid key store?

## Changes to public crypto headers

### Public hash-only `md.h`

To be considered for 1.0: make a subset of `md.h` public. Only hashes, not HMAC. As a starting point, this would be `MBEDTLS_MD_LIGHT`, which is known to work in many configurations, although we don't have to stick to it.

Reasons to do this:

* The upfront cost is small: we can take the existing `md.h` and just remove the HMAC-related code and some of the metadata-related interfaces.
* This is already tested as `MBEDTLS_MD_LIGHT`.
* As a thin wrapper over PSA (we would not keep direct calls to low-level modules), the maintenance cost is very small.
* It is used in a very large number of places, both in Mbed TLS and in third-party code. Keeping it around will both save us work during the lifetime of TF-PSA-Crypto 1.x and Mbed TLS 4.x, and facilitate the transition for our users.
* If we don't do this, then we'll have to change some code in Mbed TLS. In the `full` configuration, Mbed TLS links to several md functions: `mbedtls_md`, `mbedtls_md_error_from_psa`, `mbedtls_md_get_size`, `mbedtls_md_info_from_type` (in addition to macros, enum constants and static inline functions from `mbedtls/md.h`).

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/8450): Privatize the parts of `md.h` that are not MD-light.

### Shrunk-down `pk.h`

We treat the evolution of `pk.h` as a project with its own design document and task breakdown.

https://github.com/Mbed-TLS/mbedtls/issues/8452

https://github.com/Mbed-TLS/TF-PSA-Crypto/pull/203

### Shrunk-down `psa_util.h`

Only some of the interface elements in `mbedtls/psa_util.h` remain useful.

| Name | Uses | Fate |
| ---- | ---- | ---- |
| `mbedtls_psa_get_random` | crypto internal, `ssl_test_lib.c` | Deprecate |
| `MBEDTLS_PSA_RANDOM_STATE` | same as `mbedtls_psa_get_random` | Deprecate |
| `mbedtls_ecc_group_to_psa` | crypto internal | Internalize |
| `mbedtls_ecc_group_from_psa` | crypto internal | Internalize |
| `mbedtls_md_psa_alg_from_type` | crypto internal, SSL library | Privatize, deprecate or remove? |
| `mbedtls_md_type_from_psa_alg` | crypto internal, SSL library | Privatize, deprecate or remove? |
| `mbedtls_ecdsa_raw_to_der` | crypto internal | Keep public |
| `mbedtls_ecdsa_der_to_raw` | crypto internal | Keep public |

The RNG wrapper function `mbedtls_psa_get_random` (and the associated constant `MBEDTLS_PSA_RANDOM_STATE`) has a negligible cost for us and can help users transition their code. Keep it, but mark it as deprecated, possibly inlined, and change it (code and documentation) to just return the return value of `psa_generate_random`.

The ECC group conversion functions are now purely private since the type `mbedtls_ecp_group_id` is private. They aren't used by Mbed TLS, so move their declaration to an internal header.

The digest algorithm conversion functions are heavily used in TLS code (mostly if not exclusively TLS 1.3). They perform a very simple conversion, so it should be easy to get rid of them. However, as long as `mbedtls_md_type_t` still exists (see [Public hash-only `md.h`](#public-hash-only-mdh)), it could make sense to have them as public functions in `md.h`.

The ECDSA signature format conversion functions are used to implement `mbedtls_pk_{sign,verify}`, but they are also useful to applications. Keep them. We may move them to a different header name though (`psa/crypto_extra.h`?).

## Loss of functionality

Some loss of functionality is accepted in TF-PSA-Crypto 1.x and Mbed TLS 4.x compared to Mbed TLS 3.6. This section discusses some of the loss, with no expectation of exhaustivity.

### Loss of cipher modes

#### XTS migration

As of Mbed TLS 3.6, the PSA API specification includes XTS mode (`PSA_ALG_XTS`), but the Mbed TLS implementation does not support it yet. It is planned to be added ([Mbed-TLS/mbedtls#6384](https://github.com/Mbed-TLS/mbedtls/issues/6384)) but we currently plan to do that after the TF-PSA-Crypto 1.0 release.

In the meantime, XTS code remains in `cipher.c` and low-level crypto modules, but it is unreachable.

#### Obsolete cipher padding modes

We plan to remove all cipher padding modes other than PKCS7. We are not aware of a compelling use case for these modes. If there is a need for them, we would require them to be added to the PSA API specification.

```
MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
MBEDTLS_CIPHER_PADDING_ZEROS
MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
```

### Loss of asymmetric cryptography

#### Removal of small elliptic curves

We would like to remove all elliptic curves smaller than 250 bits, because they are largely unused and uncomfortably close to being insecure. This is planned in [Mbed-TLS/mbedtls#8136](https://github.com/Mbed-TLS/mbedtls/issues/8136).

In the specific case of secp224k1, it is annoying to implement (because it is the only curve where private keys and public coordinates do not have the same bit size). We have not implemented it in PSA and do not plan to do so. As a consequence, all code guarded by `MBEDTLS_ECP_DP_SECP224K1_ENABLED` is now dead code.

#### Removal of low-level DHM

We are removing the low-level DHM module for finite-field Diffie-Hellman (FFDH). FFDH remains available in the PSA API, but only with a small set of predefined groups, whereas the legacy API supports arbitrary groups.

The PSA code is not based on the DHM module (it calls bignum directly), so the DHM module is now dead code.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/9956): Remove `dhm.h`, `dhm.c`, `MBEDTLS_DHM_C`, `test_suite_dhm`.

### PSA self-test interface

TF-PSA-Crypto 0ε, and in all likelihood 1.0, will not have a self-test interface. This feature is necessary for some security certifications, and otherwise useless. It is good to have because it's a commonly expected feature in a crypto provider, but it's also acceptable to wait until we see how important it is for our users.

The current implementation is not available for all mechanisms and is tied to the legacy interface, so it would be of limited usefuless to expose it. A PSA interface should work for all mechanisms and should not be tied to the built-in interface. We would have to rewrite most of the code anyway.

Keeping the self-test code around with a promise that it keeps working will slow us down when we refactor low-level crypto. Keeping the self-test code around as dead code is not useful.

Hence for 0ε we will remove `MBEDTLS_SELF_TEST` and the guarded code.
