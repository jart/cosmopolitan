This directory contains example configuration files.

The examples are generally focused on a particular use case (eg, support for
a restricted set of ciphersuites) and aim to minimize resource usage for
the target. They can be used as a basis for custom configurations.

These files come in pairs and are complete replacements for the default
mbedtls_config.h and crypto_config.h. The two files of a pair share the same or
very similar name, with the crypto file prefixed by "crypto-". Note
that some of the cryptography configuration files may be located in
tf-psa-crypto/configs.

To use one of these pairs, you can pick one of the following methods:

1. Replace the default files include/mbedtls/mbedtls_config.h and
   tf-psa-crypto/include/psa/crypto_config.h with the chosen ones.

2. Use the MBEDTLS_CONFIG_FILE and TF_PSA_CRYPTO_CONFIG_FILE CMake options. For
   example, to build out-of-tree with the config-ccm-psk-tls1_2.h and
   crypto-config-ccm-psk-tls1_2.h configuration pair:

   cmake -DMBEDTLS_CONFIG_FILE="configs/config-ccm-psk-tls1_2.h" \
         -DTF_PSA_CRYPTO_CONFIG_FILE="configs/crypto-config-ccm-psk-tls1_2.h"
         -B build-psktls12 .
   cmake --build build-psktls12

The second method also works if you want to keep your custom configuration
files outside the Mbed TLS tree.
