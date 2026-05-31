TF-PSA-Crypto sample programs
=============================

This subdirectory mostly contains sample programs that illustrate specific features of the library, as well as a few test and support programs.

We try to ensure that the sample programs are good examples of how to use Mbed TLS but we make no hard guarantees about their security. They should not be used in production unless they have been separately tested and thoroughly audited for security. Note that this means vulnerabilities in the sample programs are out of the scope of our usual security process and will be fixed in public.

## PSA cryptography API examples

* [`psa/aead_demo.c`](psa/aead_demo.c): demonstrates the AEAD multi-part PSA Cryptography API.

* [`psa/crypto_examples.c`](psa/crypto_examples.c): demonstrates the unauthenticated cipher PSA Cryptography API.

* [`psa/hmac_demo.c`](psa/hmac_demo.c): demonstrates HMAC operations using the MAC multi-part PSA Cryptography API.

* [`psa/key_ladder_demo.c`](psa/key_ladder_demo.c): demonstrates the key derivation PSA Cryptography API.

* [`psa/psa_hash.c`](psa/psa_hash.c): demonstrates the hash PSA Cryptography API.

## Test utilities

* [`test/benchmark.c`](test/benchmark.c): benchmark for cryptographic algorithms.

* [`test/which_aes.c`](test/which_aes.c): a program that prints the current AES implementation.
