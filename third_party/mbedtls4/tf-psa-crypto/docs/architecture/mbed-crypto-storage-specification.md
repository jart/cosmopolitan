Mbed TLS storage specification
=================================

This document specifies how Mbed TLS uses storage.
Key storage was originally introduced in a product called Mbed Crypto, which was re-distributed via Mbed TLS and has since been merged into Mbed TLS.
This document contains historical information both from before and after this merge.

Mbed Crypto may be upgraded on an existing device with the storage preserved. Therefore:

1. Any change may break existing installations and may require an upgrade path.
1. This document retains historical information about all past released versions. Do not remove information from this document unless it has always been incorrect or it is about a version that you are sure was never released.

Mbed Crypto 0.1.0
-----------------

Tags: mbedcrypto-0.1.0b, mbedcrypto-0.1.0b2

Released in November 2018. <br>
Integrated in Mbed OS 5.11.

Supported backends:

* [PSA ITS](#file-namespace-on-its-for-0.1.0)
* [C stdio](#file-namespace-on-stdio-for-0.1.0)

Supported features:

* [Persistent transparent keys](#key-file-format-for-0.1.0) designated by a [slot number](#key-names-for-0.1.0).
* [Nonvolatile random seed](#nonvolatile-random-seed-file-format-for-0.1.0) on ITS only.

This is a beta release, and we do not promise backward compatibility, with one exception:

> On Mbed OS, if a device has a nonvolatile random seed file produced with Mbed OS 5.11.x and is upgraded to a later version of Mbed OS, the nonvolatile random seed file is preserved or upgraded.

We do not make any promises regarding key storage, or regarding the nonvolatile random seed file on other platforms.

### Key names for 0.1.0

Information about each key is stored in a dedicated file whose name is constructed from the key identifier. The way in which the file name is constructed depends on the storage backend. The content of the file is described [below](#key-file-format-for-0.1.0).

The valid values for a key identifier are the range from 1 to 0xfffeffff. This limitation on the range is not documented in user-facing documentation: according to the user-facing documentation, arbitrary 32-bit values are valid.

The code uses the following constant in an internal header (note that despite the name, this value is actually one plus the maximum permitted value):

    #define PSA_MAX_PERSISTENT_KEY_IDENTIFIER 0xffff0000

There is a shared namespace for all callers.

### Key file format for 0.1.0

All integers are encoded in little-endian order in 8-bit bytes.

The layout of a key file is:

* magic (8 bytes): `"PSA\0KEY\0"`
* version (4 bytes): 0
* type (4 bytes): `psa_key_type_t` value
* policy usage flags (4 bytes): `psa_key_usage_t` value
* policy usage algorithm (4 bytes): `psa_algorithm_t` value
* key material length (4 bytes)
* key material: output of `psa_export_key`
* Any trailing data is rejected on load.

### Nonvolatile random seed file format for 0.1.0

The nonvolatile random seed file contains a seed for the random generator. If present, it is rewritten at each boot as part of the random generator initialization.

The file format is just the seed as a byte string with no metadata or encoding of any kind.

### File namespace on ITS for 0.1.0

Assumption: ITS provides a 32-bit file identifier namespace. The Crypto service can use arbitrary file identifiers and no other part of the system accesses the same file identifier namespace.

* File 0: unused.
* Files 1 through 0xfffeffff: [content](#key-file-format-for-0.1.0) of the [key whose identifier is the file identifier](#key-names-for-0.1.0).
* File 0xffffff52 (`PSA_CRYPTO_ITS_RANDOM_SEED_UID`): [nonvolatile random seed](#nonvolatile-random-seed-file-format-for-0.1.0).
* Files 0xffff0000 through 0xffffff51, 0xffffff53 through 0xffffffff: unused.

### File namespace on stdio for 0.1.0

Assumption: C stdio, allowing names containing lowercase letters, digits and underscores, of length up to 23.

An undocumented build-time configuration value `CRYPTO_STORAGE_FILE_LOCATION` allows storing the key files in a directory other than the current directory. This value is simply prepended to the file name (so it must end with a directory separator to put the keys in a different directory).

* `CRYPTO_STORAGE_FILE_LOCATION "psa_key_slot_0"`: used as a temporary file. Must be writable. May be overwritten or deleted if present.
* `sprintf(CRYPTO_STORAGE_FILE_LOCATION "psa_key_slot_%lu", key_id)` [content](#key-file-format-for-0.1.0) of the [key whose identifier](#key-names-for-0.1.0) is `key_id`.
* Other files: unused.

Mbed Crypto 1.0.0
-----------------

Tags: mbedcrypto-1.0.0d4, mbedcrypto-1.0.0

Released in February 2019. <br>
Integrated in Mbed OS 5.12.

Supported integrations:

* [PSA platform](#file-namespace-on-a-psa-platform-for-1.0.0)
* [library using PSA ITS](#file-namespace-on-its-as-a-library-for-1.0.0)
* [library using C stdio](#file-namespace-on-stdio-for-1.0.0)

Supported features:

* [Persistent transparent keys](#key-file-format-for-1.0.0) designated by a [key identifier and owner](#key-names-for-1.0.0).
* [Nonvolatile random seed](#nonvolatile-random-seed-file-format-for-1.0.0) on ITS only.

Backward compatibility commitments: TBD

### Key names for 1.0.0

Information about each key is stored in a dedicated file designated by the key identifier. In integrations where there is no concept of key owner (in particular, in library integrations), the key identifier is exactly the key identifier as defined in the PSA Cryptography API specification (`psa_key_id_t`). In integrations where there is a concept of key owner (integration into a service for example), the key identifier is made of an owner identifier (its semantics and type are integration specific) and of the key identifier (`psa_key_id_t`) from the key owner point of view.

The way in which the file name is constructed from the key identifier depends on the storage backend. The content of the file is described [below](#key-file-format-for-1.0.0).

* Library integration: the key file name is just the key identifier as defined in the PSA crypto specification. This is a 32-bit value.
* PSA service integration: the key file name is `(uint64_t)owner_uid << 32 | key_id` where `key_id` is the key identifier from the owner point of view and `owner_uid` (of type `int32_t`) is the calling partition identifier provided to the server by the partition manager. This is a 64-bit value.

### Key file format for 1.0.0

The layout is identical to [0.1.0](#key-file-format-for-0.1.0) so far. However note that the encoding of key types, algorithms and key material has changed, therefore the storage format is not compatible (despite using the same value in the version field so far).

### Nonvolatile random seed file format for 1.0.0

The nonvolatile random seed file contains a seed for the random generator. If present, it is rewritten at each boot as part of the random generator initialization.

The file format is just the seed as a byte string with no metadata or encoding of any kind.

This is unchanged since [the feature was introduced in Mbed Crypto 0.1.0](#nonvolatile-random-seed-file-format-for-0.1.0).

### File namespace on a PSA platform for 1.0.0

Assumption: ITS provides a 64-bit file identifier namespace. The Crypto service can use arbitrary file identifiers and no other part of the system accesses the same file identifier namespace.

Assumption: the owner identifier is a nonzero value of type `int32_t`.

* Files 0 through 0xffffff51, 0xffffff53 through 0xffffffff: unused, reserved for internal use of the crypto library or crypto service.
* File 0xffffff52 (`PSA_CRYPTO_ITS_RANDOM_SEED_UID`): [nonvolatile random seed](#nonvolatile-random-seed-file-format-for-0.1.0).
* Files 0x100000000 through 0xffffffffffff: [content](#key-file-format-for-1.0.0) of the [key whose identifier is the file identifier](#key-names-for-1.0.0). The upper 32 bits determine the owner.

### File namespace on ITS as a library for 1.0.0

Assumption: ITS provides a 64-bit file identifier namespace. The entity using the crypto library can use arbitrary file identifiers and no other part of the system accesses the same file identifier namespace.

This is a library integration, so there is no owner. The key file identifier is identical to the key identifier.

* File 0: unused.
* Files 1 through 0xfffeffff: [content](#key-file-format-for-1.0.0) of the [key whose identifier is the file identifier](#key-names-for-1.0.0).
* File 0xffffff52 (`PSA_CRYPTO_ITS_RANDOM_SEED_UID`): [nonvolatile random seed](#nonvolatile-random-seed-file-format-for-1.0.0).
* Files 0xffff0000 through 0xffffff51, 0xffffff53 through 0xffffffff, 0x100000000 through 0xffffffffffffffff: unused.

### File namespace on stdio for 1.0.0

This is a library integration, so there is no owner. The key file identifier is identical to the key identifier.

[Identical to 0.1.0](#file-namespace-on-stdio-for-0.1.0).

### Upgrade from 0.1.0 to 1.0.0.

* Delete files 1 through 0xfffeffff, which contain keys in a format that is no longer supported.

### Suggested changes to make before 1.0.0

The library integration and the PSA platform integration use different sets of file names. This is annoyingly non-uniform. For example, if we want to store non-key files, we have room in different ranges (0 through 0xffffffff on a PSA platform, 0xffff0000 through 0xffffffffffffffff in a library integration).

It would simplify things to always have a 32-bit owner, with a nonzero value, and thus reserve the range 0–0xffffffff for internal library use.

Mbed Crypto 1.1.0
-----------------

Tags: mbedcrypto-1.1.0

Released in early June 2019. <br>
Integrated in Mbed OS 5.13.

Changes since [1.0.0](#mbed-crypto-1.0.0):

* The stdio backend for storage has been replaced by an implementation of [PSA ITS over stdio](#file-namespace-on-stdio-for-1.1.0).
* [Some changes in the key file format](#key-file-format-for-1.1.0).

### File namespace on stdio for 1.1.0

Assumption: C stdio, allowing names containing lowercase letters, digits and underscores, of length up to 23.

An undocumented build-time configuration value `PSA_ITS_STORAGE_PREFIX` allows storing the key files in a directory other than the current directory. This value is simply prepended to the file name (so it must end with a directory separator to put the keys in a different directory).

* `PSA_ITS_STORAGE_PREFIX "tempfile.psa_its"`: used as a temporary file. Must be writable. May be overwritten or deleted if present.
* `sprintf(PSA_ITS_STORAGE_PREFIX "%016llx.psa_its", key_id)`: a key or non-key file. The `key_id` in the name is the 64-bit file identifier, which is the [key identifier](#key-names-for-mbed-tls-2.25.0) for a key file or some reserved identifier for a non-key file (currently: only the [nonvolatile random seed](#nonvolatile-random-seed-file-format-for-1.0.0)). The contents of the file are:
    * Magic header (8 bytes): `"PSA\0ITS\0"`
    * File contents.

### Key file format for 1.1.0

The key file format is identical to [1.0.0](#key-file-format-for-1.0.0), except for the following changes:

* A new policy field, marked as [NEW:1.1.0] below.
* The encoding of key types, algorithms and key material has changed, therefore the storage format is not compatible (despite using the same value in the version field so far).

A self-contained description of the file layout follows.

All integers are encoded in little-endian order in 8-bit bytes.

The layout of a key file is:

* magic (8 bytes): `"PSA\0KEY\0"`
* version (4 bytes): 0
* type (4 bytes): `psa_key_type_t` value
* policy usage flags (4 bytes): `psa_key_usage_t` value
* policy usage algorithm (4 bytes): `psa_algorithm_t` value
* policy enrollment algorithm (4 bytes): `psa_algorithm_t` value [NEW:1.1.0]
* key material length (4 bytes)
* key material: output of `psa_export_key`
* Any trailing data is rejected on load.

Mbed Crypto TBD
---------------

Tags: TBD

Released in TBD 2019. <br>
Integrated in Mbed OS TBD.

### Changes introduced in TBD

* The layout of a key file now has a lifetime field before the type field.
* Key files can store references to keys in a secure element. In such key files, the key material contains the slot number.

### File namespace on a PSA platform on TBD

Assumption: ITS provides a 64-bit file identifier namespace. The Crypto service can use arbitrary file identifiers and no other part of the system accesses the same file identifier namespace.

Assumption: the owner identifier is a nonzero value of type `int32_t`.

* Files 0 through 0xfffeffff: unused.
* Files 0xffff0000 through 0xffffffff: reserved for internal use of the crypto library or crypto service. See [non-key files](#non-key-files-on-tbd).
* Files 0x100000000 through 0xffffffffffff: [content](#key-file-format-for-1.0.0) of the [key whose identifier is the file identifier](#key-names-for-1.0.0). The upper 32 bits determine the owner.

### File namespace on ITS as a library on TBD

Assumption: ITS provides a 64-bit file identifier namespace. The entity using the crypto library can use arbitrary file identifiers and no other part of the system accesses the same file identifier namespace.

This is a library integration, so there is no owner. The key file identifier is identical to the key identifier.

* File 0: unused.
* Files 1 through 0xfffeffff: [content](#key-file-format-for-1.0.0) of the [key whose identifier is the file identifier](#key-names-for-1.0.0).
* Files 0xffff0000 through 0xffffffff: reserved for internal use of the crypto library or crypto service. See [non-key files](#non-key-files-on-tbd).
* Files 0x100000000 through 0xffffffffffffffff: unused.

### Non-key files on TBD

File identifiers in the range 0xffff0000 through 0xffffffff are reserved for internal use in Mbed Crypto.

* Files 0xfffffe02 through 0xfffffeff (`PSA_CRYPTO_SE_DRIVER_ITS_UID_BASE + lifetime`): secure element driver storage. The content of the file is the secure element driver's persistent data.
* File 0xffffff52 (`PSA_CRYPTO_ITS_RANDOM_SEED_UID`): [nonvolatile random seed](#nonvolatile-random-seed-file-format-for-1.0.0).
* File 0xffffff54 (`PSA_CRYPTO_ITS_TRANSACTION_UID`): [transaction file](#transaction-file-format-for-tbd).
* Other files are unused and reserved for future use.

### Key file format for TBD

All integers are encoded in little-endian order in 8-bit bytes except where otherwise indicated.

The layout of a key file is:

* magic (8 bytes): `"PSA\0KEY\0"`.
* version (4 bytes): 0.
* lifetime (4 bytes): `psa_key_lifetime_t` value.
* type (4 bytes): `psa_key_type_t` value.
* policy usage flags (4 bytes): `psa_key_usage_t` value.
* policy usage algorithm (4 bytes): `psa_algorithm_t` value.
* policy enrollment algorithm (4 bytes): `psa_algorithm_t` value.
* key material length (4 bytes).
* key material:
    * For a transparent key: output of `psa_export_key`.
    * For an opaque key (unified driver interface): driver-specific opaque key blob.
    * For an opaque key (key in a secure element): slot number (8 bytes), in platform endianness.
* Any trailing data is rejected on load.

### Transaction file format for TBD

The transaction file contains data about an ongoing action that cannot be completed atomically. It exists only if there is an ongoing transaction.

All integers are encoded in platform endianness.

All currently existing transactions concern a key in a secure element.

The layout of a transaction file is:

* type (2 bytes): the [transaction type](#transaction-types-on-tbd).
* unused (2 bytes)
* lifetime (4 bytes): `psa_key_lifetime_t` value that corresponds to a key in a secure element.
* slot number (8 bytes): `psa_key_slot_number_t` value. This is the unique designation of the key for the secure element driver.
* key identifier (4 bytes in a library integration, 8 bytes on a PSA platform): the internal representation of the key identifier. On a PSA platform, this encodes the key owner in the same way as [in file identifiers for key files](#file-namespace-on-a-psa-platform-on-tbd)).

#### Transaction types on TBD

* 0x0001: key creation. The following locations may or may not contain data about the key that is being created:
    * The slot in the secure element designated by the slot number.
    * The file containing the key metadata designated by the key identifier.
    * The driver persistent data.
* 0x0002: key destruction. The following locations may or may not still contain data about the key that is being destroyed:
    * The slot in the secure element designated by the slot number.
    * The file containing the key metadata designated by the key identifier.
    * The driver persistent data.

Mbed Crypto TBD
---------------

Tags: TBD

Released in TBD 2020. <br>
Integrated in Mbed OS TBD.

### Changes introduced in TBD

* The type field has been split into a type and a bits field of 2 bytes each.

### Key file format for TBD

All integers are encoded in little-endian order in 8-bit bytes except where otherwise indicated.

The layout of a key file is:

* magic (8 bytes): `"PSA\0KEY\0"`.
* version (4 bytes): 0.
* lifetime (4 bytes): `psa_key_lifetime_t` value.
* type (2 bytes): `psa_key_type_t` value.
* bits (2 bytes): `psa_key_bits_t` value.
* policy usage flags (4 bytes): `psa_key_usage_t` value.
* policy usage algorithm (4 bytes): `psa_algorithm_t` value.
* policy enrollment algorithm (4 bytes): `psa_algorithm_t` value.
* key material length (4 bytes).
* key material:
    * For a transparent key: output of `psa_export_key`.
    * For an opaque key (unified driver interface): driver-specific opaque key blob.
    * For an opaque key (key in a secure element): slot number (8 bytes), in platform endianness.
* Any trailing data is rejected on load.

Mbed TLS 2.25.0
---------------

Tags: `mbedtls-2.25.0`, `mbedtls-2.26.0`, `mbedtls-2.27.0`, `mbedtls-2.28.0`, `mbedtls-3.0.0`, `mbedtls-3.1.0`

First released in December 2020.

Note: this is the first version that is officially supported. The version number is still 0.

Backward compatibility commitments: we promise backward compatibility for stored keys when Mbed TLS is upgraded from x to y if x >= 2.25 and y < 4. See [`BRANCHES.md`](../../BRANCHES.md) for more details.

Supported integrations:

* [PSA platform](#file-namespace-on-a-psa-platform-on-mbed-tls-2.25.0)
* [library using PSA ITS](#file-namespace-on-its-as-a-library-on-mbed-tls-2.25.0)
* [library using C stdio](#file-namespace-on-stdio-for-mbed-tls-2.25.0)

Supported features:

* [Persistent keys](#key-file-format-for-mbed-tls-2.25.0) designated by a [key identifier and owner](#key-names-for-mbed-tls-2.25.0). Keys can be:
    * Transparent, stored in the export format.
    * Opaque, using the PSA driver interface with statically registered drivers. The driver determines the content of the opaque key blob.
    * Opaque, using the deprecated secure element interface with dynamically registered drivers (`MBEDTLS_PSA_CRYPTO_SE_C`). The driver picks a slot number which is stored in the place of the key material.
* [Nonvolatile random seed](#nonvolatile-random-seed-file-format-for-mbed-tls-2.25.0) on ITS only.

### Changes introduced in Mbed TLS 2.25.0

* The numerical encodings of `psa_key_type_t`, `psa_key_usage_t` and `psa_algorithm_t` have changed.

### File namespace on a PSA platform on Mbed TLS 2.25.0

Assumption: ITS provides a 64-bit file identifier namespace. The Crypto service can use arbitrary file identifiers and no other part of the system accesses the same file identifier namespace.

Assumption: the owner identifier is a nonzero value of type `int32_t`.

* Files 0 through 0xfffeffff: unused.
* Files 0xffff0000 through 0xffffffff: reserved for internal use of the crypto library or crypto service. See [non-key files](#non-key-files-on-mbed-tls-2.25.0).
* Files 0x100000000 through 0xffffffffffff: [content](#key-file-format-for-mbed-tls-2.25.0) of the [key whose identifier is the file identifier](#key-names-for-mbed-tls-2.25.0). The upper 32 bits determine the owner.

### File namespace on ITS as a library on Mbed TLS 2.25.0

Assumption: ITS provides a 64-bit file identifier namespace. The entity using the crypto library can use arbitrary file identifiers and no other part of the system accesses the same file identifier namespace.

This is a library integration, so there is no owner. The key file identifier is identical to the key identifier.

* File 0: unused.
* Files 1 through 0xfffeffff: [content](#key-file-format-for-mbed-tls-2.25.0) of the [key whose identifier is the file identifier](#key-names-for-mbed-tls-2.25.0).
* Files 0xffff0000 through 0xffffffff: reserved for internal use of the crypto library or crypto service. See [non-key files](#non-key-files-on-mbed-tls-2.25.0).
* Files 0x100000000 through 0xffffffffffffffff: unused.

### File namespace on stdio for Mbed TLS 2.25.0

Assumption: C stdio, allowing names containing lowercase letters, digits and underscores, of length up to 23.

An undocumented build-time configuration value `PSA_ITS_STORAGE_PREFIX` allows storing the key files in a directory other than the current directory. This value is simply prepended to the file name (so it must end with a directory separator to put the keys in a different directory).

* `PSA_ITS_STORAGE_PREFIX "tempfile.psa_its"`: used as a temporary file. Must be writable. May be overwritten or deleted if present.
* `sprintf(PSA_ITS_STORAGE_PREFIX "%016llx.psa_its", key_id)`: a key or non-key file. The `key_id` in the name is the 64-bit file identifier, which is the [key identifier](#key-names-for-mbed-tls-2.25.0) for a key file or some reserved identifier for a [non-key file](#non-key-files-on-mbed-tls-2.25.0). The contents of the file are:
    * Magic header (8 bytes): `"PSA\0ITS\0"`
    * File contents.

### Key names for Mbed TLS 2.25.0

Information about each key is stored in a dedicated file designated by the key identifier. In integrations where there is no concept of key owner (in particular, in library integrations), the key identifier is exactly the key identifier as defined in the PSA Cryptography API specification (`psa_key_id_t`). In integrations where there is a concept of key owner (integration into a service for example), the key identifier is made of an owner identifier (its semantics and type are integration specific) and of the key identifier (`psa_key_id_t`) from the key owner point of view.

The way in which the file name is constructed from the key identifier depends on the storage backend. The content of the file is described [below](#key-file-format-for-mbed-tls-2.25.0).

* Library integration: the key file name is just the key identifier as defined in the PSA crypto specification. This is a 32-bit value which must be in the range 0x00000001..0x3fffffff (`PSA_KEY_ID_USER_MIN`..`PSA_KEY_ID_USER_MAX`).
* PSA service integration: the key file name is `(uint64_t)owner_uid << 32 | key_id` where `key_id` is the key identifier from the owner point of view and `owner_uid` (of type `int32_t`) is the calling partition identifier provided to the server by the partition manager. This is a 64-bit value.

### Key file format for Mbed TLS 2.25.0

All integers are encoded in little-endian order in 8-bit bytes except where otherwise indicated.

The layout of a key file is:

* magic (8 bytes): `"PSA\0KEY\0"`.
* version (4 bytes): 0.
* lifetime (4 bytes): `psa_key_lifetime_t` value.
* type (2 bytes): `psa_key_type_t` value.
* bits (2 bytes): `psa_key_bits_t` value.
* policy usage flags (4 bytes): `psa_key_usage_t` value.
* policy usage algorithm (4 bytes): `psa_algorithm_t` value.
* policy enrollment algorithm (4 bytes): `psa_algorithm_t` value.
* key material length (4 bytes).
* key material:
    * For a transparent key: output of `psa_export_key`.
    * For an opaque key (unified driver interface): driver-specific opaque key blob.
    * For an opaque key (key in a dynamic secure element): slot number (8 bytes), in platform endianness.
* Any trailing data is rejected on load.

### Non-key files on Mbed TLS 2.25.0

File identifiers that are outside the range of persistent key identifiers are reserved for internal use by the library. The only identifiers currently in use have the owner id (top 32 bits) set to 0.

* Files 0xfffffe02 through 0xfffffeff (`PSA_CRYPTO_SE_DRIVER_ITS_UID_BASE + lifetime`): dynamic secure element driver storage. The content of the file is the secure element driver's persistent data.
* File 0xffffff52 (`PSA_CRYPTO_ITS_RANDOM_SEED_UID`): [nonvolatile random seed](#nonvolatile-random-seed-file-format-for-mbed-tls-2.25.0).
* File 0xffffff54 (`PSA_CRYPTO_ITS_TRANSACTION_UID`): [transaction file](#transaction-file-format-for-mbed-tls-2.25.0).
* Other files are unused and reserved for future use.

### Nonvolatile random seed file format for Mbed TLS 2.25.0

[Identical to Mbed Crypto 0.1.0](#nonvolatile-random-seed-file-format-for-0.1.0).

### Transaction file format for Mbed TLS 2.25.0

The transaction file contains data about an ongoing action that cannot be completed atomically. It exists only if there is an ongoing transaction.

All integers are encoded in platform endianness.

All currently existing transactions concern a key in a dynamic secure element.

The layout of a transaction file is:

* type (2 bytes): the [transaction type](#transaction-types-on-mbed-tls-2.25.0).
* unused (2 bytes)
* lifetime (4 bytes): `psa_key_lifetime_t` value that corresponds to a key in a secure element.
* slot number (8 bytes): `psa_key_slot_number_t` value. This is the unique designation of the key for the secure element driver.
* key identifier (4 bytes in a library integration, 8 bytes on a PSA platform): the internal representation of the key identifier. On a PSA platform, this encodes the key owner in the same way as [in file identifiers for key files](#file-namespace-on-a-psa-platform-on-mbed-tls-2.25.0)).

#### Transaction types on Mbed TLS 2.25.0

* 0x0001: key creation. The following locations may or may not contain data about the key that is being created:
    * The slot in the secure element designated by the slot number.
    * The file containing the key metadata designated by the key identifier.
    * The driver persistent data.
* 0x0002: key destruction. The following locations may or may not still contain data about the key that is being destroyed:
    * The slot in the secure element designated by the slot number.
    * The file containing the key metadata designated by the key identifier.
    * The driver persistent data.


## TF-PSA-Crypto 1.0

The dynamic secure element interface `MBEDTLS_PSA_CRYPTO_SE_C` is removed. As a consequence files identifiers 0xfffffe02 through 0xfffffeff are no more used by the secure element driver storage.
