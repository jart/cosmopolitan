/*
 *  X.509 test certificates
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "tf_psa_crypto_common.h"

#include <test/certs.h>

#include "mbedtls/build_info.h"

#include "mbedtls/pk.h"
#if defined(MBEDTLS_PK_HAVE_PRIVATE_HEADER)
#include <mbedtls/private/pk_private.h>
#endif /* MBEDTLS_PK_HAVE_PRIVATE_HEADER */

#include "test/test_certs.h"

/*
 *
 * Test certificates and keys as C variables
 *
 */

/*
 * CA
 */

const char mbedtls_test_ca_crt_ec_pem[]           = TEST_CA_CRT_EC_PEM;
const char mbedtls_test_ca_key_ec_pem[]           = TEST_CA_KEY_EC_PEM;
const char mbedtls_test_ca_pwd_ec_pem[]           = TEST_CA_PWD_EC_PEM;
const char mbedtls_test_ca_key_rsa_pem[]          = TEST_CA_KEY_RSA_PEM;
const char mbedtls_test_ca_pwd_rsa_pem[]          = TEST_CA_PWD_RSA_PEM;
const char mbedtls_test_ca_crt_rsa_sha1_pem[]     = TEST_CA_CRT_RSA_SHA1_PEM;
const char mbedtls_test_ca_crt_rsa_sha256_pem[]   = TEST_CA_CRT_RSA_SHA256_PEM;

const unsigned char mbedtls_test_ca_crt_ec_der[]   = TEST_CA_CRT_EC_DER;
const unsigned char mbedtls_test_ca_key_ec_der[]   = TEST_CA_KEY_EC_DER;
const unsigned char mbedtls_test_ca_key_rsa_der[]  = TEST_CA_KEY_RSA_DER;
const unsigned char mbedtls_test_ca_crt_rsa_sha1_der[]   =
    TEST_CA_CRT_RSA_SHA1_DER;
const unsigned char mbedtls_test_ca_crt_rsa_sha256_der[] =
    TEST_CA_CRT_RSA_SHA256_DER;

const size_t mbedtls_test_ca_crt_ec_pem_len =
    sizeof(mbedtls_test_ca_crt_ec_pem);
const size_t mbedtls_test_ca_key_ec_pem_len =
    sizeof(mbedtls_test_ca_key_ec_pem);
const size_t mbedtls_test_ca_pwd_ec_pem_len =
    sizeof(mbedtls_test_ca_pwd_ec_pem) - 1;
const size_t mbedtls_test_ca_key_rsa_pem_len =
    sizeof(mbedtls_test_ca_key_rsa_pem);
const size_t mbedtls_test_ca_pwd_rsa_pem_len =
    sizeof(mbedtls_test_ca_pwd_rsa_pem) - 1;
const size_t mbedtls_test_ca_crt_rsa_sha1_pem_len =
    sizeof(mbedtls_test_ca_crt_rsa_sha1_pem);
const size_t mbedtls_test_ca_crt_rsa_sha256_pem_len =
    sizeof(mbedtls_test_ca_crt_rsa_sha256_pem);

const size_t mbedtls_test_ca_crt_ec_der_len =
    sizeof(mbedtls_test_ca_crt_ec_der);
const size_t mbedtls_test_ca_key_ec_der_len =
    sizeof(mbedtls_test_ca_key_ec_der);
const size_t mbedtls_test_ca_pwd_ec_der_len = 0;
const size_t mbedtls_test_ca_key_rsa_der_len =
    sizeof(mbedtls_test_ca_key_rsa_der);
const size_t mbedtls_test_ca_pwd_rsa_der_len = 0;
const size_t mbedtls_test_ca_crt_rsa_sha1_der_len =
    sizeof(mbedtls_test_ca_crt_rsa_sha1_der);
const size_t mbedtls_test_ca_crt_rsa_sha256_der_len =
    sizeof(mbedtls_test_ca_crt_rsa_sha256_der);

