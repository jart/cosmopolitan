Summary
-------

The file:

* crypto_config_profile_medium.h

is copyright The Mbed TLS Contributors, and is distributed under the license normally
used by Mbed TLS: a dual Apache 2.0 or GPLv2-or-later license.

Background
----------

The file crypto_config_profile_medium.h was derived from the two files crypto_config_profile_medium.h and tfm_mbedcrypto_config_profile_medium.h taken from the TF-M source code here:

https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/tree/lib/ext/mbedcrypto/mbedcrypto_config

Initially, it was derived according to the Mbed TLS configuration file split that occurred as part of the Mbed TLS repository split, see https://github.com/Mbed-TLS/mbedtls/blob/development/docs/proposed/config-split.md. It then evolved further during the rework of the cryptographic options for version 1.0 of the library, see https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/docs/architecture/0e-plans.md.

In TF-M, the two original files are distributed under a 3-Clause BSD license, as noted at the top of the files.

In Mbed TLS, with permission from the TF-M project,  crypto_config_profile_medium.h is distributed under a dual [Apache-2.0](https://spdx.org/licenses/Apache-2.0.html) OR [GPL-2.0-or-later](https://spdx.org/licenses/GPL-2.0-or-later.html) license, with copyright assigned to The Mbed TLS Contributors.
