# Mbed TLS invasive testing strategy

## Introduction

In Mbed TLS, we use black-box testing as much as possible: test the documented behavior of the product, in a realistic environment. However this is not always sufficient.

The goal of this document is to identify areas where black-box testing is insufficient and to propose solutions.

This is a test strategy document, not a test plan. A description of exactly what is tested is out of scope.

This document is structured as follows:

* [“Rules”](#rules) gives general rules and is written for brevity.
* [“Requirements”](#requirements) explores the reasons why invasive testing is needed and how it should be done.
* [“Possible approaches”](#possible-approaches) discusses some general methods for non-black-box testing.
* [“Solutions”](#solutions) explains how we currently solve, or intend to solve, specific problems.

### TLS

This document currently focuses on data structure manipulation and storage, which is what the crypto/keystore and X.509 parts of the library are about. More work is needed to fully take TLS into account.

## Rules

Always follow these rules unless you have a good reason not to. If you deviate, document the rationale somewhere.

See the section [“Possible approaches”](#possible-approaches) for a rationale.

### Interface design for testing

Do not add test-specific interfaces if there's a practical way of doing it another way. All public interfaces should be useful in at least some configurations. Features with a significant impact on the code size or attack surface should have a compile-time guard.

### Reliance on internal details

In unit tests and in test programs, it's ok to include internal header files from `library/`. Do not define non-public interfaces in public headers. In contrast, sample programs must not include header files from `library/`.

Sometimes it makes sense to have unit tests on functions that aren't part of the public API. Declare such functions in `library/*.h` and include the corresponding header in the test code. If the function should be `static` for optimization but can't be `static` for testing, declare it as `MBEDTLS_STATIC_TESTABLE`, and make the tests that use it depend on `MBEDTLS_TEST_HOOKS` (see [“rules for compile-time options”](#rules-for-compile-time-options)).

If test code or test data depends on internal details of the library and not just on its documented behavior, add a comment in the code that explains the dependency. For example:

> ```
> /* This test file is specific to the ITS implementation in PSA Crypto
>  * on top of stdio. It expects to know what the stdio name of a file is
>  * based on its keystore name.
>  */
> ```

> ```
> # This test assumes that PSA_MAX_KEY_BITS (currently 65536-8 bits = 8191 bytes
> # and not expected to be raised any time soon) is less than the maximum
> # output from HKDF-SHA512 (255*64 = 16320 bytes).
> ```

### Rules for compile-time options

If the most practical way to test something is to add code to the product that is only useful for testing, do so, but obey the following rules. For more information, see the [rationale](#guidelines-for-compile-time-options).

* **Only use test-specific code when necessary.** Anything that can be tested through the documented API must be tested through the documented API.
* **Test-specific code must be guarded by `#if defined(MBEDTLS_TEST_HOOKS)`**. Do not create fine-grained guards for test-specific code.
* **Do not use `MBEDTLS_TEST_HOOKS` for security checks or assertions.** Security checks belong in the product.
* **Merely defining `MBEDTLS_TEST_HOOKS` must not change the behavior**. It may define extra functions. It may add fields to structures, but if so, make it very clear that these fields have no impact on non-test-specific fields.
* **Where tests must be able to change the behavior, do it by function substitution.** See [“rules for function substitution”](#rules-for-function-substitution) for more details.

#### Rules for function substitution

This section explains how to replace a library function `mbedtls_foo()` by alternative code for test purposes. That is, library code calls `mbedtls_foo()`, and there is a mechanism to arrange for these calls to invoke different code.

Often `mbedtls_foo` is a macro which is defined to be a system function (like `mbedtls_calloc` or `mbedtls_fopen`), which we replace to mock or wrap the system function. This is useful to simulate I/O failure, for example. Note that if the macro can be replaced at compile time to support alternative platforms, the test code should be compatible with this compile-time configuration so that it works on these alternative platforms as well.

Sometimes the substitutable function is a `static inline` function that does nothing (not a macro, to avoid accidentally skipping side effects in its parameters), to provide a hook for test code; such functions should have a name that starts with the prefix `mbedtls_test_hook_`. In such cases, the function should generally not modify its parameters, so any pointer argument should be const. The function should return void.

With `MBEDTLS_TEST_HOOKS` set, `mbedtls_foo` is a global variable of function pointer type. This global variable is initialized to the system function, or to a function that does nothing. The global variable is defined in a header in the `library` directory such as `psa_crypto_invasive.h`. This is similar to the platform function configuration mechanism with `MBEDTLS_PLATFORM_xxx_ALT`.

In unit test code that needs to modify the internal behavior:

* The test function (or the whole test file) must depend on `MBEDTLS_TEST_HOOKS`.
* At the beginning of the test function, set the global function pointers to the desired value.
* In the test function's cleanup code, restore the global function pointers to their default value.

## Requirements

### General goals

We need to balance the following goals, which are sometimes contradictory.

* Coverage: we need to test behaviors which are not easy to trigger by using the API or which cannot be triggered deterministically, for example I/O failures.
* Correctness: we want to test the actual product, not a modified version, since conclusions drawn from a test of a modified product may not apply to the real product.
* Effacement: the product should not include features that are solely present for test purposes, since these increase the attack surface and the code size.
* Portability: tests should work on every platform. Skipping tests on certain platforms may hide errors that are only apparent on such platforms.
* Maintainability: tests should only enforce the documented behavior of the product, to avoid extra work when the product's internal or implementation-specific behavior changes. We should also not give the impression that whatever the tests check is guaranteed behavior of the product which cannot change in future versions.

Where those goals conflict, we should at least mitigate the goals that cannot be fulfilled, and document the architectural choices and their rationale.

### Problem areas

#### Allocation

Resource allocation can fail, but rarely does so in a typical test environment. How does the product cope if some allocations fail?

Resources include:

* Memory.
* Files in storage (PSA API only — in the Mbed TLS API, black-box unit tests are sufficient).
* Key slots (PSA API only).
* Key slots in a secure element (PSA SE HAL).
* Communication handles (PSA crypto service only).

#### Storage

Storage can fail, either due to hardware errors or to active attacks on trusted storage. How does the code cope if some storage accesses fail?

We also need to test resilience: if the system is reset during an operation, does it restart in a correct state?

#### Cleanup

When code should clean up resources, how do we know that they have truly been cleaned up?

* Zeroization of confidential data after use.
* Freeing memory.
* Freeing key slots.
* Freeing key slots in a secure element.
* Deleting files in storage (PSA API only).

#### Internal data

Sometimes it is useful to peek or poke internal data.

* Check consistency of internal data (e.g. output of key generation).
* Check the format of files (which matters so that the product can still read old files after an upgrade).
* Inject faults and test corruption checks inside the product.

## Possible approaches

Key to requirement tables:

* ++ requirement is fully met
* \+ requirement is mostly met
* ~ requirement is partially met but there are limitations
* ! requirement is somewhat problematic
* !! requirement is very problematic

### Fine-grained public interfaces

We can include all the features we want to test in the public interface. Then the tests can be truly black-box. The limitation of this approach is that this requires adding a lot of interfaces that are not useful in production. These interfaces have costs: they increase the code size, the attack surface, and the testing burden (exponentially, because we need to test all these interfaces in combination).

As a rule, we do not add public interfaces solely for testing purposes. We only add public interfaces if they are also useful in production, at least sometimes. For example, the main purpose of `mbedtls_psa_crypto_free` is to clean up all resources in tests, but this is also useful in production in some applications that only want to use PSA Crypto during part of their lifetime.

Mbed TLS traditionally has very fine-grained public interfaces, with many platform functions that can be substituted (`MBEDTLS_PLATFORM_xxx` macros). PSA Crypto has more opacity and less platform substitution macros.

| Requirement | Analysis |
| ----------- | -------- |
| Coverage | ~ Many useful tests are not reasonably achievable |
| Correctness | ++ Ideal |
| Effacement | !! Requires adding many otherwise-useless interfaces |
| Portability | ++ Ideal; the additional interfaces may be useful for portability beyond testing |
| Maintainability | !! Combinatorial explosion on the testing burden |
|                 | ! Public interfaces must remain for backward compatibility even if the test architecture changes |

### Fine-grained undocumented interfaces

We can include all the features we want to test in undocumented interfaces. Undocumented interfaces are described in public headers for the sake of the C compiler, but are described as “do not use” in comments (or not described at all) and are not included in Doxygen-rendered documentation. This mitigates some of the downsides of [fine-grained public interfaces](#fine-grained-public-interfaces), but not all. In particular, the extra interfaces do increase the code size, the attack surface and the test surface.

Mbed TLS traditionally has a few internal interfaces, mostly intended for cross-module abstraction leakage rather than for testing. For the PSA API, we favor [internal interfaces](#internal-interfaces).

| Requirement | Analysis |
| ----------- | -------- |
| Coverage | ~ Many useful tests are not reasonably achievable |
| Correctness | ++ Ideal |
| Effacement | !! Requires adding many otherwise-useless interfaces |
| Portability | ++ Ideal; the additional interfaces may be useful for portability beyond testing |
| Maintainability | ! Combinatorial explosion on the testing burden |

### Internal interfaces

We can write tests that call internal functions that are not exposed in the public interfaces. This is nice when it works, because it lets us test the unchanged product without compromising the design of the public interface.

A limitation is that these interfaces must exist in the first place. If they don't, this has mostly the same downside as public interfaces: the extra interfaces increase the code size and the attack surface for no direct benefit to the product.

Another limitation is that internal interfaces need to be used correctly. We may accidentally rely on internal details in the tests that are not necessarily always true (for example that are platform-specific). We may accidentally use these internal interfaces in ways that don't correspond to the actual product.

This approach is mostly portable since it only relies on C interfaces. A limitation is that the test-only interfaces must not be hidden at link time (but link-time hiding is not something we currently do). Another limitation is that this approach does not work for users who patch the library by replacing some modules; this is a secondary concern since we do not officially offer this as a feature.

| Requirement | Analysis |
| ----------- | -------- |
| Coverage | ~ Many useful tests require additional internal interfaces |
| Correctness | + Does not require a product change |
|             | ~ The tests may call internal functions in a way that does not reflect actual usage inside the product |
| Effacement | ++ Fine as long as the internal interfaces aren't added solely for test purposes |
| Portability | + Fine as long as we control how the tests are linked |
|             | ~ Doesn't work if the users rewrite an internal module |
| Maintainability | + Tests interfaces that are documented; dependencies in the tests are easily noticed when changing these interfaces |

### Static analysis

If we guarantee certain properties through static analysis, we don't need to test them. This puts some constraints on the properties:

* We need to have confidence in the specification (but we can gain this confidence by evaluating the specification on test data).
* This does not work for platform-dependent properties unless we have a formal model of the platform.

| Requirement | Analysis |
| ----------- | -------- |
| Coverage | ~ Good for platform-independent properties, if we can guarantee them statically |
| Correctness | + Good as long as we have confidence in the specification |
| Effacement | ++ Zero impact on the code |
| Portability | ++ Zero runtime burden |
| Maintainability | ~ Static analysis is hard, but it's also helpful |

### Compile-time options

If there's code that we want to have in the product for testing, but not in production, we can add a compile-time option to enable it. This is very powerful and usually easy to use, but comes with a major downside: we aren't testing the same code anymore.

| Requirement | Analysis |
| ----------- | -------- |
| Coverage | ++ Most things can be tested that way |
| Correctness | ! Difficult to ensure that what we test is what we run |
| Effacement | ++ No impact on the product when built normally or on the documentation, if done right |
|             | ! Risk of getting “no impact” wrong |
| Portability | ++ It's just C code so it works everywhere |
|             | ~ Doesn't work if the users rewrite an internal module |
| Maintainability | + Test interfaces impact the product source code, but at least they're clearly marked as such in the code |

#### Guidelines for compile-time options

* **Minimize the number of compile-time options.**<br>
  Either we're testing or we're not. Fine-grained options for testing would require more test builds, especially if combinatorics enters the play.
* **Merely enabling the compile-time option should not change the behavior.**<br>
  When building in test mode, the code should have exactly the same behavior. Changing the behavior should require some action at runtime (calling a function or changing a variable).
* **Minimize the impact on code**.<br>
  We should not have test-specific conditional compilation littered through the code, as that makes the code hard to read.

### Runtime instrumentation

Some properties can be tested through runtime instrumentation: have the compiler or a similar tool inject something into the binary.

* Sanitizers check for certain bad usage patterns (ASan, MSan, UBSan, Valgrind).
* We can inject external libraries at link time. This can be a way to make system functions fail.

| Requirement | Analysis |
| ----------- | -------- |
| Coverage | ! Limited scope |
| Correctness | + Instrumentation generally does not affect the program's functional behavior |
| Effacement | ++ Zero impact on the code |
| Portability | ~ Depends on the method |
| Maintainability | ~ Depending on the instrumentation, this may require additional builds and scripts |
|                 | + Many properties come for free, but some require effort (e.g. the test code itself must be leak-free to avoid false positives in a leak detector) |

### Debugger-based testing

If we want to do something in a test that the product isn't capable of doing, we can use a debugger to read or modify the memory, or hook into the code at arbitrary points.

This is a very powerful approach, but it comes with limitations:

* The debugger may introduce behavior changes (e.g. timing). If we modify data structures in memory, we may do so in a way that the code doesn't expect.
* Due to compiler optimizations, the memory may not have the layout that we expect.
* Writing reliable debugger scripts is hard. We need to have confidence that we're testing what we mean to test, even in the face of compiler optimizations. Languages such as gdb make it hard to automate even relatively simple things such as finding the place(s) in the binary corresponding to some place in the source code.
* Debugger scripts are very much non-portable.

| Requirement | Analysis |
| ----------- | -------- |
| Coverage | ++ The sky is the limit |
| Correctness | ++ The code is unmodified, and tested as compiled (so we even detect compiler-induced bugs) |
|             | ! Compiler optimizations may hinder |
|             | ~ Modifying the execution may introduce divergence |
| Effacement | ++ Zero impact on the code |
| Portability | !! Not all environments have a debugger, and even if they do, we'd need completely different scripts for every debugger |
| Maintainability | ! Writing reliable debugger scripts is hard |
|                 | !! Very tight coupling with the details of the source code and even with the compiler |

## Solutions

This section lists some strategies that are currently used for invasive testing, or planned to be used. This list is not intended to be exhaustive.

### Memory management

#### Zeroization testing

Goal: test that `mbedtls_platform_zeroize` does wipe the memory buffer.

Solution ([debugger](#debugger-based-testing)): implemented in `framework/tests/programs/test_zeroize.gdb`.

Rationale: this cannot be tested by adding C code, because the danger is that the compiler optimizes the zeroization away, and any C code that observes the zeroization would cause the compiler not to optimize it away.

#### Memory cleanup

Goal: test the absence of memory leaks.

Solution ([instrumentation](#runtime-instrumentation)): run tests with ASan. (We also use Valgrind, but it's slower than ASan, so we favor ASan.)

Since we run many test jobs with a memory leak detector, each test function or test program must clean up after itself. Use the cleanup code (after the `exit` label in test functions) to free any memory that the function may have allocated.

#### Robustness against memory allocation failure

Solution: TODO. We don't test this at all at this point.

#### PSA key store memory cleanup

Goal: test the absence of resource leaks in the PSA key store code, in particular that `psa_close_key` and `psa_destroy_key` work correctly.

Solution ([internal interface](#internal-interfaces)): in most tests involving PSA functions, the cleanup code explicitly calls `PSA_DONE()` instead of `mbedtls_psa_crypto_free()`. `PSA_DONE` fails the test if the key store in memory is not empty.

Note there must also be tests that call `mbedtls_psa_crypto_free` with keys still open, to verify that it does close all keys.

`PSA_DONE` is a macro defined in `psa_crypto_helpers.h` which uses `mbedtls_psa_get_stats()` to get information about the keystore content before calling `mbedtls_psa_crypto_free()`. This feature is mostly but not exclusively useful for testing, and may be moved under `MBEDTLS_TEST_HOOKS`.

### PSA storage

#### PSA storage cleanup on success

Goal: test that no stray files are left over in the key store after a test that succeeded.

Solution: TODO. Currently the various test suites do it differently.

#### PSA storage cleanup on failure

Goal: ensure that no stray files are left over in the key store even if a test has failed (as that could cause other tests to fail).

Solution: TODO. Currently the various test suites do it differently.

#### PSA storage resilience

Goal: test the resilience of PSA storage against power failures.

Solution: TODO.

See the [secure element driver interface test strategy](driver-interface-test-strategy.html) for more information.

#### Corrupted storage

Goal: test the robustness against corrupted storage.

Solution ([internal interface](#internal-interfaces)): call `psa_its` functions to modify the storage.

#### Storage read failure

Goal: test the robustness against read errors.

Solution: TODO

#### Storage write failure

Goal: test the robustness against write errors (`STORAGE_FAILURE` or `INSUFFICIENT_STORAGE`).

Solution: TODO

#### Storage format stability

Goal: test that the storage format does not change between versions (or if it does, an upgrade path must be provided).

Solution ([internal interface](#internal-interfaces)): call internal functions to inspect the content of the file.

Note that the storage format is defined not only by the general layout, but also by the numerical values of encodings for key types and other metadata. For numerical values, there is a risk that we would accidentally modify a single value or a few values, so the tests should be exhaustive. This probably requires some compile-time analysis (perhaps the automation for `psa_constant_names` can be used here). TODO

### Other fault injection

#### PSA crypto init failure

Goal: test the failure of `psa_crypto_init`.

Solution ([compile-time option](#compile-time-options)): replace entropy initialization functions by functions that can fail. This is the only failure point for `psa_crypto_init` that is present in all builds.

When we implement the PSA entropy driver interface, this should be reworked to use the entropy driver interface.

#### PSA crypto data corruption

The PSA crypto subsystem has a few checks to detect corrupted data in memory. We currently don't have a way to exercise those checks.

Solution: TODO. To corrupt a multipart operation structure, we can do it by looking inside the structure content, but only when running without isolation. To corrupt the key store, we would need to add a function to the library or to use a debugger.