/*
 * Server
 */

const char mbedtls_test_srv_crt_ec_pem[]           = TEST_SRV_CRT_EC_PEM;
const char mbedtls_test_srv_key_ec_pem[]           = TEST_SRV_KEY_EC_PEM;
const char mbedtls_test_srv_pwd_ec_pem[]           = "";
const char mbedtls_test_srv_key_rsa_pem[]          = TEST_SRV_KEY_RSA_PEM;
const char mbedtls_test_srv_pwd_rsa_pem[]          = "";
const char mbedtls_test_srv_crt_rsa_sha1_pem[]     = TEST_SRV_CRT_RSA_SHA1_PEM;
const char mbedtls_test_srv_crt_rsa_sha256_pem[]   = TEST_SRV_CRT_RSA_SHA256_PEM;

const unsigned char mbedtls_test_srv_crt_ec_der[]   = TEST_SRV_CRT_EC_DER;
const unsigned char mbedtls_test_srv_key_ec_der[]   = TEST_SRV_KEY_EC_DER;
const unsigned char mbedtls_test_srv_key_rsa_der[]  = TEST_SRV_KEY_RSA_DER;
const unsigned char mbedtls_test_srv_crt_rsa_sha1_der[]   =
    TEST_SRV_CRT_RSA_SHA1_DER;
const unsigned char mbedtls_test_srv_crt_rsa_sha256_der[] =
    TEST_SRV_CRT_RSA_SHA256_DER;

const size_t mbedtls_test_srv_crt_ec_pem_len =
    sizeof(mbedtls_test_srv_crt_ec_pem);
const size_t mbedtls_test_srv_key_ec_pem_len =
    sizeof(mbedtls_test_srv_key_ec_pem);
const size_t mbedtls_test_srv_pwd_ec_pem_len =
    sizeof(mbedtls_test_srv_pwd_ec_pem) - 1;
const size_t mbedtls_test_srv_key_rsa_pem_len =
    sizeof(mbedtls_test_srv_key_rsa_pem);
const size_t mbedtls_test_srv_pwd_rsa_pem_len =
    sizeof(mbedtls_test_srv_pwd_rsa_pem) - 1;
const size_t mbedtls_test_srv_crt_rsa_sha1_pem_len =
    sizeof(mbedtls_test_srv_crt_rsa_sha1_pem);
const size_t mbedtls_test_srv_crt_rsa_sha256_pem_len =
    sizeof(mbedtls_test_srv_crt_rsa_sha256_pem);

const size_t mbedtls_test_srv_crt_ec_der_len =
    sizeof(mbedtls_test_srv_crt_ec_der);
const size_t mbedtls_test_srv_key_ec_der_len =
    sizeof(mbedtls_test_srv_key_ec_der);
const size_t mbedtls_test_srv_pwd_ec_der_len = 0;
const size_t mbedtls_test_srv_key_rsa_der_len =
    sizeof(mbedtls_test_srv_key_rsa_der);
const size_t mbedtls_test_srv_pwd_rsa_der_len = 0;
const size_t mbedtls_test_srv_crt_rsa_sha1_der_len =
    sizeof(mbedtls_test_srv_crt_rsa_sha1_der);
const size_t mbedtls_test_srv_crt_rsa_sha256_der_len =
    sizeof(mbedtls_test_srv_crt_rsa_sha256_der);

/*
 * Client
 */

const char mbedtls_test_cli_crt_ec_pem[]   = TEST_CLI_CRT_EC_PEM;
const char mbedtls_test_cli_key_ec_pem[]   = TEST_CLI_KEY_EC_PEM;
const char mbedtls_test_cli_pwd_ec_pem[]   = "";
const char mbedtls_test_cli_key_rsa_pem[]  = TEST_CLI_KEY_RSA_PEM;
const char mbedtls_test_cli_pwd_rsa_pem[]  = "";
const char mbedtls_test_cli_crt_rsa_pem[]  = TEST_CLI_CRT_RSA_PEM;

