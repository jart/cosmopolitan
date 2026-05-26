/** Code to exercise a PSA key object, i.e. validate that it seems well-formed
 * and can do what it is supposed to do.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "test_common.h"
#include <test/helpers.h>
#include <test/macros.h>
#include <test/psa_exercise_key.h>

#if (MBEDTLS_VERSION_MAJOR < 4 && defined(MBEDTLS_PSA_CRYPTO_C)) || \
    (MBEDTLS_VERSION_MAJOR >= 4 && defined(MBEDTLS_PSA_CRYPTO_CLIENT))

#include <mbedtls/asn1.h>
#include <psa/crypto.h>

#include <test/asn1_helpers.h>
#include <psa_crypto_slot_management.h>
#include <test/psa_crypto_helpers.h>

#if defined(MBEDTLS_PK_C)
#include <pk_internal.h>
#endif
#if defined(MBEDTLS_ECP_C)

#if !defined(MBEDTLS_VERSION_MAJOR) || MBEDTLS_VERSION_MAJOR >= 4
#include <mbedtls/private/ecp.h>
#else
#include <mbedtls/ecp.h>
#endif

#endif
#if defined(MBEDTLS_RSA_C)
#include <rsa_internal.h>
#endif

#if defined(__IAR_SYSTEMS_ICC__)
/* Suppress a very overeager warning from IAR: it dislikes a forward goto
 * that bypasses the initialization of a variable, even if that variable
 * is not used after the jump. (This is perfectly valid C; it would only
 * be invalid C if jumping into a block from outside that block.)
 */
#pragma diag_suppress=Pe546 // transfer of control bypasses initialization
#endif

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
static int lifetime_is_dynamic_secure_element(psa_key_lifetime_t lifetime)
{
    return PSA_KEY_LIFETIME_GET_LOCATION(lifetime) !=
           PSA_KEY_LOCATION_LOCAL_STORAGE;
}
#endif

static int check_key_attributes_sanity(mbedtls_svc_key_id_t key,
                                       int key_destroyable)
{
    int ok = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_lifetime_t lifetime;
    mbedtls_svc_key_id_t id;
    psa_key_type_t type;
    size_t bits;
    psa_status_t status = psa_get_key_attributes(key, &attributes);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        psa_reset_key_attributes(&attributes);
        return 1;
    }
    PSA_ASSERT(status);
    lifetime = psa_get_key_lifetime(&attributes);
    id = psa_get_key_id(&attributes);
    type = psa_get_key_type(&attributes);
    bits = psa_get_key_bits(&attributes);

    /* Persistence */
    if (PSA_KEY_LIFETIME_IS_VOLATILE(lifetime)) {
        TEST_ASSERT(
            (PSA_KEY_ID_VOLATILE_MIN <=
             MBEDTLS_SVC_KEY_ID_GET_KEY_ID(id)) &&
            (MBEDTLS_SVC_KEY_ID_GET_KEY_ID(id) <=
             PSA_KEY_ID_VOLATILE_MAX));
    } else {
        TEST_ASSERT(
            (PSA_KEY_ID_USER_MIN <= MBEDTLS_SVC_KEY_ID_GET_KEY_ID(id)) &&
            (MBEDTLS_SVC_KEY_ID_GET_KEY_ID(id) <= PSA_KEY_ID_USER_MAX));
    }
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    /* MBEDTLS_PSA_CRYPTO_SE_C does not support thread safety. */
    if (key_destroyable == 0) {
        /* randomly-generated 64-bit constant, should never appear in test data */
        psa_key_slot_number_t slot_number = 0xec94d4a5058a1a21;
        status = psa_get_key_slot_number(&attributes, &slot_number);
        if (lifetime_is_dynamic_secure_element(lifetime)) {
            /* Mbed TLS currently always exposes the slot number to
             * applications. This is not mandated by the PSA specification
             * and may change in future versions. */
            TEST_EQUAL(status, 0);
            TEST_ASSERT(slot_number != 0xec94d4a5058a1a21);
        } else {
            TEST_EQUAL(status, PSA_ERROR_INVALID_ARGUMENT);
        }
    }
#endif

    /* Type and size */
    TEST_ASSERT(type != 0);
    TEST_ASSERT(bits != 0);
    TEST_ASSERT(bits <= PSA_MAX_KEY_BITS);
    if (PSA_KEY_TYPE_IS_UNSTRUCTURED(type)) {
        TEST_ASSERT(bits % 8 == 0);
    }

    /* MAX macros concerning specific key types */
    if (PSA_KEY_TYPE_IS_ECC(type)) {
        TEST_ASSERT(bits <= PSA_VENDOR_ECC_MAX_CURVE_BITS);
    } else if (PSA_KEY_TYPE_IS_RSA(type)) {
        TEST_ASSERT(bits <= PSA_VENDOR_RSA_MAX_KEY_BITS);
    }
    TEST_ASSERT(PSA_BLOCK_CIPHER_BLOCK_LENGTH(type) <= PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE);

    ok = 1;

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    return ok;
}

static int exercise_mac_key(mbedtls_svc_key_id_t key,
                            psa_key_usage_t usage,
                            psa_algorithm_t alg,
                            int key_destroyable)
{
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
    const unsigned char input[] = "foo";
    unsigned char mac[PSA_MAC_MAX_SIZE] = { 0 };
    size_t mac_length = sizeof(mac);
    psa_status_t status = PSA_SUCCESS;
    /* Convert wildcard algorithm to exercisable algorithm */
    if (alg & PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG) {
        alg = PSA_ALG_TRUNCATED_MAC(alg, PSA_MAC_TRUNCATED_LENGTH(alg));
    }

    if (usage & PSA_KEY_USAGE_SIGN_HASH) {
        status = psa_mac_sign_setup(&operation, key, alg);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            PSA_ASSERT(psa_mac_abort(&operation));
            return 1;
        }
        PSA_ASSERT(status);
        PSA_ASSERT(psa_mac_update(&operation,
                                  input, sizeof(input)));
        PSA_ASSERT(psa_mac_sign_finish(&operation,
                                       mac, sizeof(mac),
                                       &mac_length));
    }

    if (usage & PSA_KEY_USAGE_VERIFY_HASH) {
        psa_status_t verify_status =
            (usage & PSA_KEY_USAGE_SIGN_HASH ?
             PSA_SUCCESS :
             PSA_ERROR_INVALID_SIGNATURE);
        status = psa_mac_verify_setup(&operation, key, alg);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            PSA_ASSERT(psa_mac_abort(&operation));
            return 1;
        }
        PSA_ASSERT(status);
        PSA_ASSERT(psa_mac_update(&operation,
                                  input, sizeof(input)));
        TEST_EQUAL(psa_mac_verify_finish(&operation, mac, mac_length),
                   verify_status);
    }

    return 1;

