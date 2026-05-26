# psasim

PSASIM holds necessary C source and header files which allows to test Mbed TLS in a "pure crypto client" scenario, i.e `MBEDTLS_PSA_CRYPTO_CLIENT && !MBEDTLS_PSA_CRYPTO_C`.
In practical terms it means that this allow to build PSASIM with Mbed TLS sources and get 2 Linux applications, a client and a server, which are connected through Linux's shared memeory, and in which the client relies on the server to perform all PSA Crypto operations.

The goal of PSASIM is _not_ to provide a ready-to-use solution for anyone looking to implement the pure crypto client structure (see [Limitations](#limitations) for details), but to provide an example of TF-PSA-Crypto RPC (Remote Procedure Call) implementation using Mbed TLS.

## Limitations

In the current implementation:

- Only Linux PC is supported.
- There can be only 1 client connected to 1 server.
- Shared memory is the only communication medium allowed. Others can be implemented (ex: net sockets), but in terms of simulation speed shared memory proved to be the fastest.
- Server is not secure at all: keys and operation structs are stored on the RAM, so they can easily be dumped.

## Building

### Build tooling

Building PSASIM requires the following tools:

* GNU make.
* A C compiler.
* Perl.
* The JSON package for Perl (`cpain -i JSON` or `apt install libjson-perl`).

### Build instructions

The build instructions are in `framework/psasim/Makefile`, with the assistance of `scripts/crypto-common.make` in TF-PSA-Crypto. The main targets are:

* `client_libs`: builds object files to be linked with a client. The client code is expected to include TF-PSA-Crypto with `MBEDTLS_PSA_CRYPTO_CLIENT` enabled and `MBEDTLS_PSA_CRYPTO_C` disabled, with no local cryptographic primitives.
* `test/psa_server`: builds a server, including the crypto partition. This requires TF-PSA-Crypto compiled with the PSA core (`MBEDTLS_PSA_CRYPTO_C`) and cryptographic primitives.

Note in particular that the client and the server require different builds of `libtfpsacrypto`, since they must have different configurations.

Note that at the time of writing, building PSASIM only officially works from an Mbed TLS tree. It might not work from a standalone TF-PSA-Crypto tree.

Some C files are generated from JSON data and from parsing TF-PSA-Crypto header files, using the scripts `src/psa_sim_generate.pl` and `src/psa_sim_serialise.pl`. They are not committed into version control, but rather generated during the build of the client or the server, according to the instructions in the makefile.

### Testing

Please refer to [`tests/scripts/components-psasim.sh` in Mbed TLS](https://github.com/Mbed-TLS/mbedtls/blob/development/tests/scripts/components-psasim.sh) and the `helper_psasim_xxx` auxiliary functions in [`framework/scripts/all-helpers.sh`](../scripts/all-helpers.sh) for guidance on how to build & test PSASIM:

- `component_test_psasim()`: builds the server and a couple of test clients which are used to evaluate some basic PSA Crypto API commands.
- `component_test_suite_with_psasim()`: builds the server and _all_ the usual test suites (those found under the `<mbedtls-root>/tests/suites/*` folder) which are used by the CI and runs them. A small subset of test suites (`test_suite_constant_time_hmac`,`test_suite_lmots`,`test_suite_lms`) are being skipped, for CI turnover time optimization. They can be run locally if required.
