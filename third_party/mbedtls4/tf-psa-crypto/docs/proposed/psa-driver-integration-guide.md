Building Mbed TLS with PSA cryptoprocessor drivers
==================================================

**This is a specification of work in progress. The implementation is not yet merged into Mbed TLS.**
For a description of the current state of drivers Mbed TLS, see our [PSA Cryptoprocessor driver development examples](../psa-driver-example-and-guide.html).

This document describes how to build Mbed TLS with additional cryptoprocessor drivers that follow the PSA cryptoprocessor driver interface.

The interface is not fully implemented in Mbed TLS yet. Please note that the interface may still change: until further notice, we do not guarantee backward compatibility with existing driver code.

## Introduction

The PSA cryptography driver interface provides a way to build Mbed TLS with additional code that implements certain cryptographic primitives. This is primarily intended to support platform-specific hardware.

Note that such drivers are only available through the PSA cryptography API (crypto functions beginning with `psa_`, and X.509 and TLS interfaces that reference PSA types).

Concretely speaking, a driver consists of one or more **driver description files** in JSON format and some code to include in the build. The driver code can either be provided in binary form as additional object file to link, or in source form.

## How to build Mbed TLS with drivers

To build Mbed TLS with drivers:

1. Pass the driver description files through the Make variable `PSA_DRIVERS` when building the library.

    ```
    cd /path/to/mbedtls
    make PSA_DRIVERS="/path/to/acme/driver.json /path/to/nadir/driver.json" lib
    ```

2. Link your application with the implementation of the driver functions.

    ```
    cd /path/to/application
    ld myapp.o -L/path/to/acme -lacmedriver -L/path/to/nadir -lnadirdriver -L/path/to/mbedtls -lmbedcrypto
    ```

<!-- TODO: what if the driver is provided as C source code? -->

<!-- TODO: what about additional include files? -->