exit:
    psa_mac_abort(&operation);
    return 0;
}

static int exercise_cipher_key(mbedtls_svc_key_id_t key,
                               psa_key_usage_t usage,
                               psa_algorithm_t alg,
                               int key_destroyable)
{
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    unsigned char iv[PSA_CIPHER_IV_MAX_SIZE] = { 0 };
    size_t iv_length;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t key_type;
    const unsigned char plaintext[16] = "Hello, world...";
    /* We need to tell the compiler that we meant to leave out the null character. */
    unsigned char ciphertext[32] MBEDTLS_ATTRIBUTE_UNTERMINATED_STRING =
        "(wabblewebblewibblewobblewubble)";
    size_t ciphertext_length = sizeof(ciphertext);
    unsigned char decrypted[sizeof(ciphertext)];
    size_t part_length;
    psa_status_t status = PSA_SUCCESS;

    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    key_type = psa_get_key_type(&attributes);
    iv_length = PSA_CIPHER_IV_LENGTH(key_type, alg);

    if (usage & PSA_KEY_USAGE_ENCRYPT) {
        status = psa_cipher_encrypt_setup(&operation, key, alg);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            PSA_ASSERT(psa_cipher_abort(&operation));
            return 1;
        }
        PSA_ASSERT(status);
        if (iv_length != 0) {
            PSA_ASSERT(psa_cipher_generate_iv(&operation,
                                              iv, sizeof(iv),
                                              &iv_length));
        }
        PSA_ASSERT(psa_cipher_update(&operation,
                                     plaintext, sizeof(plaintext),
                                     ciphertext, sizeof(ciphertext),
                                     &ciphertext_length));
        PSA_ASSERT(psa_cipher_finish(&operation,
                                     ciphertext + ciphertext_length,
                                     sizeof(ciphertext) - ciphertext_length,
                                     &part_length));
        ciphertext_length += part_length;
    }

    if (usage & PSA_KEY_USAGE_DECRYPT) {
        int maybe_invalid_padding = 0;
        if (!(usage & PSA_KEY_USAGE_ENCRYPT)) {
            maybe_invalid_padding = !PSA_ALG_IS_STREAM_CIPHER(alg);
        }
        status = psa_cipher_decrypt_setup(&operation, key, alg);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            PSA_ASSERT(psa_cipher_abort(&operation));
            return 1;
        }
        PSA_ASSERT(status);
        if (iv_length != 0) {
            PSA_ASSERT(psa_cipher_set_iv(&operation,
                                         iv, iv_length));
        }
        PSA_ASSERT(psa_cipher_update(&operation,
                                     ciphertext, ciphertext_length,
                                     decrypted, sizeof(decrypted),
                                     &part_length));
        status = psa_cipher_finish(&operation,
                                   decrypted + part_length,
                                   sizeof(decrypted) - part_length,
                                   &part_length);
        /* For a stream cipher, all inputs are valid. For a block cipher,
         * if the input is some arbitrary data rather than an actual
           ciphertext, a padding error is likely.  */
        if (maybe_invalid_padding) {
            TEST_ASSERT(status == PSA_SUCCESS ||
                        status == PSA_ERROR_INVALID_PADDING);
        } else {
            PSA_ASSERT(status);
        }
    }

    return 1;

exit:
    psa_cipher_abort(&operation);
    psa_reset_key_attributes(&attributes);
    return 0;
}

static int exercise_aead_key(mbedtls_svc_key_id_t key,
                             psa_key_usage_t usage,
                             psa_algorithm_t alg,
                             int key_destroyable)
{
    unsigned char nonce[PSA_AEAD_NONCE_MAX_SIZE] = { 0 };
    size_t nonce_length;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t key_type;
    unsigned char plaintext[16] = "Hello, world...";
    unsigned char ciphertext[48] = "(wabblewebblewibblewobblewubble)";
    size_t ciphertext_length = sizeof(ciphertext);
    size_t plaintext_length = sizeof(ciphertext);
    psa_status_t status = PSA_SUCCESS;

    /* Convert wildcard algorithm to exercisable algorithm */
    if (alg & PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG) {
        alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, PSA_ALG_AEAD_GET_TAG_LENGTH(alg));
    }

    PSA_ASSERT(psa_get_key_attributes(key, &attributes));
    key_type = psa_get_key_type(&attributes);
    nonce_length = PSA_AEAD_NONCE_LENGTH(key_type, alg);

    if (usage & PSA_KEY_USAGE_ENCRYPT) {
        status = psa_aead_encrypt(key, alg,
                                  nonce, nonce_length,
                                  NULL, 0,
                                  plaintext, sizeof(plaintext),
                                  ciphertext, sizeof(ciphertext),
                                  &ciphertext_length);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            return 1;
        }
        PSA_ASSERT(status);
    }

    if (usage & PSA_KEY_USAGE_DECRYPT) {
        psa_status_t verify_status =
            (usage & PSA_KEY_USAGE_ENCRYPT ?
             PSA_SUCCESS :
             PSA_ERROR_INVALID_SIGNATURE);
        status = psa_aead_decrypt(key, alg,
                                  nonce, nonce_length,
                                  NULL, 0,
                                  ciphertext, ciphertext_length,
                                  plaintext, sizeof(plaintext),
                                  &plaintext_length);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            return 1;
        }
        TEST_ASSERT(status == verify_status);
    }

    return 1;

exit:
    psa_reset_key_attributes(&attributes);
    return 0;
}

static int can_sign_or_verify_message(psa_key_usage_t usage,
                                      psa_algorithm_t alg)
{
    /* Sign-the-unspecified-hash algorithms can only be used with
     * {sign,verify}_hash, not with {sign,verify}_message. */
    if (alg == PSA_ALG_ECDSA_ANY || alg == PSA_ALG_RSA_PKCS1V15_SIGN_RAW) {
        return 0;
    }
    return usage & (PSA_KEY_USAGE_SIGN_MESSAGE |
                    PSA_KEY_USAGE_VERIFY_MESSAGE);
}

