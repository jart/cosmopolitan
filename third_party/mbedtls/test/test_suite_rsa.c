/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "third_party/mbedtls/test/test.inc"
/*
 * *** THIS FILE WAS MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * Test file      : ./test_suite_rsa.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_rsa.function
 *      Test suite data     : suites/test_suite_rsa.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_BIGNUM_C)
#if defined(MBEDTLS_GENPRIME)
#include "third_party/mbedtls/rsa.h"
#include "third_party/mbedtls/rsa_internal.h"
#include "third_party/mbedtls/md5.h"
#include "third_party/mbedtls/sha1.h"
#include "third_party/mbedtls/sha256.h"
#include "third_party/mbedtls/sha512.h"
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/ctr_drbg.h"

#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_rsa_invalid_param( )
{
    mbedtls_rsa_context ctx;
    const int valid_padding = MBEDTLS_RSA_PKCS_V21;
    const int invalid_padding = 42;
    const int valid_mode = MBEDTLS_RSA_PRIVATE;
    const int invalid_mode = 42;
    unsigned char buf[42] = { 0 };
    size_t olen;

    TEST_INVALID_PARAM( mbedtls_rsa_init( NULL, valid_padding, 0 ) );
    TEST_INVALID_PARAM( mbedtls_rsa_init( &ctx, invalid_padding, 0 ) );
    TEST_VALID_PARAM( mbedtls_rsa_free( NULL ) );

    /* No more variants because only the first argument must be non-NULL. */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_import( NULL, NULL, NULL,
                                                NULL, NULL, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_import_raw( NULL,
                                                    NULL, 0,
                                                    NULL, 0,
                                                    NULL, 0,
                                                    NULL, 0,
                                                    NULL, 0 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_complete( NULL ) );

    /* No more variants because only the first argument must be non-NULL. */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_export( NULL, NULL, NULL,
                                                NULL, NULL, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_export_raw( NULL,
                                                    NULL, 0,
                                                    NULL, 0,
                                                    NULL, 0,
                                                    NULL, 0,
                                                    NULL, 0 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_export_crt( NULL, NULL, NULL, NULL ) );

    TEST_INVALID_PARAM( mbedtls_rsa_set_padding( NULL,
                                                 valid_padding, 0 ) );
    TEST_INVALID_PARAM( mbedtls_rsa_set_padding( &ctx,
                                                 invalid_padding, 0 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_gen_key( NULL,
                                                 mbedtls_test_rnd_std_rand,
                                                 NULL, 0, 0 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_gen_key( &ctx, NULL,
                                                 NULL, 0, 0 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_check_pubkey( NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_check_privkey( NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_check_pub_priv( NULL, &ctx ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_check_pub_priv( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_public( NULL, buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_public( &ctx, NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_public( &ctx, buf, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_private( NULL, NULL, NULL,
                                                 buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_private( &ctx, NULL, NULL,
                                                 NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_private( &ctx, NULL, NULL,
                                                 buf, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_encrypt( NULL, NULL, NULL,
                                                       valid_mode,
                                                       sizeof( buf ), buf,
                                                       buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_encrypt( &ctx, NULL, NULL,
                                                       invalid_mode,
                                                       sizeof( buf ), buf,
                                                       buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_encrypt( &ctx, NULL, NULL,
                                                       valid_mode,
                                                       sizeof( buf ), NULL,
                                                       buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_encrypt( &ctx, NULL, NULL,
                                                       valid_mode,
                                                       sizeof( buf ), buf,
                                                       NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_pkcs1_v15_encrypt( NULL, NULL,
                                                           NULL,
                                                           valid_mode,
                                                           sizeof( buf ), buf,
                                                           buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_pkcs1_v15_encrypt( &ctx, NULL,
                                                           NULL,
                                                           invalid_mode,
                                                           sizeof( buf ), buf,
                                                           buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_pkcs1_v15_encrypt( &ctx, NULL,
                                                           NULL,
                                                           valid_mode,
                                                           sizeof( buf ), NULL,
                                                           buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_pkcs1_v15_encrypt( &ctx, NULL,
                                                           NULL,
                                                           valid_mode,
                                                           sizeof( buf ), buf,
                                                           NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_oaep_encrypt( NULL, NULL, NULL,
                                                            valid_mode,
                                                            buf, sizeof( buf ),
                                                            sizeof( buf ), buf,
                                                            buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_oaep_encrypt( &ctx, NULL, NULL,
                                                            invalid_mode,
                                                            buf, sizeof( buf ),
                                                            sizeof( buf ), buf,
                                                            buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_oaep_encrypt( &ctx, NULL, NULL,
                                                            valid_mode,
                                                            NULL, sizeof( buf ),
                                                            sizeof( buf ), buf,
                                                            buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_oaep_encrypt( &ctx, NULL, NULL,
                                                            valid_mode,
                                                            buf, sizeof( buf ),
                                                            sizeof( buf ), NULL,
                                                            buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_oaep_encrypt( &ctx, NULL, NULL,
                                                            valid_mode,
                                                            buf, sizeof( buf ),
                                                            sizeof( buf ), buf,
                                                            NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_decrypt( NULL, NULL, NULL,
                                                       valid_mode, &olen,
                                                       buf, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_decrypt( &ctx, NULL, NULL,
                                                       invalid_mode, &olen,
                                                       buf, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_decrypt( &ctx, NULL, NULL,
                                                       valid_mode, NULL,
                                                       buf, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_decrypt( &ctx, NULL, NULL,
                                                       valid_mode, &olen,
                                                       NULL, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_decrypt( &ctx, NULL, NULL,
                                                       valid_mode, &olen,
                                                       buf, NULL, 42 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_pkcs1_v15_decrypt( NULL, NULL,
                                                           NULL,
                                                           valid_mode, &olen,
                                                           buf, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_pkcs1_v15_decrypt( &ctx, NULL,
                                                           NULL,
                                                           invalid_mode, &olen,
                                                           buf, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_pkcs1_v15_decrypt( &ctx, NULL,
                                                           NULL,
                                                           valid_mode, NULL,
                                                           buf, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_pkcs1_v15_decrypt( &ctx, NULL,
                                                           NULL,
                                                           valid_mode, &olen,
                                                           NULL, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_pkcs1_v15_decrypt( &ctx, NULL,
                                                           NULL,
                                                           valid_mode, &olen,
                                                           buf, NULL, 42 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_oaep_decrypt( NULL, NULL, NULL,
                                                            valid_mode,
                                                            buf, sizeof( buf ),
                                                            &olen,
                                                            buf, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_oaep_decrypt( &ctx, NULL, NULL,
                                                            invalid_mode,
                                                            buf, sizeof( buf ),
                                                            &olen,
                                                            buf, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_oaep_decrypt( &ctx, NULL, NULL,
                                                            valid_mode,
                                                            NULL, sizeof( buf ),
                                                            NULL,
                                                            buf, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_oaep_decrypt( &ctx, NULL, NULL,
                                                            valid_mode,
                                                            buf, sizeof( buf ),
                                                            &olen,
                                                            NULL, buf, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsaes_oaep_decrypt( &ctx, NULL, NULL,
                                                            valid_mode,
                                                            buf, sizeof( buf ),
                                                            &olen,
                                                            buf, NULL, 42 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_sign( NULL, NULL, NULL,
                                                    valid_mode,
                                                    0, sizeof( buf ), buf,
                                                    buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_sign( &ctx, NULL, NULL,
                                                    invalid_mode,
                                                    0, sizeof( buf ), buf,
                                                    buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_sign( &ctx, NULL, NULL,
                                                    valid_mode,
                                                    0, sizeof( buf ), NULL,
                                                    buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_sign( &ctx, NULL, NULL,
                                                    valid_mode,
                                                    0, sizeof( buf ), buf,
                                                    NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_sign( &ctx, NULL, NULL,
                                                    valid_mode,
                                                    MBEDTLS_MD_SHA1,
                                                    0, NULL,
                                                    buf ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pkcs1_v15_sign( NULL, NULL, NULL,
                                                        valid_mode,
                                                        0, sizeof( buf ), buf,
                                                        buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pkcs1_v15_sign( &ctx, NULL, NULL,
                                                        invalid_mode,
                                                        0, sizeof( buf ), buf,
                                                        buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pkcs1_v15_sign( &ctx, NULL, NULL,
                                                        valid_mode,
                                                        0, sizeof( buf ), NULL,
                                                        buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pkcs1_v15_sign( &ctx, NULL, NULL,
                                                        valid_mode,
                                                        0, sizeof( buf ), buf,
                                                        NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pkcs1_v15_sign( &ctx, NULL, NULL,
                                                        valid_mode,
                                                        MBEDTLS_MD_SHA1,
                                                        0, NULL,
                                                        buf ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_sign( NULL, NULL, NULL,
                                                         valid_mode,
                                                         0, sizeof( buf ), buf,
                                                         buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_sign( &ctx, NULL, NULL,
                                                         invalid_mode,
                                                         0, sizeof( buf ), buf,
                                                         buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_sign( &ctx, NULL, NULL,
                                                         valid_mode,
                                                         0, sizeof( buf ), NULL,
                                                         buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_sign( &ctx, NULL, NULL,
                                                         valid_mode,
                                                         0, sizeof( buf ), buf,
                                                         NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_sign( &ctx, NULL, NULL,
                                                         valid_mode,
                                                         MBEDTLS_MD_SHA1,
                                                         0, NULL,
                                                         buf ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_verify( NULL, NULL, NULL,
                                                      valid_mode,
                                                      0, sizeof( buf ), buf,
                                                      buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_verify( &ctx, NULL, NULL,
                                                      invalid_mode,
                                                      0, sizeof( buf ), buf,
                                                      buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_verify( &ctx, NULL, NULL,
                                                      valid_mode,
                                                      0, sizeof( buf ), NULL,
                                                      buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_verify( &ctx, NULL, NULL,
                                                      valid_mode,
                                                      0, sizeof( buf ), buf,
                                                      NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_pkcs1_verify( &ctx, NULL, NULL,
                                                      valid_mode,
                                                      MBEDTLS_MD_SHA1, 0, NULL,
                                                      buf ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pkcs1_v15_verify( NULL, NULL,
                                                          NULL,
                                                          valid_mode,
                                                          0, sizeof( buf ), buf,
                                                          buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pkcs1_v15_verify( &ctx, NULL,
                                                          NULL,
                                                          invalid_mode,
                                                          0, sizeof( buf ), buf,
                                                          buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pkcs1_v15_verify( &ctx, NULL,
                                                          NULL,
                                                          valid_mode,
                                                          0, sizeof( buf ),
                                                          NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pkcs1_v15_verify( &ctx, NULL,
                                                          NULL,
                                                          valid_mode,
                                                          0, sizeof( buf ), buf,
                                                          NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pkcs1_v15_verify( &ctx, NULL,
                                                          NULL,
                                                          valid_mode,
                                                          MBEDTLS_MD_SHA1,
                                                          0, NULL,
                                                          buf ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_verify( NULL, NULL, NULL,
                                                           valid_mode,
                                                           0, sizeof( buf ),
                                                           buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_verify( &ctx, NULL, NULL,
                                                           invalid_mode,
                                                           0, sizeof( buf ),
                                                           buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_verify( &ctx, NULL, NULL,
                                                           valid_mode,
                                                           0, sizeof( buf ),
                                                           NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_verify( &ctx, NULL, NULL,
                                                           valid_mode,
                                                           0, sizeof( buf ),
                                                           buf, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_verify( &ctx, NULL, NULL,
                                                           valid_mode,
                                                           MBEDTLS_MD_SHA1,
                                                           0, NULL,
                                                           buf ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_verify_ext( NULL, NULL, NULL,
                                                               valid_mode,
                                                               0, sizeof( buf ),
                                                               buf,
                                                               0, 0,
                                                               buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_verify_ext( &ctx, NULL, NULL,
                                                               invalid_mode,
                                                               0, sizeof( buf ),
                                                               buf,
                                                               0, 0,
                                                               buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_verify_ext( &ctx, NULL, NULL,
                                                               valid_mode,
                                                               0, sizeof( buf ),
                                                               NULL, 0, 0,
                                                               buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_verify_ext( &ctx, NULL, NULL,
                                                               valid_mode,
                                                               0, sizeof( buf ),
                                                               buf, 0, 0,
                                                               NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_rsassa_pss_verify_ext( &ctx, NULL, NULL,
                                                               valid_mode,
                                                               MBEDTLS_MD_SHA1,
                                                               0, NULL,
                                                               0, 0,
                                                               buf ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_copy( NULL, &ctx ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_RSA_BAD_INPUT_DATA,
                            mbedtls_rsa_copy( &ctx, NULL ) );

exit:
    return;
}

void test_rsa_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_rsa_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_rsa_init_free( int reinit )
{
    mbedtls_rsa_context ctx;

    /* Double free is not explicitly documented to work, but we rely on it
     * even inside the library so that you can call mbedtls_rsa_free()
     * unconditionally on an error path without checking whether it has
     * already been called in the success path. */

    mbedtls_rsa_init( &ctx, 0, 0 );
    mbedtls_rsa_free( &ctx );

    if( reinit )
        mbedtls_rsa_init( &ctx, 0, 0 );
    mbedtls_rsa_free( &ctx );

    /* This test case always succeeds, functionally speaking. A plausible
     * bug might trigger an invalid pointer dereference or a memory leak. */
    goto exit;
exit:
    ;
}

void test_rsa_init_free_wrapper( void ** params )
{

    test_rsa_init_free( *( (int *) params[0] ) );
}
void test_mbedtls_rsa_pkcs1_sign( data_t * message_str, int padding_mode,
                             int digest, int mod, int radix_P, char * input_P,
                             int radix_Q, char * input_Q, int radix_N,
                             char * input_N, int radix_E, char * input_E,
                             data_t * result_str, int result )
{
    unsigned char hash_result[MBEDTLS_MD_MAX_SIZE];
    unsigned char output[256];
    mbedtls_rsa_context ctx;
    mbedtls_mpi N, P, Q, E;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, padding_mode, 0 );

    memset( hash_result, 0x00, sizeof( hash_result ) );
    memset( output, 0x00, sizeof( output ) );
    memset( &rnd_info, 0, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, &P, &Q, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( mod / 8 ) );
    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );
    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == 0 );


    if( mbedtls_md_info_from_type( digest ) != NULL )
        TEST_ASSERT( mbedtls_md( mbedtls_md_info_from_type( digest ), message_str->x, message_str->len, hash_result ) == 0 );

    TEST_ASSERT( mbedtls_rsa_pkcs1_sign( &ctx, &mbedtls_test_rnd_pseudo_rand,
                                         &rnd_info, MBEDTLS_RSA_PRIVATE, digest,
                                         0, hash_result, output ) == result );
    if( result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                          ctx.len, result_str->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &P );
    mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_mbedtls_rsa_pkcs1_sign_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data13 = {(uint8_t *) params[13], *( (uint32_t *) params[14] )};

    test_mbedtls_rsa_pkcs1_sign( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ), (char *) params[8], *( (int *) params[9] ), (char *) params[10], *( (int *) params[11] ), (char *) params[12], &data13, *( (int *) params[15] ) );
}
void test_mbedtls_rsa_pkcs1_verify( data_t * message_str, int padding_mode,
                               int digest, int mod, int radix_N,
                               char * input_N, int radix_E, char * input_E,
                               data_t * result_str, int result )
{
    unsigned char hash_result[MBEDTLS_MD_MAX_SIZE];
    mbedtls_rsa_context ctx;

    mbedtls_mpi N, E;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, padding_mode, 0 );
    memset( hash_result, 0x00, sizeof( hash_result ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( mod / 8 ) );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == 0 );


    if( mbedtls_md_info_from_type( digest ) != NULL )
        TEST_ASSERT( mbedtls_md( mbedtls_md_info_from_type( digest ), message_str->x, message_str->len, hash_result ) == 0 );

    TEST_ASSERT( mbedtls_rsa_pkcs1_verify( &ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, digest, 0, hash_result, result_str->x ) == result );

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_mbedtls_rsa_pkcs1_verify_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_mbedtls_rsa_pkcs1_verify( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ), (char *) params[8], &data9, *( (int *) params[11] ) );
}
void test_rsa_pkcs1_sign_raw( data_t * hash_result,
                         int padding_mode, int mod, int radix_P,
                         char * input_P, int radix_Q, char * input_Q,
                         int radix_N, char * input_N, int radix_E,
                         char * input_E, data_t * result_str )
{
    unsigned char output[256];
    mbedtls_rsa_context ctx;
    mbedtls_mpi N, P, Q, E;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_rsa_init( &ctx, padding_mode, 0 );
    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &E );

    memset( output, 0x00, sizeof( output ) );
    memset( &rnd_info, 0, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, &P, &Q, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( mod / 8 ) );
    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );
    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == 0 );


    TEST_ASSERT( mbedtls_rsa_pkcs1_sign( &ctx, &mbedtls_test_rnd_pseudo_rand,
                                         &rnd_info, MBEDTLS_RSA_PRIVATE,
                                         MBEDTLS_MD_NONE, hash_result->len,
                                         hash_result->x, output ) == 0 );


    TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                      ctx.len, result_str->len ) == 0 );

#if defined(MBEDTLS_PKCS1_V15)
    /* For PKCS#1 v1.5, there is an alternative way to generate signatures */
    if( padding_mode == MBEDTLS_RSA_PKCS_V15 )
    {
        int res;
        memset( output, 0x00, sizeof( output) );

        res = mbedtls_rsa_rsaes_pkcs1_v15_encrypt( &ctx,
                  &mbedtls_test_rnd_pseudo_rand, &rnd_info,
                  MBEDTLS_RSA_PRIVATE, hash_result->len,
                  hash_result->x, output );

#if !defined(MBEDTLS_RSA_ALT)
        TEST_ASSERT( res == 0 );
#else
        TEST_ASSERT( ( res == 0 ) ||
                     ( res == MBEDTLS_ERR_RSA_UNSUPPORTED_OPERATION ) );
#endif

        if( res == 0 )
        {
            TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                              ctx.len,
                                              result_str->len ) == 0 );
        }
    }
#endif /* MBEDTLS_PKCS1_V15 */

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &P );
    mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &E );

    mbedtls_rsa_free( &ctx );
}

void test_rsa_pkcs1_sign_raw_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_rsa_pkcs1_sign_raw( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], *( (int *) params[8] ), (char *) params[9], *( (int *) params[10] ), (char *) params[11], &data12 );
}
void test_rsa_pkcs1_verify_raw( data_t * hash_result,
                           int padding_mode, int mod, int radix_N,
                           char * input_N, int radix_E, char * input_E,
                           data_t * result_str, int correct )
{
    unsigned char output[256];
    mbedtls_rsa_context ctx;

    mbedtls_mpi N, E;
    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );

    mbedtls_rsa_init( &ctx, padding_mode, 0 );
    memset( output, 0x00, sizeof( output ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( mod / 8 ) );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == 0 );


    TEST_ASSERT( mbedtls_rsa_pkcs1_verify( &ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_NONE, hash_result->len, hash_result->x, result_str->x ) == correct );

#if defined(MBEDTLS_PKCS1_V15)
    /* For PKCS#1 v1.5, there is an alternative way to verify signatures */
    if( padding_mode == MBEDTLS_RSA_PKCS_V15 )
    {
        int res;
        int ok;
        size_t olen;

        res = mbedtls_rsa_rsaes_pkcs1_v15_decrypt( &ctx,
                    NULL, NULL, MBEDTLS_RSA_PUBLIC,
                    &olen, result_str->x, output, sizeof( output ) );

#if !defined(MBEDTLS_RSA_ALT)
        TEST_ASSERT( res == 0 );
#else
        TEST_ASSERT( ( res == 0 ) ||
                     ( res == MBEDTLS_ERR_RSA_UNSUPPORTED_OPERATION ) );
#endif

        if( res == 0 )
        {
            ok = olen == hash_result->len && memcmp( output, hash_result->x, olen ) == 0;
            if( correct == 0 )
                TEST_ASSERT( ok == 1 );
            else
                TEST_ASSERT( ok == 0 );
        }
    }
#endif /* MBEDTLS_PKCS1_V15 */

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_rsa_pkcs1_verify_raw_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_rsa_pkcs1_verify_raw( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], &data8, *( (int *) params[10] ) );
}
void test_mbedtls_rsa_pkcs1_encrypt( data_t * message_str, int padding_mode,
                                int mod, int radix_N, char * input_N,
                                int radix_E, char * input_E,
                                data_t * result_str, int result )
{
    unsigned char output[256];
    mbedtls_rsa_context ctx;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_mpi N, E;
    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );

    memset( &rnd_info, 0, sizeof( mbedtls_test_rnd_pseudo_info ) );

    mbedtls_rsa_init( &ctx, padding_mode, 0 );
    memset( output, 0x00, sizeof( output ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( mod / 8 ) );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == 0 );


    TEST_ASSERT( mbedtls_rsa_pkcs1_encrypt( &ctx,
                                            &mbedtls_test_rnd_pseudo_rand,
                                            &rnd_info, MBEDTLS_RSA_PUBLIC,
                                            message_str->len, message_str->x,
                                            output ) == result );
    if( result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                          ctx.len, result_str->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_mbedtls_rsa_pkcs1_encrypt_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_rsa_pkcs1_encrypt( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], &data8, *( (int *) params[10] ) );
}
void test_rsa_pkcs1_encrypt_bad_rng( data_t * message_str, int padding_mode,
                                int mod, int radix_N, char * input_N,
                                int radix_E, char * input_E,
                                data_t * result_str, int result )
{
    unsigned char output[256];
    mbedtls_rsa_context ctx;

    mbedtls_mpi N, E;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, padding_mode, 0 );
    memset( output, 0x00, sizeof( output ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( mod / 8 ) );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == 0 );


    TEST_ASSERT( mbedtls_rsa_pkcs1_encrypt( &ctx, &mbedtls_test_rnd_zero_rand,
                                            NULL, MBEDTLS_RSA_PUBLIC,
                                            message_str->len, message_str->x,
                                            output ) == result );
    if( result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                          ctx.len, result_str->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_rsa_pkcs1_encrypt_bad_rng_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_rsa_pkcs1_encrypt_bad_rng( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], &data8, *( (int *) params[10] ) );
}
void test_mbedtls_rsa_pkcs1_decrypt( data_t * message_str, int padding_mode,
                                int mod, int radix_P, char * input_P,
                                int radix_Q, char * input_Q, int radix_N,
                                char * input_N, int radix_E, char * input_E,
                                int max_output, data_t * result_str,
                                int result )
{
    unsigned char output[32];
    mbedtls_rsa_context ctx;
    size_t output_len;
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_mpi N, P, Q, E;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &E );

    mbedtls_rsa_init( &ctx, padding_mode, 0 );

    memset( output, 0x00, sizeof( output ) );
    memset( &rnd_info, 0, sizeof( mbedtls_test_rnd_pseudo_info ) );


    TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, &P, &Q, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( mod / 8 ) );
    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );
    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == 0 );

    output_len = 0;

    TEST_ASSERT( mbedtls_rsa_pkcs1_decrypt( &ctx, mbedtls_test_rnd_pseudo_rand,
                                            &rnd_info, MBEDTLS_RSA_PRIVATE,
                                            &output_len, message_str->x, output,
                                            max_output ) == result );
    if( result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                          output_len,
                                          result_str->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &P );
    mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_mbedtls_rsa_pkcs1_decrypt_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data13 = {(uint8_t *) params[13], *( (uint32_t *) params[14] )};

    test_mbedtls_rsa_pkcs1_decrypt( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], *( (int *) params[8] ), (char *) params[9], *( (int *) params[10] ), (char *) params[11], *( (int *) params[12] ), &data13, *( (int *) params[15] ) );
}
void test_mbedtls_rsa_public( data_t * message_str, int mod, int radix_N,
                         char * input_N, int radix_E, char * input_E,
                         data_t * result_str, int result )
{
    unsigned char output[256];
    mbedtls_rsa_context ctx, ctx2; /* Also test mbedtls_rsa_copy() while at it */

    mbedtls_mpi N, E;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V15, 0 );
    mbedtls_rsa_init( &ctx2, MBEDTLS_RSA_PKCS_V15, 0 );
    memset( output, 0x00, sizeof( output ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( mod / 8 ) );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == 0 );


    TEST_ASSERT( mbedtls_rsa_public( &ctx, message_str->x, output ) == result );
    if( result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                          ctx.len, result_str->len ) == 0 );
    }

    /* And now with the copy */
    TEST_ASSERT( mbedtls_rsa_copy( &ctx2, &ctx ) == 0 );
    /* clear the original to be sure */
    mbedtls_rsa_free( &ctx );

    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx2 ) == 0 );

    memset( output, 0x00, sizeof( output ) );
    TEST_ASSERT( mbedtls_rsa_public( &ctx2, message_str->x, output ) == result );
    if( result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                          ctx.len, result_str->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
    mbedtls_rsa_free( &ctx2 );
}

void test_mbedtls_rsa_public_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_mbedtls_rsa_public( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], &data7, *( (int *) params[9] ) );
}
void test_mbedtls_rsa_private( data_t * message_str, int mod, int radix_P,
                          char * input_P, int radix_Q, char * input_Q,
                          int radix_N, char * input_N, int radix_E,
                          char * input_E, data_t * result_str,
                          int result )
{
    unsigned char output[256];
    mbedtls_rsa_context ctx, ctx2; /* Also test mbedtls_rsa_copy() while at it */
    mbedtls_mpi N, P, Q, E;
    mbedtls_test_rnd_pseudo_info rnd_info;
    int i;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V15, 0 );
    mbedtls_rsa_init( &ctx2, MBEDTLS_RSA_PKCS_V15, 0 );

    memset( &rnd_info, 0, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, &P, &Q, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( mod / 8 ) );
    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );
    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == 0 );


    /* repeat three times to test updating of blinding values */
    for( i = 0; i < 3; i++ )
    {
        memset( output, 0x00, sizeof( output ) );
        TEST_ASSERT( mbedtls_rsa_private( &ctx, mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info, message_str->x,
                                          output ) == result );
        if( result == 0 )
        {

            TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                              ctx.len,
                                              result_str->len ) == 0 );
        }
    }

    /* And now one more time with the copy */
    TEST_ASSERT( mbedtls_rsa_copy( &ctx2, &ctx ) == 0 );
    /* clear the original to be sure */
    mbedtls_rsa_free( &ctx );

    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx2 ) == 0 );

    memset( output, 0x00, sizeof( output ) );
    TEST_ASSERT( mbedtls_rsa_private( &ctx2, mbedtls_test_rnd_pseudo_rand,
                                      &rnd_info, message_str->x,
                                      output ) == result );
    if( result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                          ctx2.len,
                                          result_str->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &P );
    mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &E );

    mbedtls_rsa_free( &ctx ); mbedtls_rsa_free( &ctx2 );
}

void test_mbedtls_rsa_private_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};

    test_mbedtls_rsa_private( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ), (char *) params[8], *( (int *) params[9] ), (char *) params[10], &data11, *( (int *) params[13] ) );
}
void test_rsa_check_privkey_null(  )
{
    mbedtls_rsa_context ctx;
    memset( &ctx, 0x00, sizeof( mbedtls_rsa_context ) );

    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == MBEDTLS_ERR_RSA_KEY_CHECK_FAILED );
exit:
    ;
}

void test_rsa_check_privkey_null_wrapper( void ** params )
{
    (void)params;

    test_rsa_check_privkey_null(  );
}
void test_mbedtls_rsa_check_pubkey( int radix_N, char * input_N, int radix_E,
                               char * input_E, int result )
{
    mbedtls_rsa_context ctx;
    mbedtls_mpi N, E;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V15, 0 );

    if( strlen( input_N ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    }
    if( strlen( input_E ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );
    }

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == result );

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_mbedtls_rsa_check_pubkey_wrapper( void ** params )
{

    test_mbedtls_rsa_check_pubkey( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ) );
}
void test_mbedtls_rsa_check_privkey( int mod, int radix_P, char * input_P,
                                int radix_Q, char * input_Q, int radix_N,
                                char * input_N, int radix_E, char * input_E,
                                int radix_D, char * input_D, int radix_DP,
                                char * input_DP, int radix_DQ,
                                char * input_DQ, int radix_QP,
                                char * input_QP, int result )
{
    mbedtls_rsa_context ctx;

    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V15, 0 );

    ctx.len = mod / 8;
    if( strlen( input_P ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &ctx.P, radix_P, input_P ) == 0 );
    }
    if( strlen( input_Q ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &ctx.Q, radix_Q, input_Q ) == 0 );
    }
    if( strlen( input_N ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &ctx.N, radix_N, input_N ) == 0 );
    }
    if( strlen( input_E ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &ctx.E, radix_E, input_E ) == 0 );
    }
    if( strlen( input_D ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &ctx.D, radix_D, input_D ) == 0 );
    }
#if !defined(MBEDTLS_RSA_NO_CRT)
    if( strlen( input_DP ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &ctx.DP, radix_DP, input_DP ) == 0 );
    }
    if( strlen( input_DQ ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &ctx.DQ, radix_DQ, input_DQ ) == 0 );
    }
    if( strlen( input_QP ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &ctx.QP, radix_QP, input_QP ) == 0 );
    }
#else
    ((void) radix_DP); ((void) input_DP);
    ((void) radix_DQ); ((void) input_DQ);
    ((void) radix_QP); ((void) input_QP);
#endif

    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == result );

exit:
    mbedtls_rsa_free( &ctx );
}

void test_mbedtls_rsa_check_privkey_wrapper( void ** params )
{

    test_mbedtls_rsa_check_privkey( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ), (char *) params[8], *( (int *) params[9] ), (char *) params[10], *( (int *) params[11] ), (char *) params[12], *( (int *) params[13] ), (char *) params[14], *( (int *) params[15] ), (char *) params[16], *( (int *) params[17] ) );
}
void test_rsa_check_pubpriv( int mod, int radix_Npub, char * input_Npub,
                        int radix_Epub, char * input_Epub, int radix_P,
                        char * input_P, int radix_Q, char * input_Q,
                        int radix_N, char * input_N, int radix_E,
                        char * input_E, int radix_D, char * input_D,
                        int radix_DP, char * input_DP, int radix_DQ,
                        char * input_DQ, int radix_QP, char * input_QP,
                        int result )
{
    mbedtls_rsa_context pub, prv;

    mbedtls_rsa_init( &pub, MBEDTLS_RSA_PKCS_V15, 0 );
    mbedtls_rsa_init( &prv, MBEDTLS_RSA_PKCS_V15, 0 );

    pub.len = mod / 8;
    prv.len = mod / 8;

    if( strlen( input_Npub ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &pub.N, radix_Npub, input_Npub ) == 0 );
    }
    if( strlen( input_Epub ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &pub.E, radix_Epub, input_Epub ) == 0 );
    }

    if( strlen( input_P ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &prv.P, radix_P, input_P ) == 0 );
    }
    if( strlen( input_Q ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &prv.Q, radix_Q, input_Q ) == 0 );
    }
    if( strlen( input_N ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &prv.N, radix_N, input_N ) == 0 );
    }
    if( strlen( input_E ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &prv.E, radix_E, input_E ) == 0 );
    }
    if( strlen( input_D ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &prv.D, radix_D, input_D ) == 0 );
    }
#if !defined(MBEDTLS_RSA_NO_CRT)
    if( strlen( input_DP ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &prv.DP, radix_DP, input_DP ) == 0 );
    }
    if( strlen( input_DQ ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &prv.DQ, radix_DQ, input_DQ ) == 0 );
    }
    if( strlen( input_QP ) )
    {
        TEST_ASSERT( mbedtls_mpi_read_string( &prv.QP, radix_QP, input_QP ) == 0 );
    }
#else
    ((void) radix_DP); ((void) input_DP);
    ((void) radix_DQ); ((void) input_DQ);
    ((void) radix_QP); ((void) input_QP);
#endif

    TEST_ASSERT( mbedtls_rsa_check_pub_priv( &pub, &prv ) == result );

exit:
    mbedtls_rsa_free( &pub );
    mbedtls_rsa_free( &prv );
}

void test_rsa_check_pubpriv_wrapper( void ** params )
{

    test_rsa_check_pubpriv( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ), (char *) params[8], *( (int *) params[9] ), (char *) params[10], *( (int *) params[11] ), (char *) params[12], *( (int *) params[13] ), (char *) params[14], *( (int *) params[15] ), (char *) params[16], *( (int *) params[17] ), (char *) params[18], *( (int *) params[19] ), (char *) params[20], *( (int *) params[21] ) );
}
#if defined(MBEDTLS_CTR_DRBG_C)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(ENTROPY_HAVE_STRONG)
void test_mbedtls_rsa_gen_key( int nrbits, int exponent, int result)
{
    mbedtls_rsa_context ctx;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "test_suite_rsa";

    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );
    mbedtls_rsa_init ( &ctx, 0, 0 );

    TEST_ASSERT( mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func,
                                        &entropy, (const unsigned char *) pers,
                                        strlen( pers ) ) == 0 );

    TEST_ASSERT( mbedtls_rsa_gen_key( &ctx, mbedtls_ctr_drbg_random, &ctr_drbg, nrbits, exponent ) == result );
    if( result == 0 )
    {
        TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == 0 );
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &ctx.P, &ctx.Q ) > 0 );
    }

exit:
    mbedtls_rsa_free( &ctx );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );
}

void test_mbedtls_rsa_gen_key_wrapper( void ** params )
{

    test_mbedtls_rsa_gen_key( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* ENTROPY_HAVE_STRONG */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_CTR_DRBG_C */
#if defined(MBEDTLS_CTR_DRBG_C)
#if defined(MBEDTLS_ENTROPY_C)
void test_mbedtls_rsa_deduce_primes( int radix_N, char *input_N,
                                int radix_D, char *input_D,
                                int radix_E, char *input_E,
                                int radix_P, char *output_P,
                                int radix_Q, char *output_Q,
                                int corrupt, int result )
{
    mbedtls_mpi N, P, Pp, Q, Qp, D, E;

    mbedtls_mpi_init( &N );
    mbedtls_mpi_init( &P );  mbedtls_mpi_init( &Q  );
    mbedtls_mpi_init( &Pp ); mbedtls_mpi_init( &Qp );
    mbedtls_mpi_init( &D ); mbedtls_mpi_init( &E );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &D, radix_D, input_D ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Qp, radix_P, output_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Pp, radix_Q, output_Q ) == 0 );

    if( corrupt )
        TEST_ASSERT( mbedtls_mpi_add_int( &D, &D, 2 ) == 0 );

    /* Try to deduce P, Q from N, D, E only. */
    TEST_ASSERT( mbedtls_rsa_deduce_primes( &N, &D, &E, &P, &Q ) == result );

    if( !corrupt )
    {
        /* Check if (P,Q) = (Pp, Qp) or (P,Q) = (Qp, Pp) */
        TEST_ASSERT( ( mbedtls_mpi_cmp_mpi( &P, &Pp ) == 0 && mbedtls_mpi_cmp_mpi( &Q, &Qp ) == 0 ) ||
                     ( mbedtls_mpi_cmp_mpi( &P, &Qp ) == 0 && mbedtls_mpi_cmp_mpi( &Q, &Pp ) == 0 ) );
    }

exit:
    mbedtls_mpi_free( &N );
    mbedtls_mpi_free( &P  ); mbedtls_mpi_free( &Q  );
    mbedtls_mpi_free( &Pp ); mbedtls_mpi_free( &Qp );
    mbedtls_mpi_free( &D ); mbedtls_mpi_free( &E );
}

void test_mbedtls_rsa_deduce_primes_wrapper( void ** params )
{

    test_mbedtls_rsa_deduce_primes( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], *( (int *) params[8] ), (char *) params[9], *( (int *) params[10] ), *( (int *) params[11] ) );
}
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_CTR_DRBG_C */
void test_mbedtls_rsa_deduce_private_exponent( int radix_P, char *input_P,
                                          int radix_Q, char *input_Q,
                                          int radix_E, char *input_E,
                                          int radix_D, char *output_D,
                                          int corrupt, int result )
{
    mbedtls_mpi P, Q, D, Dp, E, R, Rp;

    mbedtls_mpi_init( &P ); mbedtls_mpi_init( &Q );
    mbedtls_mpi_init( &D ); mbedtls_mpi_init( &Dp );
    mbedtls_mpi_init( &E );
    mbedtls_mpi_init( &R ); mbedtls_mpi_init( &Rp );

    TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Dp, radix_D, output_D ) == 0 );

    if( corrupt )
    {
        /* Make E even */
        TEST_ASSERT( mbedtls_mpi_set_bit( &E, 0, 0 ) == 0 );
    }

    /* Try to deduce D from N, P, Q, E. */
    TEST_ASSERT( mbedtls_rsa_deduce_private_exponent( &P, &Q,
                                                      &E, &D ) == result );

    if( !corrupt )
    {
        /*
         * Check that D and Dp agree modulo LCM(P-1, Q-1).
         */

        /* Replace P,Q by P-1, Q-1 */
        TEST_ASSERT( mbedtls_mpi_sub_int( &P, &P, 1 ) == 0 );
        TEST_ASSERT( mbedtls_mpi_sub_int( &Q, &Q, 1 ) == 0 );

        /* Check D == Dp modulo P-1 */
        TEST_ASSERT( mbedtls_mpi_mod_mpi( &R,  &D,  &P ) == 0 );
        TEST_ASSERT( mbedtls_mpi_mod_mpi( &Rp, &Dp, &P ) == 0 );
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R,  &Rp )     == 0 );

        /* Check D == Dp modulo Q-1 */
        TEST_ASSERT( mbedtls_mpi_mod_mpi( &R,  &D,  &Q ) == 0 );
        TEST_ASSERT( mbedtls_mpi_mod_mpi( &Rp, &Dp, &Q ) == 0 );
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R,  &Rp )     == 0 );
    }

