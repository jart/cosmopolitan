/*
 * Helper functions for PK.
 * This is only for TF-PSA-Crypto 1.0 and above.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "test_common.h"
#include <test/macros.h>
#include <test/helpers.h>
#include <test/pk_helpers.h>
#include <test/psa_helpers.h>
#include <test/test_keys.h>
#include <mbedtls/pk.h>
#include "psa_util_internal.h"

/* Functions like mbedtls_pk_wrap_psa() are only available in tf-psa-crypto and
 * not in 3.6 LTS branch, so we need a guard for this. */
#if TF_PSA_CRYPTO_VERSION_MAJOR >= 1

#if defined(MBEDTLS_PK_C)

typedef struct {
    psa_key_type_t key_type;
    psa_key_bits_t key_bits;
    const uint8_t *key;
    size_t key_len;
} mbedtls_pk_helpers_predefined_key_t;

#define EC_KEY(family_type, bits, array_base_name)                   \
    { PSA_KEY_TYPE_ECC_KEY_PAIR(family_type), bits,                  \
      array_base_name ## _priv, sizeof(array_base_name ## _priv) },  \
    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(family_type), bits,                \
      array_base_name ## _pub, sizeof(array_base_name ## _pub) }

#define RSA_KEY(bits, array_base_name)                               \
    { PSA_KEY_TYPE_RSA_KEY_PAIR, bits,                               \
      array_base_name ## _priv, sizeof(array_base_name ## _priv) },  \
    { PSA_KEY_TYPE_RSA_PUBLIC_KEY, bits,                             \
      array_base_name ## _pub, sizeof(array_base_name ## _pub) }

static mbedtls_pk_helpers_predefined_key_t predefined_keys_psa[] = {
    EC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1, 256, test_ec_bp256r1),
    EC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1, 384, test_ec_bp384r1),
    EC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1, 512, test_ec_bp512r1),

    EC_KEY(PSA_ECC_FAMILY_MONTGOMERY, 255, test_ec_curve25519),
    EC_KEY(PSA_ECC_FAMILY_MONTGOMERY, 448, test_ec_curve448),

    EC_KEY(PSA_ECC_FAMILY_SECP_K1, 256, test_ec_secp256k1),

    EC_KEY(PSA_ECC_FAMILY_SECP_R1, 256, test_ec_secp256r1),
    EC_KEY(PSA_ECC_FAMILY_SECP_R1, 384, test_ec_secp384r1),
    EC_KEY(PSA_ECC_FAMILY_SECP_R1, 521, test_ec_secp521r1),

    RSA_KEY(1024, test_rsa_1024),
    RSA_KEY(1026, test_rsa_1026),
    RSA_KEY(1028, test_rsa_1028),
    RSA_KEY(1030, test_rsa_1030),
    RSA_KEY(1536, test_rsa_1536),
    RSA_KEY(2048, test_rsa_2048),
    RSA_KEY(4096, test_rsa_4096),
};

int mbedtls_pk_helpers_get_predefined_key_data(psa_key_type_t key_type, psa_key_bits_t key_bits,
                                               const uint8_t **output, size_t *output_len)
{
    for (size_t i = 0; i < ARRAY_LENGTH(predefined_keys_psa); i++) {
        if ((key_type == predefined_keys_psa[i].key_type) &&
            (key_bits == predefined_keys_psa[i].key_bits)) {
            *output = predefined_keys_psa[i].key;
            *output_len = predefined_keys_psa[i].key_len;
            return 0;
        }
    }

    *output = NULL;
    *output_len = 0;
    TEST_FAIL("Predefined key not available");

exit:
    return MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE;
}

mbedtls_svc_key_id_t mbedtls_pk_helpers_make_psa_key_from_predefined(psa_key_type_t key_type,
                                                                     psa_key_bits_t key_bits,
                                                                     psa_algorithm_t alg,
                                                                     psa_algorithm_t alg2,
                                                                     psa_key_usage_t usage_flags)
{
    mbedtls_svc_key_id_t key_id = MBEDTLS_SVC_KEY_ID_INIT;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    const uint8_t *key = NULL;
    size_t key_len = 0;

    mbedtls_pk_helpers_get_predefined_key_data(key_type, key_bits, &key, &key_len);

    psa_set_key_type(&attr, key_type);
    psa_set_key_usage_flags(&attr, usage_flags);
    psa_set_key_algorithm(&attr, alg);
    psa_set_key_enrollment_algorithm(&attr, alg2);
    PSA_ASSERT(psa_import_key(&attr, key, key_len, &key_id));

exit:
    return key_id;
}

int mbedtls_pk_helpers_populate_context(mbedtls_pk_context *pk, mbedtls_svc_key_id_t key_id,
                                        pk_context_populate_method_t method)
{
    int ret = -1;

    switch (method) {
        case TEST_PK_WRAP_PSA:
            TEST_EQUAL(mbedtls_pk_wrap_psa(pk, key_id), 0);
            break;
        case TEST_PK_COPY_FROM_PSA:
            TEST_EQUAL(mbedtls_pk_copy_from_psa(key_id, pk), 0);
            break;
        case TEST_PK_COPY_PUBLIC_FROM_PSA:
            TEST_EQUAL(mbedtls_pk_copy_public_from_psa(key_id, pk), 0);
            break;
        default:
            TEST_FAIL("Unknown population method");
    }

    ret = 0;

exit:
    return ret;
}

#endif /* MBEDTLS_PK_C */

#endif /* !TF_PSA_CRYPTO_VERSION_MAJOR */
