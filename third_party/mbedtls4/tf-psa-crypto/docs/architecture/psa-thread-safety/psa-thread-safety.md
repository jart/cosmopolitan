# Thread-safety of the PSA subsystem

Currently, PSA Crypto API calls in Mbed TLS releases are not thread-safe.

As of Mbed TLS 3.6, an MVP for making the [PSA Crypto key management API](https://arm-software.github.io/psa-api/crypto/1.1/api/keys/management.html) and [`psa_crypto_init`](https://arm-software.github.io/psa-api/crypto/1.1/api/library/library.html#c.psa_crypto_init) thread-safe has been implemented. Implementations which only ever call PSA functions from a single thread are not affected by this new feature.

Summary of recent work:

- Key Store:
    - Slot states are described in the [Key slot states](#key-slot-states) section. They guarantee safe concurrent access to slot contents.
    - Key slots are protected by a global mutex, as described in [Key store consistency and abstraction function](#key-store-consistency-and-abstraction-function).
    - Key destruction strategy abiding by [Key destruction guarantees](#key-destruction-guarantees), with an implementation discussed in [Key destruction implementation](#key-destruction-implementation).
- `global_data` variables in `psa_crypto.c` and `psa_crypto_slot_management.c` are now protected by mutexes, as described in the [Global data](#global-data) section.
- The testing system has now been made thread-safe. Tests can now spin up multiple threads, see [Thread-safe testing](#thread-safe-testing) for details.
- Some multithreaded testing of the key management API has been added, this is outlined in [Testing-and-analysis](#testing-and-analysis).
- The solution uses the pre-existing `MBEDTLS_THREADING_C` threading abstraction.
- The core makes no additional guarantees for drivers. See [Driver policy](#driver-policy) for details.

The other functions in the PSA Crypto API are planned to be made thread-safe in future, but currently we are not testing this.

## Overview of the document

* The [Guarantees](#guarantees) section describes the properties that are followed when PSA functions are invoked by multiple threads.
* The [Usage guide](#usage-guide) section gives guidance on initializing, using and freeing PSA when using multiple threads.
* The [Current strategy](#current-strategy) section describes how thread-safety of key management and `global_data` is achieved.
* The [Testing and analysis](#testing-and-analysis) section discusses the state of our testing, as well as how this testing will be extended in future.
* The [Future work](#future-work) section outlines our long-term goals for thread-safety; it also analyses how we might go about achieving these goals.

## Definitions

*Concurrent calls*

The PSA specification defines concurrent calls as: "In some environments, an application can make calls to the Crypto API in separate threads. In such an environment, concurrent calls are two or more calls to the API whose execution can overlap in time." (See PSA documentation [here](https://arm-software.github.io/psa-api/crypto/1.1/overview/conventions.html#concurrent-calls).)

*Thread-safety*

In general, a system is thread-safe if any valid set of concurrent calls is handled as if the effect and return code of every call is equivalent to some sequential ordering. We implement a weaker notion of thread-safety, we only guarantee thread-safety in the circumstances described in the [PSA Concurrent calling conventions](#psa-concurrent-calling-conventions) section.

## Guarantees

### Correctness out of the box

Building with `MBEDTLS_PSA_CRYPTO_C` and `MBEDTLS_THREADING_C` gives code which is correct; there are no race-conditions, deadlocks or livelocks when concurrently calling any set of PSA key management functions once `psa_crypto_init` has been called (see the [Initialization](#initialization) section for details on how to correctly initialize the PSA subsystem when using multiple threads).

We do not test or support calling other PSA API functions concurrently.

There is no busy-waiting in our implementation, every API call completes in a finite number of steps regardless of the locking policy of the underlying mutexes.

When only considering key management functions: Mbed TLS 3.6 abides by the minimum expectation for concurrent calls set by the PSA specification (see [PSA Concurrent calling conventions](#psa-concurrent-calling-conventions)).

#### PSA Concurrent calling conventions

These are the conventions which are planned to be added to the PSA 1.2 specification, Mbed TLS 3.6 abides by these when only considering [key management functions](https://arm-software.github.io/psa-api/crypto/1.1/api/keys/management.html):

> The result of two or more concurrent calls must be consistent with the same set of calls being executed sequentially in some order, provided that the calls obey the following constraints:
>
> * There is no overlap between an output parameter of one call and an input or output parameter of another call. Overlap between input parameters is permitted.
>
> * A call to `psa_destroy_key()` must not overlap with a concurrent call to any of the following functions:
>     - Any call where the same key identifier is a parameter to the call.
>     - Any call in a multi-part operation, where the same key identifier was used as a parameter to a previous step in the multi-part operation.
>
> * Concurrent calls must not use the same operation object.
>
> If any of these constraints are violated, the behaviour is undefined.
>
> The consistency requirement does not apply to errors that arise from resource failures or limitations. For example, errors resulting from resource exhaustion can arise in concurrent execution that do not arise in sequential execution.
>
> As an example of this rule: suppose two calls are executed concurrently which both attempt to create a new key with the same key identifier that is not already in the key store. Then:
> * If one call returns `PSA_ERROR_ALREADY_EXISTS`, then the other call must succeed.
> * If one of the calls succeeds, then the other must fail: either with `PSA_ERROR_ALREADY_EXISTS` or some other error status.
> * Both calls can fail with error codes that are not `PSA_ERROR_ALREADY_EXISTS`.
>
> If the application concurrently modifies an input parameter while a function call is in progress, the behaviour is undefined.

### Backwards compatibility

Code which was working prior to Mbed TLS 3.6 will still work. Implementations which only ever call PSA functions from a single thread, or which protect all PSA calls using a mutex, are not affected by this new feature. If an application previously worked with a 3.X version, it will still work on version 3.6.

### Supported threading implementations

Currently, the only threading library with support shipped in the code base is pthread (enabled by `MBEDTLS_THREADING_PTHREAD`). The only concurrency primitives we use are mutexes, see [Condition variables](#condition-variables) for discussion about implementing new primitives in future major releases.

Users can add support to any platform which has mutexes using the Mbed TLS platform abstraction layer (see `include/mbedtls/threading.h` for details).

We intend to ship support for other platforms including Windows in future releases.

### Key destruction guarantees

Much like all other API calls, `psa_destroy_key` does not block indefinitely, and when `psa_destroy_key` returns:

1. The key identifier does not exist. This is a functional requirement for persistent keys: any thread can immediately create a new key with the same identifier.
2. The resources from the key have been freed. This allows threads to create similar keys immediately after destruction, regardless of resources.

When `psa_destroy_key` is called on a key that is in use, guarantee 2 may be violated. This is consistent with the PSA specification requirements, as destruction of a key in use is undefined.

In future versions we aim to enforce stronger requirements for key destruction, see [Long term key destruction requirements](#long-term-key-destruction-requirements) for details.

### Driver policy

The core makes no additional guarantees for drivers. Driver entry points may be called concurrently from multiple threads. Threads can concurrently call entry points using the same key, there is also no protection from destroying a key which is in use.

### Random number generators

The PSA RNG can be accessed both from various PSA functions, and from application code via `mbedtls_psa_get_random`.

When using the built-in RNG implementations, i.e. when `MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG` is disabled, querying the RNG is thread-safe (`mbedtls_psa_random_init` and `mbedtls_psa_random_seed` are only thread-safe when called while holding `mbedtls_threading_psa_rngdata_mutex`. `mbedtls_psa_random_free` is not thread-safe).

When `MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG` is enabled, it is down to the external implementation to ensure thread-safety, should threading be enabled.

## Usage guide

### Initialization

The PSA subsystem is initialized via a call to [`psa_crypto_init`](https://arm-software.github.io/psa-api/crypto/1.1/api/library/library.html#c.psa_crypto_init). This is a thread-safe function, and multiple calls to `psa_crypto_init` are explicitly allowed. It is valid to have multiple threads each calling `psa_crypto_init` followed by a call to any PSA key management function (if the init succeeds).

### General usage

Once initialized, threads can use any PSA function if there is no overlap between their calls. All threads share the same set of keys, as soon as one thread returns from creating/loading a key via a key management API call the key can be used by any thread. If multiple threads attempt to load the same persistent key, with the same key identifier, only one thread can succeed - the others will return `PSA_ERROR_ALREADY_EXISTS`.

Applications may need careful handling of resource management errors. As explained in ([PSA Concurrent calling conventions](#psa-concurrent-calling-conventions)), operations in progress can have memory related side effects. It is possible for a lack of resources to cause errors which do not arise in sequential execution. For example, multiple threads attempting to load the same persistent key can lead to some threads returning `PSA_ERROR_INSUFFICIENT_MEMORY` if the key is not currently in the key store - while trying to load a persistent key into the key store a thread temporarily reserves a free key slot.

If a mutex operation fails, which only happens if the mutex implementation fails, the error code `PSA_ERROR_SERVICE_FAILURE` will be returned. If this code is returned, execution of the PSA subsystem must be stopped. All functions which have internal mutex locks and unlocks (except for when the lock/unlock occurs in a function that has no return value) will return with this error code in this situation.

### Freeing

There is no thread-safe way to free all PSA resources. This is because any such operation would need to wait for all other threads to complete their tasks before wiping resources.

`mbedtls_psa_crypto_free` must only be called by a single thread once all threads have completed their operations.

## Current strategy

This section describes how we have implemented thread-safety. There is discussion of: techniques, internal properties for enforcing thread-safe access, how the system stays consistent and our abstraction model.

### Protected resources

#### Global data

We have added a mutex `mbedtls_threading_psa_globaldata_mutex` defined in `include/mbedtls/threading.h`, which is used to make `psa_crypto_init` thread-safe.

There are two `psa_global_data_t` structs, each with a single instance `global_data`:

* The struct in `library/psa_crypto.c` is protected by `mbedtls_threading_psa_globaldata_mutex`. The RNG fields within this struct are not protected by this mutex, and are not always thread-safe (see [Random number generators](#random-number-generators)).
* The struct in `library/psa_crypto_slot_management.c` has two fields: `key_slots` is protected as described in [Key slots](#key-slots), `key_slots_initialized` is protected by the global data mutex.

#### Mutex usage

A deadlock would occur if a thread attempts to lock a mutex while already holding it. Functions which need to be called while holding the global mutex have documentation to say this.

To avoid performance degradation, functions must hold mutexes for as short a time as possible. In particular, they must not start expensive operations (eg. doing cryptography) while holding the mutex.

#### Key slots


Keys are stored internally in a global array of key slots known as the "key store", defined in `library/psa_slot_management.c`.

##### Key slot states

Each key slot has a state variable and a `registered_readers` counter. These two variables dictate whether an operation can access a slot, and in what way the slot can be used.

There are four possible states for a key slot:

* `PSA_SLOT_EMPTY`: no thread is currently accessing the slot, and no information is stored in the slot. Any thread is able to change the slot's state to `PSA_SLOT_FILLING` and begin to load data into the slot.
* `PSA_SLOT_FILLING`: one thread is currently loading or creating material to fill the slot, this thread is responsible for the next state transition. Other threads cannot read the contents of a slot which is in this state.
* `PSA_SLOT_FULL`: the slot contains a key, and any thread is able to use the key after registering as a reader, increasing `registered_readers` by 1.
* `PSA_SLOT_PENDING_DELETION`: the key within the slot has been destroyed or marked for destruction, but at least one thread is still registered as a reader (`registered_readers > 0`). No thread can register to read this slot. The slot must not be wiped until the last reader unregisters. It is during the last unregister that the contents of the slot are wiped, and the slot's state is set to `PSA_SLOT_EMPTY`.

###### Key slot state transition diagram
![](key-slot-state-transitions.png)

In the state transition diagram above, an arrow between two states `q1` and `q2` with label `f` indicates that if the state of a slot is `q1` immediately before `f`'s linearization point, it may be `q2` immediately after `f`'s linearization point. Internal functions have italicized labels. The `PSA_SLOT_PENDING_DELETION -> PSA_SLOT_EMPTY` transition can be done by any function which calls `psa_unregister_read`.

The state transition diagram can be generated in https://app.diagrams.net/ via this [url](https://viewer.diagrams.net/?tags=%7B%7D&highlight=0000ff&edit=_blank&layers=1&nav=1#R3Vxbd5s4EP4t%2B%2BDH5CBxf6zrJJvW7aYn7W7dFx9qZFstBg7gW379CnMxkoUtY%2BGQ%2BiVISCPQjD59mhnSU98vNg%2BRE84%2FBS7yelBxNz110IMQAEsnf9KabVZjmHnFLMJu3mhf8YxfUF6p5LVL7KKYapgEgZfgkK6cBL6PJglV50RRsKabTQOPHjV0Zuig4nnieIe1%2F2E3mWe1FjT39X8jPJsXIwPDzu4snKJx%2Fibx3HGDdaVKveup76MgSLKrxeY98tLJK%2BYl63dfc7d8sAj5iUiHH%2BBlOP338cP6i%2B37%2Ff7oV%2Fjr442aSVk53jJ%2F4R40PCKv7%2BIVuZyll%2FffhsOimsiv3OE0njvxOEKOi6K4uPszYtuzUnbzk2yLSScPTvRLCv31HCfoOXQm6Z01MbF0hGThkRIgl04cZkqf4g1yS1HVScnnaYWiBG0qVfkkPaBggZJoS5rkdzUrV1hhsUpeXlf0n1fNK6ov6pzc4mal5L1SyEWulzN0BABHSeyM%2Be671NpJaeI5cYwn9ERFwdJ30xkaKKREJifafs9v7QqjamGwqbYbbIvSBidlJ3I9qtTvu6SFoketNuJgGU3QabtMnGiGkiPttKwdcqlVfKjbiu50ju6Kugh5ToJX9NrnKTQf4SnA5M1qTUc3GJvI3jvvVV2rrCDTvrUrP4sSq6mM2GyaDsTurK2chAsMENaiBC7WcBg746UfoRmOExTtEKCy2HH9UieaGzo%2Fya5BL2wPz%2FzUmInloIhUpOsXE1h%2Bl99YYNdNZfQjFOMX5%2BdOXmpzYToLu3nR%2Bz19wLXC48uMRYpyc8lHofCbhyDKLVRMm1LZDbzMwAoxgOkSTKcxakfpIjvD3aenr6O3CfOdQ3lbOsrneK1U8BocxetyXygLo2qhZl9ojvJQEOVBt1CetpwDNBYG%2BRObRcuoXvDSU6g%2BdbA3%2Fo224wkB9QQH%2FlvD9WJhdRHXc8mQEsr2bw%2FkDzf2%2B8fh8PHzQ6exWjVeGas1kb3xrFPTX3%2FcsenVlaSLKOnp7vNgZ%2B6CehrcDe%2B%2BPv7z%2BW3qqHOkx2yL84ifUZudhZtznsKJdYrzwE5xHqiQzc%2FSoAnI2VTTDXoX1DXj1gS6CS1TJwWVES9KiIDBMCvtuozIEkEMLkciZAVFKzSeRgjtuFLsBQmfJwkCDXeYmExAwuViXBw6OWpnOVuBC12kbKUY7VosDfD4hnyYvNWbHA6zXq96POyWEzCFSkUpoNIgqEaDGkhdewVWqpZiNgNLTWHAkti6yphk237B5oA5xT6O5wLHyjcGXOVSvRi5bogVabZJQ5cqx0ItrtQrABmPkzO6nCzJRuqWFOx6YQ1xN1lzRBMNa6idQjStiNmWMdyGHi%2FdYASxB4sawCI24GwrzfLlWf%2FANo2NpqIcfy7ItAcn2mvWMfnkInvipotn0NcmAD9MQu8FLR%2Fxs%2F7uaSN2nq1hpyejMpew0pqwTzNKKjYkMZKx47tjL5j8Lvn2%2BPtFA6VyJ14Q7wj8Wb3CJbHaaq%2BDwf8wel7iuIxdDqgWvZou5Oe5ZJr0Q%2F1ae5zKS6mQQtarG5SgT6PCztuN5GiCG1u3IjnQhJSV6HrDjQ3UOdauxMRV3gmRi1UuipMo2F6OcXLwtLMQVy5jCS4IzTLoM2CxDC403xuaTdktQByXicj32nKJ%2Bym0Oh8X28e3bnltVYbX6k1D1arJOBsEibssi6t3NDR1w3YBeI4uLinUymYc9ZJwBxRujjY9CNzZuUqSjLAnlIarFj2hon4DvdPwY4Cm8MOkyhjtJUByra547orZHXCpzgKKtPSXFFCKrpKJDO3mbCP9ha%2FXK2VWn4aGJjDUHE50QTjp2Gmtxkt3NpxAhs0Y7WXe8c0O1tKZhr42eZ61NQ4PqdPbdV8dX%2FYywsvlF05yIRGorwSJPKrNaFJ6iKaxX6oryMTEGxoHSFTNvIWWpWtQszUbqpbKyqVCy1AIts6NnpC3qY4CbPohTEW9NaFS%2FtTjbwTso8IAOEeY3vzJ2gnKcLP23%2FKnMcdBQQJgKrpFc0hJFLKNbJwnvNwMp3BsWbMvqx%2F3Hye%2BH3I%2FjJHDGanEmkZf47XGGEWzFruViqMyOTI667YSxmX9hCNNHmPk2pwQYUxxBi%2FCIEsRPMtPP0M%2BipykgYM%2FCM%2BPJaT00kURXu3yfsbBMgmX1DOfn1X9GlB5FB0kIKWuAe65%2BGLvHSX0almMsLMJDCeyCeScfv6wT%2FdEAyKimUz7YFkRebtSbpNNu7IPcs6F8zEZQaIh4L0gqUvww0j7vh7F%2FW9ujL7iR%2FfmYWy1QF0KOy2JxzmWSicnvP4nF93KumPJi9n4UMmQFxOKWea550bW3W9qcrPiuCZdz4yaJ4x1gVwcXb8SyAWwDTlsQmUijIxPogmYkeL%2B3%2BJkzff%2FXEi9%2Bx8%3D).
##### Key slot access primitives

The state of a key slot is updated via the internal function `psa_key_slot_state_transition`. To change the state of `slot` from `expected_state` to `new_state`, when `new_state` is not `PSA_SLOT_EMPTY`, one must call `psa_key_slot_state_transition(slot, expected_state, new_state)`; if the state was not `expected_state` then `PSA_ERROR_CORRUPTION_DETECTED` is returned. The sole reason for having an expected state parameter here is to help guarantee that our functions work as expected, this error code cannot occur without an internal coding error.

Changing a slot's state to `PSA_SLOT_EMPTY` is done via `psa_wipe_key_slot`, this function wipes the entirety of the key slot.

The reader count of a slot is incremented via `psa_register_read`, and decremented via `psa_unregister_read`. Library functions register to read a slot via the `psa_get_and_lock_key_slot_X` functions, read from the slot, then call `psa_unregister_read` to make known that they have finished reading the slot's contents.

##### Key store consistency and abstraction function

The key store is protected by a single global mutex `mbedtls_threading_key_slot_mutex`.

We maintain the consistency of the key store by ensuring that all reads and writes to `slot->state` and `slot->registered_readers` are performed under `mbedtls_threading_key_slot_mutex`. All the access primitives described above must be called while the mutex is held; there is a convenience function `psa_unregister_read_under_mutex` which wraps a call to `psa_unregister_read` in a mutex lock/unlock pair.

A thread can only traverse the key store while holding `mbedtls_threading_key_slot_mutex`, the set of keys within the key store which the thread holding the mutex can access is equivalent to the set:

    {mbedtls_svc_key_id_t k : (\exists slot := &global_data.key_slots[i]) [
                                  (slot->state == PSA_SLOT_FULL) &&
                                  (slot->attr.id == k)]}

The union of this set and the set of persistent keys not currently loaded into slots is our abstraction function for the key store, any key not in this union does not currently exist as far as the code is concerned (even if the key is in a slot which has a `PSA_SLOT_FILLING` or `PSA_SLOT_PENDING_DELETION` state). Attempting to start using any key which is not a member of the union will result in a `PSA_ERROR_INVALID_HANDLE` error code.

##### Locking and unlocking the mutex

If a lock or unlock operation fails and this is the first failure within a function, the function will return `PSA_ERROR_SERVICE_FAILURE`. If a lock or unlock operation fails after a different failure has been identified, the status code is not overwritten.

We have defined a set of macros in `library/psa_crypto_core.h` to capture the common pattern of (un)locking the mutex and returning or jumping to an exit label upon failure.

##### Key creation and loading

To load a new key into a slot, the following internal utility functions are used:

* `psa_reserve_free_key_slot` - This function, which must be called under `mbedtls_threading_key_slot_mutex`, iterates through the key store to find a slot whose state is `PSA_SLOT_EMPTY`. If found, it reserves the slot by setting its state to `PSA_SLOT_FILLING`. If not found, it will see if there are any persistent keys loaded which do not have any readers, if there are it will kick one such key out of the key store.
* `psa_start_key_creation` - This function wraps around `psa_reserve_free_key_slot`, if a slot has been found then the slot id is set. This second step is not done under the mutex, at this point the calling thread has exclusive access to the slot.
* `psa_finish_key_creation` - After the contents of the key have been loaded (again this loading is not done under the mutex), the thread calls `psa_finish_key_creation`. This function takes the mutex, checks that the key does not exist in the key store (this check cannot be done before this stage), sets the slot's state to `PSA_SLOT_FULL` and releases the mutex. Upon success, any thread is immediately able to use the new key.
* `psa_fail_key_creation` - If there is a failure at any point in the key creation stage, this clean-up function takes the mutex, wipes the slot, and releases the mutex. Immediately after this unlock, any thread can start to use the slot for another key load.

##### Re-loading persistent keys

As described above, persistent keys can be kicked out of the key slot array provided they are not currently being used (`registered_readers == 0`). When attempting to use a persistent key that has been kicked out of a slot, the call to `psa_get_and_lock_key_slot` will see that the key is not in a slot, call `psa_reserve_free_key_slot` and load the key back into the reserved slot. This entire sequence is done during a single mutex lock, which is necessary for thread-safety (see documentation of `psa_get_and_lock_key_slot`).

If `psa_reserve_free_key_slot` cannot find a suitable slot, the key cannot be loaded back in. This will lead to a `PSA_ERROR_INSUFFICIENT_MEMORY` error.

##### Using existing keys

One-shot operations follow a standard pattern when using an existing key:

* They call one of the `psa_get_and_lock_key_slot_X` functions, which then finds the key and registers the thread as a reader.
* They operate on the key slot, usually copying the key into a separate buffer to be used by the operation. This step is not performed under the key slot mutex.
* Once finished, they call `psa_unregister_read_under_mutex`.

Multi-part and restartable operations each have a "setup" function where the key is passed in, these functions follow the above pattern. The key is copied into the `operation` object, and the thread unregisters from reading the key (the operations do not access the key slots again). The copy of the key will not be destroyed during a call to `psa_destroy_key`, the thread running the operation is responsible for deleting its copy in the clean-up. This may need to change to enforce the long term key requirements ([Long term key destruction requirements](#long-term-key-destruction-requirements)).

##### Key destruction implementation

The locking strategy here is explained in `library/psa_crypto.c`. The destroying thread (the thread calling `psa_destroy_key`) does not always wipe the key slot. The destroying thread registers to read the key, sets the slot's state to `PSA_SLOT_PENDING_DELETION`, wipes the slot from memory if the key is persistent, and then unregisters from reading the slot.

`psa_unregister_read` internally calls `psa_wipe_key_slot` if and only if the slot's state is `PSA_SLOT_PENDING_DELETION` and the slot's registered reader counter is equal to 1. This implements a "last one out closes the door" approach. The final thread to unregister from reading a destroyed key will automatically wipe the contents of the slot; no readers remain to reference the slot post deletion, so there cannot be corruption.

### linearizability of the system

To satisfy the requirements in [Correctness out of the box](#correctness-out-of-the-box), we require our functions to be "linearizable" (under certain constraints). This means that any (constraint satisfying) set of concurrent calls are performed as if they were executed in some sequential order.

The standard way of reasoning that this is the case is to identify a "linearization point" for each call, this is a single execution step where the function takes effect (this is usually a step in which the effects of the call become visible to other threads). If every call has a linearization point, the set of calls is equivalent to sequentially performing the calls in order of when their linearization point occurred.

We only require linearizability to hold in the case where a resource-management error is not returned. In a set of concurrent calls, it is permitted for a call c to fail with a `PSA_ERROR_INSUFFICIENT_MEMORY` return code even if there does not exist a sequential ordering of the calls in which c returns this error. Even if such an error occurs, all calls are still required to be functionally correct.

To help justify that our system is linearizable, here are the linearization points/planned linearization points of each PSA call :

* Key creation functions (including `psa_copy_key`) - The linearization point for a successful call is the mutex unlock within `psa_finish_key_creation`; it is at this point that the key becomes visible to other threads. The linearization point for a failed call is the closest mutex unlock after the failure is first identified.
* `psa_destroy_key` - The linearization point for a successful destruction is the mutex unlock, the slot is now in the state `PSA_SLOT_PENDING_DELETION` meaning that the key has been destroyed. For failures, the linearization point is the same.
* `psa_purge_key`, `psa_close_key` - The linearization point is the mutex unlock after wiping the slot for a success, or unregistering for a failure.
* One shot operations - The linearization point is the final unlock of the mutex within `psa_get_and_lock_key_slot`, as that is the point in which it is decided whether or not the key exists.
* Multi-part operations - The linearization point of the key input function is the final unlock of the mutex within `psa_get_and_lock_key_slot`. All other steps have no non resource-related side effects (except for key derivation, covered in the key creation functions).

Please note that one shot operations and multi-part operations are not yet considered thread-safe, as we have not yet tested whether they rely on unprotected global resources. The key slot access in these operations is thread-safe.

## Testing and analysis

### Thread-safe testing

It is now possible for individual tests to spin up multiple threads. This work has made the global variables used in tests thread-safe. If multiple threads fail a test assert, the first failure will be reported with correct line numbers.

Although the `step` feature used in some tests is thread-safe, it may produce unexpected results for multi-threaded tests. `mbedtls_test_set_step` or `mbedtls_test_increment_step` calls within threads can happen in any order, thus may not produce the desired result when precise ordering is required.

### Current state of testing

Our testing is a work in progress. It is not feasible to run our traditional, single-threaded, tests in such a way that tests concurrency. We need to write new test suites for concurrency testing.

Our tests currently only run on pthread, we hope to expand this in the future (our API already allows this).

We run tests using [ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html) to detect data races. We test the key store, and test that our key slot state system is enforced. We also test the thread-safety of `psa_crypto_init`.

Currently, not every API call is tested, we also cannot feasibly test every combination of concurrent API calls. API calls can in general be split into a few categories, each category calling the same internal key management functions in the same order - it is the internal functions that are in charge of locking mutexes and interacting with the key store; we test the thread-safety of these functions.

Since we do not run every cryptographic operation concurrently, we do not test that operations are free of unexpected global variables.

### Expanding testing

Through future work on testing, it would be good to:

* For every API call, have a test which runs multiple copies of the call simultaneously.
* After implementing other threading platforms, expand the tests to these platforms.
* Have increased testing for kicking persistent keys out of slots.
* Explicitly test that all global variables are protected, for this we would need to cover every operation in a concurrent scenario while running ThreadSanitizer.
* Run tests on more threading implementations, once these implementations are supported.

### Performance

Key loading does somewhat run in parallel, deriving the key and copying it key into the slot is not done under any mutex.

Key destruction is entirely sequential, this is required for persistent keys to stop issues with re-loading keys which cannot otherwise be avoided without changing our approach to thread-safety.


## Future work

### Long term requirements

As explained previously, we eventually aim to make the entirety of the PSA API thread-safe. This will build on the work that we have already completed. This requires a full suite of testing, see [Expanding testing](#expanding-testing) for details.

### Long term performance requirements

Our plan for cryptographic operations is that they are not performed under any global mutex. One-shot operations and multi-part operations will each only hold the global mutex for finding the relevant key in the key slot, and unregistering as a reader after the operation, using their own operation-specific mutexes to guard any shared data that they use.

We aim to eventually replace some/all of the mutexes with RWLocks, if possible.

### Long term key destruction requirements

The [PSA Crypto Key destruction specification](https://arm-software.github.io/psa-api/crypto/1.1/api/keys/management.html#key-destruction) mandates that implementations make a best effort to ensure that the key material cannot be recovered. In the long term, it would be good to guarantee that `psa_destroy_key` wipes all copies of the key material.

Here are our long term key destruction goals:

`psa_destroy_key` does not block indefinitely, and when `psa_destroy_key` returns:

1. The key identifier does not exist. This is a functional requirement for persistent keys: any thread can immediately create a new key with the same identifier.
2. The resources from the key have been freed. This allows threads to create similar keys immediately after destruction, regardless of resources.
4. No copy of the key material exists. Rationale: this is a security requirement. We do not have this requirement yet, but we need to document this as a security weakness, and we would like to satisfy this security requirement in the future.

#### Condition variables

It would be ideal to add these to a future major version; we cannot add these as requirements to the default `MBEDTLS_THREADING_C` for backwards compatibility reasons.

Condition variables would enable us to fulfil the final requirement in [Long term key destruction requirements](#long-term-key-destruction-requirements). Destruction would then work as follows:

 * When a thread calls `psa_destroy_key`, they continue as normal until the `psa_unregister_read` call.
 * Instead of calling `psa_unregister_read`, the thread waits until the condition `slot->registered_readers == 1` is true (the destroying thread is the final reader).
 * At this point, the destroying thread directly calls `psa_wipe_key_slot`.

A few changes are needed for this to follow our destruction requirements:

 * Multi-part operations will need to remain registered as readers of their key slot until their copy of the key is destroyed, i.e. at the end of the finish/abort call.
 * The functionality where `psa_unregister_read` can wipe the key slot will need to be removed, slot wiping is now only done by the destroying/wiping thread.

### Protecting operation contexts

Currently, we rely on the crypto service to ensure that the same operation is not invoked concurrently. This abides by the PSA Crypto API Specification ([PSA Concurrent calling conventions](#psa-concurrent-calling-conventions)).

Concurrent access to the same operation object can compromise the crypto service. For example, if the operation context has a pointer (depending on the compiler and the platform, the pointer assignment may or may not be atomic). This violates the functional correctness requirement of the crypto service.

If, in future, we want to protect against this within the library then operations will require a status field protected by a global mutex. On entry, API calls would check the state and return an error if the state is ACTIVE. If the state is INACTIVE, then the call will set the state to ACTIVE, do the operation section and then restore the state to INACTIVE before returning.

### Future driver work

A future policy we may wish to enforce for drivers is:

* By default, each driver only has at most one entry point active at any given time. In other words, each driver has its own exclusive lock.
* Drivers have an optional `"thread_safe"` boolean property. If true, it allows concurrent calls to this driver.
* Even with a thread-safe driver, the core never starts the destruction of a key while there are operations in progress on it, and never performs concurrent calls on the same multipart operation.

In the non-thread-safe case we have these natural assumptions/requirements:

1. Drivers don't call the core for any operation for which they provide an entry point.
2. The core doesn't hold the driver mutex between calls to entry points.

With these, the only way of a deadlock is when there are several drivers with circular dependencies. That is, Driver A makes a call that is dispatched to Driver B; upon executing this call Driver B makes a call that is dispatched to Driver A. For example Driver A does CCM, which calls driver B to do CBC-MAC, which in turn calls Driver A to perform AES.

Potential ways for resolving this:

1. Non-thread-safe drivers must not call the core.
2. Provide a new public API that drivers can safely call.
3. Make the dispatch layer public for drivers to call.
4. There is a whitelist of core APIs that drivers can call. Drivers providing entry points to these must not make a call to the core when handling these calls. (Drivers are still allowed to call any core API that can't have a driver entry point.)

The first is too restrictive, the second and the third would require making it a stable API, and would likely increase the code size for a relatively rare feature. We are choosing the fourth as that is the most viable option.

**Thread-safe drivers:**

A driver would be non-thread-safe if the `thread-safe` property is set to true.

To make re-entrancy in non-thread-safe drivers work, thread-safe drivers must not make a call to the core when handling a call that is on the non-thread-safe driver core API whitelist.

Thread-safe drivers have fewer guarantees from the core and need to implement more complex logic. We can reasonably expect them to be more flexible in terms of re-entrancy as well. At this point it is hard to see what further guarantees would be useful and feasible. Therefore, we don't provide any further guarantees for now.

Thread-safe drivers must not make any assumption about the operation of the core beyond what is discussed here.