exit:

    mbedtls_mpi_free( &P ); mbedtls_mpi_free( &Q  );
    mbedtls_mpi_free( &D ); mbedtls_mpi_free( &Dp );
    mbedtls_mpi_free( &E );
    mbedtls_mpi_free( &R ); mbedtls_mpi_free( &Rp );
}

void test_mbedtls_rsa_deduce_private_exponent_wrapper( void ** params )
{

    test_mbedtls_rsa_deduce_private_exponent( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], *( (int *) params[8] ), *( (int *) params[9] ) );
}
#if defined(MBEDTLS_CTR_DRBG_C)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(ENTROPY_HAVE_STRONG)
void test_mbedtls_rsa_import( int radix_N, char *input_N,
                         int radix_P, char *input_P,
                         int radix_Q, char *input_Q,
                         int radix_D, char *input_D,
                         int radix_E, char *input_E,
                         int successive,
                         int is_priv,
                         int res_check,
                         int res_complete )
{
    mbedtls_mpi N, P, Q, D, E;
    mbedtls_rsa_context ctx;

    /* Buffers used for encryption-decryption test */
    unsigned char *buf_orig = NULL;
    unsigned char *buf_enc  = NULL;
    unsigned char *buf_dec  = NULL;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "test_suite_rsa";

    const int have_N = ( strlen( input_N ) > 0 );
    const int have_P = ( strlen( input_P ) > 0 );
    const int have_Q = ( strlen( input_Q ) > 0 );
    const int have_D = ( strlen( input_D ) > 0 );
    const int have_E = ( strlen( input_E ) > 0 );

    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );
    mbedtls_rsa_init( &ctx, 0, 0 );

    mbedtls_mpi_init( &N );
    mbedtls_mpi_init( &P ); mbedtls_mpi_init( &Q );
    mbedtls_mpi_init( &D ); mbedtls_mpi_init( &E );

    TEST_ASSERT( mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const unsigned char *) pers, strlen( pers ) ) == 0 );

    if( have_N )
        TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );

    if( have_P )
        TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );

    if( have_Q )
        TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );

    if( have_D )
        TEST_ASSERT( mbedtls_mpi_read_string( &D, radix_D, input_D ) == 0 );

    if( have_E )
        TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    if( !successive )
    {
        TEST_ASSERT( mbedtls_rsa_import( &ctx,
                             have_N ? &N : NULL,
                             have_P ? &P : NULL,
                             have_Q ? &Q : NULL,
                             have_D ? &D : NULL,
                             have_E ? &E : NULL ) == 0 );
    }
    else
    {
        /* Import N, P, Q, D, E separately.
         * This should make no functional difference. */

        TEST_ASSERT( mbedtls_rsa_import( &ctx,
                               have_N ? &N : NULL,
                               NULL, NULL, NULL, NULL ) == 0 );

        TEST_ASSERT( mbedtls_rsa_import( &ctx,
                               NULL,
                               have_P ? &P : NULL,
                               NULL, NULL, NULL ) == 0 );

        TEST_ASSERT( mbedtls_rsa_import( &ctx,
                               NULL, NULL,
                               have_Q ? &Q : NULL,
                               NULL, NULL ) == 0 );

        TEST_ASSERT( mbedtls_rsa_import( &ctx,
                               NULL, NULL, NULL,
                               have_D ? &D : NULL,
                               NULL ) == 0 );

        TEST_ASSERT( mbedtls_rsa_import( &ctx,
                               NULL, NULL, NULL, NULL,
                               have_E ? &E : NULL ) == 0 );
    }

    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == res_complete );

    /* On expected success, perform some public and private
     * key operations to check if the key is working properly. */
    if( res_complete == 0 )
    {
        if( is_priv )
            TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == res_check );
        else
            TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == res_check );

        if( res_check != 0 )
            goto exit;

        buf_orig = mbedtls_calloc( 1, mbedtls_rsa_get_len( &ctx ) );
        buf_enc  = mbedtls_calloc( 1, mbedtls_rsa_get_len( &ctx ) );
        buf_dec  = mbedtls_calloc( 1, mbedtls_rsa_get_len( &ctx ) );
        if( buf_orig == NULL || buf_enc == NULL || buf_dec == NULL )
            goto exit;

        TEST_ASSERT( mbedtls_ctr_drbg_random( &ctr_drbg,
                              buf_orig, mbedtls_rsa_get_len( &ctx ) ) == 0 );

        /* Make sure the number we're generating is smaller than the modulus */
        buf_orig[0] = 0x00;

        TEST_ASSERT( mbedtls_rsa_public( &ctx, buf_orig, buf_enc ) == 0 );

        if( is_priv )
        {
            TEST_ASSERT( mbedtls_rsa_private( &ctx, mbedtls_ctr_drbg_random,
                                              &ctr_drbg, buf_enc,
                                              buf_dec ) == 0 );

            TEST_ASSERT( memcmp( buf_orig, buf_dec,
                                 mbedtls_rsa_get_len( &ctx ) ) == 0 );
        }
    }