const unsigned char mbedtls_test_cli_crt_ec_der[]   = TEST_CLI_CRT_EC_DER;
const unsigned char mbedtls_test_cli_key_ec_der[]   = TEST_CLI_KEY_EC_DER;
const unsigned char mbedtls_test_cli_key_rsa_der[]  = TEST_CLI_KEY_RSA_DER;
const unsigned char mbedtls_test_cli_crt_rsa_der[]  = TEST_CLI_CRT_RSA_DER;

const size_t mbedtls_test_cli_crt_ec_pem_len =
    sizeof(mbedtls_test_cli_crt_ec_pem);
const size_t mbedtls_test_cli_key_ec_pem_len =
    sizeof(mbedtls_test_cli_key_ec_pem);
const size_t mbedtls_test_cli_pwd_ec_pem_len =
    sizeof(mbedtls_test_cli_pwd_ec_pem) - 1;
const size_t mbedtls_test_cli_key_rsa_pem_len =
    sizeof(mbedtls_test_cli_key_rsa_pem);
const size_t mbedtls_test_cli_pwd_rsa_pem_len =
    sizeof(mbedtls_test_cli_pwd_rsa_pem) - 1;
const size_t mbedtls_test_cli_crt_rsa_pem_len =
    sizeof(mbedtls_test_cli_crt_rsa_pem);

const size_t mbedtls_test_cli_crt_ec_der_len =
    sizeof(mbedtls_test_cli_crt_ec_der);
const size_t mbedtls_test_cli_key_ec_der_len =
    sizeof(mbedtls_test_cli_key_ec_der);
const size_t mbedtls_test_cli_key_rsa_der_len =
    sizeof(mbedtls_test_cli_key_rsa_der);
const size_t mbedtls_test_cli_crt_rsa_der_len =
    sizeof(mbedtls_test_cli_crt_rsa_der);

/*
 *
 * Definitions of test CRTs without specification of all parameters, choosing
 * them automatically according to the config. For example, mbedtls_test_ca_crt
 * is one of mbedtls_test_ca_crt_{rsa|ec}_{sha1|sha256}_{pem|der}.
 *
 */

/*
 * Dispatch between PEM and DER according to config
 */

#if defined(MBEDTLS_PEM_PARSE_C)

/* PEM encoded test CA certificates and keys */

#define TEST_CA_KEY_RSA        TEST_CA_KEY_RSA_PEM
#define TEST_CA_PWD_RSA        TEST_CA_PWD_RSA_PEM
#define TEST_CA_CRT_RSA_SHA256 TEST_CA_CRT_RSA_SHA256_PEM
#define TEST_CA_CRT_RSA_SHA1   TEST_CA_CRT_RSA_SHA1_PEM
#define TEST_CA_KEY_EC         TEST_CA_KEY_EC_PEM
#define TEST_CA_PWD_EC         TEST_CA_PWD_EC_PEM
#define TEST_CA_CRT_EC         TEST_CA_CRT_EC_PEM

/* PEM encoded test server certificates and keys */

#define TEST_SRV_KEY_RSA        TEST_SRV_KEY_RSA_PEM
#define TEST_SRV_PWD_RSA        ""
#define TEST_SRV_CRT_RSA_SHA256 TEST_SRV_CRT_RSA_SHA256_PEM
#define TEST_SRV_CRT_RSA_SHA1   TEST_SRV_CRT_RSA_SHA1_PEM
#define TEST_SRV_KEY_EC         TEST_SRV_KEY_EC_PEM
#define TEST_SRV_PWD_EC         ""
#define TEST_SRV_CRT_EC         TEST_SRV_CRT_EC_PEM

/* PEM encoded test client certificates and keys */

