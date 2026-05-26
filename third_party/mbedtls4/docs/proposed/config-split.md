Configuration file split
========================

## Why split the configuration file?

The objective of the repository split is to reach the point where in Mbed TLS
all the cryptography code and its tests are located in a `tf-psa-crypto`
directory that just contains the TF-PSA-Crypto repository as a submodule.
The cryptography APIs exposed by Mbed TLS are just the TF-PSA-Crypto ones.
Mbed TLS relies solely on the TF-PSA-Crypto build system to build its
cryptography library and its tests.

The TF-PSA-Crypto configuration file `tf_psa_crypto_config.h` configures
entirely the cryptography interface exposed by Mbed TLS through TF-PSA-Crypto.
Mbed TLS configuration is split in two files: `mbedtls_config.h` for TLS and
x509, `tf_psa_crypto_config.h` for the cryptography.

## How do we split the configuration file?

We extend the so-called PSA cryptographic configuration scheme based on
`mbedtls_config.h` and `crypto_config.h`. The configuration file `crypto_config.h`
is extended to become the TF-PSA-Crypto configuration file, `mbedtls_config.h`
becomes the configuration file for the TLS and x509 libraries. All the options
to select the cryptographic mechanisms and to configure their implementation
are moved from `mbedtls_config.h` to `(tf_psa_)crypto_config.h`.

The configuration options that are relevant to both Mbed TLS and TF-PSA-Crypto
like platform or system ones are moved to `(tf_psa_)crypto_config.h`. That way
they are available in both repositories (as Mbed TLS includes
`tf_psa_crypto_config.h`) without duplication. Later, we may duplicate or
create aliases for some of them to align with the naming conventions of the
repositories.

The cryptographic configuration options in `tf_psa_crypto_config.h` are
organized into sections that are different from the ones in the pre-split
`mbedtls_config.h` (see below). This is first to take into account the
specifics of TF-PSA-Crypto, for example a specific section for the
configuration of builtin drivers. We also get rid of the grouping of non
boolean options into a dedicated section: related boolean and non boolean
configuration options are rather grouped together into the same section.

Finally, for consistency, the sections in `mbedtls_config.h` are reorganized
to be better aligned with the `tf_psa_crypto_config.h` ones.


## Configuration files and `config.py`

Each repository contains a `config.py` script to create and modify
configurations.

In Mbed TLS, `config.py` handles both `mbedtls_config.h` and
`tf_psa_crypto_config.h`. It can set or unset TLS, x509 and cryptographic
configuration options without having to specify the configuration file the
options belong to. Commands like full and baremetal affect both configuration
files.

In TF-PSA-Crypto, `config.py` addresses only `tf_psa_crypto_config.h`.

## Sections in `tf_psa_crypto_config.h`

The `tf_psa_crypto_config.h` configuration file is organized into eight
sections.