exit:

    mbedtls_free( buf_orig );
    mbedtls_free( buf_enc  );
    mbedtls_free( buf_dec  );

    mbedtls_rsa_free( &ctx );

    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

    mbedtls_mpi_free( &N );
    mbedtls_mpi_free( &P ); mbedtls_mpi_free( &Q );
    mbedtls_mpi_free( &D ); mbedtls_mpi_free( &E );
}

void test_mbedtls_rsa_import_wrapper( void ** params )
{

    test_mbedtls_rsa_import( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], *( (int *) params[8] ), (char *) params[9], *( (int *) params[10] ), *( (int *) params[11] ), *( (int *) params[12] ), *( (int *) params[13] ) );
}
#endif /* ENTROPY_HAVE_STRONG */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_CTR_DRBG_C */
void test_mbedtls_rsa_export( int radix_N, char *input_N,
                         int radix_P, char *input_P,
                         int radix_Q, char *input_Q,
                         int radix_D, char *input_D,
                         int radix_E, char *input_E,
                         int is_priv,
                         int successive )
{
    /* Original MPI's with which we set up the RSA context */
    mbedtls_mpi N, P, Q, D, E;

    /* Exported MPI's */
    mbedtls_mpi Ne, Pe, Qe, De, Ee;

    const int have_N = ( strlen( input_N ) > 0 );
    const int have_P = ( strlen( input_P ) > 0 );
    const int have_Q = ( strlen( input_Q ) > 0 );
    const int have_D = ( strlen( input_D ) > 0 );
    const int have_E = ( strlen( input_E ) > 0 );

    mbedtls_rsa_context ctx;

    mbedtls_rsa_init( &ctx, 0, 0 );

    mbedtls_mpi_init( &N );
    mbedtls_mpi_init( &P ); mbedtls_mpi_init( &Q );
    mbedtls_mpi_init( &D ); mbedtls_mpi_init( &E );

    mbedtls_mpi_init( &Ne );
    mbedtls_mpi_init( &Pe ); mbedtls_mpi_init( &Qe );
    mbedtls_mpi_init( &De ); mbedtls_mpi_init( &Ee );

    /* Setup RSA context */

    if( have_N )
        TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );

    if( have_P )
        TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );

    if( have_Q )
        TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );

    if( have_D )
        TEST_ASSERT( mbedtls_mpi_read_string( &D, radix_D, input_D ) == 0 );

    if( have_E )
        TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx,
                                     strlen( input_N ) ? &N : NULL,
                                     strlen( input_P ) ? &P : NULL,
                                     strlen( input_Q ) ? &Q : NULL,
                                     strlen( input_D ) ? &D : NULL,
                                     strlen( input_E ) ? &E : NULL ) == 0 );

    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );

    /*
     * Export parameters and compare to original ones.
     */

    /* N and E must always be present. */
    if( !successive )
    {
        TEST_ASSERT( mbedtls_rsa_export( &ctx, &Ne, NULL, NULL, NULL, &Ee ) == 0 );
    }
    else
    {
        TEST_ASSERT( mbedtls_rsa_export( &ctx, &Ne, NULL, NULL, NULL, NULL ) == 0 );
        TEST_ASSERT( mbedtls_rsa_export( &ctx, NULL, NULL, NULL, NULL, &Ee ) == 0 );
    }
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &N, &Ne ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &E, &Ee ) == 0 );

    /* If we were providing enough information to setup a complete private context,
     * we expect to be able to export all core parameters. */

    if( is_priv )
    {
        if( !successive )
        {
            TEST_ASSERT( mbedtls_rsa_export( &ctx, NULL, &Pe, &Qe,
                                             &De, NULL ) == 0 );
        }
        else
        {
            TEST_ASSERT( mbedtls_rsa_export( &ctx, NULL, &Pe, NULL,
                                             NULL, NULL ) == 0 );
            TEST_ASSERT( mbedtls_rsa_export( &ctx, NULL, NULL, &Qe,
                                             NULL, NULL ) == 0 );
            TEST_ASSERT( mbedtls_rsa_export( &ctx, NULL, NULL, NULL,
                                             &De, NULL ) == 0 );
        }

        if( have_P )
            TEST_ASSERT( mbedtls_mpi_cmp_mpi( &P, &Pe ) == 0 );

        if( have_Q )
            TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Q, &Qe ) == 0 );

        if( have_D )
            TEST_ASSERT( mbedtls_mpi_cmp_mpi( &D, &De ) == 0 );

        /* While at it, perform a sanity check */
        TEST_ASSERT( mbedtls_rsa_validate_params( &Ne, &Pe, &Qe, &De, &Ee,
                                                       NULL, NULL ) == 0 );
    }