#define TEST_CLI_KEY_RSA  TEST_CLI_KEY_RSA_PEM
#define TEST_CLI_PWD_RSA  ""
#define TEST_CLI_CRT_RSA  TEST_CLI_CRT_RSA_PEM
#define TEST_CLI_KEY_EC   TEST_CLI_KEY_EC_PEM
#define TEST_CLI_PWD_EC   ""
#define TEST_CLI_CRT_EC   TEST_CLI_CRT_EC_PEM

#else /* MBEDTLS_PEM_PARSE_C */

/* DER encoded test CA certificates and keys */

#define TEST_CA_KEY_RSA        TEST_CA_KEY_RSA_DER
#define TEST_CA_PWD_RSA        ""
#define TEST_CA_CRT_RSA_SHA256 TEST_CA_CRT_RSA_SHA256_DER
#define TEST_CA_CRT_RSA_SHA1   TEST_CA_CRT_RSA_SHA1_DER
#define TEST_CA_KEY_EC         TEST_CA_KEY_EC_DER
#define TEST_CA_PWD_EC         ""
#define TEST_CA_CRT_EC         TEST_CA_CRT_EC_DER

/* DER encoded test server certificates and keys */

#define TEST_SRV_KEY_RSA        TEST_SRV_KEY_RSA_DER
#define TEST_SRV_PWD_RSA        ""
#define TEST_SRV_CRT_RSA_SHA256 TEST_SRV_CRT_RSA_SHA256_DER
#define TEST_SRV_CRT_RSA_SHA1   TEST_SRV_CRT_RSA_SHA1_DER
#define TEST_SRV_KEY_EC         TEST_SRV_KEY_EC_DER
#define TEST_SRV_PWD_EC         ""
#define TEST_SRV_CRT_EC         TEST_SRV_CRT_EC_DER

/* DER encoded test client certificates and keys */

#define TEST_CLI_KEY_RSA  TEST_CLI_KEY_RSA_DER
#define TEST_CLI_PWD_RSA  ""
#define TEST_CLI_CRT_RSA  TEST_CLI_CRT_RSA_DER
#define TEST_CLI_KEY_EC   TEST_CLI_KEY_EC_DER
#define TEST_CLI_PWD_EC   ""
#define TEST_CLI_CRT_EC   TEST_CLI_CRT_EC_DER

#endif /* MBEDTLS_PEM_PARSE_C */

const char mbedtls_test_ca_key_rsa[]         = TEST_CA_KEY_RSA;
const char mbedtls_test_ca_pwd_rsa[]         = TEST_CA_PWD_RSA;
const char mbedtls_test_ca_crt_rsa_sha256[]  = TEST_CA_CRT_RSA_SHA256;
const char mbedtls_test_ca_crt_rsa_sha1[]    = TEST_CA_CRT_RSA_SHA1;
const char mbedtls_test_ca_key_ec[]          = TEST_CA_KEY_EC;
const char mbedtls_test_ca_pwd_ec[]          = TEST_CA_PWD_EC;
const char mbedtls_test_ca_crt_ec[]          = TEST_CA_CRT_EC;

const char mbedtls_test_srv_key_rsa[]        = TEST_SRV_KEY_RSA;
const char mbedtls_test_srv_pwd_rsa[]        = TEST_SRV_PWD_RSA;
const char mbedtls_test_srv_crt_rsa_sha256[] = TEST_SRV_CRT_RSA_SHA256;
const char mbedtls_test_srv_crt_rsa_sha1[]   = TEST_SRV_CRT_RSA_SHA1;
const char mbedtls_test_srv_key_ec[]         = TEST_SRV_KEY_EC;
const char mbedtls_test_srv_pwd_ec[]         = TEST_SRV_PWD_EC;
const char mbedtls_test_srv_crt_ec[]         = TEST_SRV_CRT_EC;

