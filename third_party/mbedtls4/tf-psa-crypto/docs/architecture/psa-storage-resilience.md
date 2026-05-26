# PSA storage resilience design

## Introduction

The PSA crypto subsystem includes a persistent key store. It is possible to create a persistent key and read it back later. This must work even if the underlying storage exhibits non-nominal behavior. In this document, _resilience_ means correct behavior of the key store even under if the underlying platform behaves in a non-nominal, but still partially controlled way.

At this point, we are only concerned about one specific form of resilience: to a system crash or power loss. That is, we assume that the underlying platform behaves nominally, except that occasionally it may restart. In the field, this can happen due to a sudden loss of power.

This document explores the problem space, defines a library design and a test design.

## Resilience goals for API functions

**Goal: PSA Crypto API functions are atomic and committing.**

_Atomic_ means that when an application calls an API function, as far as the application is concerned, at any given point in time, the system is either in a state where the function has not started yet, or in a state where the function has returned. The application never needs to worry about an intermediate state.

_Committing_ means that when a function returns, the data has been written to the persistent storage. As a consequence, if the system restarts during a sequence of storage modifications $M_1, M_2, \ldots, M_n$, we know that when the system restarts, a prefix of the sequence has been performed. For example, there will never be a situation where $M_2$ has been performed but not $M_1$.

The committing property is important not only for sequences of operations, but also when reporting the result of an operation to an external system. For example, if a key creation function in the PSA Crypto API reports to the application that a key has been created, and the application reports to a server that the key has been created, it is guaranteed that the key exists even if the system restarts.

## Assumptions on the underlying file storage

PSA relies on a PSA ITS (Internal Trusted Storage) interface, which exposes a simple API. There are two functions to modify files:

* `set()` writes a whole file (either creating it, or replacing the previous content).
* `remove()` removes a file (returning a specific error code if the file does not exist).

**Assumption: the underlying ITS functions are atomic and committing.**

Since the underlying functions are atomic, the content of a file is always a version that was previously passed to `set()`. We do not try to handle the case where a file might be partially written.

## Overview of API functions

For a transparent key, all key management operations (creation or destruction) on persistent keys rely on a single call to the underlying storage (`set()` for a key creation, `remove()` for a key destruction). This also holds for an opaque key stored in a secure element that does not have its own key store: in this case, the core stores a wrapped (i.e. encrypted) copy of the key material, but this does not impact how the core interacts with the storage. Other API functions do not modify the storage.

The following case requires extra work related to resilience:

* [Key management for stateful secure element keys](#designing-key-management-for-secure-element-keys).

As a consequence, apart from the listed cases, the API calls inherit directly from the [resilience properties of the underyling storage](#assumptions-on-the-underlying-file-storage). We do not need to take any special precautions in the library design, and we do not need to perform any testing of resilience for transparent keys.

(This section was last updated for Mbed TLS 3.4.0 implementing PSA Crypto API 1.1.)

## Designing key management for secure element keys

In this section, we use “(stateful) secure element key” to mean a key stored in a stateful secure element, i.e. a secure element that stores keys. This excludes keys in a stateleess secure element for which the core stores a wrapped copy of the key. We study the problem of how key management in stateful secure elements interacts with storage and explore the design space.

### Assumptions on stateful secure elements

**Assumption: driver calls for key management in stateful secure elements are atomic and committing.**

(For stateless secure elements, this assumption is vacuously true.)

### Dual management of keys: the problem

For a secure element key, key management requires a commitment on both sites. For example, consider a successful key creation operation:

1. The core sends a request to the secure element to create a key.
2. The secure element modifies its key store to create the key.
3. The secure element reports to the core that the key has been created.
4. The core reports to the application that the key has been created.

If the core loses power between steps 1 and 2, the key does not exist yet. This is fine from an application's perspective since the core has not committed to the key's existence, but the core needs to take care not to leave resources in storage that are related to the non-existent key. If the core loses power between steps 2 and 3, the key exists in the secure element. From an application's perspective, the core may either report that the key exists or that it does not exist, but in the latter case, the core needs to free the key in the secure element, to avoid leaving behind inaccessible resources.

As a consequence, the content of the storage cannot remain the same between the end of step 1 and the end of step 3, since the core must behave differently depending on whether step 2 has taken place.

Accomplishing a transaction across system boundaries is a well-known problem in database management, with a well-known solution: two-phase commit.

### Overview of two-phase commit with stateful secure elements

With a key in a stateful secure element, a successful creation process goes as follows (see [“Key management in a secure element with storage” in the driver interface specification](../../proposed/psa-driver-interface.html#key-management-in-a-secure-element-with-storage)):

1. The core calls the driver's `"allocate_key"` entry point.
2. The driver allocates a unique identifier _D_ for the key. This is unrelated to the key identifier _A_ used by the application interface. This step must not modify the state of the secure element.
3. The core updates the storage to indicate that key identifier _A_ has the identifier _D_ in the driver, and that _A_ is in a half-created state.
4. The core calls the driver's key creation entry point, passing it the driver's chosen identifier _D_.
5. The driver creates the key in the secure element. When this happens, it concludes the voting phase of the two-phase commit: effectively, the secure element decides to commit. (It is however possible to revert this commitment by giving the secure element the order to destroy the key.)
6. The core updates the storage to indicate that _A_ is now in a fully created state. This concludes the commit phase of the two-phase commit.

If there is a loss of power:

* Before step 3: the system state has not changed at all. As far as the world is concerned, the key creation attempt never happened.
* Between step 3 and step 6: upon restart, the core needs to find out whether the secure element completed step 5 or not, and reconcile the state of the storage with the state of the secure element.
* After step 6: the key has been created successfully.

Key destruction goes as follows:

1. The core updates the storage indicating that the key is being destroyed.
2. The core calls the driver's `"destroy_key"` entry point.
3. The secure element destroys the key.
4. The core updates the storage to indicate that the key has been destroyed.

If there is a loss of power:

* Before step 1: the system state has not changed at all. As far as the world is concerned, the key destruction attempt never happened.
* Between step 1 and step 4: upon restart, the core needs to find out whether the secure element completed step 3 or not, and reconcile the state of the storage with the state of the secure element.
* After step 4: the key has been destroyed successfully.

In both cases, upon restart, the core needs to perform a transaction recovery. When a power loss happens, the core decides whether to commit or abort the transaction.

Note that the analysis in this section assumes that the driver does not update its persistent state during a key management operation (or at least not in a way that is influences the key management process — for example, it might renew an authorization token).

### Optimization considerations for transactions

We assume that power failures are rare. Therefore we will primarily optimize for the normal case. Transaction recovery needs to be practical, but does not have to be fully optimized.

The main quantity we will optimize for is the number of storage updates in the nominal case. This is good for performance because storage writes are likely to dominate the runtime in some hardware configurations where storage writes are slow and communication with the secure element is fast, for key management operations that require a small amount of computation. In addition, minimizing the number of storage updates is good for the longevity of flash media.

#### Information available during recovery

The PSA ITS API does not support enumerating files in storage: an ITS call can only access one file identifier. Therefore transaction recovery cannot be done by traversing files whose name is or encodes the key identifier. It must start by traversing a small number of files whose names are independent of the key identifiers involved.

#### Minimum effort for a transaction

Per the [assumptions on the underlying file storage](#assumptions-on-the-underlying-file-storage), each atomic operation in the internal storage concerns a single file: either removing it, or setting its content. Furthermore there is no way to enumerate the files in storage.

A key creation function must transform the internal storage from a state where file `id` does not exist, to a state where file `id` exists and has its desired final content (containing the key attributes and the driver's key identifier). The situation is similar with key destruction, except that the initial and final states are exchanged. Neither the initial state nor the final state reference `id` otherwise.

For a key that is not in a stateful element, the transaction consists of a single write operation. As discussed previously, this is not possible with a stateful secure element because the state of the internal storage needs to change both before and after the state change in the secure element. No other single-write algorithm works.

If there is a power failure around the time of changing the state of the secure element, there must be information in the internal storage that indicates that key `id` has a transaction in progress. The file `id` cannot be used for this purpose because there is no way to enumerate all keys (and even if there was, it would not be practical). Therefore the transaction will need to modify some other file `t` with a fixed name (a name that doesn't depend on the key). Since the final system state should be identical to the initial state except for the file `id`, the minimum number of storage operations for a transaction is 3:

* Write (create or update) a file `t` referencing `id`.
* Write the final state of `id`.
* Restore `t` to its initial state.

The strategies discussed in the [overview above](#overview-of-two-phase-commit-with-stateful-secure-elements) follow this pattern, with `t` being the file containing the transaction list that the recovery consults. We have just proved that this pattern is optimal.

Note that this pattern requires the state of `id` to be modified only once. In particular, if a key management involves writing an intermediate state for `id` before modifying the secure element state and writing a different state after that, this will require a total of 4 updates to internal storage. Since we want to minimize the number of storage updates, we will not explore designs that involved updating `id` twice or more.

### Recovery strategies

When the core starts, it needs to know about transaction(s) that need to be resumed. This information will be stored in a persistent “transaction list”, with one entry per key. In this section, we explore recovery strategies, and we determine what the transaction list needs to contain as well as when it needs to be updated. Other sections will explore the format of the transaction list, as well as how many keys it needs to contain.

#### Exploring the recovery decision tree

There are four cases for recovery when a transaction is in progress. In each case, the core can either decide to commit the transaction (which may require replaying the interrupted part) or abort it (which may require a rewind in the secure element). It may call the secure element driver's `"get_key_attributes"` entry point to find out whether the key is present.

* Key creation, key not present in the secure element:
    * Committing means replaying the driver call in the key creation. This requires all the input, for example the data to import. This seems impractical in general. Also, the second driver call require a new call to `"allocate_key"` which will in general changing the key's driver identifier, which complicates state management in the core. Given the likely complexity, we exclude this strategy.
    * Aborting means removing any trace of the key creation.
* Key creation, key present in the secure element:
    * Committing means finishing the update of the core's persistent state, as would have been done if the transaction had not been interrupted.
    * Aborting means destroying the key in the secure element and removing any local storage used for that key.
* Key destruction, key not present in the secure element:
    * Committing means finishing the update of the core's persistent state, as would have been done if the transaction had not been interrupted, by removing any remaining local storage used for that key.
    * Aborting would mean re-creating the key in the secure element, which is impossible in general since the key material is no longer present.
* Key destruction, key present in the secure element:
    * Committing means finishing the update of the core's persistent state, as would have been done if the transaction had not been interrupted, by removing any remaining local storage used for that key and destroying the key in the secure element.
    * Aborting means keeping the key. This requires no action on the secure element, and is only practical locally if the local storage is intact.

#### Comparing recovery strategies

From the analysis above, assuming that all keys are treated in the same way, there are 4 possible strategies.

* [Always follow the state of the secure element](#exploring-the-follow-the-secure-element-strategy). This requires the secure element driver to have a `"get_key_attributes"` entry point. Recovery means resuming the operation where it left off. For key creation, this means that the key metadata needs to be saved before calling the secure element's key creation entry point.
* Minimize the information processing: [always destroy the key](#exploring-the-always-destroy-strategy), i.e. abort all key creations and commit all key destructions. This does not require querying the state of the secure element. This does not require any special precautions to preserve information about the key during the transaction. It simplifies recovery in that the recovery process might not even need to know whether it's recovering a key creation or a key destruction.
* Follow the state of the secure element for key creation, but always go ahead with key destruction. This requires the secure element driver to have a `"get_key_attributes"` entry point. Compared to always following the state of the secure element, this has the advantage of maximizing the chance that a command to destroy key material is effective. Compared to always destroying the key, this has a performance advantage if a key creation is interrupted. These do not seem like decisive advantages, so we will not consider this strategy further.
* Always abort key creation, but follow the state of the secure element for key destruction. I can't think of a good reason to choose this strategy.

Requiring the driver to have a `"get_key_attributes"` entry point is potentially problematic because some secure elements don't have room to store key attributes: a key slot always exists, and it's up to the user to remember what, if anything, they put in it. The driver has to remember anyway, so that it can find a free slot when creating a key. But with a recovery strategy that doesn't involve a `"get_key_attributes"` entry point, the driver design is easier: the driver doesn't need to protect the information about slots in use against a power failure, the core takes care of that.

#### Exploring the follow-the-secure-element strategy

Each entry in the transaction list contains the API key identifier, the key lifetime (or at least the location), the driver key identifier (not constant-size), and an indication of whether the key is being created or destroyed.

For key creation, we have all the information to store in the key file once the `"allocate_key"` call returns. We must store all the information that will go in the key file before calling the driver's key creation entry point. Therefore the normal sequence of operations is:

1. Call the driver's `"allocate_key"` entry point.
2. Add the key to the transaction list, indicating that it is being created.
3. Write the key file.
4. Call the driver's key creation entry point.
5. Remove the key from the transaction list.

During recovery, for each key in the transaction list that was being created:

* If the key exists in the secure element, just remove it from the transaction list.
* If the key does not exist in the secure element, first remove the key file if it is present, then remove the key from the transaction list.

For key destruction, we need to preserve the key file until after the key has been destroyed. Therefore the normal sequence of operations is:

1. Add the key to the transaction list, indicating that it is being destroyed.
2. Call the driver's `"destroy_key"` entry point.
3. Remove the key file.
4. Remove the key from the transaction list.

During recovery, for each key in the transaction list that was being created:

* If the key exists in the secure element, call the driver's `"destroy_key"` entry point, then remove the key file, and finally remote the key from the transaction lits.
* If the key does not exist in the secure element, remove the key file if it is still present, then remove the key from the transaction list.

#### Exploring the always-destroy strategy

Each entry in the transaction list contains the API key identifier, the key lifetime (or at least the location), and the driver key identifier (not constant-size).

For key creation, we do not need to store the key's metadata until it has been created in the secure element. Therefore the normal sequence of operations is:

1. Call the driver's `"allocate_key"` entry point.
2. Add the key to the transaction list.
3. Call the driver's key creation entry point.
4. Write the key file.
5. Remove the key from the transaction list.

For key destruction, we can remove the key file before contacting the secure element. Therefore the normal sequence of operations is:

1. Add the key to the transaction list.
2. Remove the key file.
3. Call the driver's `"destroy_key"` entry point.
4. Remove the key from the transaction list.

Recovery means removing all traces of all keys on the transaction list. This means following the destruction process, starting after the point where the key has been added to the transaction list, and ignoring any failure of a removal action if the item to remove does not exist:

1. Remove the key file, treating `DOES_NOT_EXIST` as a success.
2. Call the driver's `"destroy_key"` entry point, treating `DOES_NOT_EXIST` as a success.
3. Remove the key from the transaction list.

#### Always-destroy strategy with a simpler transaction file

We can modify the [always-destroy strategy](#exploring-the-always-destroy-strategy) to make the transaction file simpler: if we ensure that the key file always exists if the key exists in the secure element, then the transaction list does not need to include the driver key identifier: it can be read from the key file.

For key creation, we need to store the key's metadata before creating in the secure element. Therefore the normal sequence of operations is:

1. Call the driver's `"allocate_key"` entry point.
2. Add the key to the transaction list.
3. Write the key file.
4. Call the driver's key creation entry point.
5. Remove the key from the transaction list.

For key destruction, we need to contact the secure element before removing the key file. Therefore the normal sequence of operations is:

1. Add the key to the transaction list.
2. Call the driver's `"destroy_key"` entry point.
3. Remove the key file.
4. Remove the key from the transaction list.

Recovery means removing all traces of all keys on the transaction list. This means following the destruction process, starting after the point where the key has been added to the transaction list, and ignoring any failure of a removal action if the item to remove does not exist:

1. Load the driver key identifier from the key file. If the key file does not exist, skip to step 4.
2. Call the driver's `"destroy_key"` entry point, treating `DOES_NOT_EXIST` as a success.
3. Remove the key file, treating `DOES_NOT_EXIST` as a success.
4. Remove the key from the transaction list.

Compared with the basic always-destroy strategy:

* The transaction file handling is simpler since its entries have a fixed size.
* The flow of information is somewhat different from transparent keys and keys in stateless secure elements: we aren't just replacing “create the key material” by “tell the secure element to create the key material”, those happen at different times. But there's a different flow for stateful secure elements anyway, since the call to `"allocate_key"` has no analog in the stateless secure element or transparent cases.

#### Assisting secure element drivers with recovery

The actions of the secure element driver may themselves be non-atomic. So the driver must be given a chance to perform recovery.

To simplify the design of the driver, the core should guarantee that the driver will know if a transaction was in progress and the core cannot be sure about the state of the secure element. Merely calling a read-only entry point such as `"get_key_attributes"` does not provide enough information to the driver for it to know that it should actively perform recovery related to that key.

This gives an advantage to the “always destroy” strategy. Under this strategy, if the key might be in a transitional state, the core will request a key destruction from the driver. This means that, if the driver has per-key auxiliary data to clean up, it can bundle that as part of the key's destruction.

### Testing non-atomic processes

In this section, we discuss how to test non-atomic processes that must implement an atomic and committing interface. As discussed in [“Overview of API functions”](#overview-of-api-functions), this concerns key management in stateful secure elements.

#### Naive test strategy for non-atomic processes

Non-atomic processes consist of a series of atomic, committing steps.

Our general strategy to test them is as follows: every time there is a modification of persistent state, either in storage or in the (simulated) secure element, try both the nominal case and simulating a power loss. If a power loss occurs, restart the system (i.e. clean up and call `psa_crypto_init()`), and check that the system ends up in a consistent state.

Note that this creates a binary tree of possibilities: after each state modification, there may or may not be a restart, and after that different state modifications may occur, each of which may or may not be followed by a restart.

For example, consider testing of one key creation operation (see [“Overview of two-phase commit with stateful secure elements”](#overview-of-two-phase-commit-with-stateful-secure-elements), under the simplifying assumption that each storage update step, as well as the recovery after a restart, each make a single (atomic) storage modification and no secure element access. The nominal case consists of three state modifications: storage modification (start transaction), creation on the secure element, storage modification (commit transaction). We need to test the following sequences:

* Start transaction, restart, recovery.
* Start transaction, secure element operation, restart, recovery.
* Start transaction, secure element operation, commit transaction.

If, for example, recovery consists of two atomic steps, the tree of possibilities expands and may be infinite:

* Start transaction, restart, recovery step 1, restart, recovery step 1, recovery step 2.
* Start transaction, restart, recovery step 1, restart, recovery step 1, restart, recovery step 1, recovery step 2.
* Start transaction, restart, recovery step 1, restart, recovery step 1, restart, recovery step 1, restart, recovery step 1, recovery step 2.
* etc.
* Start transaction, secure element operation, restart, ...
* Start transaction, secure element operation, commit transaction.

In order to limit the possibilities, we need to make some assumptions about the recovery step. For example, if we have confidence that recovery step 1 is idempotent (i.e. doing it twice is the same as doing it once), we don't need to test what happens in execution sequences that take recovery step 1 more than twice in a row.

### Splitting normal behavior and transaction recovery

We introduce an abstraction level in transaction recovery:

* Normal operation must maintain a certain invariant on the state of the world (internal storage and secure element).
* Transaction recovery is defined over all states of the world that satisfy this invariant.

This separation of concerns greatly facilitates testing, since it is now split into two parts:

* During the testing of normal operation, we can use read-only invasive testing to ensure that the invariant is maintained. No modification of normal behavior (such as simulated power failures) is necessary.
* Testing of transaction recovery is independent of how the system state was reached. We only need to artificially construct a representative sample of system states that match the invariant. Transaction recovery is itself an operation that must respect the invariant, and so we do not need any special testing for the case of an interrupted recovery.

Another benefit of this approach is that it is easier to specify and test what happens if the library is updated on a device with leftovers from an interrupted transaction. We will require and test that the new version of the library supports recovery of the old library's states, without worrying how those states were reached.

#### Towards an invariant for transactions

As discussed in the section [“Recovery strategies”](#recovery-strategies), the information about active transactions is stored in a transaction list file. The name of the transaction list file does not depend on the identifiers of the keys in the list, but there may be more than one transaction list, for example one per secure element. If so, each transaction list can be considered independently.

When no transaction is in progress, the transaction list does not exist, or is empty. The empty case must be supported because this is the initial state of the filesystem. When no transaction is in progress, the state of the secure element must be consistent with references to keys in that secure element contained in key files. More generally, if a key is not in the transaction list, then the key must be present in the secure element if and only if the key file is in the internal storage.

For the purposes of the state invariant, it matters whether the transaction list file contains the driver key identifier, or if the driver key identifier is only stored in the key file. This is because the core needs to know the driver key id in order to access the secure element. If the transaction list does not contain the driver key identifier, and the key file does not exist, the key must not be present in the secure element.

We thus have two scenarios, each with their own invariant: one where the transaction list contains only key identifiers, and one where it also contains the secure element's key identifier (as well as the location of the secure element if this is not encoded in the name of the transaction list file).

#### Storage invariant if the transaction list contains application key identifiers only

Invariants:

* If the file `id` does not exist, then no resources corresponding to that key are in a secure element. This holds whether `id` is in the transaction list or not.
* If `id` is not in the transaction list and the file `id` exists and references a key in a stateful secure element, then the key is present in the secure element.

If `id` is in the transaction list and the file `id` exists, the key may or may not be present in the secure element.

The invariant imposes constraints on the [order of operations for the two-phase commit](#overview-of-two-phase-commit-with-stateful-secure-elements): key creation must create `id` before calling the secure element's key creation entry point, and key destruction must remove `id` after calling the secure element's key destruction entry point.

For recovery:

* If the file `id` does not exist, then nothing needs to be done for recovery, other than removing `id` from the transaction list.
* If the file `id` exists:
    * It is correct to destroy the key in the secure element (treating a `DOES_NOT_EXIST` error as a success), then remove `id`.
    * It is correct to check whether the key exists in the secure element, and if it does, keep it and keep `id`. If not, remove `id` from the internal storage.

#### Storage invariant if the transaction list contains driver key identifiers

Invariants:

* If `id` is not in the transaction list and the file `id` does not exist, then no resources corresponding to that key are in a secure element.
* If `id` is not in the transaction list and the file `id` exists, then the key is present in the secure element.

If `id` is in the transaction list, neither the state of `id` in the internal storage nor the state of the key in the secure element is known.

For recovery:

* If the file `id` does not exist, then destroy the key in the secure element (treating a `DOES_NOT_EXIST` error as a success).
* If the file `id` exists:
    * It is correct to destroy the key in the secure element (treating a `DOES_NOT_EXIST` error as a success), then remove `id`.
    * It is correct to check whether the key exists in the secure element, and if it does, keep it and keep `id`. If not, remove `id` from the internal storage.

#### Coverage of states that respect the invariant

For a given key, we have to consider three a priori independent boolean states:

* Whether the key file exists.
* Whether the key is in the secure element.
* Whether the key is in the transaction list.

There is full coverage for one key if we have tests of recovery for the states among these $2^3 = 8$ possibilities that satisfy the storage invariant.

In addition, testing should adequately cover the case of multiple keys in the transaction list. How much coverage is adequate depends on the layout of the list as well as white-box considerations of how the list is manipulated.

### Choice of a transaction design

#### Chosen transaction algorithm

Based on [“Optimization considerations for transactions”](#optimization-considerations-for-transactions), we choose a transaction algorithm that consists in the following operations:

1. Add the key identifier to the transaction list.
2. Call the secure element's key creation or destruction entry point.
3. Remove the key identifier from the transaction list.

In addition, before or after step 2, create or remove the key file in the internal storage.

In order to conveniently support multiple transactions at the same time, we pick the simplest possible layout for the transaction list: a simple array of key identifiers. Since the transaction list does not contain the driver key identifier:

* During key creation, create the key file in internal storage in the internal storage before calling the secure element's key creation entry point.
* During key destruction, call the secure element's key destruction entry point before removing the key file in internal storage.

This choice of algorithm does not require the secure element driver to have a `"get_key_attributes"` entry point.

#### Chosen storage invariant

The [storage invariant](#storage-invariant-if-the-transaction-list-contains-application-key-identifiers-only) is as follows:

* If the file `id` does not exist, then no resources corresponding to that key are in a secure element. This holds whether `id` is in the transaction list or not.
* If `id` is not in the transaction list and the file `id` exists and references a key in a stateful secure element, then the key is present in the secure element.
* If `id` is in the transaction list and a key exists by that identifier, the key's location is a stateful secure element.

#### Chosen recovery process

To [assist secure element drivers with recovery](#assisting-secure-element-drivers-with-recovery), we pick the [always-destroy recovery strategy with a simple transaction file](#always-destroy-strategy-with-a-simpler-transaction-file). The the recovery process is as follows:

* If the file `id` does not exist, then nothing needs to be done for recovery, other than removing `id` from the transaction list.
* If the file `id` exists, call the secure element's key destruction entry point (treating a `DOES_NOT_EXIST` error as a success), then remove `id`.

## Specification of key management in stateful secure elements

This section only concerns stateful secure elements as discussed in [“Designing key management for secure element keys”](#designing-key-management-for-secure-element-keys), i.e. secure elements with an `"allocate_key"` entry point. The design follows the general principle described in [“Overview of two-phase commit with stateful secure elements”](#overview-of-two-phase-commit-with-stateful-secure-elements) and the specific choices justified in [“Choice of a transaction design”](choice-of-a-transaction-design).

### Transaction list file manipulation

The transaction list is a simple array of key identifiers.

To add a key identifier to the list:

1. Load the current list from the transaction list if it exists and it is not already cached in memory.
2. Append the key identifier to the array.
3. Write the updated list file.

To remove a key identifier from the list:

1. Load the current list if it is not already cached in memory. It is an error if the file does not exist since it must contain this identifier.
2. Remove the key identifier from the array. If it wasn't the last element in array, move array elements to fill the hole.
3. If the list is now empty, remove the transaction list file. Otherwise write the updated list to the file.

### Key creation process in the core

Let _A_ be the application key identifier.

1. Call the driver's `"allocate_key"` entry point, obtaining the driver key identifier _D_ chosen by the driver.
2. Add _A_ [to the transaction list file](#transaction-list-file-manipulation).
3. Create the key file _A_ in the internal storage. Note that this is done at a different time from what happens when creating a transparent key or a key in a stateless secure element: in those cases, creating the key file happens after the actual creation of the key material.
4. Call the secure element's key creation entry point.
5. Remove _A_ [from the transaction list file](#transaction-list-file-manipulation).

If any step fails:

* If the secure element's key creation entry point has been called and succeeded, call the secure element's destroy entry point.
* If the key file has been created in the internal storage, remove it.
* Remove the key from the transaction list.

Note that this process is identical to key destruction, except that the key is already in the transaction list.

### Key destruction process in the core

Let _A_ be the application key identifier.

We assume that the key is loaded in a key slot in memory: the core needs to know the key's location in order to determine whether the key is in a stateful secure element, and if so to know the driver key identifier. A possible optimization would be to load only that information in local variables, without occupying a key store; this has the advantage that key destruction works even if the key store is full.

1. Add _A_ [to the transaction list file](#transaction-list-file-manipulation).
2. Call the secure element's `"destroy_key"` entry point.
3. Remove the key file _A_ from the internal storage.
4. Remove _A_ [from the transaction list file](#transaction-list-file-manipulation).
5. Free the corresponding key slot in memory.

If any step fails, remember the error but continue the process, to destroy the resources associated with the key as much as is practical.

### Transaction recovery

For each key _A_ in the transaction list file, if the file _A_ exists in the internal storage:

1. Load the key into a key slot in memory (to get its location and the driver key identifier, although we could get the location from the transaction list).
2. Call the secure element's `"destroy_key"` entry point.
3. Remove the key file _A_ from the internal storage.
4. Remove _A_ [from the transaction list file](#transaction-list-file-manipulation).
5. Free the corresponding key slot in memory.

The transaction list file can be processed in any order.

It is correct to update the transaction list after recovering each key, or to only delete the transaction list file once the recovery is over.

### Concrete format of the transaction list file

The transaction list file contains a [fixed header](#transaction-list-header-format) followed by a list of [fixed-size elements](#transaction-list-element-format).

The file uid is `PSA_CRYPTO_ITS_TRANSACTION_LIST_UID` = 0xffffff53.

#### Transaction list header format

* Version (2 bytes): 0x0003. (Chosen to differ from the first two bytes of a [dynamic secure element transaction file](#dynamic-secure-element-transaction-file), to reduce the risk of a mix-up.)
* Key name size (2 bytes): `sizeof(psa_storage_uid_t)`. Storing this size avoids reading bad data if Mbed TLS is upgraded to a different integration that names keys differently.

#### Transaction list element format

In practice, there will rarely be more than one active transaction at a time, so the size of an element is not critical for efficiency. Therefore, in addition to the key identifier which is required, we add some potentially useful information in case it becomes useful later. We do not put the driver key identifier because its size is not a constant.

* Key id: `sizeof(psa_storage_uid_t)` bytes.
* Key lifetime: 4 bytes (`sizeof(psa_key_lifetime_t)`). Currently unused during recovery.
* Operation type: 1 byte. Currently unused during recovery.
    * 0: destroy key.
    * 1: import key.
    * 2: generate key.
    * 3: derive key.
    * 4: import key.
* Padding: 3 bytes. Reserved for future use. Currently unused during recovery.

## Testing key management in secure elements

### Instrumentation for checking the storage invariant

#### Test hook locations

When `MBEDTLS_TEST_HOOKS` is enabled, each call to `psa_its_set()` or `psa_its_remove()` also calls a test hook, passing the file UID as an argument to the hook.

When a stateful secure element driver is present in the build, we use this hook to verify that the storage respects the [storage invariant](#chosen-storage-invariant). In addition, if there is some information about key ongoing operation (set explicitly by the test function as a global variable in the test framework), the hook tests that the content of the storage is compatible with the ongoing operation.

#### Test hook behavior

The storage invariant check cannot check all keys in storage, and does not need to (for example, it would be pointless to check anything about transparent keys). It checks the following keys:

* When invoked from the test hook on a key file: on that key.
* When invoked from the test hook on the transaction file: on all the keys listed in the transaction file.
* When invoked from a test secure element: on the specified key.

#### Test hook extra data

Some tests set global variables to indicate which persistent keys they manipulate. We instrument at least some of these tests to also indicate what operation is in progress on the key. See the GitHub issues or the source code for details.

### Testing of transaction recovery

When no secure element driver is present in the build, the presence of a transaction list file during initialization is an error.

#### Recovery testing process

When the stateful test secure element driver is present in the build, we run test cases on a representative selection of states of the internal storage and the test secure element. Each test case for transaction recovery has the following form:

1. Create the initial state:
    * Create a transaction list file with a certain content.
    * Create key files that we want to have in the test.
    * Call the secure element test driver to create keys without going throught the PSA API.
2. Call `psa_crypto_init()`. Expect success if the initial state satisfies the [storage invariant](#chosen-storage-invariant) and failure otherwise.
3. On success, check that the expected keys exist, and that keys that are expected to have been destroyed by recovery do not exist.
4. Clean up the storage and the secure element test driver's state.

#### States to test recovery on

For a given key located in a secure element, the following combination of states are possible:

* Key file: present, absent.
* Key in secure element: present, absent.
* Key in the transaction file: no, creation (import), destruction.

We test all $2 \times 2 \times 3 = 12$ possibilities, each in its own test case. In each case, call the test function that checks the storage invariant and check that its result is as expected. Then, if the storage invariant is met, follow the [recovery testing process](#recovery-testing-process).

In addition, have at least one positive test case for each creation method other than import, to ensure that we don't reject a valid value.

Note: testing of a damaged filesystem (including a filesystem that doesn't meet the invariant) is out of scope of the present document.