exit:

    mbedtls_rsa_free( &ctx );

    mbedtls_mpi_free( &N );
    mbedtls_mpi_free( &P ); mbedtls_mpi_free( &Q );
    mbedtls_mpi_free( &D ); mbedtls_mpi_free( &E );

    mbedtls_mpi_free( &Ne );
    mbedtls_mpi_free( &Pe ); mbedtls_mpi_free( &Qe );
    mbedtls_mpi_free( &De ); mbedtls_mpi_free( &Ee );
}

void test_mbedtls_rsa_export_wrapper( void ** params )
{

    test_mbedtls_rsa_export( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], *( (int *) params[8] ), (char *) params[9], *( (int *) params[10] ), *( (int *) params[11] ) );
}
#if defined(MBEDTLS_ENTROPY_C)
#if defined(ENTROPY_HAVE_STRONG)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_mbedtls_rsa_validate_params( int radix_N, char *input_N,
                                  int radix_P, char *input_P,
                                  int radix_Q, char *input_Q,
                                  int radix_D, char *input_D,
                                  int radix_E, char *input_E,
                                  int prng, int result )
{
    /* Original MPI's with which we set up the RSA context */
    mbedtls_mpi N, P, Q, D, E;

    const int have_N = ( strlen( input_N ) > 0 );
    const int have_P = ( strlen( input_P ) > 0 );
    const int have_Q = ( strlen( input_Q ) > 0 );
    const int have_D = ( strlen( input_D ) > 0 );
    const int have_E = ( strlen( input_E ) > 0 );

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "test_suite_rsa";

    mbedtls_mpi_init( &N );
    mbedtls_mpi_init( &P ); mbedtls_mpi_init( &Q );
    mbedtls_mpi_init( &D ); mbedtls_mpi_init( &E );

    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );
    TEST_ASSERT( mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func,
                                        &entropy, (const unsigned char *) pers,
                                        strlen( pers ) ) == 0 );

    if( have_N )
        TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );

    if( have_P )
        TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );

    if( have_Q )
        TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );

    if( have_D )
        TEST_ASSERT( mbedtls_mpi_read_string( &D, radix_D, input_D ) == 0 );

    if( have_E )
        TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_validate_params( have_N ? &N : NULL,
                                        have_P ? &P : NULL,
                                        have_Q ? &Q : NULL,
                                        have_D ? &D : NULL,
                                        have_E ? &E : NULL,
                                        prng ? mbedtls_ctr_drbg_random : NULL,
                                        prng ? &ctr_drbg : NULL ) == result );