static int exercise_signature_key(mbedtls_svc_key_id_t key,
                                  psa_key_usage_t usage,
                                  psa_algorithm_t alg,
                                  int key_destroyable)
{
    /* If the policy allows signing with any hash, just pick one. */
    psa_algorithm_t hash_alg = PSA_ALG_SIGN_GET_HASH(alg);
    if (PSA_ALG_IS_SIGN_HASH(alg) && hash_alg == PSA_ALG_ANY_HASH &&
        usage & (PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH |
                 PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE)) {
#if defined(KNOWN_SUPPORTED_HASH_ALG)
        hash_alg = KNOWN_SUPPORTED_HASH_ALG;
        alg ^= PSA_ALG_ANY_HASH ^ hash_alg;
#else
        TEST_FAIL("No hash algorithm for hash-and-sign testing");
#endif
    }
    psa_status_t status = PSA_SUCCESS;

    if (usage & (PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH) &&
        PSA_ALG_IS_SIGN_HASH(alg)) {
        unsigned char payload[PSA_HASH_MAX_SIZE] = { 1 };
        size_t payload_length = 16;
        unsigned char signature[PSA_SIGNATURE_MAX_SIZE] = { 0 };
        size_t signature_length = sizeof(signature);

        /* Some algorithms require the payload to have the size of
         * the hash encoded in the algorithm. Use this input size
         * even for algorithms that allow other input sizes. */
        if (hash_alg != 0) {
            payload_length = PSA_HASH_LENGTH(hash_alg);
        }

        if (usage & PSA_KEY_USAGE_SIGN_HASH) {
            status = psa_sign_hash(key, alg,
                                   payload, payload_length,
                                   signature, sizeof(signature),
                                   &signature_length);
            if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
                /* The key has been destroyed. */
                return 1;
            }
            PSA_ASSERT(status);
        }

        if (usage & PSA_KEY_USAGE_VERIFY_HASH) {
            psa_status_t verify_status =
                (usage & PSA_KEY_USAGE_SIGN_HASH ?
                 PSA_SUCCESS :
                 PSA_ERROR_INVALID_SIGNATURE);
            status = psa_verify_hash(key, alg,
                                     payload, payload_length,
                                     signature, signature_length);
            if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
                /* The key has been destroyed. */
                return 1;
            }
            TEST_ASSERT(status == verify_status);
        }
    }

    if (can_sign_or_verify_message(usage, alg)) {
        unsigned char message[256] = "Hello, world...";
        unsigned char signature[PSA_SIGNATURE_MAX_SIZE] = { 0 };
        size_t message_length = 16;
        size_t signature_length = sizeof(signature);

        if (usage & PSA_KEY_USAGE_SIGN_MESSAGE) {
            status = psa_sign_message(key, alg,
                                      message, message_length,
                                      signature, sizeof(signature),
                                      &signature_length);
            if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
                /* The key has been destroyed. */
                return 1;
            }
            PSA_ASSERT(status);
        }

        if (usage & PSA_KEY_USAGE_VERIFY_MESSAGE) {
            psa_status_t verify_status =
                (usage & PSA_KEY_USAGE_SIGN_MESSAGE ?
                 PSA_SUCCESS :
                 PSA_ERROR_INVALID_SIGNATURE);
            status = psa_verify_message(key, alg,
                                        message, message_length,
                                        signature, signature_length);
            if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
                /* The key has been destroyed. */
                return 1;
            }
            TEST_ASSERT(status == verify_status);
        }
    }

    return 1;

exit:
    return 0;
}

static int exercise_asymmetric_encryption_key(mbedtls_svc_key_id_t key,
                                              psa_key_usage_t usage,
                                              psa_algorithm_t alg,
                                              int key_destroyable)
{
    unsigned char plaintext[PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE] =
        "Hello, world...";
    unsigned char ciphertext[PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE] =
        "(wabblewebblewibblewobblewubble)";
    size_t ciphertext_length = sizeof(ciphertext);
    size_t plaintext_length = 16;
    psa_status_t status = PSA_SUCCESS;
    if (usage & PSA_KEY_USAGE_ENCRYPT) {
        status = psa_asymmetric_encrypt(key, alg,
                                        plaintext, plaintext_length,
                                        NULL, 0,
                                        ciphertext, sizeof(ciphertext),
                                        &ciphertext_length);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            return 1;
        }
        PSA_ASSERT(status);
    }

    if (usage & PSA_KEY_USAGE_DECRYPT) {
        status = psa_asymmetric_decrypt(key, alg,
                                        ciphertext, ciphertext_length,
                                        NULL, 0,
                                        plaintext, sizeof(plaintext),
                                        &plaintext_length);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            return 1;
        }
        TEST_ASSERT(status == PSA_SUCCESS ||
                    ((usage & PSA_KEY_USAGE_ENCRYPT) == 0 &&
                     (status == PSA_ERROR_INVALID_ARGUMENT ||
                      status == PSA_ERROR_INVALID_PADDING)));
    }

    return 1;

exit:
    return 0;
}

int mbedtls_test_psa_setup_key_derivation_wrap(
    psa_key_derivation_operation_t *operation,
    mbedtls_svc_key_id_t key,
    psa_algorithm_t alg,
    const unsigned char *input1, size_t input1_length,
    const unsigned char *input2, size_t input2_length,
    size_t capacity, int key_destroyable)
{
    PSA_ASSERT(psa_key_derivation_setup(operation, alg));
    psa_status_t status = PSA_SUCCESS;
    if (PSA_ALG_IS_HKDF(alg)) {
        PSA_ASSERT(psa_key_derivation_input_bytes(operation,
                                                  PSA_KEY_DERIVATION_INPUT_SALT,
                                                  input1, input1_length));
        status = psa_key_derivation_input_key(operation,
                                              PSA_KEY_DERIVATION_INPUT_SECRET,
                                              key);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            return 1;
        }
        PSA_ASSERT(status);
        PSA_ASSERT(psa_key_derivation_input_bytes(operation,
                                                  PSA_KEY_DERIVATION_INPUT_INFO,
                                                  input2,
                                                  input2_length));
    } else if (PSA_ALG_IS_HKDF_EXTRACT(alg)) {
        PSA_ASSERT(psa_key_derivation_input_bytes(operation,
                                                  PSA_KEY_DERIVATION_INPUT_SALT,
                                                  input1, input1_length));
        status = psa_key_derivation_input_key(operation,
                                              PSA_KEY_DERIVATION_INPUT_SECRET,
                                              key);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            return 1;
        }
        PSA_ASSERT(status);
    } else if (PSA_ALG_IS_HKDF_EXPAND(alg)) {
        status = psa_key_derivation_input_key(operation,
                                              PSA_KEY_DERIVATION_INPUT_SECRET,
                                              key);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            return 1;
        }
        PSA_ASSERT(status);
        PSA_ASSERT(psa_key_derivation_input_bytes(operation,
                                                  PSA_KEY_DERIVATION_INPUT_INFO,
                                                  input2,
                                                  input2_length));
    } else if (PSA_ALG_IS_TLS12_PRF(alg) ||
               PSA_ALG_IS_TLS12_PSK_TO_MS(alg)) {
        PSA_ASSERT(psa_key_derivation_input_bytes(operation,
                                                  PSA_KEY_DERIVATION_INPUT_SEED,
                                                  input1, input1_length));
        status = psa_key_derivation_input_key(operation,
                                              PSA_KEY_DERIVATION_INPUT_SECRET,
                                              key);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            return 1;
        }
        PSA_ASSERT(status);
        PSA_ASSERT(psa_key_derivation_input_bytes(operation,
                                                  PSA_KEY_DERIVATION_INPUT_LABEL,
                                                  input2, input2_length));
    } else if (PSA_ALG_IS_PBKDF2(alg)) {
        PSA_ASSERT(psa_key_derivation_input_integer(operation,
                                                    PSA_KEY_DERIVATION_INPUT_COST,
                                                    1U));
        PSA_ASSERT(psa_key_derivation_input_bytes(operation,
                                                  PSA_KEY_DERIVATION_INPUT_SALT,
                                                  input2,
                                                  input2_length));
        status = psa_key_derivation_input_key(operation,
                                              PSA_KEY_DERIVATION_INPUT_PASSWORD,
                                              key);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            return 1;
        }
        PSA_ASSERT(status);
    } else if (alg == PSA_ALG_TLS12_ECJPAKE_TO_PMS) {
        PSA_ASSERT(psa_key_derivation_input_bytes(operation,
                                                  PSA_KEY_DERIVATION_INPUT_SECRET,
                                                  input1, input1_length));
    } else {
        TEST_FAIL("Key derivation algorithm not supported");
    }

    if (capacity != SIZE_MAX) {
        PSA_ASSERT(psa_key_derivation_set_capacity(operation, capacity));
    }

    return 1;