The pre-split `mbedtls_config.h` configuration file contains configuration
options that apply to the whole code base (TLS, x509, crypto and tests) mostly
related to the platform abstraction layer and testing. In
`tf_psa_crypto_config.h` these configurations options are organized into two
sections, one for the platform abstraction layer options and one for the others,
respectively named ["Platform abstraction layer"](#section-platform-abstraction-layer)
and ["General and test configuration options"](#section-general-and-test-configuration-options).

Then, the ["Cryptographic mechanism selection (PSA API)"](#section-cryptographic-mechanism-selection-PSA-API)
section is the equivalent of the pre-split `crypto_config.h` configuration file
containing the PSA_WANT_ prefixed macros.

The following section named
["Cryptographic mechanism selection (extended API)"](#section-cryptographic-mechanism-selection-extended-API)
contains the configuration options for the cryptography mechanisms that are not
yet part of the PSA cryptography API (like LMS or PK).

It is followed by the ["Data format support"](#section-data-format-support)
section that contains configuration options of utilities related to various data
formats (like Base64 or ASN.1 APIs). These utilities aim to facilitate the
usage of the PSA cryptography API in other cryptography projects.

Compared to Mbed TLS, the cryptography code in TF-PSA-Crypto is not located
in a single directory but split between the PSA core (core directory) and the
PSA builtin drivers (drivers/builtin/src directory). This is reflected in
`tf_psa_crypto_config.h` with two sections respectively named ["PSA core"](#section-psa-core)
and ["Builtin drivers"](#section-builtin-drivers).

Finally, the last section named ["Legacy cryptography"](#section-legacy-cryptography)
contains the configuration options that will eventually be removed as duplicates
of PSA_WANT_\* and MBEDTLS_PSA_ACCEL_\* configuration options.

## Sections in `mbedtls_config.h`

The sections in `mbedtls_config.h` are reorganized to be better aligned with
the ones in `tf_psa_crypto_config.h`. The main change is the reorganization
of the "Mbed TLS modules", "Mbed TLS feature support" and
"Module configuration options" sections into the
["TLS feature selection"](#section-tls-feature-selection) and
["X.509 feature selection"](#section-x-509-feature-selection) sections. That
way all TLS/x509 options are grouped into one section and there is no
section dedicated to non boolean configuration options anymore.


## Repartition of the configuration options

### In `tf_psa_crypto_config.h`, we have:
#### SECTION Platform abstraction layer
```
#define MBEDTLS_FS_IO
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_HAVE_TIME_DATE
//#define MBEDTLS_MEMORY_BACKTRACE
//#define MBEDTLS_MEMORY_BUFFER_ALLOC_C
//#define MBEDTLS_MEMORY_DEBUG
#define MBEDTLS_PLATFORM_C
//#define MBEDTLS_PLATFORM_EXIT_ALT
//#define MBEDTLS_PLATFORM_FPRINTF_ALT
//#define MBEDTLS_PLATFORM_GMTIME_R_ALT
//#define MBEDTLS_PLATFORM_MEMORY
//#define MBEDTLS_PLATFORM_MS_TIME_ALT
//#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
//#define MBEDTLS_PLATFORM_NV_SEED_ALT
//#define MBEDTLS_PLATFORM_PRINTF_ALT
//#define MBEDTLS_PLATFORM_SETBUF_ALT
//#define MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT
//#define MBEDTLS_PLATFORM_SNPRINTF_ALT
//#define MBEDTLS_PLATFORM_TIME_ALT
//#define MBEDTLS_PLATFORM_VSNPRINTF_ALT
//#define MBEDTLS_PLATFORM_ZEROIZE_ALT
//#define MBEDTLS_THREADING_ALT
//#define MBEDTLS_THREADING_C
//#define MBEDTLS_THREADING_PTHREAD

//#define MBEDTLS_MEMORY_ALIGN_MULTIPLE      4
//#define MBEDTLS_PLATFORM_CALLOC_MACRO        calloc
//#define MBEDTLS_PLATFORM_EXIT_MACRO            exit
//#define MBEDTLS_PLATFORM_FREE_MACRO            free
//#define MBEDTLS_PLATFORM_FPRINTF_MACRO      fprintf
//#define MBEDTLS_PLATFORM_MS_TIME_TYPE_MACRO   int64_t
//#define MBEDTLS_PLATFORM_NV_SEED_READ_MACRO   mbedtls_platform_std_nv_seed_read
//#define MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO  mbedtls_platform_std_nv_seed_write
//#define MBEDTLS_PLATFORM_PRINTF_MACRO        printf
//#define MBEDTLS_PLATFORM_SETBUF_MACRO      setbuf
//#define MBEDTLS_PLATFORM_SNPRINTF_MACRO    snprintf
//#define MBEDTLS_PLATFORM_STD_CALLOC        calloc
//#define MBEDTLS_PLATFORM_STD_EXIT            exit
//#define MBEDTLS_PLATFORM_STD_EXIT_FAILURE       1
//#define MBEDTLS_PLATFORM_STD_EXIT_SUCCESS       0
//#define MBEDTLS_PLATFORM_STD_FPRINTF      fprintf
//#define MBEDTLS_PLATFORM_STD_FREE            free
//#define MBEDTLS_PLATFORM_STD_MEM_HDR   <stdlib.h>
//#define MBEDTLS_PLATFORM_STD_NV_SEED_FILE  "seedfile"
//#define MBEDTLS_PLATFORM_STD_NV_SEED_READ   mbedtls_platform_std_nv_seed_read
//#define MBEDTLS_PLATFORM_STD_NV_SEED_WRITE  mbedtls_platform_std_nv_seed_write
//#define MBEDTLS_PLATFORM_STD_PRINTF        printf
//#define MBEDTLS_PLATFORM_STD_SETBUF      setbuf
//#define MBEDTLS_PLATFORM_STD_SNPRINTF    snprintf
//#define MBEDTLS_PLATFORM_STD_TIME            time
//#define MBEDTLS_PLATFORM_TIME_MACRO            time
//#define MBEDTLS_PLATFORM_TIME_TYPE_MACRO       time_t
//#define MBEDTLS_PLATFORM_VSNPRINTF_MACRO    vsnprintf
//#define MBEDTLS_PRINTF_MS_TIME    PRId64
```

#### SECTION General and test configuration options
Note: for consistency with the configuration file name change from
`crypto_config.h` to `tf_psa_crypto_config.h`, the configuration options
`MBEDTLS_PSA_CRYPTO_CONFIG_FILE` and `MBEDTLS_PSA_CRYPTO_USER_CONFIG_FILE` are
respectively renamed `TF_PSA_CRYPTO_CONFIG_FILE` and
`TF_PSA_CRYPTO_USER_CONFIG_FILE`. These are the only configuration options
renamed by this document.
```
//#define MBEDTLS_CHECK_RETURN_WARNING
//#define MBEDTLS_DEPRECATED_REMOVED
//#define MBEDTLS_DEPRECATED_WARNING
#define MBEDTLS_SELF_TEST
//#define MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN
//#define MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND
//#define MBEDTLS_TEST_HOOKS

//#define MBEDTLS_CHECK_RETURN __attribute__((__warn_unused_result__))
//#define MBEDTLS_IGNORE_RETURN( result ) ((void) !(result))
//#define TF_PSA_CRYPTO_CONFIG_FILE "psa/tf_psa_crypto_config.h"
//#define TF_PSA_CRYPTO_USER_CONFIG_FILE "/dev/null"
```

#### SECTION Cryptographic mechanism selection (PSA API)
PSA_WANT_\* macros as in current `crypto_config.h`.


#### SECTION Cryptographic mechanism selection (extended API)
```
#define MBEDTLS_LMS_C
//#define MBEDTLS_LMS_PRIVATE
#define MBEDTLS_MD_C
#define MBEDTLS_NIST_KW_C
#define MBEDTLS_PKCS5_C
#define MBEDTLS_PKCS12_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_PARSE_EC_COMPRESSED
#define MBEDTLS_PK_PARSE_EC_EXTENDED
#define MBEDTLS_PK_RSA_ALT_SUPPORT
#define MBEDTLS_PK_WRITE_C

//#define MBEDTLS_CTR_DRBG_ENTROPY_LEN               48
//#define MBEDTLS_CTR_DRBG_MAX_INPUT                256
//#define MBEDTLS_CTR_DRBG_MAX_REQUEST             1024
//#define MBEDTLS_CTR_DRBG_MAX_SEED_INPUT           384
//#define MBEDTLS_CTR_DRBG_RESEED_INTERVAL        10000
//#define MBEDTLS_HMAC_DRBG_MAX_INPUT           256
//#define MBEDTLS_HMAC_DRBG_MAX_REQUEST        1024
//#define MBEDTLS_HMAC_DRBG_MAX_SEED_INPUT      384
//#define MBEDTLS_HMAC_DRBG_RESEED_INTERVAL   10000
//#define MBEDTLS_PSA_HMAC_DRBG_MD_TYPE MBEDTLS_MD_SHA256
```


#### SECTION Data format support
```
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_OID_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PEM_WRITE_C
```


#### SECTION PSA core
```
#define MBEDTLS_ENTROPY_C
//#define MBEDTLS_ENTROPY_FORCE_SHA256
//#define MBEDTLS_ENTROPY_HARDWARE_ALT
//#define MBEDTLS_ENTROPY_NV_SEED
//#define MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
//#define MBEDTLS_NO_PLATFORM_ENTROPY
//#define MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS
//#define MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS
#define MBEDTLS_PSA_CRYPTO_C
//#define MBEDTLS_PSA_CRYPTO_CLIENT
//#define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
//#define MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
//#define MBEDTLS_PSA_CRYPTO_SPM
#define MBEDTLS_PSA_CRYPTO_STORAGE_C
//#define MBEDTLS_PSA_INJECT_ENTROPY
#define MBEDTLS_PSA_ITS_FILE_C
#define MBEDTLS_PSA_KEY_STORE_DYNAMIC
//#define MBEDTLS_PSA_STATIC_KEY_SLOTS

//#define MBEDTLS_ENTROPY_MAX_GATHER                128
//#define MBEDTLS_ENTROPY_MAX_SOURCES                20
//#define MBEDTLS_ENTROPY_MIN_HARDWARE               32
//#define MBEDTLS_PSA_CRYPTO_PLATFORM_FILE "psa/crypto_platform_alt.h"
//#define MBEDTLS_PSA_CRYPTO_STRUCT_FILE "psa/crypto_struct_alt.h"
//#define MBEDTLS_PSA_KEY_SLOT_COUNT 32
//#define MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE       256
```

#### SECTION Builtin drivers
```
#define MBEDTLS_AESCE_C
#define MBEDTLS_AESNI_C
//#define MBEDTLS_AES_FEWER_TABLES
//#define MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH
//#define MBEDTLS_AES_ROM_TABLES
//#define MBEDTLS_AES_USE_HARDWARE_ONLY
//#define MBEDTLS_BLOCK_CIPHER_NO_DECRYPT
//#define MBEDTLS_CAMELLIA_SMALL_MEMORY
//#define MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED
#define MBEDTLS_ECP_NIST_OPTIM
//#define MBEDTLS_ECP_RESTARTABLE
//#define MBEDTLS_ECP_WITH_MPI_UINT
//#define MBEDTLS_GCM_LARGE_TABLE
#define MBEDTLS_HAVE_ASM
//#define MBEDTLS_HAVE_SSE2
//#define MBEDTLS_NO_UDBL_DIVISION
//#define MBEDTLS_NO_64BIT_MULTIPLICATION
//#define MBEDTLS_PSA_P256M_DRIVER_ENABLED
//#define MBEDTLS_RSA_NO_CRT
//#define MBEDTLS_SHA256_SMALLER
//#define MBEDTLS_SHA256_USE_A64_CRYPTO_IF_PRESENT
//#define MBEDTLS_SHA256_USE_A64_CRYPTO_ONLY
//#define MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT
//#define MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY
//#define MBEDTLS_SHA512_SMALLER
//#define MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT
//#define MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY

//#define MBEDTLS_ECP_FIXED_POINT_OPTIM      1
//#define MBEDTLS_ECP_WINDOW_SIZE            4
//#define MBEDTLS_MPI_MAX_SIZE            1024
//#define MBEDTLS_MPI_WINDOW_SIZE            2
//#define MBEDTLS_RSA_GEN_KEY_MIN_BITS            1024
```


#### SECTION Legacy cryptography
```
#define MBEDTLS_AES_C
#define MBEDTLS_ARIA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CAMELLIA_C
#define MBEDTLS_CCM_C
#define MBEDTLS_CHACHA20_C
#define MBEDTLS_CHACHAPOLY_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_MODE_CFB
#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_CIPHER_MODE_OFB
#define MBEDTLS_CIPHER_MODE_XTS
//#define MBEDTLS_CIPHER_NULL_CIPHER
#define MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
#define MBEDTLS_CIPHER_PADDING_PKCS7
#define MBEDTLS_CIPHER_PADDING_ZEROS
#define MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
#define MBEDTLS_CMAC_C
#define MBEDTLS_CTR_DRBG_C
//#define MBEDTLS_CTR_DRBG_USE_128_BIT_KEY
#define MBEDTLS_DES_C
#define MBEDTLS_DHM_C
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECP_C
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_ECP_DP_BP384R1_ENABLED
#define MBEDTLS_ECP_DP_BP512R1_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#define MBEDTLS_ECP_DP_CURVE448_ENABLED
#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECDSA_DETERMINISTIC
#define MBEDTLS_ECJPAKE_C
#define MBEDTLS_GCM_C
#define MBEDTLS_GENPRIME
#define MBEDTLS_HKDF_C
#define MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_MD5_C
#define MBEDTLS_PADLOCK_C
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_POLY1305_C
//#define MBEDTLS_PSA_CRYPTO_SE_C
#define MBEDTLS_RIPEMD160_C
#define MBEDTLS_RSA_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA224_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA384_C
#define MBEDTLS_SHA3_C
#define MBEDTLS_SHA512_C
```


### In `mbedtls_config.h`, we have:
#### SECTION Platform abstraction layer
```
#define MBEDTLS_NET_C
//#define MBEDTLS_TIMING_ALT
#define MBEDTLS_TIMING_C
```


#### SECTION General configuration options
```
#define MBEDTLS_ERROR_C
#define MBEDTLS_ERROR_STRERROR_DUMMY
#define MBEDTLS_VERSION_C
#define MBEDTLS_VERSION_FEATURES

//#define MBEDTLS_CONFIG_FILE "mbedtls/mbedtls_config.h"
//#define MBEDTLS_USER_CONFIG_FILE "/dev/null"
```


#### SECTION TLS feature selection
```
#define MBEDTLS_DEBUG_C
#define MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
//#define MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES
#define MBEDTLS_SSL_ALPN
//#define MBEDTLS_SSL_ASYNC_PRIVATE
#define MBEDTLS_SSL_CACHE_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_CONTEXT_SERIALIZATION
#define MBEDTLS_SSL_COOKIE_C
//#define MBEDTLS_SSL_DEBUG_ALL
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#define MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#define MBEDTLS_SSL_DTLS_CONNECTION_ID
#define MBEDTLS_SSL_DTLS_CONNECTION_ID_COMPAT 0
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
//#define MBEDTLS_SSL_DTLS_SRTP
//#define MBEDTLS_SSL_EARLY_DATA
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_PROTO_TLS1_3
//#define MBEDTLS_SSL_RECORD_SIZE_LIMIT
#define MBEDTLS_SSL_RENEGOTIATION
#define MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_SESSION_TICKETS
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TICKET_C
#define MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
#define MBEDTLS_SSL_TLS_C
//#define MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH

//#define MBEDTLS_PSK_MAX_LEN               32
//#define MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES      50
//#define MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT       86400
//#define MBEDTLS_SSL_CID_IN_LEN_MAX 32
//#define MBEDTLS_SSL_CID_OUT_LEN_MAX 32
//#define MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY 16
//#define MBEDTLS_SSL_CIPHERSUITES MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
//#define MBEDTLS_SSL_COOKIE_TIMEOUT        60
//#define MBEDTLS_SSL_DTLS_MAX_BUFFERING             32768
//#define MBEDTLS_SSL_IN_CONTENT_LEN              16384
//#define MBEDTLS_SSL_MAX_EARLY_DATA_SIZE        1024
//#define MBEDTLS_SSL_OUT_CONTENT_LEN             16384
//#define MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS 1
//#define MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE 6000
//#define MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH 32
```


#### SECTION X.509 feature selection
```
#define MBEDTLS_PKCS7_C
#define MBEDTLS_X509_CREATE_C
#define MBEDTLS_X509_CRL_PARSE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_CRT_WRITE_C
#define MBEDTLS_X509_CSR_PARSE_C
#define MBEDTLS_X509_CSR_WRITE_C
//#define MBEDTLS_X509_REMOVE_INFO
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT
//#define MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK
#define MBEDTLS_X509_USE_C

//#define MBEDTLS_X509_MAX_FILE_PATH_LEN     512
//#define MBEDTLS_X509_MAX_INTERMEDIATE_CA   8
```
