Migrating to an auto generated psa_crypto_driver_wrappers.h file
================================================================

This document describes how to migrate to the auto generated psa_crypto_driver_wrappers.h file.
It is meant to give the library user migration guidelines while the Mbed TLS project tides over multiple minor revs of version 1.0, after which this will be merged into psa-driver-interface.md.

For a practical guide with a description of the current state of drivers Mbed TLS, see our [PSA Cryptoprocessor driver development examples](../psa-driver-example-and-guide.md).

## Introduction

The design of the Driver Wrappers code generation is based on the design proposal https://github.com/Mbed-TLS/mbedtls/pull/5067
During the process of implementation there might be minor variations wrt versioning and broader implementation specific ideas, but the design remains the same.

## Prerequisites

Python3, Jinja2 rev 2.10.1 and jsonschema rev 3.2.0

## Feature Version

1.1

### What's critical for a migrating user

The Driver Wrapper auto generation project is designed to use a python templating library ( Jinja2 ) to render templates based on drivers that are defined using a Driver description JSON file(s).

While that is the larger goal, for version 1.1 here's what's changed

#### What's changed

(1) psa_crypto_driver_wrappers.h will from this point on be auto generated.
(2) The auto generation is based on the template file at **scripts/data_files/driver_templates/psa_crypto_driver_wrappers.h.jinja**.
(3) The driver JSONS to be used for generating the psa_crypto_driver_wrappers.h file can be found at **scripts/data_files/driver_jsons/** as their default location, this path includes the schemas against which the driver schemas will be validated (driver_opaque_schema.json, driver_transparent_schema.json) and a driverlist.json which specifies the drivers to be considered and the order in which they want to be called into. The default location for driverlist.json and driver JSONS can be overloaded by passing an argument --json-dir while running the script generate_driver_wrappers.py.
(4) While the complete driver wrapper templating support is yet to come in, if the library user sees a need to patch psa_crypto_driver_wrappers.h file, the user will need to patch into the template file as needed (psa_crypto_driver_wrappers.h.jinja).

#### How to set your driver up

Please refer to psa-driver-interface.md for information on how a driver schema can be written.
One can also refer to the example test drivers/ JSON schemas under **scripts/data_files/driver_jsons/**.

The JSON file 'driverlist.json' is meant to be edited by the user to reflect the drivers one wants to use on a device. The order in which the drivers are passed is also essential if/when there are multiple transparent drivers on a given system to retain the same order in the templating.
