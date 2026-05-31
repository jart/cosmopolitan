# Maintained branches

At any point in time, we have a number of maintained branches, currently consisting of:

- The [`main`](https://github.com/Mbed-TLS/TF-PSA-Crypto/tree/main) branch:
  this always contains the latest release, including all publicly available
  security fixes.
- The [`development`](https://github.com/Mbed-TLS/TF-PSA-Crypto/tree/development) branch:
  This branch was used to prepare version 1.0.0 of TF-PSA-Crypto and continues to host
  development for future versions.
- [`tf-psa-crypto-1.1`](https://github.com/Mbed-TLS/TF-PSA-Crypto/tree/tf-psa-crypto-1.1)
 maintained until March 2029, see
  <https://github.com/Mbed-TLS/TF-PSA-Crypto/releases/tag/v1.1.0>.

We use [Semantic Versioning](https://semver.org/). In particular, we maintain
API compatibility in the `main` branch across minor version changes (e.g.
the API of 1.(x+1) is backward compatible with 1.x). We only break API
compatibility on major version changes (e.g. from 1.x to 2.0). We also maintain
ABI compatibility within LTS branches; see the next section for details.

We will make regular LTS releases on an 18-month cycle, each of which will have
a 3 year support lifetime. The release date of the first 1.x LTS is yet to be
determined.

Mbed TLS currently maintains the LTS branch [`mbedtls-3.6`](https://github.com/Mbed-TLS/mbedtls/tree/mbedtls-3.6).
This branch contains the implementation of the PSA Cryptography API prior to the
Mbed TLS repository split. Any relevant bug fixes made in the TF-PSA-Crypto
development branch should be backported to this branch to ensure proper
maintenance. This does not apply to Mbed TLS LTS branches created after
the repository split, such as mbedtls-4.1.

## Backwards Compatibility for application code

We maintain API compatibility in released versions of TF-PSA-Crypto. If you
have code that's working and secure with TF-PSA-Crypto x.y.z and does not rely
on undocumented features, then you should be able to re-compile it without
modification with any later release x.y'.z' with the same major version
number, and your code will still build, be secure, and work.

Note that this guarantee only applies if you either use the default
compile-time configuration (`psa/crypto_config.h`) or the same modified
compile-time configuration. Changing compile-time configuration options can
result in an incompatible API or ABI, although features will generally not
affect unrelated features (for example, enabling or disabling a
cryptographic algorithm does not break code that does not use that
algorithm).

Note that new releases of TF-PSA-Crypto may extend the API. Here are some
examples of changes that are common in minor releases of TF-PSA-Crypto, and are
not considered API compatibility breaks:

* Adding or reordering fields in a structure or union.
* Removing a field from a structure, unless the field is documented as public.
* Adding items to an enum.
* Returning an error code that was not previously documented for a function
  when a new error condition arises.
* Changing which error code is returned in a case where multiple error
  conditions apply.
* Changing the behavior of a function from failing to succeeding, when the
  change is a reasonable extension of the current behavior, i.e. the
  addition of a new feature.

There are rare exceptions where we break API compatibility: code that was
relying on something that became insecure in the meantime (for example,
crypto that was found to be weak) may need to be changed. In case security
comes in conflict with backwards compatibility, we will put security first,
but always attempt to provide a compatibility option.

## Backward compatibility for the key store

We maintain backward compatibility with previous versions of the
PSA Crypto persistent storage since Mbed TLS 2.25.0, provided that the
storage backend (PSA ITS implementation) is configured in a compatible way.
We intend to maintain this backward compatibility throughout a major version
of TF-PSA-Crypto (for example, all TF-PSA-Crypto 1.y versions will be able to
read keys written under any TF-PSA-Crypto 1.x with x <= y).

Future major version upgrades (for example from 1.x to 2.y) may require the use
of an upgrade tool.

Note that this guarantee does not currently fully extend to drivers, which
are an experimental feature. We intend to maintain compatibility with the
basic use of drivers from Mbed TLS 2.28.0 onwards, even if driver APIs
change. However, for more experimental parts of the driver interface, such
as the use of driver state, we do not yet guarantee backward compatibility.

## Long-time support branches

For the LTS branches, additionally we try very hard to also maintain ABI
compatibility (same definition as API except with re-linking instead of
re-compiling) and to avoid any increase in code size or RAM usage, or in the
minimum version of tools needed to build the code. The only exception, as
before, is in case those goals would conflict with fixing a security issue, we
will put security first but provide a compatibility option. (So far we never
had to break ABI compatibility in an LTS branch, but we occasionally had to
increase code size for a security fix.)

For contributors, see the [Backwards Compatibility section of
CONTRIBUTING](CONTRIBUTING.md#backwards-compatibility).

## Current Branches

The following branches are currently maintained:

- [main](https://github.com/Mbed-TLS/TF-PSA-Crypto/tree/main)
- [`development`](https://github.com/Mbed-TLS/TF-PSA-Crypto/)
- [`tf-psa-crypto-1.1`](https://github.com/Mbed-TLS/TF-PSA-Crypto/tree/tf-psa-crypto-1.1)
 maintained until March 2029, see
  <https://github.com/Mbed-TLS/TF-PSA-Crypto/releases/tag/v1.1.0>.