const char mbedtls_test_cli_key_rsa[]        = TEST_CLI_KEY_RSA;
const char mbedtls_test_cli_pwd_rsa[]        = TEST_CLI_PWD_RSA;
const char mbedtls_test_cli_crt_rsa[]        = TEST_CLI_CRT_RSA;
const char mbedtls_test_cli_key_ec[]         = TEST_CLI_KEY_EC;
const char mbedtls_test_cli_pwd_ec[]         = TEST_CLI_PWD_EC;
const char mbedtls_test_cli_crt_ec[]         = TEST_CLI_CRT_EC;

const size_t mbedtls_test_ca_key_rsa_len =
    sizeof(mbedtls_test_ca_key_rsa);
const size_t mbedtls_test_ca_pwd_rsa_len =
    sizeof(mbedtls_test_ca_pwd_rsa) - 1;
const size_t mbedtls_test_ca_crt_rsa_sha256_len =
    sizeof(mbedtls_test_ca_crt_rsa_sha256);
const size_t mbedtls_test_ca_crt_rsa_sha1_len =
    sizeof(mbedtls_test_ca_crt_rsa_sha1);
const size_t mbedtls_test_ca_key_ec_len =
    sizeof(mbedtls_test_ca_key_ec);
const size_t mbedtls_test_ca_pwd_ec_len =
    sizeof(mbedtls_test_ca_pwd_ec) - 1;
const size_t mbedtls_test_ca_crt_ec_len =
    sizeof(mbedtls_test_ca_crt_ec);

const size_t mbedtls_test_srv_key_rsa_len =
    sizeof(mbedtls_test_srv_key_rsa);
const size_t mbedtls_test_srv_pwd_rsa_len =
    sizeof(mbedtls_test_srv_pwd_rsa) -1;
const size_t mbedtls_test_srv_crt_rsa_sha256_len =
    sizeof(mbedtls_test_srv_crt_rsa_sha256);
const size_t mbedtls_test_srv_crt_rsa_sha1_len =
    sizeof(mbedtls_test_srv_crt_rsa_sha1);
const size_t mbedtls_test_srv_key_ec_len =
    sizeof(mbedtls_test_srv_key_ec);
const size_t mbedtls_test_srv_pwd_ec_len =
    sizeof(mbedtls_test_srv_pwd_ec) - 1;
const size_t mbedtls_test_srv_crt_ec_len =
    sizeof(mbedtls_test_srv_crt_ec);

const size_t mbedtls_test_cli_key_rsa_len =
    sizeof(mbedtls_test_cli_key_rsa);
const size_t mbedtls_test_cli_pwd_rsa_len =
    sizeof(mbedtls_test_cli_pwd_rsa) - 1;
const size_t mbedtls_test_cli_crt_rsa_len =
    sizeof(mbedtls_test_cli_crt_rsa);
const size_t mbedtls_test_cli_key_ec_len =
    sizeof(mbedtls_test_cli_key_ec);
const size_t mbedtls_test_cli_pwd_ec_len =
    sizeof(mbedtls_test_cli_pwd_ec) - 1;
const size_t mbedtls_test_cli_crt_ec_len =
    sizeof(mbedtls_test_cli_crt_ec);

/*
 * Dispatch between SHA-1 and SHA-256
 */

#if defined(PSA_WANT_ALG_SHA_256)
#define TEST_CA_CRT_RSA  TEST_CA_CRT_RSA_SHA256
#define TEST_SRV_CRT_RSA TEST_SRV_CRT_RSA_SHA256
#else
#define TEST_CA_CRT_RSA  TEST_CA_CRT_RSA_SHA1
#define TEST_SRV_CRT_RSA TEST_SRV_CRT_RSA_SHA1
#endif /* PSA_WANT_ALG_SHA_256 */

const char mbedtls_test_ca_crt_rsa[]  = TEST_CA_CRT_RSA;
const char mbedtls_test_srv_crt_rsa[] = TEST_SRV_CRT_RSA;

