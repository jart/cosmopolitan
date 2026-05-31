PSA Cryptoprocessor driver developer's guide
============================================

**This is a specification of work in progress. The implementation is not yet merged into Mbed TLS.**
For a description of the current state of drivers Mbed TLS, see our [PSA Cryptoprocessor driver development examples](../psa-driver-example-and-guide.html).

This document describes how to write drivers of cryptoprocessors such as accelerators and secure elements for the PSA cryptography subsystem of Mbed TLS.

This document focuses on behavior that is specific to Mbed TLS. For a reference of the interface between Mbed TLS and drivers, refer to the [PSA Cryptoprocessor Driver Interface specification](psa-driver-interface.html).

The interface is not fully implemented in Mbed TLS yet. Please note that the interface may still change: until further notice, we do not guarantee backward compatibility with existing driver code.

## Introduction

### Purpose

The PSA cryptography driver interface provides a way to build Mbed TLS with additional code that implements certain cryptographic primitives. This is primarily intended to support platform-specific hardware.

There are two types of drivers:

* **Transparent** drivers implement cryptographic operations on keys that are provided in cleartext at the beginning of each operation. They are typically used for hardware **accelerators**. When a transparent driver is available for a particular combination of parameters (cryptographic algorithm, key type and size, etc.), it is used instead of the default software implementation. Transparent drivers can also be pure software implementations that are distributed as plug-ins to a PSA Crypto implementation.
* **Opaque** drivers implement cryptographic operations on keys that can only be used inside a protected environment such as a **secure element**, a hardware security module, a smartcard, a secure enclave, etc. An opaque driver is invoked for the specific key location that the driver is registered for: the dispatch is based on the key's lifetime.

### Deliverables for a driver

To write a driver, you need to implement some functions with C linkage, and to declare these functions in a **driver description file**. The driver description file declares which functions the driver implements and what cryptographic mechanisms they support. Depending on the driver type, you may also need to define some C types and macros in a header file.

The concrete syntax for a driver description file is JSON. The structure of this JSON file is specified in the section [“Driver description syntax”](psa-driver-interface.html#driver-description-syntax) of the PSA cryptography driver interface specification.

A driver therefore consists of:

* A driver description file (in JSON format).
* C header files defining the types required by the driver description. The names of these header files is declared in the driver description file.
* An object file compiled for the target platform defining the functions required by the driver description. Implementations may allow drivers to be provided as source files and compiled with the core instead of being pre-compiled.

## Driver C interfaces

Mbed TLS calls driver entry points [as specified in the PSA Cryptography Driver Interface specification](psa-driver-interface.html#driver-entry-points) except as otherwise indicated in this section.

## Mbed TLS extensions

The driver description can include Mbed TLS extensions (marked by the namespace "mbedtls"). Mbed TLS extensions are meant to extend/help integrating the driver into the library's infrastructure.
* `"mbedtls/h_condition"` (optional, string) can include complex preprocessor definitions to conditionally include header files for a given driver. 
* `"mbedtls/c_condition"` (optional, string) can include complex preprocessor definitions to conditionally enable dispatch capabilities for a driver.

## Building and testing your driver

<!-- TODO -->

## Dependencies on the Mbed TLS configuration

<!-- TODO -->