exit:

    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

    mbedtls_mpi_free( &N );
    mbedtls_mpi_free( &P ); mbedtls_mpi_free( &Q );
    mbedtls_mpi_free( &D ); mbedtls_mpi_free( &E );
}

void test_mbedtls_rsa_validate_params_wrapper( void ** params )
{

    test_mbedtls_rsa_validate_params( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], *( (int *) params[8] ), (char *) params[9], *( (int *) params[10] ), *( (int *) params[11] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* ENTROPY_HAVE_STRONG */
#endif /* MBEDTLS_ENTROPY_C */
#if defined(MBEDTLS_CTR_DRBG_C)
#if defined(MBEDTLS_ENTROPY_C)
void test_mbedtls_rsa_export_raw( data_t *input_N, data_t *input_P,
                             data_t *input_Q, data_t *input_D,
                             data_t *input_E, int is_priv,
                             int successive )
{
    /* Exported buffers */
    unsigned char bufNe[256];
    unsigned char bufPe[128];
    unsigned char bufQe[128];
    unsigned char bufDe[256];
    unsigned char bufEe[1];

    mbedtls_rsa_context ctx;

    mbedtls_rsa_init( &ctx, 0, 0 );

    /* Setup RSA context */
    TEST_ASSERT( mbedtls_rsa_import_raw( &ctx,
                               input_N->len ? input_N->x : NULL, input_N->len,
                               input_P->len ? input_P->x : NULL, input_P->len,
                               input_Q->len ? input_Q->x : NULL, input_Q->len,
                               input_D->len ? input_D->x : NULL, input_D->len,
                               input_E->len ? input_E->x : NULL, input_E->len ) == 0 );

    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );

    /*
     * Export parameters and compare to original ones.
     */

    /* N and E must always be present. */
    if( !successive )
    {
        TEST_ASSERT( mbedtls_rsa_export_raw( &ctx, bufNe, input_N->len,
                                             NULL, 0, NULL, 0, NULL, 0,
                                             bufEe, input_E->len ) == 0 );
    }
    else
    {
        TEST_ASSERT( mbedtls_rsa_export_raw( &ctx, bufNe, input_N->len,
                                             NULL, 0, NULL, 0, NULL, 0,
                                             NULL, 0 ) == 0 );
        TEST_ASSERT( mbedtls_rsa_export_raw( &ctx, NULL, 0,
                                             NULL, 0, NULL, 0, NULL, 0,
                                             bufEe, input_E->len ) == 0 );
    }
    TEST_ASSERT( memcmp( input_N->x, bufNe, input_N->len ) == 0 );
    TEST_ASSERT( memcmp( input_E->x, bufEe, input_E->len ) == 0 );

    /* If we were providing enough information to setup a complete private context,
     * we expect to be able to export all core parameters. */

    if( is_priv )
    {
        if( !successive )
        {
            TEST_ASSERT( mbedtls_rsa_export_raw( &ctx, NULL, 0,
                                         bufPe, input_P->len ? input_P->len : sizeof( bufPe ),
                                         bufQe, input_Q->len ? input_Q->len : sizeof( bufQe ),
                                         bufDe, input_D->len ? input_D->len : sizeof( bufDe ),
                                         NULL, 0 ) == 0 );
        }
        else
        {
            TEST_ASSERT( mbedtls_rsa_export_raw( &ctx, NULL, 0,
                                         bufPe, input_P->len ? input_P->len : sizeof( bufPe ),
                                         NULL, 0, NULL, 0,
                                         NULL, 0 ) == 0 );

            TEST_ASSERT( mbedtls_rsa_export_raw( &ctx, NULL, 0, NULL, 0,
                                         bufQe, input_Q->len ? input_Q->len : sizeof( bufQe ),
                                         NULL, 0, NULL, 0 ) == 0 );

            TEST_ASSERT( mbedtls_rsa_export_raw( &ctx, NULL, 0, NULL, 0, NULL, 0,
                                         bufDe, input_D->len ? input_D->len : sizeof( bufDe ),
                                         NULL, 0 ) == 0 );
        }

        if( input_P->len )
            TEST_ASSERT( memcmp( input_P->x, bufPe, input_P->len ) == 0 );

        if( input_Q->len )
            TEST_ASSERT( memcmp( input_Q->x, bufQe, input_Q->len ) == 0 );

        if( input_D->len )
            TEST_ASSERT( memcmp( input_D->x, bufDe, input_D->len ) == 0 );

    }

exit:
    mbedtls_rsa_free( &ctx );
}

void test_mbedtls_rsa_export_raw_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_rsa_export_raw( &data0, &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ) );
}
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_CTR_DRBG_C */
#if defined(MBEDTLS_CTR_DRBG_C)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(ENTROPY_HAVE_STRONG)
void test_mbedtls_rsa_import_raw( data_t *input_N,
                             data_t *input_P, data_t *input_Q,
                             data_t *input_D, data_t *input_E,
                             int successive,
                             int is_priv,
                             int res_check,
                             int res_complete )
{
    /* Buffers used for encryption-decryption test */
    unsigned char *buf_orig = NULL;
    unsigned char *buf_enc  = NULL;
    unsigned char *buf_dec  = NULL;

    mbedtls_rsa_context ctx;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    const char *pers = "test_suite_rsa";

    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );
    mbedtls_rsa_init( &ctx, 0, 0 );

    TEST_ASSERT( mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func,
                                        &entropy, (const unsigned char *) pers,
                                        strlen( pers ) ) == 0 );

    if( !successive )
    {
        TEST_ASSERT( mbedtls_rsa_import_raw( &ctx,
                               ( input_N->len > 0 ) ? input_N->x : NULL, input_N->len,
                               ( input_P->len > 0 ) ? input_P->x : NULL, input_P->len,
                               ( input_Q->len > 0 ) ? input_Q->x : NULL, input_Q->len,
                               ( input_D->len > 0 ) ? input_D->x : NULL, input_D->len,
                               ( input_E->len > 0 ) ? input_E->x : NULL, input_E->len ) == 0 );
    }
    else
    {
        /* Import N, P, Q, D, E separately.
         * This should make no functional difference. */

        TEST_ASSERT( mbedtls_rsa_import_raw( &ctx,
                               ( input_N->len > 0 ) ? input_N->x : NULL, input_N->len,
                               NULL, 0, NULL, 0, NULL, 0, NULL, 0 ) == 0 );

        TEST_ASSERT( mbedtls_rsa_import_raw( &ctx,
                               NULL, 0,
                               ( input_P->len > 0 ) ? input_P->x : NULL, input_P->len,
                               NULL, 0, NULL, 0, NULL, 0 ) == 0 );

        TEST_ASSERT( mbedtls_rsa_import_raw( &ctx,
                               NULL, 0, NULL, 0,
                               ( input_Q->len > 0 ) ? input_Q->x : NULL, input_Q->len,
                               NULL, 0, NULL, 0 ) == 0 );

        TEST_ASSERT( mbedtls_rsa_import_raw( &ctx,
                               NULL, 0, NULL, 0, NULL, 0,
                               ( input_D->len > 0 ) ? input_D->x : NULL, input_D->len,
                               NULL, 0 ) == 0 );

        TEST_ASSERT( mbedtls_rsa_import_raw( &ctx,
                               NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                               ( input_E->len > 0 ) ? input_E->x : NULL, input_E->len ) == 0 );
    }

    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == res_complete );

    /* On expected success, perform some public and private
     * key operations to check if the key is working properly. */
    if( res_complete == 0 )
    {
        if( is_priv )
            TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == res_check );
        else
            TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == res_check );

        if( res_check != 0 )
            goto exit;

        buf_orig = mbedtls_calloc( 1, mbedtls_rsa_get_len( &ctx ) );
        buf_enc  = mbedtls_calloc( 1, mbedtls_rsa_get_len( &ctx ) );
        buf_dec  = mbedtls_calloc( 1, mbedtls_rsa_get_len( &ctx ) );
        if( buf_orig == NULL || buf_enc == NULL || buf_dec == NULL )
            goto exit;

        TEST_ASSERT( mbedtls_ctr_drbg_random( &ctr_drbg,
                              buf_orig, mbedtls_rsa_get_len( &ctx ) ) == 0 );

        /* Make sure the number we're generating is smaller than the modulus */
        buf_orig[0] = 0x00;

        TEST_ASSERT( mbedtls_rsa_public( &ctx, buf_orig, buf_enc ) == 0 );

        if( is_priv )
        {
            TEST_ASSERT( mbedtls_rsa_private( &ctx, mbedtls_ctr_drbg_random,
                                              &ctr_drbg, buf_enc,
                                              buf_dec ) == 0 );

            TEST_ASSERT( memcmp( buf_orig, buf_dec,
                                 mbedtls_rsa_get_len( &ctx ) ) == 0 );
        }
    }

