Version-independent build and test framework
============================================

## Introduction

The [`mbedtls-framework`](https://github.com/Mbed-TLS/mbedtls-framework) repository provides tooling used to build and test TF-PSA-Crypto (all versions) and Mbed TLS from 3.6.0 onwards.

## Requirements

### Initial motivation

Mbed TLS 3.x was a library for cryptography, X.509 and TLS. In 2024, the cryptography part of the library moved to a separate repository: [TF-PSA-Crypto](https://github.com/Mbed-TLS/TF-PSA-Crypto). Many support files are used by both projects: several helper scripts in `scripts` and `tests/scripts`, most of the test data in `tests/data`, a few helper programs under `programs`, most of the test helper code in `tests/include`, `tests/src` and `tests/drivers`, etc.

The Mbed TLS project maintains long-time support (LTS) branches (with only bug fixes) in addition to the `development` branch where new features are added. Fixes to bugs often need to be backported from `development` to LTS branches, which involves backporting tests, which often involves backporting test helper code. If we had a place to put files shared among multiple maintained branches, that would reduce the amount of backporting.

The `mbedtls-framework` was created to be a shared place for files that need to be shared by two or more of Mbed TLS 3.6 LTS, Mbed TLS 4.x development and TF-PSA-Crypto (as well as other LTS branches that will be created in the future). Mbed TLS 2.28 LTS was excluded from consideration due to its short remaining lifetime which would make any benefits small.

### Usage of the repository

The [`mbedtls-framework`](https://github.com/Mbed-TLS/mbedtls-framework) repository is consumed by each maintained branch of [Mbed TLS](https://github.com/Mbed-TLS/mbedtls) and [TF-PSA-Crypto](https://github.com/Mbed-TLS/TF-PSA-Crypto). This includes development branches, release branches and long-time support branches. (Exception: the older branch Mbed TLS 2.28 LTS was excluded.)

In each consuming branch, the `mbedtls-framework` repository appears as a Git submodule located at the path `/framework`.

### Requirements for the framework repository

#### Framework repository versioning

The framework repository is not versioned: projects are only supposed to consume the tip of the `main` branch. There are no tagged releases. However, each release of a consuming branch will designate a specific commit of the framework repository (this behavior is built into Git submodules), which can be tagged accordingly.

At any point in time, each consuming branch requires a specific commit in the framework repository. Moving a consuming branch to the tip of the framework repository is a manual action. As a consequence, breaking changes are possible: they will not break any actual commit, they would only prevent a consuming branch from updating its submodule version to the tip of the framework repository.

However, breaking changes are still problematic. Breaking changes in the framework repository require the affected consuming branches to fully adapt to the changes when they want to gain access to any new features in the framework repository. Breaking changes in a consuming branch that concern a feature that is consumed by the framework repository (e.g. internal library functions called by test helper functions) have the same effect.

To facilitate parallel development, major changes should avoid breaking existing code and should provide a transition period. For example, if a function needs a new argument, define a new function with a new name, start using the new fuction, and later remove the old function.

### Requirements for consuming repositories

We generalize some current principles:

* For development work and to run the CI, you need a Git checkout.
* To build the project and run functional tests, you need a complete set of files, but you don't need a Git checkout.
* To just build the library, if the platform-independent generated files are present, you only need the `include` directory and the directories containing library C files (`library`, `3rdparty`, `core`, `drivers` depending on the repository and desired features).

#### Requirements for development in consuming branches

Consuming branches must have the framework repository as a Git submodule for development work and CI scripts.

Compared with pre-framework tooling, this means that Git submodules must be enabled. This requires an explicit step in many Git APIs (e.g. running `git submodule update --init` after `git init`, or passing `--recurse-submodules` to `git checkout`).

#### Requirements for processes involving consuming branches

Release archives must include the content of the framework repository.

#### Requirements for tooling in consuming branches

Consuming branches may assume that the `framework` submodule is present wherever they assume a Git checkout.

Consuming branches may assume that the content of the `framework` directory is present anywhere where they would normally assume that all files are present. In particular, this allows the use of framework files for:

* Generating configuration-independent files (e.g. `make generated_files`), including the ones in the `library` directory.
* `make lib` (with GNU make or CMake) from a pristine checkout (because this involves `make generated_files`).
* `make test` (even if all the tests have been built).

Consuming branches must not assume that the framework is present when merely building the library. In particular:

* Our provided build scripts (e.g. `library/Makefile`, `library/CMakeLists.txt`) must not require any files from `framework` when compiling the library.
* It's ok to have a file in `library` with a make dependency on a framework file, as long as the build works when the framework file is missing. This allows `make lib` to work as long as the generated files are present.
* Library source files must not rely on headers from the framework.

#### Requirements for users of consuming branches

Corresponding to the requirements on the repository above:

* Contributors need the framework submodule.
* Users who wish to run full CI tests need the framework submodule.
* Users who want to build or run tests need the `framework` directory content.
* Users who merely want to build the library, and who have the configuration-independent files already generated, do not need the `framework` directory content.

## Contents of the framework repository

### Criteria for inclusion

In general, a file should be in the framework repository if it is expected to be present with near-identical content in two or more consuming branches. Some files have a significant proportion of shared content and branch-specific content; such files should be split into a shared part and a non-shared part.

For example:

* `test_suite_*.function` contains a lot of code that is specific to each consuming branch. Even when the same test function exists in all maintained branches, there are often minor differences such as a deprecated alternative that only exists in older branches, differences in compile-time dependencies, etc. Thus we do not expect to share these files. Common code can go into separate `.c` files, historically under `tests/src`, that are in the framework repository.
* `test_suite_*.data` contains many test cases that exist in all maintained branches. However the exact expression of these test cases are often different, for example due to compile-time dependencies. Furthermore the set of test cases is often different, for example due to cryptographic mechanisms that are added or removed. Thus we do not expect to share these files. Where there is a lot of commonality, the test cases can be generated from a script located in the framework repository, with code in the generation script to handle the differing parts.

## CI architecture

* CI in consuming repositories must support Git submodules. Other than that, keep the CI as it is now. In particular, the CI in consuming repositories does not need to consider anything but the commit that the framework submodule points to.
* CI in the framework repository should run a subset of the CI of all consuming branches, to warn about unintended breakage. This way, most of the time, updating the framework submodule in a consuming branch to the tip of the `main` branch should work. Gatekeepers can bypass this check if the incompatibility is deliberate.
* When merging a pull request to an official branch in a consuming repository (`development`, LTS branches), check that the framework submodule's commit is on the main branch of the `mbedtls-framework` submodule.

TODO: once this is set up, detail the processes here.

## How to make a change

### Change in a consuming branch requiring a new framework feature

If a change in a consuming branch requires a new feature in the framework, you need to make both a pull request in the framework repository and a pull request in the framework repository.

1. Make a pull request (PR) in the framework repository.
2. Upload the framework branch to the framework repository itself (not a fork). This is necessary for the commit to be available on the CI of the consuming repositories (and also for it to be conveniently available to reviewers).
   Open question: can we make the CI work with a fork, and make using forks convenient enough for reviewers, so that people don't need to upload the branch to the main repository?
3. Make a pull request to the consuming branch. Include a commit that advances the submodule to the tip of the branch in the framework repository.
4. If there is rework in the framework PR that is needed for the consuming PR's review or CI, update the framework branch in the framework repository.
5. After the framework PR is merged, update the consuming PR to update the framework submodule to the merge commit (or a later commit).

### Backward-incompatible change in the framework repository

This section discusses cases where a change in the framework repository breaks one or more consuming branches. This includes cases where the change starts in a consuming branch, for example if some test helper code in the framework repository calls an internal library function which is removed or has its API changed.

#### Split approach for backward-incompatible framework changes

If a change in the framework repository breaks a consuming branch, it should ideally be split into two parts: one that adds the new feature, and one that removes the old feature. The new feature may be gated by a compilation directive if it's convenient to have only one of the versions at compile time.

1. Make and merge a pull request in the framework repository with a backward-compatible change.
2. Update all affected consuming branches to:
    1. update the framework submodule to the new version;
    2. migrate all uses of the old feature to the new feature.
3. Make and merge a pull request in the framework repository that removes the old version of the feature.

#### Watershed approach for backward-incompatible framework changes

If a change in the framework repository breaks a consuming branch, it is possible to make it in a single step in the framework repository. However, this makes it mandatory to reflect this change in consuming branches the next time their framework submodule is updated. Therefore this should only be done if the change can be reflected quickly and there are no other urgent pending framework-submodule updates.

1. Make a pull request (PR) in the framework repository with a backward-incompatible change.
2. For each affected consuming branch, make a PR that updates the framework submodule to the new version and changes the code to work with the updated framework code. Wait for those PR to be approved and passing the CI.
3. Merge the framework PR.
4. Update the PR in the consuming branches and merge them.

## Releases

Release archives for a consuming branch must include the content of the framework repository. (Note that as of Git 2.39, `git archive` does not support submodules, so it is insufficient to generate a release archive.)

The framework repository does not have releases of its own.
