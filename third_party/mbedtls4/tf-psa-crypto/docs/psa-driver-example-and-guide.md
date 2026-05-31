# PSA Cryptoprocessor driver development examples

As of Mbed TLS 3.4.0, the PSA Driver Interface has only been partially implemented. As a result, the deliverables for writing a driver and the method for integrating a driver with Mbed TLS will vary depending on the operation being accelerated. This document describes how to write and integrate cryptoprocessor drivers depending on which operation or driver type is being implemented.

The `docs/proposed/` directory contains three documents which pertain to the proposed, work-in-progress driver system. The [PSA Driver Interface](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/proposed/psa-driver-interface.md) describes how drivers will interface with Mbed TLS in the future, as well as driver types, operation types, and entry points. As many key terms and concepts used in the examples in this document are defined in the PSA Driver Interface, it is recommended that developers read it prior to starting work on implementing drivers.
The PSA Driver [Developer](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/proposed/psa-driver-developer-guide.md) Guide describes the deliverables for writing a driver that can be used with Mbed TLS, and the PSA Driver [Integration](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/proposed/psa-driver-integration-guide.md) Guide describes how a driver can be built alongside Mbed TLS.

## Contents:
[Background on how Mbed TLS calls drivers](#background-on-how-mbed-tls-calls-drivers)\
[Process for Entry Points where auto-generation is implemented](#process-for-entry-points-where-auto-generation-is-implemented) \
[Process for Entry Points where auto-generation is not implemented](#process-for-entry-points-where-auto-generation-is-not-implemented) \
[Example: Manually integrating a software accelerator alongside Mbed TLS](#example-manually-integrating-a-software-accelerator-alongside-mbed-tls)

## Background on how Mbed TLS calls drivers

The PSA Driver Interface specification specifies which cryptographic operations can be accelerated by third-party drivers. Operations that are completed within one step (one function call), such as verifying a signature, are called *Single-Part Operations*. On the other hand, operations that consist of multiple steps implemented by different functions called sequentially are called *Multi-Part Operations*. Single-part operations implemented by a driver will have one entry point, while multi-part operations will have multiple: one for each step.

There are two types of drivers: *transparent* or *opaque*. See below an excerpt from the PSA Driver Interface specification defining them:
* **Transparent** drivers implement cryptographic operations on keys that are provided in cleartext at the beginning of each operation. They are typically used for hardware **accelerators**. When a transparent driver is available for a particular combination of parameters (cryptographic algorithm, key type and size, etc.), it is used instead of the default software implementation. Transparent drivers can also be pure software implementations that are distributed as plug-ins to a PSA Cryptography implementation (for example, an alternative implementation with different performance characteristics, or a certified implementation).
* **Opaque** drivers implement cryptographic operations on keys that can only be used inside a protected environment such as a **secure element**, a hardware security module, a smartcard, a secure enclave, etc. An opaque driver is invoked for the specific [key location](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/proposed/psa-driver-interface.md#lifetimes-and-locations) that the driver is registered for: the dispatch is based on the key's lifetime.

Mbed TLS contains a **driver dispatch layer** (also called a driver wrapper layer). For each cryptographic operation that supports driver acceleration (or sub-part of a multi-part operation), the library calls the corresponding function in the driver wrapper. Using flags set at compile time, the driver wrapper ascertains whether any present drivers support the operation. When no such driver is present, the built-in library implementation is called as a fallback (if allowed). When a compatible driver is present, the driver wrapper calls the driver entry point function provided by the driver author.

The long-term goal is for the driver dispatch layer to be auto-generated using a JSON driver description file provided by the driver author.
For some cryptographic operations, this auto-generation logic has already been implemented. When accelerating these operations, the instructions in the above documents can be followed. For the remaining operations which do not yet support auto-generation of the driver wrapper, developers will have to manually edit the driver dispatch layer and call their driver's entry point functions from there.

Auto-generation of the driver wrapper is supported for the operation entry points specified in the table below. Certain operations are only permitted for opaque drivers. All other operation entry points do not support auto-generation of the driver wrapper.

| Transparent Driver  | Opaque Driver       |
|---------------------|---------------------|
| `import_key`        | `import_key`        |
| `export_public_key` | `export_public_key` |
|                     | `export_key`        |
|                     | `copy_key`          |
|                     | `get_builtin_key`   |

### Process for Entry Points where auto-generation is implemented

If the driver is accelerating operations whose entry points are in the above table, the instructions in the driver [developer](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/proposed/psa-driver-developer-guide.md) and [integration](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/proposed/psa-driver-integration-guide.md) guides should be followed.

There are three deliverables for creating such a driver. These are:
 - A driver description file (in JSON format).
 - C header files defining the types required by the driver description. The names of these header files are declared in the driver description file.
 - An object file compiled for the target platform defining the functions required by the driver description. Implementations may allow drivers to be provided as source files and compiled with the core instead of being pre-compiled.

The Mbed TLS driver tests for the aforementioned entry points provide examples of how these deliverables can be implemented. For sample driver description JSON files, see [`mbedtls_test_transparent_driver.json`](https://github.com/Mbed-TLS/mbedtls/blob/development/tf-psa-crypto/scripts/data_files/driver_jsons/mbedtls_test_transparent_driver.json) or [`mbedtls_test_opaque_driver.json`](https://github.com/Mbed-TLS/mbedtls/blob/development/tf-psa-crypto/scripts/data_files/driver_jsons/mbedtls_test_transparent_driver.json). The header file required by the driver description is [`test_driver.h`](https://github.com/Mbed-TLS/mbedtls/blob/development/framework/tests/include/test/drivers/test_driver.h). As Mbed TLS tests are built from source, there is no object file for the test driver. However, the source for the test driver can be found under `framework/tests/src/drivers`.

### Process for Entry Points where auto-generation is not implemented

If the driver is accelerating operations whose entry points are not present in the table, a different process is followed where the developer manually edits the driver dispatch layer. The following steps describe this process. Steps 1, 2, 3, and 7 only need to be done once *per driver*. Steps 4, 5, and 6 must be done *for each single-part operation* or *for each sub-part of a multi-part operation* implemented by the driver.

**1. Choose a driver prefix and a macro name that indicates whether the driver is enabled** \
A driver prefix is simply a word (often the name of the driver) that all functions/macros associated with the driver should begin with. This is similar to how most functions/macros in Mbed TLS begin with `PSA_XXX/psa_xxx` or `MBEDTLS_XXX/mbedtls_xxx`. The macro name can follow the form `DRIVER_PREFIX_ENABLED` or something similar; it will be used to indicate the driver is available to be called. When building with the driver present, define this macro at compile time.

**2. Include the following in one of the driver header files:**
```
#if defined(DRIVER_PREFIX_ENABLED)
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif

// other definitions here

#endif
```

**3. Conditionally include header files required by the driver**
Include any header files required by the driver in `psa_crypto_driver_wrappers.h`, placing the `#include` statements within an `#if defined` block which checks if the driver is available:
```
#if defined(DRIVER_PREFIX_ENABLED)
#include ...
#endif
```


**4. For each operation being accelerated, locate the function in the driver dispatch layer that corresponds to the entry point of that operation.** \
The file `psa_crypto_driver_wrappers.h.jinja` and `psa_crypto_driver_wrappers_no_static.c.jinja` contains the driver wrapper functions. For the entry points that have driver wrapper auto-generation implemented, the functions have been replaced with `jinja` templating logic. While the file has a `.jinja` extension, the driver wrapper functions for the remaining entry points are simple C functions. The names of these functions are of the form `psa_driver_wrapper` followed by the entry point name. So, for example, the function `psa_driver_wrapper_sign_hash()` corresponds to the `sign_hash` entry point.

**5. If a driver entry point function has been provided then ensure it has the same signature as the driver wrapper function.** \
If one has not been provided then write one. Its name should begin with the driver prefix, followed by transparent/opaque (depending on driver type), and end with the entry point name. It should have the same signature as the driver wrapper function. The purpose of the entry point function is to take arguments in PSA format for the implemented operation and return outputs/status codes in PSA format. \
*Return Codes:*
* `PSA_SUCCESS`: Successful Execution
* `PSA_ERROR_NOT_SUPPORTED`: Input arguments are correct, but the driver does not support the operation. If a transparent driver returns this then it allows fallback to another driver or software implementation.
* `PSA_ERROR_XXX`: Any other PSA error code, see API documentation

**6. Modify the driver wrapper function** \
Each driver wrapper function contains a `switch` statement which checks the location of the key. If the key is stored in local storage, then operations are performed by a transparent driver. If it is stored elsewhere, then operations are performed by an opaque driver.
 * **Transparent drivers:** Calls to driver entry points go under `case PSA_KEY_LOCATION_LOCAL_STORAGE`.
 * **Opaque Drivers** Calls to driver entry points go in a separate `case` block corresponding to the key location.


The diagram below shows the layout of a driver wrapper function which can dispatch to two transparent drivers `Foo` and `Bar`, and one opaque driver `Baz`.

 ```
psa_driver_wrapper_xxx()
├── switch(location)
|   |
|   ├── case PSA_KEY_LOCATION_LOCAL_STORAGE //transparent driver
|   |   ├── #if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
|   |   |   ├── #if defined(FOO_DRIVER_PREFIX_ENABLED)
|   |   |   |   ├── if(//conditions for foo driver capibilities)
|   |   |   |   ├── foo_driver_transparent_xxx() //call to driver entry point
|   |   |   |   ├── if (status != PSA_ERROR_NOT_SUPPORTED) return status
|   |   |   ├── #endif
|   |   |   ├── #if defined(BAR_DRIVER_PREFIX_ENABLED)
|   |   |   |   ├── if(//conditions for bar driver capibilities)
|   |   |   |   ├── bar_driver_transparent_xxx() //call to driver entry point
|   |   |   |   ├── if (status != PSA_ERROR_NOT_SUPPORTED) return status
|   |   |   ├── #endif
|   |   ├── #endif
|   |
|   ├── case SECURE_ELEMENT_LOCATION //opaque driver
|   |   ├── #if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
|   |   |   ├── #if defined(BAZ_DRIVER_PREFIX_ENABLED)
|   |   |   |   ├── if(//conditions for baz driver capibilities)
|   |   |   |   ├── baz_driver_opaque_xxx() //call to driver entry point
|   |   |   |   ├── if (status != PSA_ERROR_NOT_SUPPORTED) return status
|   |   |   ├── #endif
|   |   ├── #endif
└── return psa_xxx_builtin() // fall back to built in implementation
 ```

All code related to driver calls within each `case` must be contained between `#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)` and a corresponding `#endif`. Within this block, each individual driver's compatibility checks and call to the entry point must be contained between `#if defined(DRIVER_PREFIX_ENABLED)` and a corresponding `#endif`. Checks that involve accessing key material using PSA macros, such as determining the key type or number of bits, must be done in the driver wrapper.

**7. Build Mbed TLS with the driver**
This guide assumes you are building Mbed TLS from source alongside your project. If building with a driver present, the chosen driver macro (`DRIVER_PREFIX_ENABLED`) must be defined. This can be done in two ways:
* *At compile time via flags.* This is the preferred option when your project uses Mbed TLS mostly out-of-the-box without significantly modifying the configuration. This can be done by passing the option via `CFLAGS`.
  * **Make**:
    ```
    make CFLAGS="-DDRIVER_PREFIX_ENABLED"
    ```
  * **CMake**: CFLAGS must be passed to CMake when it is invoked. Invoke CMake with

    ```
    CFLAGS="-DDRIVER_PREFIX_ENABLED" cmake path/to/source
    ```
* *Providing a user config file.* This is the preferred option when your project requires a custom configuration that is significantly different to the default. Define the macro for the driver, along with any other custom configurations in a separate header file, then use `config.py`, to set `MBEDTLS_USER_CONFIG_FILE`, providing the path to the defined header file. This will include your custom config file after the default. If you wish to completely replace the default config file, set `MBEDTLS_CONFIG_FILE` instead.

### Example: Manually integrating a software accelerator alongside Mbed TLS

[p256-m](https://github.com/mpg/p256-m) is a minimalistic implementation of ECDH and ECDSA on the NIST P-256 curve, specifically optimized for use in constrained 32-bit environments. It started out as an independent project and has been integrated in Mbed TLS as a PSA transparent driver. The source code of p256-m and the driver entry points is located in the Mbed TLS source tree under `drivers/p256-m`. In this section, we will look at how this integration was done.

The Mbed TLS build system includes the instructions needed to build p256-m. To build with and use p256-m, set the macro `MBEDTLS_PSA_P256M_DRIVER_ENABLED` using `config.py`, then build as usual using make/cmake. From the root of the `mbedtls/` directory, run:

    python3 scripts/config.py set MBEDTLS_PSA_P256M_DRIVER_ENABLED
    make

(You need extra steps if you want to disable the built-in implementation of ECC algorithms, which includes more features than p256-m. Refer to the documentation of `MBEDTLS_PSA_P256M_DRIVER_ENABLED` and [`driver-only-builds.md`](driver-only-builds.md) for more information.)

The driver prefix for p256-m is `P256`/`p256`.
The p256-m driver implements the following entry points: `"import_key"`, `"export_public_key"`, `"generate_key"`, `"key_agreement"`, `"sign_hash"`, `"verify_hash"`.
There are no entry points for `"sign_message"` and `"verify_message"`, which are not necessary for a sign-and-hash algorithm. The core still implements these functions by doing the hashes and then calling the sign/verify-hash entry points.
The driver entry point functions can be found in `p256m_driver_entrypoints.[hc]`. These functions act as an interface between Mbed TLS and p256-m; converting between PSA and p256-m argument formats and performing sanity checks. If the driver's status codes differ from PSA's, it is recommended to implement a status code translation function. The function `p256_to_psa_error()` converts error codes returned by p256-m into PSA error codes.

The driver wrapper functions in `psa_crypto_driver_wrappers.h.jinja` for all four entry points have also been modified. The code block below shows the additions made to `psa_driver_wrapper_sign_hash()`. In adherence to the defined process, all code related to the driver call is placed within a check for `MBEDTLS_PSA_P256M_DRIVER_ENABLED`. p256-m only supports non-deterministic ECDSA using keys based on NIST P256; these constraints are enforced through checks (see the `if` statement). Checks that involve accessing key attributes, (e.g. checking key type or bits) **must** be performed in the driver wrapper. This is because this information is marked private and may not be accessed outside the library. Other checks can be performed here or in the entry point function. The status returned by the driver is propagated up the call hierarchy **unless** the driver does not support the operation (i.e. return `PSA_ERROR_NOT_SUPPORTED`). In that case the next available driver/built-in implementation is called.

```
#if defined (MBEDTLS_PSA_P256M_DRIVER_ENABLED)
            if( PSA_KEY_TYPE_IS_ECC( psa_get_key_type(attributes) ) &&
                PSA_ALG_IS_ECDSA(alg) &&
                !PSA_ALG_ECDSA_IS_DETERMINISTIC( alg ) &&
                PSA_KEY_TYPE_ECC_GET_FAMILY(psa_get_key_type(attributes)) == PSA_ECC_FAMILY_SECP_R1 &&
                psa_get_key_bits(attributes) == 256 )
            {
                status = p256_transparent_sign_hash( attributes,
                                                     key_buffer,
                                                     key_buffer_size,
                                                     alg,
                                                     hash,
                                                     hash_length,
                                                     signature,
                                                     signature_size,
                                                     signature_length );
                if( status != PSA_ERROR_NOT_SUPPORTED )
                return( status );
            }
#endif /* MBEDTLS_PSA_P256M_DRIVER_ENABLED */
```
Following this, p256-m is now ready to use alongside Mbed TLS as a software accelerator. If `MBEDTLS_PSA_P256M_DRIVER_ENABLED` is set in the config, p256-m's implementations of key generation, ECDH, and ECDSA will be used where applicable.