exit:

    mbedtls_free( buf_orig );
    mbedtls_free( buf_enc  );
    mbedtls_free( buf_dec  );

    mbedtls_rsa_free( &ctx );

    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

}

void test_mbedtls_rsa_import_raw_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_mbedtls_rsa_import_raw( &data0, &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ), *( (int *) params[12] ), *( (int *) params[13] ) );
}
#endif /* ENTROPY_HAVE_STRONG */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_CTR_DRBG_C */
#if defined(MBEDTLS_SELF_TEST)
void test_rsa_selftest(  )
{
    TEST_ASSERT( mbedtls_rsa_self_test( 1 ) == 0 );
exit:
    ;
}

void test_rsa_selftest_wrapper( void ** params )
{
    (void)params;

    test_rsa_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_GENPRIME */
#endif /* MBEDTLS_BIGNUM_C */
#endif /* MBEDTLS_RSA_C */

/*----------------------------------------------------------------------------*/
/* Test dispatch code */


/**
 * \brief       Evaluates an expression/macro into its literal integer value.
 *              For optimizing space for embedded targets each expression/macro
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and evaluation code is generated by script:
 *              generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * \param exp_id    Expression identifier.
 * \param out_value Pointer to int to hold the integer.
 *
 * \return       0 if exp_id is found. 1 otherwise.
 */
int get_expression( int32_t exp_id, int32_t * out_value )
{
    int ret = KEY_VALUE_MAPPING_FOUND;

    (void) exp_id;
    (void) out_value;

    switch( exp_id )
    {

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)

        case 0:
            {
                *out_value = MBEDTLS_RSA_PKCS_V15;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_RSA_VERIFY_FAILED;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_MD_SHA224;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_MD_SHA384;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_MD_SHA512;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_MD_MD2;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_MD_MD4;
            }
            break;
        case 9:
            {
                *out_value = MBEDTLS_MD_MD5;
            }
            break;
        case 10:
            {
                *out_value = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
            }
            break;
        case 11:
            {
                *out_value = MBEDTLS_MD_RIPEMD160;
            }
            break;
        case 12:
            {
                *out_value = MBEDTLS_ERR_RSA_PRIVATE_FAILED + MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            }
            break;
        case 13:
            {
                *out_value = MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE;
            }
            break;
        case 14:
            {
                *out_value = MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
            }
            break;
        case 15:
            {
                *out_value = MBEDTLS_ERR_RSA_PUBLIC_FAILED + MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            }
            break;
        case 16:
            {
                *out_value = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
            }
            break;
        case 17:
            {
                *out_value = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            }
            break;
        case 18:
            {
                *out_value = MBEDTLS_ERR_RSA_RNG_FAILED;
            }
            break;
#endif

        default:
           {
                ret = KEY_VALUE_MAPPING_NOT_FOUND;
           }
           break;
    }
    return( ret );
}