exit:
    return 0;
}


static int exercise_key_derivation_key(mbedtls_svc_key_id_t key,
                                       psa_key_usage_t usage,
                                       psa_algorithm_t alg,
                                       int key_destroyable)
{
    psa_key_derivation_operation_t operation = PSA_KEY_DERIVATION_OPERATION_INIT;
    unsigned char input1[] = "Input 1";
    size_t input1_length = sizeof(input1);
    unsigned char input2[] = "Input 2";
    size_t input2_length = sizeof(input2);
    unsigned char output[1];
    size_t capacity = sizeof(output);

    if (usage & PSA_KEY_USAGE_DERIVE) {
        if (!mbedtls_test_psa_setup_key_derivation_wrap(&operation, key, alg,
                                                        input1, input1_length,
                                                        input2, input2_length,
                                                        capacity, key_destroyable)) {
            goto exit;
        }

        psa_status_t status = psa_key_derivation_output_bytes(&operation,
                                                              output,
                                                              capacity);
        if (key_destroyable && status == PSA_ERROR_BAD_STATE) {
            /* The key has been destroyed. */
            PSA_ASSERT(psa_key_derivation_abort(&operation));
        } else {
            PSA_ASSERT(status);
            PSA_ASSERT(psa_key_derivation_abort(&operation));
        }
    }

    return 1;

exit:
    return 0;
}

/* We need two keys to exercise key agreement. Exercise the
 * private key against its own public key. */
psa_status_t mbedtls_test_psa_key_agreement_with_self(
    psa_key_derivation_operation_t *operation,
    mbedtls_svc_key_id_t key, int key_destroyable)
{
    psa_key_type_t private_key_type;
    psa_key_type_t public_key_type;
    size_t key_bits;
    uint8_t *public_key = NULL;
    size_t public_key_length;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_status_t status = psa_get_key_attributes(key, &attributes);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        psa_reset_key_attributes(&attributes);
        return PSA_SUCCESS;
    }
    PSA_ASSERT(status);

    private_key_type = psa_get_key_type(&attributes);
    key_bits = psa_get_key_bits(&attributes);
    public_key_type = PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(private_key_type);
    public_key_length = PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(public_key_type, key_bits);
    TEST_CALLOC(public_key, public_key_length);
    status = psa_export_public_key(key, public_key, public_key_length,
                                   &public_key_length);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        status = PSA_SUCCESS;
        goto exit;
    }
    PSA_ASSERT(status);

    status = psa_key_derivation_key_agreement(
        operation, PSA_KEY_DERIVATION_INPUT_SECRET, key,
        public_key, public_key_length);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        status = PSA_SUCCESS;
        goto exit;
    }
exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    mbedtls_free(public_key);
    return status;
}

/* We need two keys to exercise key agreement. Exercise the
 * private key against its own public key. */
psa_status_t mbedtls_test_psa_raw_key_agreement_with_self(
    psa_algorithm_t alg,
    mbedtls_svc_key_id_t key,
    int key_destroyable)
{
    psa_key_type_t private_key_type;
    psa_key_type_t public_key_type;
    size_t key_bits;
    uint8_t *public_key = NULL;
    size_t public_key_length;
    uint8_t output[1024];
    size_t output_length;

#if MBEDTLS_VERSION_MAJOR >= 4
    uint8_t *exported = NULL;
    size_t exported_size = 0;
    size_t exported_length = 0;

    psa_key_attributes_t export_attributes = PSA_KEY_ATTRIBUTES_INIT;

    mbedtls_svc_key_id_t shared_secret_id = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t shared_secret_attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_key_agreement_iop_t iop_operation = psa_key_agreement_iop_init();
#endif /* MBEDTLS_VERSION_MAJOR >= 4 */

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_status_t status = psa_get_key_attributes(key, &attributes);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        psa_reset_key_attributes(&attributes);
        return PSA_SUCCESS;
    }
    PSA_ASSERT(status);

    private_key_type = psa_get_key_type(&attributes);
    key_bits = psa_get_key_bits(&attributes);
    public_key_type = PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(private_key_type);
    public_key_length = PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(public_key_type, key_bits);
    TEST_CALLOC(public_key, public_key_length);
    status = psa_export_public_key(key,
                                   public_key, public_key_length,
                                   &public_key_length);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        status = PSA_SUCCESS;
        goto exit;
    }
    PSA_ASSERT(status);

    status = psa_raw_key_agreement(
        alg, key, public_key, public_key_length,
        output, sizeof(output), &output_length);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        status = PSA_SUCCESS;
        goto exit;
    }
    if (status == PSA_SUCCESS) {
        TEST_ASSERT(output_length <=
                    PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(private_key_type,
                                                      key_bits));
        TEST_ASSERT(output_length <=
                    PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE);
    }

