# Mbed TLS and TF-PSA-Crypto configuration check framework

This document describes the compile-time configuration check framework. The goal of this framework is to detect bad configurations when the library is compiled. It consists of three parts:

* [manually written checks](#manually-written-checks) (since PolarSSL);
* [generated checks](#generated-checks) (since TF-PSA-Crypto 1.0 and Mbed TLS 4.0);
* [validating that some configurations are recognized as bad](#validation) (since TF-PSA-Crypto 1.0 and Mbed TLS 4.0).

## Introduction

### What is a configuration?

A configuration (“config” for short) is a set of macro definitions that are chosen by the user who compiles the library, with defaults provided by the library maintainers. Those definitions may come from:

* the compiler command line (`-D`);
* The subproject's configuration files, if applicable (i.e. TF-PSA-Crypto's config files are part of the Mbed TLS config);
* The project's main configuration file: the macro expansion of `MBEDTLS_CONFIG_FILE` or `TF_PSA_CRYPTO_CONFIG_FILE`, defaulting to `"mbedtls/mbedtls_config.h"` or `"psa/crypto_config.h"` respectively;
* The project's user configuration, given by the macro `MBEDTLS_USER_CONFIG_FILE` or `TF_PSA_CRYPTO_USER_CONFIG_FILE`, if defined.

### Why are configurations bad?

There are several reasons why we might consider a configuration undesirable (“bad”) for short, including:

* The configuration would lead to a compile-time error. We prefer to emit a comprehensible error message, rather than let the user puzzle over e.g. a missing symbol at link time.
* The configuration is technically valid but unlikely to be desirable. For example, enabling ECC support but not at least one curve. Since we don't test such configurations, they may or may not work anyway.
* The configuration is technically valid and should work, but potentially dangerous. For example, building with a weak random generator. In such cases, if the configuration may be accidentally weak, we may want to require the user to an additional “`I_KNOW_WHAT_I_M_DOING`” macro.
* The user defined or undefined a macro which is normally set internally (typically in `*adjust*.h`). This is very likely to lead to internal consistencies, so we would like to reject the attempt.
* The user defined or undefined a macro that used to be meaningful in a previous version of the library, but no longer is. We would like to emit an error that points the user to a replacement, if applicable.
* The user defined a configuration option in the wrong project. A superproject must have the same configuration of the subproject that was used to build the subproject — concretely, setting a crypto option in Mbed TLS to a different value than in TF-PSA-Crypto is very likely to lead to inconsistencies in Mbed TLS.

Some of these cases follow generic patterns that are susceptible for automation (e.g. options removed in the last major release), while others are very ad hoc and need to be defined manually (e.g. at least one of a set). Hence we have both [generated checks](#generated-checks) and [manually written checks](#manually-written-checks).

### When is the configuration checked?

We want to check the configuration when building the library.

Applications are very likely to misbehave if they are built with different compilation options from the library.
It is tempting to check the configuration when building applications: what if the user tried editing the config file after the library was built, or passed different options on the compiler command line?

Users who try to change the configuration are very likely to reach a configuration that's still internally consistent, for example just a few more features, or different sizes. Configuration checks during the application build cannot detect that, since they have no access to the actual library configuration. Hence application-build-time checks would provide very little safety, but a false sense of safety. So since TF-PSA-Crypto 1.0 and Mbed TLS 4.0, we perform the checks only when building the library. (It was different before — see “[Configuration checks up to Mbed TLS 3.x](#configuration-checks-up-to-mbed-tls-3-x)” — but only for historical reasons, because in PolarSSL, all header files were public.)

## Analysis of bad configurations

This section analyses why certain kinds of configurations are bad. Note that not all bad configurations are actually rejected; consult the generator code for details (see “[How the checks are generated](#how-the-checks-are-generated)”).

### Command line definitions

Applications using the library must be built with the same configuration as the library. Otherwise they may make incompatible assumptions leading to undefined behavior at run time. Therefore it is problematic to define configuration options on the compiler command line (`cc -DMBEDTLS_XXX ...`).

The configuration file variables (<code><em>PROJECT</em>\_CONFIG\_FILE</code>, and possibly <code><em>PROJECT</em>\_USER\_CONFIG\_FILE</code>) are exceptions because it makes sense to, for example, compile the library with `MBEDTLS_CONFIG_FILE="../../acme_platform/include/mbedtls_config.h"` (avoiding changes to the source tree and constraints on the include path order) and then install the file as `/usr/include/mbedtls/mbedtls_config.h`. For those, what matters is the content.

The config check generator script makes it easy to detect if an option was set on the compiler command line. However, many embedded projects compile the whole platform with a single set of compiler options, and those options may include some of our config options. So if we reject command line options, we should provide an official way to bypass accept them.

As of TF-PSA-Crypto 1.0 and Mbed TLS 4.0, configuration checks do not treat options set on the command line differently from options set in the config file.

### Derived macros

We derive many macros conditionally from the configuration options, mostly in `*adjust*.h`. (These headers can also define additional options to handle implicit requirements, but that part is not relevant here.) These macros must be consistent with the configuration, otherwise the library may misbehave (e.g. overflow internal buffers).

Thus it is dangerous for derived macros to be set in the configuration file. (We could make it inoffensive by systematically either defining or explicitly undefining each derived macro, but that would be extra burden for which a mistake is easily made.)

Unsetting a derived macro is not dangerous, but won't have any effect since the macro is not set yet. So we can reject it but it isn't particularly important.

We can detect bad setting of derived macros by erroring if they are set after including the config files and before doing the adjustments.

### Subproject options

The superproject (Mbed TLS) reads the configuration of the subproject (TF-PSA-Crypto), but the converse is not true. Therefore:

* Setting a superproject option identically in the subproject and in the superproject is perfectly fine.
* Setting a superproject option only in the subproject is maybe a little weird, but harmless.
* Setting a superproject option to different values in the superproject and in a subproject has a well-defined effect, but is probably a mistake. (Note that for boolean options, setting to off is usually indistinguishable from not setting, but can technically be done with `#undef`.)
* Setting a subproject option in the superproject, except when it was already set (to the same value if applicable), is dangerous. It won't enable the feature and may lead to undefined behavior in the superproject.
* It is also dangerous to set or unset a macro in the superproject's configuration if that macro is a derived one in the subproject.

In Mbed TLS 4.x, there is a high risk that users migrating from Mbed TLS 3.6 will accidentally leave a crypto option in `mbedtls_config.h` instead of moving it to `crypto_config.h`. So we should particularly try to handle this case: macros that were configuration options in Mbed TLS 3.6 and that are now a configuration option or a derived macro in TF-PSA-Crypto.

### Removed options

Users who are upgrading from a previous major version of Mbed TLS (or TF-PSA-Crypto, once TF-PSA-Crypto 2.0 is released) may still attempt to use removed options, if they haven't fully upgraded their configuration file. This will likely not have the intended effect. Worse, if a former option has become an [internal macro that is now derived from other options](#derived-macros), this may result in an inconsistent configuration. This is notably the case for legacy crypto options from Mbed TLS 3.6, which for the most part are internal macros in TF-PSA-Crypto 1.0.

A configuration file may use removed options harmlessly if the configuration was intended to work with both the old and the new version of the library. Users who want to do that can make the definitions conditional on the library version (e.g. `#if defined(TF_PSA_CRYPTO_VERSION_MAJOR) && TF_PSA_CRYPTO_VERSION_MAJOR > 1`), so if we have a mechanism to forbid removed options, it is not particularly useful to allow users to bypass it.

Options that exist in Mbed TLS 3.6 and Mbed TLS 4.0 should not be considered removed from TF-PSA-Crypto 1.0, since it is perfectly legitimate to set them for the sake of Mbed TLS (see “[Subproject options](#subproject-options)”).

## Manually written checks

### Location of the manually written checks

Most manually configuration checks are located in <code><em>LIBRARY\_DIRECTORY</em>/<em>PROJECT\_NAME</em>\_check\_config.h</code>.
This header is included by <code><em>LIBRARY\_DIRECTORY</em>/<em>PROJECT\_NAME</em>_config.c</code>.

### Behavior of the manually written checks

The manually written checks run on the finalized configuration, i.e. at the end of `build_info.h`, after `*adjust*.h`.

The checks generally fit into a few patterns:

* If X is enabled then Y must be enabled because Y requires X to work. This was very common historically. Since Mbed TLS 3.0, we have gradually moved towards a more additive system, where enabling Y automatically enables its dependencies, but many cases of direct requirements still need to be specified manually.
* If X is enabled then one of Y1, Y2, Y3… must be enabled. We can't enable a Y automatically because we don't know which one.
* X and Y can't both be enabled.

### Configuration checks up to Mbed TLS 3.x

In Mbed TLS 2.x, there were manually written config checks in `<mbedtls/check_config.h>`. It was the job of `mbedtls/config.h` (which could be overridden by the user) to include that file. The configuration checks were thus performed whenever building a library source file or an application source file.

In Mbed TLS 3.x, there were manually written config checks in `<mbedtls/check_config.h>`. This header was systematically included by `<mbedtls/build_info.h>`. The configuration checks were thus performed whenever building a library source file or an application source file.

## Generated checks

### Location of the generated checks

The generated checks are located in
<code><em>LIBRARY\_DIRECTORY</em>/<em>PROJECT\_NAME</em>\_config\_check\_\*.h</code>.
These headers are included by <code><em>LIBRARY\_DIRECTORY</em>/<em>PROJECT\_NAME</em>\_config.c</code>.

These are internal headers, included by one library file. This way, we run the config checks exactly once during a normal build.

### Behavior of the generated checks

The generated checks consist of three parts:

1. Some initial setup in `*_before.h`, to detect the situation before including the user's configuration file (but after the command line — this is unavoidable). This header is included before `build_info.h`.
2. The actual checks on the user configuration, in `*_after.h`. This header should also clean up by undefining some temporary macros. This header is included by `build_info.h` after reading the user's configuration files, but before defining derived macros in `*adjust*.h`. (This is not the normal behavior of `build_info.h`, it is done only if the file that includes `build_info.h` defines the macro <code><em>PROJECT\_NAME</em>\_INCLUDE\_AFTER\_RAW\_CONFIG</code>.)
3. Additional checks that can be performed on the final configuration. At the time of writing, all such checks are [written manually](#manually-written-checks), but the infrastructure is in place if we want to generate some.

### How the checks are generated

The checks are generated by `scripts/generate_config_checks.py` in each project. Each project contains a description of what do check. A Python module in the framework contains code to transform this description into the C header files.

The generation happens before build time as part of `make generated_files` or similar.

### Use of historical configuration information

The generated checks are based at least in part on historical information about what configuration options and derived macros existed in previous versions of the library. This historical information is stored in the [`history`](https://github.com/Mbed-TLS/mbedtls-framework/tree/main/history) directory of the framework repository. It can be created with [`scripts/save_config_history.sh`](https://github.com/Mbed-TLS/mbedtls-framework/blob/main/scripts/save_config_history.sh).

## Validation

Each project contains a script `tests/scripts/test_generate_config_checks.py` which is invoked by `all.sh`.

### Unit tests for code generation

The config check tests include some basic unit tests around the code generation, validating that certain configurations are accepted and that others are rejected with an assertion on the `#error` message.

### Checks for forbidden configurations

The config check tests can validate that certain configurations are forbidden.

The config check tests work by attempting to compile <code><em>LIBRARY\_DIRECTORY</em>/<em>PROJECT\_NAME</em>_config.c</code> which includes both [manually written checks](#manually-written-checks) and [generated checks](#generated-checks). Both preprocessor `#error` and `static_assert` can be detected.
