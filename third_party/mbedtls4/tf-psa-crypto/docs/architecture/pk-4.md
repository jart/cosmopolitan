The PK module in TF-PSA-Crypto 1 and Mbed TLS 4
===============================================

The goal of this document is to describe the evolution of the `pk.h` interface from Mbed TLS 3.x going into TF-PSA-Crypto 1.0 and Mbed TLS 4.0.

## Requirements

### Project goal

The goal of the PK-4.0 project is to make minimal changes to the `pk.h` interface to make it suitable for TF-PSA-Crypto 1.0, and for consumption in Mbed TLS 4.0. We want to preserve the essential features of PK, while removing aspects of the interface that will make it difficult to finish the migration to PSA, and minimize the amount of work to be done before the 1.0/4.0 releases.

In the short term, before the 1.0/4.0 releases, we will make parts of `pk.h` private, and provide replacements where needed. The goal of the project is to make the desired parts private (i.e. no longer part of the documented API), and to ensure that the replacements are working. The old API can still be used under the hood in Mbed TLS.

In the medium term, in TF-PSA-Crypto 1.x and Mbed TLS 4.x, we will remove the internal uses of private APIs from Mbed TLS and TF-PSA-Crypto, then remove the implementation of those private APIs.

In the long term, we want to fully replace `pk.h` with PSA APIs. This is a work topic for the PSA Crypto API working group. The API design is still at an early stage, far too early to be taken into account here.

### Essential functionality of PK

The following functionality does not exist in PSA, but we consider it absolutely necessary for TF-PSA-Crypto:

* The ability to construct a PSA key from a PK key and vice versa.
* The ability to parse a key in various commonplace formats. These formats include metadata indicating the key type, so this cannot be a simple extension of `psa_import_key`.
* The ability to write a key in various commonplace formats.

In addition, we choose to retain the following functionality, because it is easier to keep it in PK than to reimplement it where it is used:

* A sign/verify interface, using a signature format that's ready for X.509 and TLS.

Any other functionality in the `pk.h` interface in TF-PSA-Crypto will either be justified by these needs (e.g. metadata queries, object creation and destruction), or left over from Mbed TLS 3.x if there's no particular reason to remove it. Everything else will be made private.

### Functionality that is removed from PK

The following features are deliberately removed from the PK API. (They may remain as private interfaces until TF-PSA-Crypto 1.x.)

* The ability to inspect how data is stored in PK contexts: opaque-or-not, `mbedtls_pk_ec()`, `mbedtls_pk_rsa()`, `mbedtls_pk_get_type()`, `mbedtls_pk_info_t`, etc.
* The ability to construct a PK context manually: `mbedtls_pk_setup()`.
* Direct support for opaque keys (`mbedtls_pk_setup_opaque()`, `mbedtls_pk_setup_rsa_alt()`, etc.). Go via PSA instead.
* The poorly defined type `mbedtls_pk_type_t` and the associated function `mbedtls_pk_can_do()`. Use PSA metadata instead.
* Mechanism names: `mbedtls_pk_get_name()`.
* The RSA-oriented length function: `mbedtls_pk_get_len()`. Use `mbedtls_pk_get_bitlen()`.
* Encrypt/decrypt: `mbedtls_pk_decrypt()`, `mbedtls_pk_encrypt()`. Use PSA.

### Design philosophy for the new PK

We retain the concept of a “PK context”, which can either be empty or contain a public key or contain a key pair. The new PK handles key parsing and writing, and has convenience functions to sign with a key.

A PK context has the following conceptual properties:

* A PSA key type (key pair or public key). This is `PSA_KEY_TYPE_NONE` for an empty context.
* Key material that matches the key type. This can be directly in the PK object, or indirectly via a PSA key identifier.
* Optionally, an associated PSA key identifier. The PSA key may be owned by the PK context and destroyed when the context is destroyed, or it may be referenced by the PK context and left alone when the context is destroyed.

The PK module does not enforce key policies. In particular, it is possible to copy a PK context into a context with a different signature algorithm.

### Private interfaces

In this document, a ***private*** interface is one that is not documented. Applications should not use private interfaces, and we do not promise any kind of stability about them. Mbed TLS can use private interfaces of TF-PSA-Crypto, but in the medium term (over the lifetime of TF-PSA-Crypto 1.x and Mbed TLS 4.x), it should stop doing so. Public interfaces must not rely on private interfaces, for example a private type cannot be used in the prototype of a public function. However, public types can have a private implementation (we guarantee that the type will keep existing, but it may be implemented differently, typically adding and removing fields in a structure).

An ***internal*** interface is only usable inside TF-PSA-Crypto.

## Use case studies

### RSA in TLS

#### How Mbed TLS currently uses RSA

In Mbed TLS 4.x, TLS 1.2 cipher suites using RSA encryption are not supported. Thus we only care about RSA as a signature algorithm. This subsection studies how the TLS subsystem uses RSA.