#if MBEDTLS_VERSION_MAJOR >= 4
    psa_status_t raw_status = status;

    psa_set_key_type(&shared_secret_attributes, PSA_KEY_TYPE_DERIVE);
    psa_set_key_usage_flags(&shared_secret_attributes,
                            PSA_KEY_USAGE_DERIVE | PSA_KEY_USAGE_EXPORT);

    status = psa_key_agreement(key, public_key, public_key_length, alg,
                               &shared_secret_attributes, &shared_secret_id);

    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        status = PSA_SUCCESS;
        goto exit;
    }

    /* In this function, we expect either success or a validation failure,
     * which should be identical for raw output and key output. So flag any
     * discrepancy between the two (in particular a key creation failure)
     * as a test failure. */
    TEST_EQUAL(raw_status, status);

    if (status == PSA_SUCCESS) {
        status = psa_get_key_attributes(shared_secret_id, &export_attributes);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            status = PSA_SUCCESS;
            goto exit;
        }

        exported_size =
            PSA_EXPORT_KEY_OUTPUT_SIZE(psa_get_key_type(&export_attributes),
                                       psa_get_key_bits(&export_attributes));
        TEST_CALLOC(exported, exported_size);

        status = psa_export_key(shared_secret_id,
                                exported, exported_size, &exported_length);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            status = PSA_SUCCESS;
        } else {
            PSA_ASSERT(status);
            TEST_MEMORY_COMPARE(exported, exported_length,
                                output, output_length);
        }
    }

 #if defined(MBEDTLS_ECP_RESTARTABLE) && defined(MBEDTLS_PSA_BUILTIN_ALG_ECDH)

    psa_destroy_key(shared_secret_id);

    if (PSA_KEY_TYPE_IS_ECC(private_key_type)) {

        psa_interruptible_set_max_ops(1);

        status = psa_key_agreement_iop_setup(&iop_operation, key, public_key,
                                             public_key_length, alg,
                                             &shared_secret_attributes);

        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            status = PSA_SUCCESS;
            goto exit;
        }

        /* In this function, we expect either success or a validation failure,
         * which should be identical for one-shot and interruptible. For an
         * interruptible operation, we insist on detecting error conditions
         * early, in setup() rather than complete(). So flag any discrepancy
         * between one-shot and interruptible-setup as a test failure. */
        TEST_EQUAL(raw_status, status);

        if (status == PSA_SUCCESS) {

            do {
                status = psa_key_agreement_iop_complete(&iop_operation,
                                                        &shared_secret_id);
            } while (status == PSA_OPERATION_INCOMPLETE);

            if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
                /* The key has been destroyed. */
                status = PSA_SUCCESS;
            } else {
                PSA_ASSERT(status);
                status = psa_export_key(shared_secret_id,
                                        exported, exported_size,
                                        &exported_length);
                if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
                    /* The key has been destroyed. */
                    status = PSA_SUCCESS;
                } else {
                    PSA_ASSERT(status);
                    TEST_MEMORY_COMPARE(exported, exported_length,
                                        output, output_length);
                }
            }
        }
    } else {
        TEST_EQUAL(psa_key_agreement_iop_setup(&iop_operation, key, public_key,
                                               public_key_length, alg,
                                               &shared_secret_attributes),
                   PSA_ERROR_INVALID_ARGUMENT);
    }

#endif // defined(MBEDTLS_ECP_RESTARTABLE) && defined(MBEDTLS_PSA_BUILTIN_ALG_ECDH)
#endif /* MBEDTLS_VERSION_MAJOR >= 4 */

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

#if MBEDTLS_VERSION_MAJOR >= 4
    psa_reset_key_attributes(&export_attributes);

    /* Make sure to reset and free derived key attributes and slot. */
    psa_reset_key_attributes(&shared_secret_attributes);
    psa_destroy_key(shared_secret_id);

    mbedtls_free(exported);
    psa_key_agreement_iop_abort(&iop_operation);
#endif /* MBEDTLS_VERSION_MAJOR >= 4 */

    mbedtls_free(public_key);
    return status;
}

static int exercise_raw_key_agreement_key(mbedtls_svc_key_id_t key,
                                          psa_key_usage_t usage,
                                          psa_algorithm_t alg,
                                          int key_destroyable)
{
    int ok = 0;

    if (usage & PSA_KEY_USAGE_DERIVE) {
        /* We need two keys to exercise key agreement. Exercise the
         * private key against its own public key. */
        PSA_ASSERT(mbedtls_test_psa_raw_key_agreement_with_self(alg, key,
                                                                key_destroyable));
    }
    ok = 1;

exit:
    return ok;
}

static int exercise_key_agreement_key(mbedtls_svc_key_id_t key,
                                      psa_key_usage_t usage,
                                      psa_algorithm_t alg,
                                      int key_destroyable)
{
    psa_key_derivation_operation_t operation = PSA_KEY_DERIVATION_OPERATION_INIT;
    unsigned char input[1] = { 0 };
    unsigned char output[1];
    int ok = 0;
    psa_algorithm_t kdf_alg = PSA_ALG_KEY_AGREEMENT_GET_KDF(alg);
    psa_status_t expected_key_agreement_status = PSA_SUCCESS;

    if (usage & PSA_KEY_USAGE_DERIVE) {
        /* We need two keys to exercise key agreement. Exercise the
         * private key against its own public key. */
        PSA_ASSERT(psa_key_derivation_setup(&operation, alg));
        if (PSA_ALG_IS_TLS12_PRF(kdf_alg) ||
            PSA_ALG_IS_TLS12_PSK_TO_MS(kdf_alg)) {
            PSA_ASSERT(psa_key_derivation_input_bytes(
                           &operation, PSA_KEY_DERIVATION_INPUT_SEED,
                           input, sizeof(input)));
        }

        if (PSA_ALG_IS_HKDF_EXTRACT(kdf_alg)) {
            PSA_ASSERT(psa_key_derivation_input_bytes(
                           &operation, PSA_KEY_DERIVATION_INPUT_SALT,
                           input, sizeof(input)));
        }

        /* For HKDF_EXPAND input secret may fail as secret size may not match
           to expected PRK size. In practice it means that key bits must match
           hash length. Otherwise test should fail with INVALID_ARGUMENT. */
        if (PSA_ALG_IS_HKDF_EXPAND(kdf_alg)) {
            psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
            psa_status_t status = psa_get_key_attributes(key, &attributes);
            if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
                /* The key has been destroyed. */
                ok = 1;
            }
            PSA_ASSERT(status);
            size_t key_bits = psa_get_key_bits(&attributes);
            psa_algorithm_t hash_alg = PSA_ALG_HKDF_GET_HASH(kdf_alg);

            if (PSA_BITS_TO_BYTES(key_bits) != PSA_HASH_LENGTH(hash_alg)) {
                expected_key_agreement_status = PSA_ERROR_INVALID_ARGUMENT;
            }
        }

        TEST_EQUAL(mbedtls_test_psa_key_agreement_with_self(&operation, key,
                                                            key_destroyable),
                   expected_key_agreement_status);

        if (expected_key_agreement_status != PSA_SUCCESS) {
            return 1;
        }

        if (PSA_ALG_IS_TLS12_PRF(kdf_alg) ||
            PSA_ALG_IS_TLS12_PSK_TO_MS(kdf_alg)) {
            PSA_ASSERT(psa_key_derivation_input_bytes(
                           &operation, PSA_KEY_DERIVATION_INPUT_LABEL,
                           input, sizeof(input)));
        } else if (PSA_ALG_IS_HKDF(kdf_alg) || PSA_ALG_IS_HKDF_EXPAND(kdf_alg)) {
            PSA_ASSERT(psa_key_derivation_input_bytes(
                           &operation, PSA_KEY_DERIVATION_INPUT_INFO,
                           input, sizeof(input)));
        }
        PSA_ASSERT(psa_key_derivation_output_bytes(&operation,
                                                   output,
                                                   sizeof(output)));
        PSA_ASSERT(psa_key_derivation_abort(&operation));
    }
    ok = 1;