/**
 * \brief       Checks if the dependency i.e. the compile flag is set.
 *              For optimizing space for embedded targets each dependency
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and check code is generated by script:
 *              generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * \param dep_id    Dependency identifier.
 *
 * \return       DEPENDENCY_SUPPORTED if set else DEPENDENCY_NOT_SUPPORTED
 */
int dep_check( int dep_id )
{
    int ret = DEPENDENCY_NOT_SUPPORTED;

    (void) dep_id;

    switch( dep_id )
    {

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)

        case 0:
            {
#if defined(MBEDTLS_SHA1_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_PKCS1_V15)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_SHA256_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_SHA512_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if !defined(MBEDTLS_SHA512_NO_SHA384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_MD2_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_MD4_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_MD5_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(MBEDTLS_RIPEMD160_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if !defined(MBEDTLS_RSA_NO_CRT)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if (MBEDTLS_MPI_MAX_SIZE>=1024)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(MBEDTLS_SELF_TEST)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
#endif

        default:
            break;
    }
    return( ret );
}


/**
 * \brief       Function pointer type for test function wrappers.
 *
 * A test function wrapper decodes the parameters and passes them to the
 * underlying test function. Both the wrapper and the underlying function
 * return void. Test wrappers assume that they are passed a suitable
 * parameter array and do not perform any error detection.
 *
 * \param param_array   The array of parameters. Each element is a `void *`
 *                      which the wrapper casts to the correct type and
 *                      dereferences. Each wrapper function hard-codes the
 *                      number and types of the parameters.
 */