TLS 1.2 normally uses PKCS#1v1.5. However, if an Mbed TLS client advertizes support for both TLS 1.2 and 1.3 and advertizes support for PSS-based signature algorithms, some servers may select PSS in TLS 1.2, and the Mbed TLS client supports that. In this case, the TLS 1.2 `ssl_parse_server_key_exchange()` checks an RSA-PSS signature using a public key that it finds as a `mbedtls_pk_context` in the peer's certificate. The public key is only used for a single algorithm, but the algorithm is not yet known when the PK object is created.

TLS 1.3 has a verification mechanism with the same data flow in `ssl_tls13_parse_certificate_verify()`. In TLS 1.3, this is done both on the client side and on the server side.

TLS 1.3 servers may need to produce either a PKCS#1v1.5 or a PSS signature, based on which signature algorithm the server selects among those offered by the client. The private key is a `mbedtls_pk_context` object passed to `mbedtls_conf_own_cert()`. The SSL configuration stores a chained list of key/certificate pairs, with no length limit. The same private key may be used a different RSA algorithm in each connection. The server commits to a private key and to a certificate chain at the same time, based on the keys it has available and their compatibility with the offered signature algorithms.

#### Dual-algorithm RSA verification

In both TLS and X.509, we want the following control flow:

1. Parse a certificate, creating a PK context containing a public key.
2. Determine which signature verification algorithm to use. This information does not come from the certiciate.
3. Verify a signature according to the chosen algorithm.

In Mbed TLS 3.6, the parsing step always results in an object where the key is public present in plain text (either in PSA import format or as a representation involving MPI objects). The verification step either calls built-in code or imports the key into PSA. In the latter case, it can freely choose the algorithm. Even if we change the data flow later to create a PSA key sooner, it will always be possible to export the public key, thus we do not need to take any particular precautions for future-proofing.

The ideal interface here is something similar to `mbedtls_pk_verify_ext()`, with a free selection of the verification algorithm at the time of verification.

It would be possible to create multiple PK contexts after parsing, one with each potential signature algorithm. However there is no incentive to do so. It would add complexity and memory consumption for no benefit.

#### Dual-algorithm RSA signature

The control flow for a TLS server using an RSA private key is as follows:

1. Construct a PK context around the private key. There are two ways:

    * Parse a key file. Optionally, change some metadata associated with the key.
    * Wrap an existing PSA key (opaque PK context). There is no way to change its policy.

2. Pass the PK context alongside a matching certificate to `mbedtls_ssl_conf_own_cert()`. Note that the same certificate is commonly used with the same key for both PKCS#1v1.5 and PSS.

3. Establish a connection and reach the point in the construction of the ServerKeyExchange message where the key in question is chosen, with a signature algorithm that can indicate either PKCS#1v1.5 or PSS.

4. Use the chosen algorithm to produce a signature.

In order to make it possible to use the same RSA key with both algorithm, some action is necessary at one of these steps.