exit:
    return ok;
}

int mbedtls_test_psa_exported_key_sanity_check(
    psa_key_type_t type, size_t bits,
    const uint8_t *exported, size_t exported_length)
{
    TEST_ASSERT(exported_length <= PSA_EXPORT_KEY_OUTPUT_SIZE(type, bits));

    if (PSA_KEY_TYPE_IS_UNSTRUCTURED(type)) {
        TEST_EQUAL(exported_length, PSA_BITS_TO_BYTES(bits));
    } else

#if defined(MBEDTLS_ASN1_PARSE_C)
    if (type == PSA_KEY_TYPE_RSA_KEY_PAIR) {
        uint8_t *p = (uint8_t *) exported;
        const uint8_t *end = exported + exported_length;
        size_t len;
        /*   RSAPrivateKey ::= SEQUENCE {
         *       version             INTEGER,  -- must be 0
         *       modulus             INTEGER,  -- n
         *       publicExponent      INTEGER,  -- e
         *       privateExponent     INTEGER,  -- d
         *       prime1              INTEGER,  -- p
         *       prime2              INTEGER,  -- q
         *       exponent1           INTEGER,  -- d mod (p-1)
         *       exponent2           INTEGER,  -- d mod (q-1)
         *       coefficient         INTEGER,  -- (inverse of q) mod p
         *   }
         */
        TEST_EQUAL(mbedtls_asn1_get_tag(&p, end, &len,
                                        MBEDTLS_ASN1_SEQUENCE |
                                        MBEDTLS_ASN1_CONSTRUCTED), 0);
        TEST_EQUAL(len, end - p);
        if (!mbedtls_test_asn1_skip_integer(&p, end, 0, 0, 0)) {
            goto exit;
        }
        if (!mbedtls_test_asn1_skip_integer(&p, end, bits, bits, 1)) {
            goto exit;
        }
        if (!mbedtls_test_asn1_skip_integer(&p, end, 2, bits, 1)) {
            goto exit;
        }
        /* Require d to be at least half the size of n. */
        if (!mbedtls_test_asn1_skip_integer(&p, end, bits / 2, bits, 1)) {
            goto exit;
        }
        /* Require p and q to be at most half the size of n, rounded up. */
        if (!mbedtls_test_asn1_skip_integer(&p, end, bits / 2, bits / 2 + 1, 1)) {
            goto exit;
        }
        if (!mbedtls_test_asn1_skip_integer(&p, end, bits / 2, bits / 2 + 1, 1)) {
            goto exit;
        }
        if (!mbedtls_test_asn1_skip_integer(&p, end, 1, bits / 2 + 1, 0)) {
            goto exit;
        }
        if (!mbedtls_test_asn1_skip_integer(&p, end, 1, bits / 2 + 1, 0)) {
            goto exit;
        }
        if (!mbedtls_test_asn1_skip_integer(&p, end, 1, bits / 2 + 1, 0)) {
            goto exit;
        }
        TEST_EQUAL(p - end, 0);

        TEST_ASSERT(exported_length <= PSA_EXPORT_KEY_PAIR_MAX_SIZE);
    } else
#endif /* MBEDTLS_ASN1_PARSE_C */

    if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type)) {
        /* Just the secret value */
        TEST_EQUAL(exported_length, PSA_BITS_TO_BYTES(bits));

        TEST_ASSERT(exported_length <= PSA_EXPORT_KEY_PAIR_MAX_SIZE);
    } else

#if defined(MBEDTLS_ASN1_PARSE_C)
    if (type == PSA_KEY_TYPE_RSA_PUBLIC_KEY) {
        uint8_t *p = (uint8_t *) exported;
        const uint8_t *end = exported + exported_length;
        size_t len;
        /*   RSAPublicKey ::= SEQUENCE {
         *      modulus            INTEGER,    -- n
         *      publicExponent     INTEGER  }  -- e
         */
        TEST_EQUAL(mbedtls_asn1_get_tag(&p, end, &len,
                                        MBEDTLS_ASN1_SEQUENCE |
                                        MBEDTLS_ASN1_CONSTRUCTED),
                   0);
        TEST_EQUAL(len, end - p);
        if (!mbedtls_test_asn1_skip_integer(&p, end, bits, bits, 1)) {
            goto exit;
        }
        if (!mbedtls_test_asn1_skip_integer(&p, end, 2, bits, 1)) {
            goto exit;
        }
        TEST_EQUAL(p - end, 0);


        TEST_ASSERT(exported_length <=
                    PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(type, bits));
        TEST_ASSERT(exported_length <=
                    PSA_EXPORT_PUBLIC_KEY_MAX_SIZE);
    } else
#endif /* MBEDTLS_ASN1_PARSE_C */

    if (PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type)) {

        TEST_ASSERT(exported_length <=
                    PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(type, bits));
        TEST_ASSERT(exported_length <=
                    PSA_EXPORT_PUBLIC_KEY_MAX_SIZE);

        if (PSA_KEY_TYPE_ECC_GET_FAMILY(type) == PSA_ECC_FAMILY_MONTGOMERY) {
            /* The representation of an ECC Montgomery public key is
             * the raw compressed point */
            TEST_EQUAL(PSA_BITS_TO_BYTES(bits), exported_length);
        } else if (PSA_KEY_TYPE_ECC_GET_FAMILY(type) == PSA_ECC_FAMILY_TWISTED_EDWARDS) {
            /* The representation of an ECC Edwards public key is
             * the raw compressed point */
            TEST_EQUAL(PSA_BITS_TO_BYTES(bits + 1), exported_length);
        } else {
            /* The representation of an ECC Weierstrass public key is:
             *      - The byte 0x04;
             *      - `x_P` as a `ceiling(m/8)`-byte string, big-endian;
             *      - `y_P` as a `ceiling(m/8)`-byte string, big-endian;
             *      - where m is the bit size associated with the curve.
             */
            TEST_EQUAL(1 + 2 * PSA_BITS_TO_BYTES(bits), exported_length);
            TEST_EQUAL(exported[0], 4);
        }
    } else
    if (PSA_KEY_TYPE_IS_DH_PUBLIC_KEY(type) || PSA_KEY_TYPE_IS_DH_KEY_PAIR(type)) {
        TEST_ASSERT(exported_length ==
                    PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(type, bits));
        TEST_ASSERT(exported_length <=
                    PSA_EXPORT_PUBLIC_KEY_MAX_SIZE);
    } else {
        (void) exported;
        TEST_FAIL("Sanity check not implemented for this key type");
    }