typedef void (*TestWrapper_t)( void **param_array );


/**
 * \brief       Table of test function wrappers. Used by dispatch_test().
 *              This table is populated by script:
 *              generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 */
TestWrapper_t test_funcs[] =
{
/* Function Id: 0 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_rsa_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_init_free_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_pkcs1_sign_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_pkcs1_verify_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_pkcs1_sign_raw_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_pkcs1_verify_raw_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_pkcs1_encrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_pkcs1_encrypt_bad_rng_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_pkcs1_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_public_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_private_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_check_privkey_null_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_check_pubkey_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_check_privkey_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_rsa_check_pubpriv_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME) && defined(MBEDTLS_CTR_DRBG_C) && defined(MBEDTLS_ENTROPY_C) && defined(ENTROPY_HAVE_STRONG)
    test_mbedtls_rsa_gen_key_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME) && defined(MBEDTLS_CTR_DRBG_C) && defined(MBEDTLS_ENTROPY_C)
    test_mbedtls_rsa_deduce_primes_wrapper,
#else
    NULL,
#endif
/* Function Id: 17 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_deduce_private_exponent_wrapper,
#else
    NULL,
#endif
/* Function Id: 18 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME) && defined(MBEDTLS_CTR_DRBG_C) && defined(MBEDTLS_ENTROPY_C) && defined(ENTROPY_HAVE_STRONG)
    test_mbedtls_rsa_import_wrapper,
#else
    NULL,
#endif
/* Function Id: 19 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_rsa_export_wrapper,
#else
    NULL,
#endif
/* Function Id: 20 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME) && defined(MBEDTLS_ENTROPY_C) && defined(ENTROPY_HAVE_STRONG) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_mbedtls_rsa_validate_params_wrapper,
#else
    NULL,
#endif
/* Function Id: 21 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME) && defined(MBEDTLS_CTR_DRBG_C) && defined(MBEDTLS_ENTROPY_C)
    test_mbedtls_rsa_export_raw_wrapper,
#else
    NULL,
#endif
/* Function Id: 22 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME) && defined(MBEDTLS_CTR_DRBG_C) && defined(MBEDTLS_ENTROPY_C) && defined(ENTROPY_HAVE_STRONG)
    test_mbedtls_rsa_import_raw_wrapper,
#else
    NULL,
#endif
/* Function Id: 23 */

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME) && defined(MBEDTLS_SELF_TEST)
    test_rsa_selftest_wrapper,
#else
    NULL,
#endif

};

/**
 * \brief        Execute the test function.
 *
 *               This is a wrapper function around the test function execution
 *               to allow the setjmp() call used to catch any calls to the
 *               parameter failure callback, to be used. Calls to setjmp()
 *               can invalidate the state of any local auto variables.
 *
 * \param fp     Function pointer to the test function.
 * \param params Parameters to pass to the #TestWrapper_t wrapper function.
 *
 */
void execute_function_ptr(TestWrapper_t fp, void **params)
{
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    mbedtls_test_enable_insecure_external_rng( );
#endif

#if defined(MBEDTLS_CHECK_PARAMS)
    mbedtls_test_param_failed_location_record_t location_record;

    if ( setjmp( mbedtls_test_param_failed_get_state_buf( ) ) == 0 )
    {
        fp( params );
    }
    else
    {
        /* Unexpected parameter validation error */
        mbedtls_test_param_failed_get_location_record( &location_record );
        mbedtls_test_fail( location_record.failure_condition,
                           location_record.line,
                           location_record.file );
    }

    mbedtls_test_param_failed_reset_state( );
#else
    fp( params );
#endif

#if defined(MBEDTLS_TEST_MUTEX_USAGE)
    mbedtls_test_mutex_usage_check( );
#endif /* MBEDTLS_TEST_MUTEX_USAGE */
}

/**
 * \brief        Dispatches test functions based on function index.
 *
 * \param func_idx    Test function index.
 * \param params      The array of parameters to pass to the test function.
 *                    It will be decoded by the #TestWrapper_t wrapper function.
 *
 * \return       DISPATCH_TEST_SUCCESS if found
 *               DISPATCH_TEST_FN_NOT_FOUND if not found
 *               DISPATCH_UNSUPPORTED_SUITE if not compile time enabled.
 */
int dispatch_test( size_t func_idx, void ** params )
{
    int ret = DISPATCH_TEST_SUCCESS;
    TestWrapper_t fp = NULL;

    if ( func_idx < (int)( sizeof( test_funcs ) / sizeof( TestWrapper_t ) ) )
    {
        fp = test_funcs[func_idx];
        if ( fp )
            execute_function_ptr(fp, params);
        else
            ret = DISPATCH_UNSUPPORTED_SUITE;
    }
    else
    {
        ret = DISPATCH_TEST_FN_NOT_FOUND;
    }

    return( ret );
}


/**
 * \brief       Checks if test function is supported in this build-time
 *              configuration.
 *
 * \param func_idx    Test function index.
 *
 * \return       DISPATCH_TEST_SUCCESS if found
 *               DISPATCH_TEST_FN_NOT_FOUND if not found
 *               DISPATCH_UNSUPPORTED_SUITE if not compile time enabled.
 */
int check_test( size_t func_idx )
{
    int ret = DISPATCH_TEST_SUCCESS;
    TestWrapper_t fp = NULL;

    if ( func_idx < (int)( sizeof(test_funcs)/sizeof( TestWrapper_t ) ) )
    {
        fp = test_funcs[func_idx];
        if ( fp == NULL )
            ret = DISPATCH_UNSUPPORTED_SUITE;
    }
    else
    {
        ret = DISPATCH_TEST_FN_NOT_FOUND;
    }

    return( ret );
}

int main( int argc, const char *argv[] )
{
    int ret;
    mbedtls_test_platform_setup();
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_rsa.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