At step 1, in Mbed TLS 3.6, RSA keys resulting from parsing can always be used for both algorithms, but this is not necessarily the case for opaque keys. (It can only be the case if the PSA key uses the “enrollment algorithm” policy feature, which is a proprietary extension of TF-PSA-Crypto that is not supported on some platforms such as TF-M.) Thus, if we want a single workflow for all cases, it has to be a **strict workflow**, where the application using TLS must pass two PK contexts if it wants to allow both algorithms. In the case of a PK context resulting from parsing, there must be some indication of which RSA algorithm will be chosen (this is to be documented in the PK module — parsing an RSA key defaults to PKCS#1v1.5). In this workflow, the signature operation always involves the algorithm associated with the PK context: there is no analog of `mbedtls_pk_sign_ext()`. The strict workflow has the downside that it requires each PK object to have a single associated algorithm, which was considered and discarded (see “[Rejected `mbedtls_pk_set_algorithm()`](#rejected-mbedtls_pk_set_algorithm)”).

We may also consider a **duplication workflow**. In this workflow, at step 2, when `mbedtls_ssl_conf_own_cert()` sees a PK context that could be used for both algorithms (e.g. a PK context that wraps an exportable key, or a PK context that wraps a key whose policy allows both algorithms), it adds two entries to the key/certificate list: the original PK object, and a copied object with the other protocol. This seems complicated, especially with respect to resource management (the SSL configuration object does not own the keys and certificates, but it would have to own the copy made here). Also this workflow is currently broken in TLS 1.3, since it only checks the public key from the certificate, not the private key ([#10233](https://github.com/Mbed-TLS/mbedtls/issues/10233)). Hence we will not consider this workflow further.

Steps 3 and 4 are performed in near succession for each connection, therefore there is no meaningful difference between an action taken at step 3 or step 4, and we will consider them together. Step 3 needs to determine whether the key is compatible with a given signature algorithm offered by the client. Therefore it must be able to tell when a key allows both algorithms. Then step 4 needs to use the chosen signature algorithm, using a function similar to `mbedtls_pk_sign_ext()`. Depending on how the PK context was constructed, this may be done in different ways (just pass the right algorithm to `psa_sign_hash()` if the PSA key allows it, pass the right algorithm when importing a key that wasn't in PSA already, copy the key, etc.). This is a **cheating workflow**.

Conclusion: we will continue to use the current cheating workflow. This approach minimizes the changes in how PK object construction determines which algorithms the key can be used with, and in how to query which algorithms a PK object can be used with.

The main cost of this approach is that we are committing to supporting permissive PK objects, as in, PK objects that can be used with any algorithm that the key type permits. This means that throughout the lifetime of TF-PSA-Crypto 1.x, we will continue to have code in the implementation of `mbedtls_pk_sign_ext()` that possibly exports an underlying PSA key to re-import it under a different policy. This is not ideal, but has the benefit of interface simplicity (PK doesn't do policy, period) and an easy migration from Mbed TLS 3.6 to TF-PSA-Crypto 1.x + Mbed TLS 4.x for both users and us implementers.

#### Enforcing the workflow

If the effective capabilities of `mbedtls_ssl_conf_own_cert()` change, we need to be careful not to end up in a situation where:

1. An application works fine with Mbed TLS 3.6, relying only on documented behavior.
2. The application still works in practice with Mbed TLS 4.0, but now relies on behavior that is no longer documented.
3. The application breaks when TF-PSA-Crypto 1.x moves to more PSA in PK (the change that is likely to be problematic being when `mbedtls_pk_parse_key()` starts constructing a PSA key for RSA key pairs).

We should make sure that `mbedtls_ssl_conf_own_cert()` is strict on what it accepts even in the 4.0 release, and validate this through tests. See [“Interface stability testing”](#interface-stability-testing).

## API elements

### PK context type

#### Keep `mbedtls_pk_context`

We keep `mbedtls_pk_context` largely as it is now. It will be reworked after 1.0 as needed. Keep:

```
mbedtls_pk_context
mbedtls_pk_init()
mbedtls_pk_free()
```

Also keep the following metadata access function:

```
mbedtls_pk_can_do_ext()
```

#### Meaning of `mbedtls_pk_type_t`

`mbedtls_pk_type_t` has several subtly different meanings:

* How the key is represented in a `mbedtls_pk_context`, with additional policy information for EC keys. Can be anything except `MBEDTLS_PK_RSAPSS`.
* Key type from parsing. Can be `MBEDTLS_PK_RSA`, `MBEDTLS_PK_ECKEY` or `MBEDTLS_PK_ECKEY_DH`. Note that obtaining `MBEDTLS_PK_ECKEY_DH` from parsing is fully untested.
* Signature algorithm in X.509. Can be `MBEDTLS_PK_RSA`, `MBEDTLS_PK_RSAPSS` or `MBEDTLS_PK_ECDSA`.
* In invocations of `mbedtls_pk_can_do()`, and possibly elsewhere in local variables or internal functions, it can be a union of two or more of the above.

In TF-PSA-Crypto, we don't want to expose the distinction between `MBEDTLS_PK_OPAQUE` (purely PSA-backed key) and other key types (non-PSA-backed, or partially PSA-backed in the case of ECC keys). We also don't want to guarantee the subtle distinctions between `MBEDTLS_PK_ECKEY`, `MBEDTLS_PK_ECKEY_DH` and `MBEDTLS_PK_ECDSA`. Hence the type `mbedtls_pk_type_t` will become private.

#### Public uses of `mbedtls_pk_type_t` or `mbedtls_pk_get_type()`

Public headers and sample programs are considered public. Library code (including Mbed TLS), test code and test programs are not considered public.

* In X.509 types, to specify an X.509 signature algorithm. See “[New type for signature algorithms](#new-type-for-signature-algorithms)”.
* In several of `programs/pkey/*.c`, to differentiate between RSA and ECC. See “[Use new APIs to distinguish between RSA and ECC in sample programs](#use-new-apis-to-distinguish-between-rsa-and-ecc-in-sample-programs)”.

#### New type for signature algorithms

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/346): Define a new type `mbedtls_pk_sigalg_t` which is a subset of `mbedtls_pk_type_t`, containing only the values that are meaningful as a signature algorithm in an X.509 structure. Prototyped in  [#204](https://github.com/Mbed-TLS/TF-PSA-Crypto/pull/204).

```
typedef enum {
    MBEDTLS_PK_SIGALG_NONE = MBEDTLS_PK_NONE,
    MBEDTLS_PK_SIGALG_RSA = MBEDTLS_PK_RSA,
    MBEDTLS_PK_SIGALG_ECDSA = MBEDTLS_PK_ECDSA,
    MBEDTLS_PK_SIGALG_RSASSA_PSS = MBEDTLS_PK_RSASSA_PSS,
} mbedtls_pk_sigalg_t;
```

Keep the same numerical values as `mbedtls_pk_type_t`, so that a `mbedtls_pk_sigalg_t` value can be cast to `mbedtls_pk_type_t` in library code that still uses this deprecated type.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10264): Move `x509*.h` to the new type.

This task together with the changes to `mbedtls_pk_sign_ext()` and `mbedtls_pk_verify_ext()` described in “[Signature functionality](#signature-functionality)” remove the need for `mbedtls_pk_type_t` to be in the public API of Mbed TLS. Follow-up: make it (and `mbedtls_pk_get_type()`) private in “[Privatization](#privatization)”.

Maybe define public aliases for backward compatibility (it costs us pretty much nothing, and will facilitate the transition).
```
MBEDTLS_PK_NONE = MBEDTLS_PK_SIGALG_NONE
MBEDTLS_PK_RSA = MBEDTLS_PK_SIGALG_RSA
MBEDTLS_PK_RSASSA_PSS = MBEDTLS_PK_SIGALG_RSASSA_PSS
MBEDTLS_PK_ECDSA = MBEDTLS_PK_SIGALG_ECDSA
MBEDTLS_PK_ECKEY = MBEDTLS_PK_SIGALG_ECDSA
```

TODO: investigate merging `MBEDTLS_PK_ECKEY` with `MBEDTLS_PK_ECDSA`. Do we rely on the difference anywhere that's still relevant?

#### `mbedtls_pk_can_do()`

Remove `mbedtls_pk_can_do()` from the public API.

Everyone should use PSA metadata instead. For example, the question “can this key do RSA?” (`mbedtls_pk_can_do(pk, MBEDTLS_PK_RSA)`) is ambiguous since the answer depends on the desired algorithm, so users should instead call `mbedtls_pk_can_do_ext()` which takes an algorithm as a parameter.

#### `mbedtls_pk_get_name()`

PK no longer has a concept of a name for a key type or algorithm.

#### Custom PK context data

PK no longer supports custom setup and inspection of a PK object, thus we remove the following elements from the public API, to be done in “[Privatization](#privatization)”:

* `mbedtls_pk_setup()`. Construct from a PSA key or by parsing instead.
* `mbedtls_pk_ec()`, `mbedtls_pk_rsa()`. All code must also work when a PK context is backed by a PSA key.

#### `mbedtls_pk_check_pair()`

Keep `mbedtls_pk_check_pair()`. It's no burden to implement.

### Changes to `mbedtls_pk_can_do_ext`

#### Semantics of `mbedtls_pk_can_do_ext` in Mbed TLS 3.6

We keep `mbedtls_pk_can_do_ext()` because it's useful to check what a key can do after parsing it. It is partially redundant with `mbedtls_pk_get_psa_attributes()`, but it's sometimes more convenient, already implemented, and easy to implement for any evolution of PK that can accommodate `mbedtls_pk_get_psa_attributes()`.

The semantics of `mbedtls_pk_can_do_ext()` in Mbed TLS 3.6 is somewhat weird with respect to public keys. Although the function is advertised as “Tell if context can do the operation given by PSA algorithm”, that is not quite true. The function takes an algorithm and a usage flag (or a mask thereof), with only private-key usage flags allowed. But it accepts public keys, which could only do the corresponding public-key operation. For example,
```
mbedtls_pk_can_do_ext(pk, PSA_ALG_ECDSA(PSA_ALG_SHA_256), PSA_KEY_USAGE_SIGN_HASH)
```
is true for a builtin ECC key object containing only the public part of the key.
This is misleading and not documented.

#### New semantics of `mbedtls_pk_can_do_ext`

We should change the meaning of the usage flag to indicate what operations can actually be performed on the key:

* `PSA_KEY_USAGE_SIGN_HASH` means a key pair that can be used to sign;
* `PSA_KEY_USAGE_VERIFY_HASH` means a public key or key pair that can be used to verify;
* `PSA_KEY_USAGE_DECRYPT` means a key pair that can be used to decrypt;
* `PSA_KEY_USAGE_ENCRYPT` means a public key or key pair that can be used to encrypt;
* `PSA_KEY_USAGE_DERIVE` means a key pair that can be used as the private side in a key agreement;
* `PSA_KEY_USAGE_DERIVE_PUBLIC`: flag for a key pair or public key that can be used as the public side in a key agreement. This flag does not currently exist in PSA, and [may be added as part of adding s similar function to the PSA API](https://github.com/ARM-software/psa-api/issues/279). In the meantime, give it the value 0x80000000. Note that changing the value will be an ABI change.

This will be closer to how `mbedtls_pk_get_psa_attributes()` works.

To ease the transition, we will call the new function `mbedtls_pk_can_do_psa`. We will keep the current `mbedtls_pk_can_do_ext` as a private function until Mbed TLS stops using it (a [GitHub code search](https://github.com/search?q=%22mbedtls_pk_can_do_ext%28%22+path%3A*.c+NOT+path%3A**%2Fpk.c+NOT+path%3A**%2Fssl_*.c+NOT+path%3A**%2Fx509_c%3F%3F.c+NOT+path%3A**%2Ftest_suite_pk*.c&type=code&ref=advsearch) suggests application developers don't use this function).

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/351): Implement and unit-test `mbedtls_pk_can_do_psa()`. Reuse most of the code of `mbedtls_pk_can_do_ext()` (it will probably help to break it into smaller functions). Should be done before 1.0, but can be done after.

#### Migrating to `mbedtls_pk_can_do_psa`

Note that the change of semantics on public keys will break [`ssl_pick_cert()`](https://github.com/Mbed-TLS/mbedtls/blob/mbedtls-3.6.3/library/ssl_tls12_server.c#L687) and [`ssl_tls13_pick_key_cert()`](https://github.com/Mbed-TLS/mbedtls/blob/mbedtls-3.6.3/library/ssl_tls13_server.c#L1115), as they rely on calling `mbedtls_pk_can_do_ext()` on the public key from the certificate. However, this should be an easy fix: just change these invocations to use `PSA_KEY_USAGE_VERIFY_HASH` as the usage to check.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10266): migrate TLS to `mbedtls_pk_can_do_psa()`. Can be done after 4.0.

### PSA bridges

The new PK needs to have bridges between PK contexts and PSA keys.

Given a key in one form, there are two ways to obtain a key in the other form:

* Make a copy of the key data, so that the destination object lives independently from the source object. This is far easier to use in terms of resource management. However, it may not be possible if the source object's policy makes it impossible to copy. This can be the case with PSA keys, and also with PK contexts if they wrap around a non-copiable PSA key.
* Create an object that aliases the source object: wrap a PSA key in a PK context, or peek at the underlying PSA key of a PK context. The wrapper/underlying object is only valid as long as the source object is valid. A PK context created by wrapping an existing PSA key does not destroy the PSA key. Resource management is tricky, but this has a low overhead and works for keys whose material cannot be copied.

There is currently no way to access the underlying PSA key of a PK context. A nw function to [access the underlying PSA key of a PK context](#access-the-underlying-psa-key-of-a-pk-context) is not planned for TF-PSA-Crypto 1.0.

#### Choice of ECDSA variant

PK sometimes needs to choose between ECDSA variants, when it builds PSA attributes for an ECC key:

* To import an ECC key during parsing, and to sign with a built-in ECC key, when `MBEDTLS_PK_USE_PSA_EC_DATA` is enabled. This uses the macro `MBEDTLS_PK_ALG_ECDSA` defined in `mbedtls/pk.h`.
* In `mbedtls_pk_get_psa_attributes()` to choose the default policy for an ECC key used for signature.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/345): change `MBEDTLS_PK_PSA_ALG_ECDSA_MAYBE_DET` to a public macro `MBEDTLS_PK_ALG_ECDSA`. Switch `mbedtls_pk_get_psa_attributes()` to it. This is nice but not critical for 1.0.

#### `mbedtls_pk_get_psa_attributes()`

Keep `mbedtls_pk_get_psa_attributes()`.

Notes:

* An ECC public key in SubjectPublicKeyInfo format (possibly embedded in a key pair) can specify one of two algorithms: id-ecPublicKey (allows ECDSA signature) or id-ecDH (should not be used for signature). This is encoded in the old API through the PK type: id-ecDH leads to `MBEDTLS_PK_ECKEY_DH`. This is untested (we have no test key with id-ecDH; backlog issue: https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/206). In the new API, you can find out whether a key had id-ecPublicKey or id-ecDH by looking at the algorithm chosen by `mbedtls_pk_get_psa_attributes()`.

* Arguably, since PK is now specialized towards signatures, we could remove the `usage` argument from `mbedtls_pk_get_psa_attributes()`, and make the function work only for a sign/verify usage. However, I don't think this would be right, especially because PK only handles signature: after parsing a key, to use it for something other than signature, you need to use PSA, which means you need to call `mbedtls_pk_get_psa_attributes()` then `mbedtls_pk_import_into_psa()` to create a PSA key with your desired non-signature algorithm. Also, the usage parameter allows for extracting the public part of a key when you don't know whether you have a public key or a key pair, which is very convenient in some cases such as managing a key store.

#### Copy between PK and PSA

Keep the following functions, which create a PK context from a PSA key and vice versa:
```
mbedtls_pk_import_into_psa()
mbedtls_pk_copy_from_psa()
mbedtls_pk_copy_public_from_psa()
```

#### Wrap a PSA key in PK

There is already a function to wrap a PSA private key in a PK context: `mbedtls_pk_setup_opaque()`. The function's name no longer makes sense, since there is no longer a concept of “opaque” PK contexts, and no longer a ”setup“ operation on PK contexts.

ACTION (crypto-opaque): rename `mbedtls_pk_setup_opaque()` to `mbedtls_pk_wrap_psa()` and adjust the documentation accordingly. (Updating internal references to “opaque” is out of scope and will be done later: [Update terminology from “opaque” to “wrapped”](#update-terminology-from-opaque-to-wrapped).) Prototyped in  [#204](https://github.com/Mbed-TLS/TF-PSA-Crypto/pull/204).

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/205): remove mentions of operations other than sign and verify from the documentation. Prototyped in  [#204](https://github.com/Mbed-TLS/TF-PSA-Crypto/pull/204).

The function is somewhat dangerous, since the PK context will silently become invalid if the PSA key is destroyed. Experience in 3.x has shown the function to be handy nonetheless, so we shouldn't remove it without clear alternatives.

The current function has some limitations. We should lift them soon (“[Remove limitations of `mbedtls_pk_wrap_psa()`](#remove-limitations-of-mbedtls_pk_wrap_psa)”), but it isn't a deal breaker for TF-PSA-Crypto 1.0.

### Key parsing and writing

Keep:
```
mbedtls_pk_parse_key()
mbedtls_pk_parse_public_key()
mbedtls_pk_write_key_der()
mbedtls_pk_write_pubkey_der()
mbedtls_pk_write_pubkey_pem()
mbedtls_pk_write_key_pem()
```

Keep:
```
mbedtls_pk_parse_keyfile()
mbedtls_pk_parse_public_keyfile()
```

### Signature conveniences

#### Signature functionality

Keep the following:
```
MBEDTLS_PK_SIGNATURE_MAX_SIZE
```

Keep the following:
```
mbedtls_pk_verify()
mbedtls_pk_sign()
```

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/347): document that `mbedtls_pk_sign()` and `mbedtls_pk_verify()` are legacy functions, that perform the same algorithm that `mbedtls_pk_get_psa_attributes()` would perform under the hood if given a sign or verify usage.

Tweak the following:
```
mbedtls_pk_verify_ext()
mbedtls_pk_sign_ext()
```

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/348, https://github.com/Mbed-TLS/mbedtls/issues/10265, https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/349): remove the `options` parameter to `mbedtls_pk_verify_ext`. Note that we have a changelog entry announcing that it's ignored, this changelog entry needs to be replaced.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/346): for `mbedtls_pk_sign_ext()` and `mbedtls_pk_verify_ext()`, change the `mbedtls_pk_type_t type` parameter (whose type is being removed from the API) to `mbedtls_pk_sigalg_t sigalg`. See “[New type for signature algorithms](new-type-for-signature-algorithms)”.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/5544): remove `MBEDTLS_ERR_PK_SIG_LEN_MISMATCH`. It's mostly useless for RSA, and it doesn't even work for ECDSA.

#### Restartable signature functionality

It's more convenient to keep using PK for restartable signature in X.509, for the same reason as non-restartable signature. So restartable PK should keep existing, so we might as well keep it public.

Keep:
```
mbedtls_pk_restart_ctx
mbedtls_pk_restart_init()
mbedtls_pk_restart_free()
mbedtls_pk_verify_restartable()
mbedtls_pk_sign_restartable()
```

No changes to the function's implementation: restartable behavior is only available for built-in ECDSA when built-in restartable ECC is enabled, but the function always works (in a non-restartable way if restartable is not possible).

There is a risk that the current API will be suboptimal when we port its implementation. However, I think this risk is low, since this is basically the interface that X.509 likes, and the primary goal of PK is to support X.509. If X.509 needs more adaptation than expected to migrate to PSA, PK is the natural place for the adaptation code.

### Removals

#### Removed functions

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/pull/297): remove the encrypt/decrypt functions.

```
mbedtls_pk_decrypt()
mbedtls_pk_encrypt()
```

#### Privatization

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/343): Create a header `/include/mbedtls/private/pk_private.h`. At first, it just includes `../pk.h`.

ACTION (https://github.com/Mbed-TLS/mbedtls-framework/issues/178): Conditionally include `mbedtls/private/pk_private.h` in the framework.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10263): Conditionally include `mbedtls/private/pk_private.h` in Mbed TLS.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/344): Move all private API elements to `mbedtls/private/pk_private.h`:

```
mbedtls_pk_type_t
mbedtls_pk_rsassa_pss_options
mbedtls_pk_debug_type
mbedtls_pk_debug_item
MBEDTLS_PK_DEBUG_MAX_ITEMS
mbedtls_pk_info_from_type()
mbedtls_pk_setup()
mbedtls_pk_get_len()
mbedtls_pk_can_do()
mbedtls_pk_can_do_ext()
mbedtls_pk_debug()
mbedtls_pk_get_name()
mbedtls_pk_get_type()
mbedtls_pk_rsa()
mbedtls_pk_ec()
mbedtls_pk_parse_subpubkey()
mbedtls_pk_write_pubkey()
```

Follow-up: [Make private API elements internal](#make-private-api-elements-internal)

#### Documentation update after privatization

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/205): remove all mentions of private API elements from the public documentation.

This includes both direct mentions (where a type name, constant name or function name is mentioned), and indirect mentions (e.g. “verify\_ext”, “context has been set up”).

#### Remove RSA-ALT

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/208): Remove the option `MBEDTLS_PK_RSA_ALT_SUPPORT` and all code guarded by it, as well as `MBEDTLS_PK_RSA_ALT`.

### Documentation updates

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/209): update the PSA transition guide.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/209): write the 1.0 migration guide.

ACTION (https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/209): write changelog entries.

## Changes in PK consumers

No changes are needed in X.509 before Mbed TLS 4.0. One area of TLS may need some tweaks: [private key selection in TLS servers](#changes-to-private-key-selection).

### Changes to private key selection in TLS

A TLS server can have multiple key-certificate pairs configured with `mbedtls_ssl_conf_own_cert()`. In principle, the server goes through the list of cipher suites (TLS 1.2) or signature algorithms (TLS 1.3) offered by the client, and picks the first one for which a key-certificate pair exists. See “[Dual-algorithm RSA signature](dual-algorithm-rsa-signature)” for a more detailed description.

There are known problems with the current implementation:

* [#10208](https://github.com/Mbed-TLS/mbedtls/issues/10208): for RSA keys, TLS 1.2 correctly checks the compatibility of the private key. But to perform the actual signature, it calls `mbedtls_pk_sign()`, so it uses the key's primary algorithm, which might not be the one required by the protocol (PKCS#1v1.5 for RSA keys).
* [#10220](https://github.com/Mbed-TLS/mbedtls/issues/10220): for ECDSA keys of type `MBEDTLS_PK_OPAQUE`, TLS 1.2 insists on randomized ECDSA, but the key may only permit deterministic ECDSA.
* [#10233](https://github.com/Mbed-TLS/mbedtls/issues/10233): TLS 1.3 can't cope with multiple private keys for the same certificate.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10075): determine how to pass keys to `mbedtls_ssl_conf_own_cert()` in Mbed TLS 4.0, in the sense of documenting an official way to do it, ensuring that this way works, and ensuring that applications using the old way fails (unless we decide to preserve the old way throughout the lifetime of Mbed TLS 4.x and TF-PSA-Crypto 1.x).

This isn't a problem for verification because as a last resort, it's always possible to export a public key and re-import it with a different policy.

Note: the conclusion of this investigation is that with the current design, no particular precautions are needed here.

## Testing

### Unit tests

For the most part, the testing work is a matter of adapting the existing tests, and of creating unit tests for the new interfaces. This work is distributed throughout the coding tasks.

### Interface stability testing

One of the [project goals](#project-goal) is to prepare for moving crypto to be fully PSA, and in particular PK to be purely a wrapper around PSA keys, at least for private keys. We want to avoid breaking application code in TF-PSA-Crypto 1.x if the code worked with TF-PSA-Crypto 1.0. Normally our stability guarantee does not apply to applications that use undocumented behavior. However, it may happen that an application worked fine with Mbed TLS 3.6 and was relying only on documented behavior, and TF-PSA-Crypto 1.0 or Mbed TLS 4.0 stops documenting a critical aspect of the behavior, but in practice the application still works with 1.0/4.0. To reduce user frustration, we would like to minimize such cases. Thus the API in TF-PSA-Crypto 1.0 and Mbed TLS 4.0 should be strict and should reject “permissive” behavior that could work now, but would not be easy to migrate to PSA.

Given the choice of keeping PK permissive ([signature functionality](#signature-functionality) keeping `mbedtls_pk_sign_ext()`, cheating workflow chosen for [dual-algorithm RSA signature](#dual-algorithm-rsa-signature)), the combined behavior of PK and TLS remains mostly unchanged from Mbed TLS 3.6.

ACTION (https://github.com/Mbed-TLS/mbedtls/issues/10160): test `mbedtls_ssl_conf_own_cert()` to ensure that it doesn't cheat on key policies. Note: this would be nice, but at this point, it does not seem critical to do it before the 1.0/4.0 release. However, if we don't implement the tests, we do need to be mindful of not breaking cases that are currently possible. A draft exists in [#10217](https://github.com/Mbed-TLS/mbedtls/pull/10217).

## Open questions

### Parsing and writing

#### Remove file parsing functions?

Should we remove the file parsing functions `mbedtls_pk_parse_keyfile()` and `mbedtls_pk_parse_public_keyfile()`? They look misplaced in a library that generally doesn't access files. But it isn't really difficult to keep them.

These functions are used in test code and sample programs.

### Rejected: `mbedtls_pk_set_algorithm`

An earlier draft of this document proposed that every PK context would have an associated signature algorithm, which could be set with the function `mbedtls_pk_set_algorithm`. This algorithm was used for `mbedtls_pk_sign` and `mbedtls_pk_verify`. There were no `_ext` versions of these functions: users were supposed to import the key into PSA.

#### Effectiveness of `mbedtls_pk_set_algorithm`

How do I parse an RSA key, and then select PSS? More generally, how do I indicate what policy to use after parsing a key?

In my original draft, you can parse an RSA key and then call `mbedtls_pk_set_algorithm()` to select an algorithm other than the default (which is PKCS#1v1.5 signature). This may not be the right design: it won't work when we change PK to always go through PSA for RSA key pairs (i.e. making them systematically opaque in the old sense).

An alternative approach is to require copying the key after parsing. This is what we're effectively doing when the application wants to use the key through PSA: it calls `mbedtls_pk_parse_xxx()`, then `mbedtls_pk_import_into_psa()` (after which it can free the intermediate PK object). But what if the application wants to use the key through PK? The current workflow can be:

* `mbedtls_pk_set_algorithm()` — but as noted above this is not future-proof.
* `mbedtls_pk_import_into_psa()` then `mbedtls_pk_copy_from_psa()`. Inefficient but ok. If that's the intended way to do it, we need to document it clearly, and maybe we should remove `mbedtls_pk_set_algorithm()`?
* A new function `mbedtls_pk_copy()` allowing a policy change?

#### Why `mbedtls_pk_set_algorithm` was rejected

`mbedtls_pk_set_algorithm()` had to mutate the PK object, because the normal intended workflow was to parse a key, then inspect its type, then call `mbedtls_pk_set_algorithm()`. But this made it possible to mutate an object after passing it to a function, which could have unintended effect. Furthermore, this function could fail, since it may need to allocate resources (to copy the key, when it's already in PSA and the new algorithm isn't permitted by the key's policy).

The main intent of `mbedtls_pk_set_algorithm()` was to allow mimicking current flows involving `mbedtls_pk_sign()` and `mbedtls_pk_verify()`. However, on closer inspection, this didn't work so well. In particular, a major driving goal was to keep the internal workings TLS layer mostly unchanged from Mbed TLS 3.6, but change the way users had to call `mbedtls_pk_conf_own_cert()` (see “[RSA in TLS](#rsa-in-tls)”). This would both save time for the preparation of Mbed TLS 4.0, and serve as a sample of what implementers of other protocols might face.

However, it turns out that the way TLS 1.2 and TLS 1.3 use PK private keys is buggy in different ways:

* [#10208](https://github.com/Mbed-TLS/mbedtls/issues/10208): TLS 1.2 calls `mbedtls_pk_sign()`, so it uses the key's primary algorithm, which might not be the one required by the protocol (PKCS#1v1.5 for RSA keys).
* [#10233](https://github.com/Mbed-TLS/mbedtls/issues/10233): TLS 1.3 can't cope with multiple private keys for the same certificate.

These are issues in 3.6, so fixing them is desirable. But their impact is relatively minor in 3.6 because they're uncommon cases. On the other hand, in 4.0, they become more relevant, which risked adding a signficant amount of work to be done before 4.0.

Architecturally, #10208 highlights how having an algorithm associated with a PK object is inherently fragile. Hence the current design removes this concept, and instead orients the user of the PK module towards explicitly choosing the signature algorithm.

### Resource management

#### Access the underlying PSA key of a PK context

Should we provide a function to access the underlying PSA key of a PK context, if there is one?

This would be new work, and does not seem to be needed at the moment. If the PK context was created from a PSA key, the application might as well use the original PSA key. If the PK context was created by parsing, `mbedtls_pk_import_into_psa()` works, and does not require a special case if the PK context does not have an underlying PSA key.

If we add this in the future, it will be considerably easier if all PK contexts have an underlying PSA key, or at least all PK contexts containing a private key have an underlying PSA key.

Note that this function would be somewhat dangerous, like `mbedtls_pk_wrap_psa()`, since the PK object becomes invalid if the PSA key is destroyed independently, and the PSA key identifier becomes invalid if the PK context is destroyed. It is impossible to detect invalid uses at runtime since the PSA key identifier may be reused.

## Later tasks

The tasks described in this section do not need to be done before the 1.0/4.0 release. This section is incomplete.

### Missing functionality

#### Remove limitations of `mbedtls_pk_wrap_psa()`

Remove the limitations of wrapped keys:

* Implement verify for wrapped RSA keys.
* Implement check-pair for wrapped RSA keys.

### Migrate library and test code

#### Update terminology from “opaque” to “wrapped”

Update the library code to change the obsolete terminology “opaque” (as in e.g. `MBEDTLS_PK_OPAQUE`) to “wrapped”.

Originally, PK contexts could wrap a PSA key in order to support non-exportable keys kept in a secure partition, using a PSA implementation with client-server isolation. Thus the useful purpose of wrapping a PSA key was to make the key opaque, hence the name. In TF-PSA-Crypto, we plan to evolve to making PK keys contain a PSA key in more situations. What will matter is whether the underlying PSA key is owned by the PK context (and destroyed when the context is freed), or not (thus surviving when the context is freed). A key owned by the PK context could still potentially be opaque. Hence the distinction should be between _wrapped_ and _owned_ PSA keys.

#### Retire `MBEDTLS_PK_ECDSA`

It is no longer possible to construct a PK object with the legacy type `MBEDTLS_PK_ECDSA`. So get rid of the code that handles it, and remove `MBEDTLS_PK_ECDSA`.

#### Replace all uses of `mbedtls_pk_type_t`, `mbedtls_pk_get_type()` and `mbedtls_pk_can_do()` in X.509 library and test code

#### Replace all uses of `mbedtls_pk_type_t`, `mbedtls_pk_get_type()` and `mbedtls_pk_can_do()` in TLS library and test code

#### Get rid of `mbedtls_pk_can_do()`

Once it's no longer used anywhere, we can stop implementing it.

#### Get rid of `mbedtls_pk_info`

Dispatch based on an enum rather than a method table. This simplifies the code.

#### Make `mbedtls_pk_type_t` internal

Goal: `mbedtls_pk_type_t` is only used inside `pk*.c` and in PK unit tests. It may still be exposed in the `mbedtls_pk_context` type.

### Migrate sample programs

#### Use new APIs to distinguish between RSA and ECC in sample programs

### Removals

#### Make private API elements internal

As much as possible, make private API elements internal. That is, instead of being declared in a public header, declare them in `pk_internal.h` which should not be included by anything except `pk*.c` and PK unit tests.

This should ideally be done little by little, when we eliminate the uses of these elements in Mbed TLS.

#### Remove deprecated former API elements once they are no longer used