#if defined(MBEDTLS_DES_C)
    if (type == PSA_KEY_TYPE_DES) {
        /* Check the parity bits. */
        unsigned i;
        for (i = 0; i < bits / 8; i++) {
            unsigned bit_count = 0;
            unsigned m;
            for (m = 1; m <= 0x100; m <<= 1) {
                if (exported[i] & m) {
                    ++bit_count;
                }
            }
            TEST_ASSERT(bit_count % 2 != 0);
        }
    }
#endif

    return 1;

exit:
    return 0;
}

static int exercise_export_key(mbedtls_svc_key_id_t key,
                               psa_key_usage_t usage,
                               int key_destroyable)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t *exported = NULL;
    size_t exported_size = 0;
    size_t exported_length = 0;
    int ok = 0;

    psa_status_t status = psa_get_key_attributes(key, &attributes);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        psa_reset_key_attributes(&attributes);
        return 1;
    }
    PSA_ASSERT(status);

    exported_size = PSA_EXPORT_KEY_OUTPUT_SIZE(
        psa_get_key_type(&attributes),
        psa_get_key_bits(&attributes));
    TEST_CALLOC(exported, exported_size);

    status = psa_export_key(key, exported, exported_size, &exported_length);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        ok = 1;
        goto exit;
    } else if ((usage & PSA_KEY_USAGE_EXPORT) == 0 &&
               !PSA_KEY_TYPE_IS_PUBLIC_KEY(psa_get_key_type(&attributes))) {
        TEST_EQUAL(status, PSA_ERROR_NOT_PERMITTED);
        ok = 1;
        goto exit;
    }
    PSA_ASSERT(status);
    ok = mbedtls_test_psa_exported_key_sanity_check(
        psa_get_key_type(&attributes), psa_get_key_bits(&attributes),
        exported, exported_length);

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    mbedtls_free(exported);
    return ok;
}

static int exercise_export_public_key(mbedtls_svc_key_id_t key,
                                      int key_destroyable)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t public_type;
    uint8_t *exported = NULL;
    size_t exported_size = 0;
    size_t exported_length = 0;
    int ok = 0;

    psa_status_t status = psa_get_key_attributes(key, &attributes);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        psa_reset_key_attributes(&attributes);
        return 1;
    }
    PSA_ASSERT(status);
    if (!PSA_KEY_TYPE_IS_ASYMMETRIC(psa_get_key_type(&attributes))) {
        exported_size = PSA_EXPORT_KEY_OUTPUT_SIZE(
            psa_get_key_type(&attributes),
            psa_get_key_bits(&attributes));
        TEST_CALLOC(exported, exported_size);

        status = psa_export_public_key(key, exported,
                                       exported_size, &exported_length);
        if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
            /* The key has been destroyed. */
            ok = 1;
            goto exit;
        }
        TEST_EQUAL(status, PSA_ERROR_INVALID_ARGUMENT);
        ok = 1;
        goto exit;
    }

    public_type = PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(
        psa_get_key_type(&attributes));
    exported_size = PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(public_type,
                                                      psa_get_key_bits(&attributes));
    TEST_CALLOC(exported, exported_size);

    status = psa_export_public_key(key, exported,
                                   exported_size, &exported_length);
    if (key_destroyable && status == PSA_ERROR_INVALID_HANDLE) {
        /* The key has been destroyed. */
        ok = 1;
        goto exit;
    }
    PSA_ASSERT(status);
    ok = mbedtls_test_psa_exported_key_sanity_check(
        public_type, psa_get_key_bits(&attributes),
        exported, exported_length);

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes(&attributes);

    mbedtls_free(exported);
    return ok;
}

int mbedtls_test_psa_exercise_key(mbedtls_svc_key_id_t key,
                                  psa_key_usage_t usage,
                                  psa_algorithm_t alg,
                                  int key_destroyable)
{
    int ok = 0;

    if (!check_key_attributes_sanity(key, key_destroyable)) {
        return 0;
    }

    if (alg == 0) {
        ok = 1; /* If no algorithm, do nothing (used for raw data "keys"). */
    } else if (PSA_ALG_IS_MAC(alg)) {
        ok = exercise_mac_key(key, usage, alg, key_destroyable);
    } else if (PSA_ALG_IS_CIPHER(alg)) {
        ok = exercise_cipher_key(key, usage, alg, key_destroyable);
    } else if (PSA_ALG_IS_AEAD(alg)) {
        ok = exercise_aead_key(key, usage, alg, key_destroyable);
    } else if (PSA_ALG_IS_SIGN(alg)) {
        ok = exercise_signature_key(key, usage, alg, key_destroyable);
    } else if (PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg)) {
        ok = exercise_asymmetric_encryption_key(key, usage, alg,
                                                key_destroyable);
    } else if (PSA_ALG_IS_KEY_DERIVATION(alg)) {
        ok = exercise_key_derivation_key(key, usage, alg, key_destroyable);
    } else if (PSA_ALG_IS_RAW_KEY_AGREEMENT(alg)) {
        ok = exercise_raw_key_agreement_key(key, usage, alg, key_destroyable);
    } else if (PSA_ALG_IS_KEY_AGREEMENT(alg)) {
        ok = exercise_key_agreement_key(key, usage, alg, key_destroyable);
    } else {
        TEST_FAIL("No code to exercise this category of algorithm");
    }

    ok = ok && exercise_export_key(key,
                                   usage,
                                   key_destroyable);
    ok = ok && exercise_export_public_key(key,
                                          key_destroyable);

exit:
    return ok;
}

psa_key_usage_t mbedtls_test_psa_usage_to_exercise(psa_key_type_t type,
                                                   psa_algorithm_t alg)
{
    if (PSA_ALG_IS_MAC(alg) || PSA_ALG_IS_SIGN(alg)) {
        if (PSA_ALG_IS_SIGN_HASH(alg)) {
            if (PSA_ALG_SIGN_GET_HASH(alg)) {
                return PSA_KEY_TYPE_IS_PUBLIC_KEY(type) ?
                       PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_VERIFY_MESSAGE :
                       PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH |
                       PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE;
            }
        } else if (PSA_ALG_IS_SIGN_MESSAGE(alg)) {
            return PSA_KEY_TYPE_IS_PUBLIC_KEY(type) ?
                   PSA_KEY_USAGE_VERIFY_MESSAGE :
                   PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE;
        }

        return PSA_KEY_TYPE_IS_PUBLIC_KEY(type) ?
               PSA_KEY_USAGE_VERIFY_HASH :
               PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
    } else if (PSA_ALG_IS_CIPHER(alg) || PSA_ALG_IS_AEAD(alg) ||
               PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg)) {
        return PSA_KEY_TYPE_IS_PUBLIC_KEY(type) ?
               PSA_KEY_USAGE_ENCRYPT :
               PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
    } else if (PSA_ALG_IS_KEY_DERIVATION(alg) ||
               PSA_ALG_IS_KEY_AGREEMENT(alg)) {
        return PSA_KEY_USAGE_DERIVE;
    } else {
        return 0;
    }

}