const size_t mbedtls_test_ca_crt_rsa_len =
    sizeof(mbedtls_test_ca_crt_rsa);
const size_t mbedtls_test_srv_crt_rsa_len =
    sizeof(mbedtls_test_srv_crt_rsa);

/*
 * Dispatch between RSA and EC
 */

#if defined(MBEDTLS_RSA_C)

#define TEST_CA_KEY TEST_CA_KEY_RSA
#define TEST_CA_PWD TEST_CA_PWD_RSA
#define TEST_CA_CRT TEST_CA_CRT_RSA

#define TEST_SRV_KEY TEST_SRV_KEY_RSA
#define TEST_SRV_PWD TEST_SRV_PWD_RSA
#define TEST_SRV_CRT TEST_SRV_CRT_RSA

#define TEST_CLI_KEY TEST_CLI_KEY_RSA
#define TEST_CLI_PWD TEST_CLI_PWD_RSA
#define TEST_CLI_CRT TEST_CLI_CRT_RSA

#else /* no RSA, so assume ECDSA */

#define TEST_CA_KEY TEST_CA_KEY_EC
#define TEST_CA_PWD TEST_CA_PWD_EC
#define TEST_CA_CRT TEST_CA_CRT_EC

#define TEST_SRV_KEY TEST_SRV_KEY_EC
#define TEST_SRV_PWD TEST_SRV_PWD_EC
#define TEST_SRV_CRT TEST_SRV_CRT_EC

#define TEST_CLI_KEY TEST_CLI_KEY_EC
#define TEST_CLI_PWD TEST_CLI_PWD_EC
#define TEST_CLI_CRT TEST_CLI_CRT_EC
#endif /* MBEDTLS_RSA_C */

/* API stability forces us to declare
 *   mbedtls_test_{ca|srv|cli}_{key|pwd|crt}
 * as pointers. */
static const char test_ca_key[] = TEST_CA_KEY;
static const char test_ca_pwd[] = TEST_CA_PWD;
static const char test_ca_crt[] = TEST_CA_CRT;

static const char test_srv_key[] = TEST_SRV_KEY;
static const char test_srv_pwd[] = TEST_SRV_PWD;
static const char test_srv_crt[] = TEST_SRV_CRT;

static const char test_cli_key[] = TEST_CLI_KEY;
static const char test_cli_pwd[] = TEST_CLI_PWD;
static const char test_cli_crt[] = TEST_CLI_CRT;

const char *mbedtls_test_ca_key = test_ca_key;
const char *mbedtls_test_ca_pwd = test_ca_pwd;
const char *mbedtls_test_ca_crt = test_ca_crt;

const char *mbedtls_test_srv_key = test_srv_key;
const char *mbedtls_test_srv_pwd = test_srv_pwd;
const char *mbedtls_test_srv_crt = test_srv_crt;

const char *mbedtls_test_cli_key = test_cli_key;
const char *mbedtls_test_cli_pwd = test_cli_pwd;
const char *mbedtls_test_cli_crt = test_cli_crt;

const size_t mbedtls_test_ca_key_len =
    sizeof(test_ca_key);
const size_t mbedtls_test_ca_pwd_len =
    sizeof(test_ca_pwd) - 1;
const size_t mbedtls_test_ca_crt_len =
    sizeof(test_ca_crt);

const size_t mbedtls_test_srv_key_len =
    sizeof(test_srv_key);
const size_t mbedtls_test_srv_pwd_len =
    sizeof(test_srv_pwd) - 1;
const size_t mbedtls_test_srv_crt_len =
    sizeof(test_srv_crt);

const size_t mbedtls_test_cli_key_len =
    sizeof(test_cli_key);
const size_t mbedtls_test_cli_pwd_len =
    sizeof(test_cli_pwd) - 1;
const size_t mbedtls_test_cli_crt_len =
    sizeof(test_cli_crt);

/*
 *
 * Lists of certificates
 *
 */

