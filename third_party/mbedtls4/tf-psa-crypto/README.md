README for TF-PSA-Crypto
========================

The TF-PSA-Crypto repository provides an implementation of the
[PSA Cryptography API version 1.2](https://arm-software.github.io/psa-api/crypto/1.2/),
with the limitation that the implementation of the [PAKE extension](https://arm-software.github.io/psa-api/crypto/1.2/ext-pake/)
still has minor non-compliances, such as returned error codes.

The PSA Cryptography API implementation is organized around the
[PSA Cryptography driver interface](docs/proposed/psa-driver-interface.md)
aiming to ease the support of cryptographic accelerators and processors.

PSA Cryptography API
---------------------

The [PSA Cryptography API](https://arm-software.github.io/psa-api/crypto/)
provides access to a set of cryptographic primitives. It has a dual purpose.
First, it can be used in a PSA-compliant platform to build services, such as
secure boot, secure storage and secure communication. Second, it can also be
used independently of other PSA components on any platform.

The design goals of the PSA Cryptography API include:

* The API distinguishes caller memory from internal memory, which allows the
  library to be implemented in an isolated space for additional security.
  Library calls can be implemented as direct function calls if isolation is not
  desired, and as remote procedure calls if isolation is desired.
* The structure of internal data is hidden to the application, which allows
  substituting alternative implementations at build time or run time, for example,
  in order to take advantage of hardware accelerators.
* All access to the keys happens through key identifiers, which allows support
  for external cryptoprocessors that is transparent to applications.
* The interface to algorithms is generic, favoring algorithm agility.
* The interface is designed to be easy to use and hard to accidentally misuse.

We welcomes feedback on the design of the API. If you think something could be
improved, please open an issue on our Github repository. Alternatively, if you
prefer to provide your feedback privately, please email us at
[`mbed-crypto@arm.com`](mailto:mbed-crypto@arm.com). All feedback received by
email is treated confidentially.

### PSA Cryptography API implementation in TF-PSA-Crypto

TF-PSA-Crypto includes an implementation of the PSA Cryptography API. It covers
most, but not all algorithms.

### PSA Cryptography driver interface

TF-PSA-Crypto supports drivers for cryptographic accelerators, secure elements
and random generators. This is work in progress. Please note that the driver
interfaces are not fully stable yet and may change without notice. We intend to
preserve backward compatibility for application code (using the PSA Cryptography API),
but the code of the drivers may have to change in future minor releases of TF-PSA-Crypto.

Please see the [PSA driver example and guide](docs/psa-driver-example-and-guide.md)
for information on writing a driver.

Configuration
-------------

The TF-PSA-Crypto repository should build out of the box on most systems. Its
configuration is based on C preprocessor macros defined in
`include/psa/crypto_config.h`.

These configuration options are organized into seven groups:
1. Cryptographic mechanism selection (PSA API): PSA_WANT_xxx macros that
   specify which parts of the PSA Cryptography API the user wants to enable,
   e.g. cryptographic algorithms, key types, elliptic curves.
1. Platform abstraction layer: Options to port the library to different platforms.
1. General and test configuration options: Options that are test-specific or
   not related to a particular part of the library.
1. Cryptographic mechanism selection (extended API): Options to enable
   cryptographic mechanisms beyond the current PSA Cryptography API, such as LMS
   or key wrapping.
1. Data format support: Options to enable support for various data formats,
   such as ASN.1 or PEM.
1. PSA core: Options to configure components other than cryptographic mechanisms,
   such as key management and random number generation.
1. Built-in drivers: Options to configure non-functional aspects of built-in
   cryptographic mechanisms such as performance/size trade-offs.

The file `include/psa/crypto_config.h` can be edited manually, or in a more
programmatic way using the Python script `scripts/config.py` (use `--help` for
usage instructions).

We provide some non-standard configurations focused on specific use cases in the
`configs/` directory. You can read more about those in `configs/README.txt`.

Documentation
-------------

Documentation for the PSA Cryptography API is available [on GitHub](https://arm-software.github.io/psa-api/crypto/).

To generate a local copy of the library documentation in HTML format:

1. Make sure that [Doxygen](http://www.doxygen.nl/) is installed.
1. Run `cmake -B /path/to/build_dir /path/to/TF-PSA-Crypto/source`
1. Run `cmake --build /path/to/build_dir --target tfpsacrypto-apidoc`
1. Open one of the main generated HTML files:
   * `apidoc/index.html`
   * `apidoc/modules.html` or `apidoc/topics.html`

Compiling
---------

The build system is CMake.
The CMake build system creates one library: libtfpsacrypto.

### Tool versions

You need the following tools to build the library from the main branch with the
provided CMake files. TF-PSA-Crypto minimum tool version requirements are set
based on the versions shipped in the latest or penultimate (depending on the
release cadence) long-term support releases of major Linux distributions,
namely at time of writing: Ubuntu 22.04, RHEL 9, and SLES 15 SP4.

* A C99 toolchain (compiler, linker, archiver). We actively test with GCC 5.4,
  Clang 3.8 and Arm Compiler 6.21. More recent versions should work. Slightly
  older versions may work.
* Python 3.8 or later to generate some source files (see below), the test
  code, and to generate sample programs.
* CMake 3.20.2 or later.
* A build system like Make or Ninja for which CMake can generate build files.
* Microsoft Visual Studio 2019 or later (if using Visual Studio).
* Doxygen 1.8.14 or later.

### Git usage

The supported branches (see [`BRANCHES.md`](BRANCHES.md)) use a [Git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules#_cloning_submodules),
[framework](https://github.com/TF-PSA-Crypto/mbedtls-framework).
Release tags also use Git submodules.

After cloning or checking out a branch or tag, run:
    ```
    git submodule update --init
    ```
 to initialize and update the submodule before building.

However, the official source release tarballs (e.g. [tf-psa-crypto-1.0.0.tar.bz2](https://github.com/Mbed-TLS/TF-PSA-Crypto/releases/download/tf-psa-crypto-1.0.0/tf-psa-crypto-1.0.0.tar.bz2))
include the content of the submodule.

### Generated source files in the development branch

The source code of TF-PSA-Crypto includes some files that are automatically
generated by scripts and whose content depends only on the TF-PSA-Crypto source,
not on the platform or on the library configuration. These files are not included
in the development branch of TF-PSA-Crypto, but the generated files are included
in official releases. This section explains how to generate the missing files in
the development branch.

The following tools are required:
* Python 3 and some Python packages, for some library source files, sample programs
  and test data. To install the necessary packages, run:
  ```
  python3 -m pip install --user -r scripts/basic.requirements.txt
  ```
  Depending on your Python installation, you may need to invoke `python` instead
  of `python3`. To install the packages system-wide or in a virtual environment,
  omit the `--user` option.
* A C compiler for the host platform, for some test data.

The scripts that generate the configuration-independent files will look for a
host C compiler in the following places (in order of preference):

1. The `HOSTCC` environment variable. This can be used if `CC` is pointing to a
   cross-compiler.
1. The `CC` environment variable.
1. An executable called `cc` in the current path.

Note: If you have multiple toolchains installed, it is recommended to set `CC`
or `HOSTCC` to the intended host compiler before generating the files.

Any of the following methods are available to generate the configuration-independent
files:

* On non-Windows systems, when not cross-compiling, CMake generates the required
  files automatically.
* Run `framework/scripts/make_generated_files.py` to generate all the
  configuration-independent files.

### CMake

In order to build the source using CMake in a separate directory (recommended),
just enter at the command line:

    mkdir /path/to/build_dir && cd /path/to/build_dir
    cmake /path/to/tf/psa/crypto/source
    cmake --build .

In order to run the tests, enter:

    ctest

The test suites need Python to be built. If you don't have Python installed,
you'll want to disable the test suites with:

    cmake -DENABLE_TESTING=Off /path/to/tf/psa/crypto/source

To configure CMake for building shared libraries, use:

    cmake -DUSE_SHARED_TF_PSA_CRYPTO_LIBRARY=On /path/to/tf/psa/crypto/source

There are many different build modes available within the CMake build system.
Most of them are available for gcc and clang, though some are compiler-specific:

- `Release`. This generates the default code without any unnecessary
  information in the binary files.
- `Debug`. This generates debug information and disables optimization of the code.
- `ASan`. This instruments the code with AddressSanitizer to check for memory
  errors. (This includes LeakSanitizer, with recent version of gcc and clang.)
  (With recent version of clang, this mode also instruments the code with
  UndefinedSanitizer to check for undefined behaviour.)
- `ASanDbg`. Same as ASan but slower, with debug information and better stack
  traces.
- `MemSan`. This instruments the code with MemorySanitizer to check for
  uninitialised memory reads. Experimental, needs recent clang on Linux/x86\_64.
- `MemSanDbg`. Same as MemSan but slower, with debug information, better stack
  traces and origin tracking.
- `Check`. This activates the compiler warnings that depend on optimization and
  treats all warnings as errors.
- `TSan`. This instruments the code with ThreadSanitizer to detect data races
   and other threading-related concurrency issues at runtime.
- `TSanDbg`. Same as TSan but slower, with debug information, better stack
  traces and origin tracking.

Switching build modes in CMake is simple. For debug mode, enter at the command
line:

    cmake -D CMAKE_BUILD_TYPE=Debug /path/to/tf/psa/crypto/source

To list other available CMake options, use:

    cmake -LH

Note that, with CMake, you can't adjust the compiler or its flags after the
initial invocation of cmake. This means that `CC=your_cc make` and `make
CC=your_cc` will *not* work (similarly with `CFLAGS` and other variables).
These variables need to be adjusted when invoking cmake for the first time,
for example:

    CC=your_cc cmake /path/to/tf/psa/crypto/source

If you already invoked cmake and want to change those settings, you need to
invoke the configuration phase of CMake again with the new settings.

Note that it is possible to build in-place, use:

    cmake .
    cmake --build .

Regarding variables, also note that if you set CFLAGS when invoking cmake,
your value of CFLAGS doesn't override the content provided by CMake (depending
on the build mode as seen above), it's merely prepended to it.

#### Consuming TF-PSA-Crypto

TF-PSA-Crypto provides a CMake package configuration file for consumption as a
dependency in other CMake projects. You can load its CMake library target with:

    find_package(TF-PSA-Crypto)

You can help CMake find the package:

- By setting the variable `TF-PSA-Crypto_DIR` to `${YOUR_TF_PSA_CRYPTO_BUILD_DIR}/cmake`,
  as shown in `programs/test/cmake_package/CMakeLists.txt`, or
- By adding the TF-PSA-Crypto installation prefix to `CMAKE_PREFIX_PATH`,
  as shown in `programs/test/cmake_package_install/CMakeLists.txt`.

After a successful `find_package(TF-PSA-Crypto)`, the target `TF-PSA-Crypto::tfpsacrypto`
is available.

You can then use it directly through `target_link_libraries()`:

    add_executable(xyz)

    target_link_libraries(xyz PUBLIC TF-PSA-Crypto::tfpsacrypto)

This will link the TF-PSA-Crypto library to your library or application, and
add its include directories to your target (transitively, in the case of
`PUBLIC` or `INTERFACE` link libraries).

#### TF-PSA-Crypto as a subproject

The TF-PSA-Crypto repository supports being built as a CMake subproject. One
can use `add_subdirectory()` from a parent CMake project to include
TF-PSA-Crypto as a subproject.

### Microsoft Visual Studio

The TF-PSA-Crypto library can be built with Microsoft Visual Studio Community
suitably installed as a CMake project. For a general documentation about
CMake projects in Visual Studio, please refer to its documentation.

The following instructions have been tested on Microsoft Visual Studio Community
2019 Version 16.11.26. The TF-PSA-Crypto library and its tests build out of the
box with:
* Visual Studio Community installed with the default "Desktop development with C++"
and "Python development" workloads.
* Python libraries needed for code and test generation installed as defined
in basic.requirements.txt. Refer to the Visual Studio documentation of Python
environments.
* When cloning the TF-PSA-Crypto repository in Visual Studio, a
CMakeSettings.json is created at the root of the repository. In that file, add
the line `"environments": [ {"CC" : "cl"} ]` to the configuration. That way
when building the library and its tests, a CC environment variable is set with
value "cl". This is needed by Python scripts that generate test cases. The
CMakeSettings.json file can be edited in Visual Studio by following
Project > CMake Settings for TF-PSA-Crypto > Edit JSON.
* If necessary (it may have been done automatically when updating
CMakeSettings.json), generate the CMake cache: Project > Generate Cache
* Build the library: Build > Build All
* The test suites can then be run: Test > Run CTests for TF-PSA-Crypto

Example programs
----------------

We've included example programs for different features and uses in
[`programs/`](programs/README.md). Please note that the goal of these sample
programs is to demonstrate specific features of the library, and the code may
need to be adapted to build a real-world application.

Tests
-----

The TF-PSA-Crypto repository includes an elaborate test suite in `tests/` that
initially requires Python to generate the tests files
(e.g. `test_suite_psa_crypto.c`). These files are generated from a
`function file` (e.g. `suites/test_suite_psa_crypto.function`) and a
`data file` (e.g. `suites/test_suite_psa_crypto.data`). The `function file`
contains the test functions. The `data file` contains the test cases, specified
as parameters that will be passed to the test function.

Porting TF-PSA-Crypto
---------------------

TF-PSA-Crypto can be ported to many different architectures, OS's and platforms.
Before starting a port, you may find the following Knowledge Base articles useful:

-   [Porting Mbed TLS to a new environment or OS](https://mbed-tls.readthedocs.io/en/latest/kb/how-to/how-do-i-port-mbed-tls-to-a-new-environment-OS/)
-   [What external dependencies does Mbed TLS rely on?](https://mbed-tls.readthedocs.io/en/latest/kb/development/what-external-dependencies-does-mbedtls-rely-on/)
-   [How do I configure Mbed TLS](https://mbed-tls.readthedocs.io/en/latest/kb/compiling-and-building/how-do-i-configure-mbedtls/)

TF-PSA-Crypto is mostly written in portable C99; however, it has a few platform
requirements that go beyond the standard, but are met by most modern architectures:

- Bytes must be 8 bits.
- All-bits-zero must be a valid representation of a null pointer.
- Signed integers must be represented using two's complement.
- `int` and `size_t` must be at least 32 bits wide.
- The types `uint8_t`, `uint16_t`, `uint32_t` and their signed equivalents must be available.
- Mixed-endian platforms are not supported.
- SIZE_MAX must be at least as big as INT_MAX and UINT_MAX.

License
-------

Unless specifically indicated otherwise in a file, TF-PSA-Crypto files are provided
under a dual [Apache-2.0](https://spdx.org/licenses/Apache-2.0.html) OR
[GPL-2.0-or-later](https://spdx.org/licenses/GPL-2.0-or-later.html) license.
See the [LICENSE](LICENSE) file for the full text of these licenses.

### Third-party code included in TF-PSA-Crypto

This project contains code from other projects. This code is located within the
`drivers/` directory. The original license text is included within project
subdirectories, where it differs from the normal Mbed TLS license, and/or in
source files. The projects are listed below:

* `drivers/everest/`: Files stem from [Project Everest](https://project-everest.github.io/)
  and are distributed under the Apache 2.0 license.
* `drivers/p256-m/p256-m/`: Files have been taken from the [p256-m](https://github.com/mpg/p256-m)
  repository. The code in the original repository is distributed under the
  Apache 2.0 license. It is distributed in TF-PSA-Crypto under a dual Apache-2.0
  OR GPL-2.0-or-later license with permission from the author.

Contributing
------------

We gratefully accept bug reports and contributions from the community. Please
see the [contributing guidelines](CONTRIBUTING.md) for details on how to do this.

Contact
-------

* To report a security vulnerability in TF-PSA-Crypto, please email <mbed-tls-security@lists.trustedfirmware.org>. For more information, see [`SECURITY.md`](SECURITY.md).
* To report a bug or request a feature in TF-PSA-Crypto, please [file an issue on GitHub](https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/new/choose).
* Please see [`SUPPORT.md`](SUPPORT.md) for other channels for discussion and
support about TF-PSA-Crypto.