int mbedtls_test_can_exercise_psa_algorithm(psa_algorithm_t alg)
{
    /* Reject algorithms that we know are not supported. Default to
     * attempting exercise, so that if an algorithm is missing from this
     * function, the result will be a test failure and not silently
     * omitting exercise. */
#if !defined(PSA_WANT_ALG_RSA_PKCS1V15_CRYPT)
    if (alg == PSA_ALG_RSA_PKCS1V15_CRYPT) {
        return 0;
    }
#endif
#if !defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)
    if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg)) {
        return 0;
    }
#endif
#if !defined(PSA_WANT_ALG_RSA_PSS)
    if (PSA_ALG_IS_RSA_PSS_STANDARD_SALT(alg)) {
        return 0;
    }
#endif
#if !defined(PSA_WANT_ALG_RSA_PSS_ANY_SALT)
    if (PSA_ALG_IS_RSA_PSS_ANY_SALT(alg)) {
        return 0;
    }
#endif
#if !defined(PSA_WANT_ALG_ECDSA)
    if (PSA_ALG_IS_ECDSA(alg)) {
        return 0;
    }
#endif
#if !defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA)
    if (PSA_ALG_IS_DETERMINISTIC_ECDSA(alg)) {
        return 0;
    }
#endif
#if !defined(PSA_WANT_ALG_ECDH)
    if (PSA_ALG_IS_ECDH(alg)) {
        return 0;
    }
#endif
    (void) alg;
    return 1;
}

#if defined(MBEDTLS_PK_C)
int mbedtls_test_key_consistency_psa_pk(mbedtls_svc_key_id_t psa_key,
                                        const mbedtls_pk_context *pk)
{
    psa_key_attributes_t psa_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t pk_attributes = PSA_KEY_ATTRIBUTES_INIT;
    int ok = 0;

    PSA_ASSERT(psa_get_key_attributes(psa_key, &psa_attributes));
    psa_key_type_t psa_type = psa_get_key_type(&psa_attributes);
    mbedtls_pk_type_t pk_type = mbedtls_pk_get_type(pk);

    TEST_ASSERT(PSA_KEY_TYPE_IS_PUBLIC_KEY(psa_type) ||
                PSA_KEY_TYPE_IS_KEY_PAIR(psa_type));
    TEST_EQUAL(psa_get_key_bits(&psa_attributes), mbedtls_pk_get_bitlen(pk));

    uint8_t pk_public_buffer[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
    const uint8_t *pk_public = NULL;
    size_t pk_public_length = 0;

    switch (pk_type) {
#if defined(MBEDTLS_RSA_C) || defined(MBEDTLS_PK_USE_PSA_RSA_DATA)
        case MBEDTLS_PK_RSA:
#if defined(MBEDTLS_PK_USE_PSA_RSA_DATA)
            TEST_ASSERT(PSA_KEY_TYPE_IS_RSA(psa_type));
            pk_public = pk->pub_raw;
            pk_public_length = pk->pub_raw_len;
            break;
#else /* MBEDTLS_PK_USE_PSA_RSA_DATA */
            TEST_ASSERT(PSA_KEY_TYPE_IS_RSA(psa_type));
            const mbedtls_rsa_context *rsa = mbedtls_pk_rsa(*pk);
            uint8_t *const end = pk_public_buffer + sizeof(pk_public_buffer);
            uint8_t *cursor = end;
            TEST_LE_U(1, mbedtls_rsa_write_pubkey(rsa,
                                                  pk_public_buffer, &cursor));
            pk_public = cursor;
            pk_public_length = end - pk_public;
            break;
#endif /* MBEDTLS_PK_USE_PSA_RSA_DATA */
#endif /* MBEDTLS_RSA_C || MBEDTLS_PK_USE_PSA_RSA_DATA */

#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
#if defined(MBEDTLS_PK_USE_PSA_EC_DATA)
        case MBEDTLS_PK_ECKEY:
        case MBEDTLS_PK_ECKEY_DH:
        case MBEDTLS_PK_ECDSA:
            TEST_ASSERT(PSA_KEY_TYPE_IS_ECC(psa_type));
            TEST_EQUAL(PSA_KEY_TYPE_ECC_GET_FAMILY(psa_type), pk->ec_family);
            pk_public = pk->pub_raw;
            pk_public_length = pk->pub_raw_len;
            break;
#else /* MBEDTLS_PK_USE_PSA_EC_DATA */
        case MBEDTLS_PK_ECKEY:
        case MBEDTLS_PK_ECKEY_DH:
        case MBEDTLS_PK_ECDSA:
            TEST_ASSERT(PSA_KEY_TYPE_IS_ECC(psa_get_key_type(&psa_attributes)));
            const mbedtls_ecp_keypair *ec = mbedtls_pk_ec_ro(*pk);
            TEST_EQUAL(mbedtls_ecp_write_public_key(
                           ec, MBEDTLS_ECP_PF_UNCOMPRESSED, &pk_public_length,
                           pk_public_buffer, sizeof(pk_public_buffer)), 0);
            pk_public = pk_public_buffer;
            break;
#endif /* MBEDTLS_PK_USE_PSA_EC_DATA */
#endif /* PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY */

#if defined(MBEDTLS_USE_PSA_CRYPTO)
        case MBEDTLS_PK_OPAQUE:
            PSA_ASSERT(psa_get_key_attributes(pk->priv_id, &pk_attributes));
            psa_key_type_t pk_psa_type = psa_get_key_type(&pk_attributes);
            TEST_EQUAL(PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(psa_type),
                       PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(pk_psa_type));
            PSA_ASSERT(psa_export_public_key(psa_key,
                                             pk_public_buffer,
                                             sizeof(pk_public_buffer),
                                             &pk_public_length));
            pk_public = pk_public_buffer;
            break;
#endif /* MBEDTLS_USE_PSA_CRYPTO */

        default:
            TEST_FAIL("pk type not supported");
    }

    uint8_t psa_public[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
    size_t psa_public_length = 0;
    PSA_ASSERT(psa_export_public_key(psa_key,
                                     psa_public, sizeof(psa_public),
                                     &psa_public_length));
    TEST_MEMORY_COMPARE(pk_public, pk_public_length,
                        psa_public, psa_public_length);

    ok = 1;

exit:
    psa_reset_key_attributes(&psa_attributes);
    psa_reset_key_attributes(&pk_attributes);
    return ok;
}
#endif /* MBEDTLS_PK_C */

#endif /* MBEDTLS_PSA_CRYPTO_C || MBEDTLS_PSA_CRYPTO_CLIENT */
