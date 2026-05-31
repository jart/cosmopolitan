PSA API functions and shared memory
===================================

## Introduction

This document discusses the security architecture of systems where PSA API functions might receive arguments that are in memory that is shared with an untrusted process. On such systems, the untrusted process might access a shared memory buffer while the cryptography library is using it, and thus cause unexpected behavior in the cryptography code.

### Core assumptions

We assume the following scope limitations:

* Only PSA Crypto API functions are in scope (including Mbed TLS extensions to the official API specification). Legacy crypto, X.509, TLS, or any other function which is not called `psa_xxx` is out of scope.
* We only consider [input buffers](https://arm-software.github.io/psa-api/crypto/1.1/overview/conventions.html#input-buffer-sizes) and [output buffers](https://arm-software.github.io/psa-api/crypto/1.1/overview/conventions.html#output-buffer-sizes). Any other data is assumed to be in non-shared memory.

## System architecture discussion

### Architecture overview

We consider a system that has memory separation between partitions: a partition can't access another partition's memory directly. Partitions are meant to be isolated from each other: a partition may only affect the integrity of another partition via well-defined system interfaces. For example, this can be a Unix/POSIX-like system that isolates processes, or isolation between the secure world and the non-secure world relying on a mechanism such as TrustZone, or isolation between secure-world applications on such a system.

More precisely, we consider such a system where our PSA Crypto implementation is running inside one partition, called the **crypto service**. The crypto service receives remote procedure calls (RPC) from other partitions, validates their arguments (e.g. validation of key identifier ownership), and calls a PSA Crypto API function. This document is concerned with environments where the arguments passed to a PSA Crypto API function may be in shared memory (as opposed to environments where the inputs are always copied into memory that is solely accessible by the crypto service before calling the API function, and likewise with output buffers after the function returns).

When the data is accessible to another partition, there is a risk that this other partition will access it while the crypto implementation is working. Although this could be prevented by suspending the whole system while crypto is working, such a limitation is rarely desirable and most systems don't offer a way to do it. (Even systems that have absolute thread priorities, and where crypto has a higher priority than any untrusted partition, may be vulnerable due to having multiple cores or asynchronous data transfers with peripherals.)

The crypto service must guarantee that it behaves as if the rest of the world was suspended while it is executed. A behavior that is only possible if an untrusted entity accesses a buffer while the crypto service is processing the data is a security violation.

### Risks and vulnerabilities

We consider a security architecture with two or three entities:

* a crypto service, which offers PSA crypto API calls over RPC (remote procedure call) using shared memory for some input or output arguments;
* a client of the crypto service, which makes a RPC to the crypto service;
* in some scenarios, a client of the client, which makes a RPC to the crypto client which re-shares the memory with the crypto service.

The behavior of RPC is defined for in terms of values of inputs and outputs. This models an ideal world where the content of input and output buffers is not accessible outside the crypto service while it is processing an RPC. It is a security violation if the crypto service behaves in a way that cannot be achieved by setting the inputs before the RPC call, and reading the outputs after the RPC call is finished.

#### Read-read inconsistency

If an input argument is in shared memory, there is a risk of a **read-read inconsistency**:

1. The crypto code reads part of the input and validates it, or injects it into a calculation.
2. The client (or client's client) modifies the input.
3. The crypto code reads the same part again, and performs an action which would be impossible if the input had had the same value all along.

Vulnerability example (parsing): suppose the input contains data with a type-length-value or length-value encoding (for example, importing an RSA key). The crypto code reads the length field and checks that it fits within the buffer. (This could be the length of the overall data, or the length of an embedded field) Later, the crypto code reads the length again and uses it without validation. A malicious client can modify the length field in the shared memory between the two reads and thus cause a buffer overread on the second read.

Vulnerability example (dual processing): consider an RPC to perform authenticated encryption, using a mechanism with an encrypt-and-MAC structure. The authenticated encryption implementation separately calculates the ciphertext and the MAC from the plaintext. A client sets the plaintext input to `"PPPP"`, then starts the RPC call, then changes the input buffer to `"QQQQ"` while the crypto service is working.

* Any of `enc("PPPP")+mac("PPPP")`, `enc("PPQQ")+mac("PPQQ")` or `enc("QQQQ")+mac("QQQQ")` are valid outputs: they are outputs that can be produced by this authenticated encryption RPC.
* If the authenticated encryption calculates the ciphertext before the client changes the output buffer and calculates the MAC after that change, reading the input buffer again each time, the output will be `enc("PPPP")+mac("QQQQ")`. There is no input that can lead to this output, hence this behavior violates the security guarantees of the crypto service.

#### Write-read inconsistency

If an output argument is in shared memory, there is a risk of a **write-read inconsistency**:

1. The crypto code writes some intermediate data into the output buffer.
2. The client (or client's client) modifies the intermediate data.
3. The crypto code reads the intermediate data back and continues the calculation, leading to an outcome that would not be possible if the intermediate data had not been modified.

Vulnerability example: suppose that an RSA signature function works by formatting the data in place in the output buffer, then applying the RSA private-key operation in place. (This is how `mbedtls_rsa_pkcs1_sign` works.) A malicious client may write badly formatted data into the buffer, so that the private-key operation is not a valid signature (e.g. it could be a decryption), violating the RSA key's usage policy.

Vulnerability example with chained calls: we consider the same RSA signature operation as before. In this example, we additionally assume that the data to sign comes from an attestation application which signs some data on behalf of a final client: the key and the data to sign are under the attestation application's control, and the final client must not be able to obtain arbitrary signatures. The final client shares an output buffer for the signature with the attestation application, and the attestation application re-shares this buffer with the crypto service. A malicious final client can modify the intermediate data and thus sign arbitrary data.

#### Write-write disclosure

If an output argument is in shared memory, there is a risk of a **write-write disclosure**:

1. The crypto code writes some intermediate data into the output buffer. This intermediate data must remain confidential.
2. The client (or client's client) reads the intermediate data.
3. The crypto code overwrites the intermediate data.

Vulnerability example with chained calls (temporary exposure): an application encrypts some data, and lets its clients store the ciphertext. Clients may not have access to the plaintext. To save memory, when it calls the crypto service, it passes an output buffer that is in the final client's memory. Suppose the encryption mechanism works by copying its input to the output buffer then encrypting in place (for example, to simplify considerations related to overlap, or because the implementation relies on a low-level API that works in place). In this scenario, the plaintext is exposed to the final client while the encryption in progress, which violates the confidentiality of the plaintext.

Vulnerability example with chained calls (backtrack): we consider a provisioning application that provides a data encryption service on behalf of multiple clients, using a single shared key. Clients are not allowed to access each other's data. The provisioning application isolates clients by including the client identity in the associated data. Suppose that an AEAD decryption function processes the ciphertext incrementally by simultaneously writing the plaintext to the output buffer and calculating the tag. (This is how AEAD decryption usually works.) At the end, if the tag is wrong, the decryption function wipes the output buffer. Assume that the output buffer for the plaintext is shared from the client to the provisioning application, which re-shares it with the crypto service. A malicious client can read another client (the victim)'s encrypted data by passing the ciphertext to the provisioning application, which will attempt to decrypt it with associated data identifying the requesting client. Although the operation will fail beacuse the tag is wrong, the malicious client still reads the victim plaintext.

#### Write-read feedback

If a function both has an input argument and an output argument in shared memory, and processes its input incrementally to emit output incrementally, the following sequence of events is possible:

1. The crypto code processes part of the input and writes the corresponding part of the output.
2. The client reads the early output and uses that to calculate the next part of the input.
3. The crypto code processes the rest of the input.

There are cryptographic mechanisms for which this breaks security properties. An example is [CBC encryption](https://link.springer.com/content/pdf/10.1007/3-540-45708-9_2.pdf): if the client can choose the content of a plaintext block after seeing the immediately preceding ciphertext block, this gives the client a decryption oracle. This is a security violation if the key policy only allowed the client to encrypt, not to decrypt.

TODO: is this a risk we want to take into account? Although this extends the possible behaviors of the one-shot interface, the client can do the same thing legitimately with the multipart interface.

### Possible countermeasures

In this section, we briefly discuss generic countermeasures.

#### Copying

Copying is a valid countermeasure. It is conceptually simple. However, it is often unattractive because it requires additional memory and time.

Note that although copying is very easy to write into a program, there is a risk that a compiler (especially with whole-program optimization) may optimize the copy away, if it does not understand that copies between shared memory and non-shared memory are semantically meaningful.

Example: the PSA Firmware Framework 1.0 forbids shared memory between partitions. This restriction is lifted in version 1.1 due to concerns over RAM usage.

#### Careful accesses

The following rules guarantee that shared memory cannot result in a security violation other than [write-read feedback](#write-read-feedback):

* Never read the same input twice at the same index.
* Never read back from an output.
* Never write to the output twice at the same index.
    * This rule can usefully be relaxed in many circumstances. It is ok to write data that is independent of the inputs (and not otherwise confidential), then overwrite it. For example, it is ok to zero the output buffer before starting to process the input.

These rules are very difficult to enforce.

Example: these are the rules that a GlobalPlatform TEE Trusted Application (application running on the secure side of TrustZone on Cortex-A) must follow.

## Protection requirements

### Responsibility for protection

A call to a crypto service to perform a crypto operation involves the following components:

1. The remote procedure call framework provided by the operating system.
2. The code of the crypto service.
3. The code of the PSA Crypto dispatch layer (also known as the core), which is provided by Mbed TLS.
4. The driver implementing the cryptographic mechanism, which may be provided by Mbed TLS (built-in driver) or by a third-party driver.

The [PSA Crypto API specification](https://arm-software.github.io/psa-api/crypto/1.1/overview/conventions.html#stability-of-parameters) puts the responsibility for protection on the implementation of the PSA Crypto API, i.e. (3) or (4).

> In an environment with multiple threads or with shared memory, the implementation carefully accesses non-overlapping buffer parameters in order to prevent any security risk resulting from the content of the buffer being modified or observed during the execution of the function. (...)

In Mbed TLS 2.x and 3.x up to and including 3.5.0, there is no defense against buffers in shared memory. The responsibility shifts to (1) or (2), but this is not documented.

In the remainder of this chapter, we will discuss how to implement this high-level requirement where it belongs: inside the implementation of the PSA Crypto API. Note that this allows two possible levels: in the dispatch layer (independently of the implementation of each mechanism) or in the driver (specific to each implementation).

#### Protection in the dispatch layer

The dispatch layer has no control over how the driver layer will access buffers. Therefore the only possible protection at this layer method is to ensure that drivers have no access to shared memory. This means that any buffer located in shared memory must be copied into or out of a buffer in memory owned by the crypto service (heap or stack). This adds inefficiency, mostly in terms of RAM usage.

For buffers with a small static size limit, this is something we often do for convenience, especially with output buffers. However, as of Mbed TLS 3.5.0, it is not done systematically.

It is ok to skip the copy if it is known for sure that a buffer is not in shared memory. However, the location of the buffer is not under the control of Mbed TLS. This means skipping the copy would have to be a compile-time or run-time option which has to be set by the application using Mbed TLS. This is both an additional maintenance cost (more code to analyze, more testing burden), and a residual security risk in case the party who is responsible for setting this option does not set it correctly. As a consequence, Mbed TLS will not offer this configurability unless there is a compelling argument.

#### Protection in the driver layer

Putting the responsibility for protection in the driver layer increases the overall amount of work since there are more driver implementations than dispatch implementations. (This is true even inside Mbed TLS: almost all API functions have multiple underlying implementations, one for each algorithm.) It also increases the risk to the ecosystem since some drivers might not protect correctly. Therefore having drivers be responsible for protection is only a good choice if there is a definite benefit to it, compared to allocating an internal buffer and copying. An expected benefit in some cases is that there are practical protection methods other than copying.

Some cryptographic mechanisms are naturally implemented by processing the input in a single pass, with a low risk of ever reading the same byte twice, and by writing the final output directly into the output buffer. For such mechanism, it is sensible to mandate that drivers respect these rules.

In the next section, we will analyze how susceptible various cryptographic mechanisms are to shared memory vulnerabilities.

### Susceptibility of different mechanisms

#### Operations involving small buffers

For operations involving **small buffers**, the cost of copying is low. For many of those, the risk of not copying is high:

* Any parsing of formatted data has a high risk of [read-read inconsistency](#read-read-inconsistency).
* An internal review shows that for RSA operations, it is natural for an implementation to have a [write-read inconsistency](#write-read-inconsistency) or a [write-write disclosure](#write-write-disclosure).

Note that in this context, a “small buffer” is one with a size limit that is known at compile time, and small enough that copying the data is not prohibitive. For example, an RSA key fits in a small buffer. A hash input is not a small buffer, even if it happens to be only a few bytes long in one particular call.

The following buffers are considered small buffers:

* Any input or output directly related to asymmetric cryptography (signature, encryption/decryption, key exchange, PAKE), including key import and export.
    * Note that this does not include inputs or outputs that are not processed by an asymmetric primitives, for example the message input to `psa_sign_message` or `psa_verify_message`.
* Cooked key derivation output.
* The output of a hash or MAC operation.

**Design decision: the dispatch layer shall copy all small buffers**.

#### Symmetric cryptography inputs with small output

Message inputs to hash, MAC and key derivation operations are at a low risk of [read-read inconsistency](#read-read-inconsistency) because they are unformatted data, and for all specified algorithms, it is natural to process the input one byte at a time.

**Design decision: require symmetric cryptography drivers to read their input without a risk of read-read inconsistency**.

TODO: what about IV/nonce inputs? They are typically small, but don't necessarily have a static size limit (e.g. GCM recommends a 12-byte nonce, but also allows large nonces).

#### Key derivation outputs

Key derivation typically emits its output as a stream, with no error condition detected after setup other than operational failures (e.g. communication failure with an accelerator) or running out of data to emit (which can easily be checked before emitting any data, since the data size is known in advance).

(Note that this is about raw byte output, not about cooked key derivation, i.e. deriving a structured key, which is considered a [small buffer](#operations-involving-small-buffers).)

**Design decision: require key derivation drivers to emit their output without reading back from the output buffer**.

#### Cipher and AEAD

AEAD decryption is at risk of [write-write disclosure](#write-write-disclosure) when the tag does not match.

AEAD encryption and decryption are at risk of [read-read inconsistency](#read-read-inconsistency) if they process the input multiple times, which is natural in a number of cases:

* when encrypting with an encrypt-and-authenticate or authenticate-then-encrypt structure (one read to calculate the authentication tag and another read to encrypt);
* when decrypting with an encrypt-then-authenticate structure (one read to decrypt and one read to calculate the authentication tag);
* with SIV modes (not yet present in the PSA API, but likely to come one day) (one full pass to calculate the IV, then another full pass for the core authenticated encryption);

Cipher and AEAD outputs are at risk of [write-read inconsistency](#write-read-inconsistency) and [write-write disclosure](#write-write-disclosure) if they are implemented by copying the input into the output buffer with `memmove`, then processing the data in place. In particular, this approach makes it easy to fully support overlapping, since `memmove` will take care of overlapping cases correctly, which is otherwise hard to do portably (C99 does not offer an efficient, portable way to check whether two buffers overlap).

**Design decision: the dispatch layer shall allocate an intermediate buffer for cipher and AEAD plaintext/ciphertext inputs and outputs**.

Note that this can be a single buffer for the input and the output if the driver supports in-place operation (which it is supposed to, since it is supposed to support arbitrary overlap, although this is not always the case in Mbed TLS, a [known issue](https://github.com/Mbed-TLS/mbedtls/issues/3266)). A side benefit of doing this intermediate copy is that overlap will be supported.

For all currently implemented AEAD modes, the associated data is only processed once to calculate an intermediate value of the authentication tag.

**Design decision: for now, require AEAD drivers to read the additional data without a risk of read-read inconsistency**. Make a note to revisit this when we start supporting an SIV mode, at which point the dispatch layer shall copy the input for modes that are not known to be low-risk.

#### Message signature

For signature algorithms with a hash-and-sign framework, the input to sign/verify-message is passed to a hash, and thus can follow the same rules as [symmetric cryptography inputs with small output](#symmetric-cryptography-inputs-with-small-output). This is also true for `PSA_ALG_RSA_PKCS1V15_SIGN_RAW`, which is the only non-hash-and-sign signature mechanism implemented in Mbed TLS 3.5. This is not true for PureEdDSA (`#PSA_ALG_PURE_EDDSA`), which is not yet implemented: [PureEdDSA signature](https://www.rfc-editor.org/rfc/rfc8032#section-5.1.6) processes the message twice. (However, PureEdDSA verification only processes the message once.)

**Design decision: for now, require sign/verify-message drivers to read their input without a risk of read-read inconsistency**. Make a note to revisit this when we start supporting PureEdDSA, at which point the dispatch layer shall copy the input for algorithms such as PureEdDSA that are not known to be low-risk.

## Design of shared memory protection

This section explains how Mbed TLS implements the shared memory protection strategy summarized below.

### Shared memory protection strategy

* The core (dispatch layer) shall make a copy of the following buffers, so that drivers do not receive arguments that are in shared memory:
    * Any input or output from asymmetric cryptography (signature, encryption/decryption, key exchange, PAKE), including key import and export.
    * Plaintext/ciphertext inputs and outputs for cipher and AEAD.
    * The output of a hash or MAC operation.
    * Cooked key derivation output.

* A document shall explain the requirements on drivers for arguments whose access needs to be protected:
    * Hash and MAC input.
    * Cipher/AEAD IV/nonce (to be confirmed).
    * AEAD associated data (to be confirmed).
    * Key derivation input (excluding key agreement).
    * Raw key derivation output (excluding cooked key derivation output).

* The built-in implementations of cryptographic mechanisms with arguments whose access needs to be protected shall protect those arguments.

Justification: see “[Susceptibility of different mechanisms](#susceptibility-of-different-mechanisms)”.

### Implementation of copying

Copy what needs copying. This is broadly straightforward, however there are a few things to consider.

#### Compiler optimization of copies

It is unclear whether the compiler will attempt to optimize away copying operations.

Once the copying code is implemented, it should be evaluated to see whether compiler optimization is a problem. Specifically, for the major compilers supported by Mbed TLS:
* Write a small program that uses a PSA function which copies inputs or outputs.
* Build the program with link-time optimization / full-program optimization enabled (e.g. `-flto` with `gcc`). Try also enabling the most extreme optimization options such as `-Ofast` (`gcc`) and `-Oz` (`clang`).
* Inspect the generated code with `objdump` or a similar tool to see if copying operations are preserved.

If copying behaviour is preserved by all major compilers then assume that compiler optimization is not a problem.

If copying behaviour is optimized away by the compiler, further investigation is needed. Experiment with using the `volatile` keyword to force the compiler not to optimize accesses to the copied buffers. If the `volatile` keyword is not sufficient, we may be able to use compiler or target-specific techniques to prevent optimization, for example memory barriers or empty `asm` blocks. These may be implemented and verified for important platforms while retaining a C implementation that is likely to be correct on most platforms as a fallback - the same approach taken by the constant-time module.

**Open questions: Will the compiler optimize away copies? If so, can it be prevented from doing so in a portable way?**

#### Copying code

We may either copy buffers on an ad-hoc basis using `memcpy()` in each PSA function, or use a unified set of functions for copying input and output data. The advantages of the latter are obvious:

* Any test hooks need only be added in one place.
* Copying code must only be reviewed for correctness in one place, rather than in all functions where it occurs.
* Copy bypass is simpler as we can just replace these functions with no-ops in a single place.
* Any complexity needed to prevent the compiler optimizing copies away does not have to be duplicated.

On the other hand, the only advantage of ad-hoc copying is slightly greater flexibility.

**Design decision: Create a unified set of functions for copying input and output data.**

#### Copying in multipart APIs

Multipart APIs may follow one of 2 possible approaches for copying of input:

##### 1. Allocate a buffer and copy input on each call to `update()`

This is simple and mirrors the approach for one-shot APIs nicely. However, allocating memory in the middle of a multi-part operation is likely to be bad for performance. Multipart APIs are designed in part for systems that do not have time to perform an operation at once, so introducing poor performance may be a problem here.

**Open question: Does memory allocation in `update()` cause a performance problem? If so, to what extent?**

##### 2. Allocate a buffer at the start of the operation and subdivide calls to `update()`

In this approach, input and output buffers are allocated at the start of the operation that are large enough to hold the expected average call to `update()`. When `update()` is called with larger buffers than these, the PSA API layer makes multiple calls to the driver, chopping the input into chunks of the temporary buffer size and filling the output from the results until the operation is finished.

This would be more complicated than approach (1) and introduces some extra issues. For example, if one of the intermediate calls to the driver's `update()` returns an error, it is not possible for the driver's state to be rolled back to before the first call to `update()`. It is unclear how this could be solved.

However, this approach would reduce memory usage in some cases and prevent memory allocation during an operation. Additionally, since the input and output buffers would be fixed-size it would be possible to allocate them statically, avoiding the need for any dynamic memory allocation at all.

**Design decision: Initially use approach (1) and treat approach (2) as an optimization to be done if necessary.**

### Validation of copying

#### Validation of copying by review

This is fairly self-explanatory. Review all functions that use shared memory and ensure that they each copy memory. This is the simplest strategy to implement but is less reliable than automated validation.

#### Validation of copying with memory pools

Proposed general idea: have tests where the test code calling API functions allocates memory in a certain pool, and code in the library allocates memory in a different pool. Test drivers check that needs-copying arguments are within the library pool, not within the test pool.

#### Validation of copying by memory poisoning

Proposed general idea: in test code, “poison” the memory area used by input and output parameters that must be copied. Poisoning means something that prevents accessing memory while it is poisoned. This could be via memory protection (allocate with `mmap` then disable access with `mprotect`), or some kind of poisoning for an analyzer such as MSan or Valgrind.

In the library, the code that does the copying temporarily unpoisons the memory by calling a test hook.

```c
static void copy_to_user(void *copy_buffer, void *const input_buffer, size_t length) {
#if defined(MBEDTLS_TEST_HOOKS)
    if (memory_poison_hook != NULL) {
        memory_poison_hook(copy_buffer, length);
    }
#endif
    memcpy(copy_buffer, input_buffer, length);
#if defined(MBEDTLS_TEST_HOOKS)
    if (memory_unpoison_hook != NULL) {
        memory_unpoison_hook(copy_buffer, length);
    }
#endif
}
```
The reason to poison the memory before calling the library, rather than after the copy-in (and symmetrically for output buffers) is so that the test will fail if we forget to copy, or we copy the wrong thing. This would not be the case if we relied on the library's copy function to do the poisoning: that would only validate that the driver code does not access the memory on the condition that the copy is done as expected.

##### Options for implementing poisoning

There are several different ways that poisoning could be implemented:

1. Using Valgrind's memcheck tool. Valgrind provides a macro `VALGRIND_MAKE_MEM_NO_ACCESS` that allows manual memory poisoning. Valgrind memory poisoning is already used for constant-flow testing in Mbed TLS.
2. Using Memory Sanitizer (MSan), which allows us to mark memory as uninitialized. This is also used for constant-flow testing. It is suitable for input buffers only, since it allows us to detect when a poisoned buffer is read but not when it is written.
3. Using Address Sanitizer (ASan). This provides `ASAN_POISON_MEMORY_REGION` which marks memory as inaccessible.
4. Allocating buffers separate pages and calling `mprotect()` to set pages as inaccessible. This has the disadvantage that we will have to manually ensure that buffers sit in their own pages, which likely means making a copy.
5. Filling buffers with random data, keeping a copy of the original. For input buffers, keep a copy of the original and copy it back once the PSA function returns. For output buffers, fill them with random data and keep a separate copy of it. In the memory poisoning hooks, compare the copy of random data with the original to ensure that the output buffer has not been written directly.

Approach (2) is insufficient for the full testing we require as we need to be able to check both input and output buffers.

Approach (5) is simple and requires no extra tooling. It is likely to have good performance as it does not use any sanitizers. However, it requires the memory poisoning test hooks to maintain extra copies of the buffers, which seems difficult to implement in practice. Additionally, it does not precisely test the property we want to validate, so we are relying on the tests to fail if given random data as input. It is possible (if unlikely) that the PSA function will access the poisoned buffer without causing the test to fail. This becomes more likely when we consider test cases that call PSA functions on incorrect inputs to check that the correct error is returned. For these reasons, this memory poisoning approach seems unsuitable.

All three remaining approaches are suitable for our purposes. However, approach (4) is more complex than the other two. To implement it, we would need to allocate poisoned buffers in separate memory pages. They would require special handling and test code would likely have to be designed around this special handling.

Meanwhile, approaches (1) and (3) are much more convenient. We are simply required to call a special macro on some buffer that was allocated by us and the sanitizer takes care of everything else. Of these two, ASan appears to have a limitation related to buffer alignment. From code comments quoted in [the documentation](https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning):

> This function is not guaranteed to poison the whole region - it may poison only subregion of [addr, addr+size) due to ASan alignment restrictions.

Specifically, ASan will round the buffer size down to 8 bytes before poisoning due to details of its implementation. For more information on this, see [Microsoft documentation of this feature](https://learn.microsoft.com/en-us/cpp/sanitizers/asan-runtime?view=msvc-170#alignment-requirements-for-addresssanitizer-poisoning).

It should be possible to work around this by manually rounding buffer lengths up to the nearest multiple of 8 in the poisoning function, although it's remotely possible that this will cause other problems. Valgrind does not appear to have this limitation (unless Valgrind is simply more poorly documented). However, running tests under Valgrind causes a much greater slowdown compared with ASan. As a result, it would be beneficial to implement support for both Valgrind and ASan, to give the extra flexibility to choose either performance or accuracy as required. This should be simple as both have very similar memory poisoning interfaces.

**Design decision: Implement memory poisoning tests with both Valgrind's memcheck and ASan manual poisoning.**

##### Validation with new tests

Validation with newly created tests would be simpler to implement than using existing tests, since the tests can be written to take into account memory poisoning. It is also possible to build such a testsuite using existing tests as a starting point - `mbedtls_test_psa_exercise_key` is a test helper that already exercises many PSA operations on a key. This would need to be extended to cover operations without keys (e.g. hashes) and multipart operations, but it provides a good base from which to build all of the required testing.

Additionally, we can ensure that all functions are exercised by automatically generating test data files.

##### Validation with existing tests

An alternative approach would be to integrate memory poisoning validation with existing tests. This has two main advantages:

* All of the tests are written already, potentially saving development time.
* The code coverage of these tests is greater than would be achievable writing new tests from scratch. In practice this advantage is small as buffer copying will take place in the dispatch layer. The tests are therefore independent of the values of parameters passed to the driver, so extra coverage in these parameters does not gain anything.

It may be possible to transparently implement memory poisoning so that existing tests can work without modification. This would be achieved by replacing the implementation of `malloc()` with one that allocates poisoned buffers. However, there are some difficulties with this:

* Not all buffers allocated by tests are used as inputs and outputs to PSA functions being tested.
* Those buffers that are inputs to a PSA function need to be unpoisoned right up until the function is called, so that they can be filled with input data.
* Those buffers that are outputs from a PSA function need to be unpoisoned straight after the function returns, so that they can be read to check the output is correct.

These issues may be solved by creating some kind of test wrapper around every PSA function call that poisons the memory. However, it is unclear how straightforward this will be in practice. If this is simple to achieve, the extra coverage and time saved on new tests will be a benefit. If not, writing new tests is the best strategy.

**Design decision: Add memory poisoning transparently to existing tests.**

#### Discussion of copying validation

Of all discussed approaches, validation by memory poisoning appears as the best. This is because it:

* Does not require complex linking against different versions of `malloc()` (as is the case with the memory pool approach).
* Allows automated testing (unlike the review approach).

**Design decision: Use a memory poisoning approach to validate copying.**

### Shared memory protection requirements

TODO: write document and reference it here.

### Validation of careful access for built-in drivers

For PSA functions whose inputs and outputs are not copied, it is important that we validate that the builtin drivers are correctly accessing their inputs and outputs so as not to cause a security issue. Specifically, we must check that each memory location in a shared buffer is not accessed more than once by a driver function. In this section we examine various possible methods for performing this validation.

Note: We are focusing on read-read inconsistencies for now, as most of the cases where we aren't copying are inputs.

#### Review

As with validation of copying, the simplest method of validation we can implement is careful code review. This is the least desirable method of validation for several reasons:

1. It is tedious for the reviewers.
2. Reviewers are prone to make mistakes (especially when performing tedious tasks).
3. It requires engineering time linear in the number of PSA functions to be tested.
4. It cannot assure the quality of third-party drivers, whereas automated tests can be ported to any driver implementation in principle.

If all other approaches turn out to be prohibitively difficult, code review exists as a fallback option. However, it should be understood that this is far from ideal.

#### Tests using `mprotect()`

Checking that a memory location is not accessed more than once may be achieved by using `mprotect()` on a Linux system to cause a segmentation fault whenever a memory access happens. Tests based on this approach are sketched below.

##### Linux mprotect+ptrace

Idea: call `mmap` to allocate memory for arguments and `mprotect` to deny or reenable access. Use `ptrace` from a parent process to react to SIGSEGV from a denied access. On SIGSEGV happening in the faulting region:

1. Use `ptrace` to execute a `mprotect` system call in the child to enable access. TODO: How? `ptrace` can modify registers and memory in the child, which includes changing parameters of a syscall that's about to be executed, but not directly cause the child process to execute a syscall that it wasn't about to execute.
2. Use `ptrace` with `PTRACE_SINGLESTEP` to re-execute the failed load/store instrution.
3. Use `ptrace` to execute a `mprotect` system call in the child to disable access.
4. Use `PTRACE_CONT` to resume the child execution.

Record the addresses that are accessed. Mark the test as failed if the same address is read twice.

##### Debugger + mprotect

Idea: call `mmap` to allocate memory for arguments and `mprotect` to deny or reenable access. Use a debugger to handle SIGSEGV (Gdb: set signal catchpoint). If the segfault was due to accessing the protected region:

1. Execute `mprotect` to allow access.
2. Single-step the load/store instruction.
3. Execute `mprotect` to disable access.
4. Continue execution.

Record the addresses that are accessed. Mark the test as failed if the same address is read twice. This part might be hard to do in the gdb language, so we may want to just log the addresses and then use a separate program to analyze the logs, or do the gdb tasks from Python.

#### Instrumentation (Valgrind)

An alternative approach is to use a dynamic instrumentation tool (the most obvious being Valgrind) to trace memory accesses and check that each of the important memory addresses is accessed no more than once.

Valgrind has no tool specifically that checks the property that we are looking for. However, it is possible to generate a memory trace with Valgrind using the following:

```
valgrind --tool=lackey --trace-mem=yes --log-file=logfile ./myprogram
```
This will execute `myprogram` and dump a record of every memory access to `logfile`, with its address and data width. If `myprogram` is a test that does the following:

1. Set up input and output buffers for a PSA function call.
2. Leak the start and end address of each buffer via `print()`.
3. Write data into the input buffer exactly once.
4. Call the PSA function.
5. Read data from the output buffer exactly once.

Then it should be possible to parse the output from the program and from Valgrind and check that each location was accessed exactly twice: once by the program's setup and once by the PSA function.

#### Fixed Virtual Platform testing

It may be possible to measure double accesses by running tests on a Fixed Virtual Platform such as Corstone 310 ecosystem FVP, available [here](https://developer.arm.com/downloads/-/arm-ecosystem-fvps). There exists a pre-packaged example program for the Corstone 310 FVP available as part of the Open IoT SDK [here](https://git.gitlab.arm.com/iot/open-iot-sdk/examples/sdk-examples/-/tree/main/examples/mbedtls/cmsis-rtx/corstone-310) that could provide a starting point for a set of tests.

Running on an FVP allows two approaches to careful-access testing:

* Convenient scripted use of a debugger with [Iris](https://developer.arm.com/documentation/101196/latest/). This allows memory watchpoints to be set, perhaps more flexibly than with GDB.
* Tracing of all memory accesses with [Tarmac Trace](https://developer.arm.com/documentation/100964/1123/Plug-ins-for-Fast-Models/TarmacTrace). To validate the single-access properties, the [processor memory access trace source](https://developer.arm.com/documentation/100964/1123/Plug-ins-for-Fast-Models/TarmacTrace/Processor-memory-access-trace) can be used to output all memory accesses happening on the FVP. This output can then be easily parsed and processed to ensure that the input and output buffers are accessed only once. The addresses of buffers can either be leaked by the program through printing to the serial port or set to fixed values in the FVP's linker script.

#### Discussion of careful-access validation

The best approach for validating the correctness of memory accesses is an open question that requires further investigation. To answer this question, each of the test strategies discussed above must be prototyped as follows:

1. Take 1-2 days to create a basic prototype of a test that uses the approach.
2. Document the prototype - write a short guide that can be followed to arrive at the same prototype.
3. Evaluate the prototype according to its usefulness. The criteria of evaluation should include:
   * Ease of implementation - Was the prototype simple to implement? Having implemented it, is it simple to extend it to do all of the required testing?
   * Flexibility - Could the prototype be extended to cover other careful-access testing that may be needed in future?
   * Performance - Does the test method perform well? Will it cause significant slowdown to CI jobs?
   * Ease of reproduction - Does the prototype require a particular platform or tool to be set up? How easy would it be for an external user to run the prototype?
   * Comprehensibility - Accounting for the lower code quality of a prototype, would developers unfamiliar with the tests based on the prototype be able to understand them easily?
   * Portability - How well can this approach be ported to multiple platforms? This would allow us to ensure that there are no double-accesses due to a bug that only affects a specific target.

Once each prototype is complete, choose the best approach to implement the careful-access testing. Implement tests using this approach for each of the PSA interfaces that require careful-access testing:

* Hash
* MAC
* AEAD (additional data only)
* Key derivation
* Asymmetric signature (input only)

##### New vs existing tests

Most of the test methods discussed above need extra setup. Some require leaking of buffer bounds, predictable memory access patterns or allocation of special buffers. FVP testing even requires the tests to be run on a non-host target.

With this complexity in mind it does not seem feasible to run careful-access tests using existing testsuites. Instead, new tests should be written that exercise the drivers in the required way. Fortunately, the only interfaces that need testing are hash, MAC, AEAD (testing over AD only), Key derivation and Asymmetric signature, which limits the number of new tests that must be written.

#### Validation of validation for careful-access

In order to ensure that the careful-access validation works, it is necessary to write tests to check that we can correctly detect careful-access violations when they occur. To do this, write a test function that:

* Reads its input multiple times at the same location.
* Writes to its output multiple times at the same location.

Then, write a careful-access test for this function and ensure that it fails.

## Analysis of argument protection in built-in drivers

TODO: analyze the built-in implementations of mechanisms for which there is a requirement on drivers. By code inspection, how satisfied are we that they meet the requirement?

## Copy bypass

For efficiency, we are likely to want mechanisms to bypass the copy and process buffers directly in builds that are not affected by shared memory considerations.

Expand this section to document any mechanisms that bypass the copy.

Make sure that such mechanisms preserve the guarantees when buffers overlap.

## Detailed design

### Implementation by module

Module | Input protection strategy | Output protection strategy | Notes
---|---|---|---
Hash and MAC | Careful access | Careful access | Low risk of multiple-access as the input and output are raw unformatted data.
Cipher | Copying | Copying |
AEAD | Copying (careful access for additional data) | Copying |
Key derivation | Careful access | Careful access |
Asymmetric signature | Careful access | Copying | Inputs to signatures are passed to a hash. This will no longer hold once PureEdDSA support is implemented.
Asymmetric encryption | Copying | Copying |
Key agreement | Copying | Copying |
PAKE | Copying | Copying |
Key import / export | Copying | Copying | Keys may be imported and exported in DER format, which is a structured format and therefore susceptible to read-read inconsistencies and potentially write-read inconsistencies.

### Copying functions

As discussed in [Copying code](#copying-code), it is simpler to use a single unified API for copying. Therefore, we create the following functions:

* `psa_crypto_copy_input(const uint8_t *input, size_t input_length, uint8_t *input_copy, size_t input_copy_length)`
* `psa_crypto_copy_output(const uint8_t *output_copy, size_t output_copy_length, uint8_t *output, size_t output_length)`

These seem to be a repeat of the same function, however it is useful to retain two separate functions for input and output parameters so that we can use different test hooks in each when using memory poisoning for tests.

Given that the majority of functions will be allocating memory on the heap to copy, it is helpful to build convenience functions that allocate the memory as well.

In order to keep track of allocated copies on the heap, we can create new structs:

```c
typedef struct psa_crypto_local_input_s {
    uint8_t *buffer;
    size_t length;
} psa_crypto_local_input_t;

typedef struct psa_crypto_local_output_s {
    uint8_t *original;
    uint8_t *buffer;
    size_t length;
} psa_crypto_local_output_t;
```

These may be used to keep track of input and output copies' state, and ensure that their length is always stored with them. In the case of output copies, we keep a pointer to the original buffer so that it is easy to perform a writeback to the original once we have finished outputting.

With these structs we may create 2 pairs of functions, one pair for input copies:

```c
psa_status_t psa_crypto_local_input_alloc(const uint8_t *input, size_t input_len,
                                          psa_crypto_local_input_t *local_input);

void psa_crypto_local_input_free(psa_crypto_local_input_t *local_input);
```

* `psa_crypto_local_input_alloc()` calls `calloc()` to allocate a new buffer of length `input_len`, copies the contents across from `input`. It then stores `input_len` and the pointer to the copy in the struct `local_input`.
* `psa_crypto_local_input_free()` calls `free()` on the local input that is referred to by `local_input` and sets the pointer in the struct to `NULL`.

We also create a pair of functions for output copies:

```c
psa_status_t psa_crypto_local_output_alloc(uint8_t *output, size_t output_len,
                                           psa_crypto_local_output_t *local_output);

psa_status_t psa_crypto_local_output_free(psa_crypto_local_output_t *local_output);
```

* `psa_crypto_local_output_alloc()` calls `calloc()` to allocate a new buffer of length `output_len` and stores `output_len` and the pointer to the buffer in the struct `local_output`. It also stores a pointer to `output` in `local_output->original`.
* `psa_crypto_local_output_free()` copies the contents of the output buffer `local_output->buffer` into the buffer `local_output->original`, calls `free()` on `local_output->buffer` and sets it to `NULL`.

Some PSA functions may not use these convenience functions as they may have local optimizations that reduce memory usage. For example, ciphers may be able to use a single intermediate buffer for both input and output.

In order to abstract the management of the copy state further, to make it simpler to add, we create the following 6 convenience macros:

For inputs:

* `LOCAL_INPUT_DECLARE(input, input_copy_name)`, which declares and initializes a `psa_crypto_local_input_t` and a pointer with the name `input_copy_name` in the current scope.
* `LOCAL_INPUT_ALLOC(input, input_size, input_copy)`, which tries to allocate an input using `psa_crypto_local_input_alloc()`. On failure, it sets an error code and jumps to an exit label. On success, it sets `input_copy` to point to the copy of the buffer.
* `LOCAL_INPUT_FREE(input, input_copy)`, which frees the input copy using `psa_crypto_local_input_free()` and sets `input_copy` to `NULL`.

For outputs:

* `LOCAL_OUTPUT_DECLARE(output, output_copy_name)`, analogous to `LOCAL_INPUT_DECLARE()` for `psa_crypto_local_output_t`.
* `LOCAL_OUTPUT_ALLOC(output, output_size, output_copy)`, analogous to `LOCAL_INPUT_ALLOC()` for outputs, calling `psa_crypto_local_output_alloc()`.
* `LOCAL_OUTPUT_FREE(output, output_copy)`, analogous to `LOCAL_INPUT_FREE()` for outputs. If the `psa_crypto_local_output_t` is in an invalid state (the copy pointer is valid, but the original pointer is `NULL`) this macro sets an error status.

These macros allow PSA functions to have copying added while keeping the code mostly unmodified. Consider a hypothetical PSA function:

```c
psa_status_t psa_foo(const uint8_t *input, size_t input_length,
                     uint8_t *output, size_t output_size, size_t *output_length)
{
    /* Do some operation on input and output */
}
```

By changing the name of the input and output parameters, we can retain the original variable name as the name of the local copy while using a new name (e.g. with the suffix `_external`) for the original buffer. This allows copying to be added near-seamlessly as follows:

```c
psa_status_t psa_foo(const uint8_t *input_external, size_t input_length,
                     uint8_t *output_external, size_t output_size, size_t *output_length)
{
    psa_status_t status;

    LOCAL_INPUT_DECLARE(input_external, input);
    LOCAL_OUTPUT_DECLARE(output_external, output);

    LOCAL_INPUT_ALLOC(input_external, input);
    LOCAL_OUTPUT_ALLOC(output_external, output);

    /* Do some operation on input and output */

exit:
    LOCAL_INPUT_FREE(input_external, input);
    LOCAL_OUTPUT_FREE(output_external, output);
}
```

A second advantage of using macros for the copying (other than simple convenience) is that it allows copying to be easily disabled by defining alternate macros that function as no-ops. Since buffer copying is specific to systems where shared memory is passed to PSA functions, it is useful to be able to disable it where it is not needed, to save code size.

To this end, the macros above are defined conditionally on a new config option, `MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS`, which may be set whenever PSA functions are assumed to have exclusive access to their input and output buffers. When `MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS` is set, the macros do not perform copying.

### Implementation of copying validation

As discussed in the [design exploration of copying validation](#validation-of-copying), the best strategy for validation of copies appears to be validation by memory poisoning, implemented using Valgrind and ASan.

To perform memory poisoning, we must implement the functions alluded to in [Validation of copying by memory poisoning](#validation-of-copying-by-memory-poisoning):
```c
void mbedtls_test_memory_poison(const unsigned char *ptr, size_t size);
void mbedtls_test_memory_unpoison(const unsigned char *ptr, size_t size);
```
This should poison or unpoison the given buffer, respectively.

* `mbedtls_test_memory_poison()` is equivalent to calling `VALGRIND_MAKE_MEM_NOACCESS(ptr, size)` or `ASAN_POISON_MEMORY_REGION(ptr, size)`.
* `mbedtls_test_memory_unpoison()` is equivalent to calling `VALGRIND_MAKE_MEM_DEFINED(ptr, size)` or `ASAN_UNPOISON_MEMORY_REGION(ptr, size)`.

The PSA copying function must then have test hooks implemented as outlined in [Validation of copying by memory poisoning](#validation-of-copying-by-memory-poisoning).

As discussed in [the design exploration](#validation-with-existing-tests), the preferred approach for implementing copy-testing is to implement it transparently using existing tests. This is specified in more detail below.

#### Transparent allocation-based memory poisoning

In order to implement transparent memory poisoning we require a wrapper around all PSA function calls that poisons any input and output buffers.

The easiest way to do this is to create wrapper functions that poison the memory and then `#define` PSA function names to be wrapped versions of themselves. For example, to replace `psa_aead_update()`:
```c
psa_status_t mem_poison_psa_aead_update(psa_aead_operation_t *operation,
                                        const uint8_t *input,
                                        size_t input_length,
                                        uint8_t *output,
                                        size_t output_size,
                                        size_t *output_length)
{
    mbedtls_test_memory_poison(input, input_length);
    mbedtls_test_memory_poison(output, output_size);
    psa_status_t status = psa_aead_update(operation, input, input_length,
                                          output, output_size, output_length);
    mbedtls_test_memory_unpoison(input, input_length);
    mbedtls_test_memory_unpoison(output, output_size);

    return status;
}

#define psa_aead_update(...) mem_poison_psa_aead_update(__VA_ARGS__)
```

There now exists a more generic mechanism for making exactly this kind of transformation - the PSA test wrappers, which exist in the files `tests/include/test/psa_test_wrappers.h` and `tests/src/psa_test_wrappers.c`. These are wrappers around all PSA functions that allow testing code to be inserted at the start and end of a PSA function call.

The test wrappers are generated by a script, although they are not automatically generated as part of the build process. Instead, they are checked into source control and must be manually updated when functions change by running `framework/scripts/generate_psa_wrappers.py`.

Poisoning code is added to these test wrappers where relevant in order to pre-poison and post-unpoison the parameters to the functions.

#### Configuration of poisoning tests

Since the memory poisoning tests will require the use of interfaces specific to the sanitizers used to poison memory, they must only be enabled when we are building with ASan or Valgrind. For now, we can auto-detect ASan at compile-time and set an option: `MBEDTLS_TEST_MEMORY_CAN_POISON`. When this option is enabled, we build with memory-poisoning support. This enables transparent testing with ASan without needing any extra configuration options.

Auto-detection and memory-poisoning with Valgrind is left for future work.

#### Validation of validation for copying

To make sure that we can correctly detect functions that access their input/output buffers rather than the copies, it would be best to write a test function that misbehaves and test it with memory poisoning. Specifically, the function should:

* Read its input buffer and after calling the input-buffer-copying function to create a local copy of its input.
* Write to its output buffer before and after calling the output-buffer-copying function to copy-back its output.

Then, we could write a test that uses this function with memory poisoning and ensure that it fails. Since we are expecting a failure due to memory-poisoning, we would run this test separately from the rest of the memory-poisoning testing.

This testing is implemented in `framework/tests/programs/metatest.c`, which is a program designed to check that test failures happen correctly. It may be run via the script `tests/scripts/run-metatests.sh`.