/* List of CAs in PEM or DER, depending on config */
const char *mbedtls_test_cas[] = {
#if defined(MBEDTLS_RSA_C) && defined(PSA_WANT_ALG_SHA_1)
    mbedtls_test_ca_crt_rsa_sha1,
#endif
#if defined(MBEDTLS_RSA_C) && defined(PSA_WANT_ALG_SHA_256)
    mbedtls_test_ca_crt_rsa_sha256,
#endif
#if defined(PSA_HAVE_ALG_SOME_ECDSA)
    mbedtls_test_ca_crt_ec,
#endif
    NULL
};
const size_t mbedtls_test_cas_len[] = {
#if defined(MBEDTLS_RSA_C) && defined(PSA_WANT_ALG_SHA_1)
    sizeof(mbedtls_test_ca_crt_rsa_sha1),
#endif
#if defined(MBEDTLS_RSA_C) && defined(PSA_WANT_ALG_SHA_256)
    sizeof(mbedtls_test_ca_crt_rsa_sha256),
#endif
#if defined(PSA_HAVE_ALG_SOME_ECDSA)
    sizeof(mbedtls_test_ca_crt_ec),
#endif
    0
};

/* List of all available CA certificates in DER format */
const unsigned char *mbedtls_test_cas_der[] = {
#if defined(MBEDTLS_RSA_C)
#if defined(PSA_WANT_ALG_SHA_256)
    mbedtls_test_ca_crt_rsa_sha256_der,
#endif /* PSA_WANT_ALG_SHA_256 */
#if defined(PSA_WANT_ALG_SHA_1)
    mbedtls_test_ca_crt_rsa_sha1_der,
#endif /* PSA_WANT_ALG_SHA_1 */
#endif /* MBEDTLS_RSA_C */
#if defined(PSA_HAVE_ALG_SOME_ECDSA) && defined(PSA_WANT_ECC_SECP_R1_384)
    mbedtls_test_ca_crt_ec_der,
#endif /* PSA_HAVE_ALG_SOME_ECDSA */
    NULL
};

const size_t mbedtls_test_cas_der_len[] = {
#if defined(MBEDTLS_RSA_C)
#if defined(PSA_WANT_ALG_SHA_256)
    sizeof(mbedtls_test_ca_crt_rsa_sha256_der),
#endif /* PSA_WANT_ALG_SHA_256 */
#if defined(PSA_WANT_ALG_SHA_1)
    sizeof(mbedtls_test_ca_crt_rsa_sha1_der),
#endif /* PSA_WANT_ALG_SHA_1 */
#endif /* MBEDTLS_RSA_C */
#if defined(PSA_HAVE_ALG_SOME_ECDSA) && defined(PSA_WANT_ECC_SECP_R1_384)
    sizeof(mbedtls_test_ca_crt_ec_der),
#endif /* PSA_HAVE_ALG_SOME_ECDSA */
    0
};

/* Concatenation of all available CA certificates in PEM format */
#if defined(MBEDTLS_PEM_PARSE_C)
const char mbedtls_test_cas_pem[] =
#if defined(MBEDTLS_RSA_C)
#if defined(PSA_WANT_ALG_SHA_256)
    TEST_CA_CRT_RSA_SHA256_PEM
#endif /* PSA_WANT_ALG_SHA_256 */
#if defined(PSA_WANT_ALG_SHA_1)
    TEST_CA_CRT_RSA_SHA1_PEM
#endif /* PSA_WANT_ALG_SHA_1 */
#endif /* MBEDTLS_RSA_C */
#if defined(PSA_HAVE_ALG_SOME_ECDSA)
    TEST_CA_CRT_EC_PEM
#endif /* PSA_HAVE_ALG_SOME_ECDSA */
    "";
const size_t mbedtls_test_cas_pem_len = sizeof(mbedtls_test_cas_pem);
#endif /* MBEDTLS_PEM_PARSE_C */
