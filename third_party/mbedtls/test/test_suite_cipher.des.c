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
 * Test file      : ./test_suite_cipher.des.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_cipher.function
 *      Test suite data     : suites/test_suite_cipher.des.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_CIPHER_C)
#include "third_party/mbedtls/cipher.h"

#if defined(MBEDTLS_AES_C)
#include "third_party/mbedtls/aes.h"
#endif

#if defined(MBEDTLS_GCM_C)
#include "third_party/mbedtls/gcm.h"
#endif

#if defined(MBEDTLS_CIPHER_MODE_AEAD) || defined(MBEDTLS_NIST_KW_C)
#define MBEDTLS_CIPHER_AUTH_CRYPT
#endif

#if defined(MBEDTLS_CIPHER_AUTH_CRYPT)
/* Helper for resetting key/direction
 *
 * The documentation doesn't explicitly say whether calling
 * mbedtls_cipher_setkey() twice is allowed or not. This currently works with
 * the default software implementation, but only by accident. It isn't
 * guaranteed to work with new ciphers or with alternative implementations of
 * individual ciphers, and it doesn't work with the PSA wrappers. So don't do
 * it, and instead start with a fresh context.
 */
static int cipher_reset_key( mbedtls_cipher_context_t *ctx, int cipher_id,
        int use_psa, size_t tag_len, const data_t *key, int direction )
{
    mbedtls_cipher_free( ctx );
    mbedtls_cipher_init( ctx );
    TEST_ASSERT( 0 == mbedtls_cipher_setup( ctx, mbedtls_cipher_info_from_type( cipher_id ) ) );
    TEST_ASSERT( 0 == mbedtls_cipher_setkey( ctx, key->x, 8 * key->len, direction ) );
    return( 1 );
exit:
    return( 0 );
}

/*
 * Check if a buffer is all-0 bytes:
 * return   1 if it is,
 *          0 if it isn't.
 */
int buffer_is_all_zero( const uint8_t *buf, size_t size )
{
    for( size_t i = 0; i < size; i++ )
        if( buf[i] != 0 )
            return 0;
    return 1;
}
#endif /* MBEDTLS_CIPHER_AUTH_CRYPT */

void test_mbedtls_cipher_list(  )
{
    const int *cipher_type;
    for( cipher_type = mbedtls_cipher_list(); *cipher_type != 0; cipher_type++ )
        TEST_ASSERT( mbedtls_cipher_info_from_type( *cipher_type ) != NULL );
exit:
    ;
}

void test_mbedtls_cipher_list_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_cipher_list(  );
}
void test_cipher_invalid_param_unconditional( )
{
    mbedtls_cipher_context_t valid_ctx;
    mbedtls_cipher_context_t invalid_ctx;
    mbedtls_operation_t valid_operation = MBEDTLS_ENCRYPT;
    mbedtls_cipher_padding_t valid_mode = MBEDTLS_PADDING_ZEROS;
    unsigned char valid_buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    int valid_size = sizeof(valid_buffer);
    int valid_bitlen = valid_size * 8;
    const mbedtls_cipher_info_t *valid_info = mbedtls_cipher_info_from_type(
        *( mbedtls_cipher_list() ) );
    size_t size_t_var;

    (void)valid_mode; /* In some configurations this is unused */

    mbedtls_cipher_init( &valid_ctx );
    mbedtls_cipher_setup( &valid_ctx, valid_info );
    mbedtls_cipher_init( &invalid_ctx );

    /* mbedtls_cipher_setup() */
    TEST_ASSERT( mbedtls_cipher_setup( &valid_ctx, NULL ) ==
                 MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    /* mbedtls_cipher_get_block_size() */
    TEST_ASSERT( mbedtls_cipher_get_block_size( &invalid_ctx ) == 0 );

    /* mbedtls_cipher_get_cipher_mode() */
    TEST_ASSERT( mbedtls_cipher_get_cipher_mode( &invalid_ctx ) ==
                 MBEDTLS_MODE_NONE );

    /* mbedtls_cipher_get_iv_size() */
    TEST_ASSERT( mbedtls_cipher_get_iv_size( &invalid_ctx ) == 0 );

    /* mbedtls_cipher_get_type() */
    TEST_ASSERT(
        mbedtls_cipher_get_type( &invalid_ctx ) ==
        MBEDTLS_CIPHER_NONE);

    /* mbedtls_cipher_get_name() */
    TEST_ASSERT( mbedtls_cipher_get_name( &invalid_ctx ) == 0 );

    /* mbedtls_cipher_get_key_bitlen() */
    TEST_ASSERT( mbedtls_cipher_get_key_bitlen( &invalid_ctx ) ==
                 MBEDTLS_KEY_LENGTH_NONE );

    /* mbedtls_cipher_get_operation() */
    TEST_ASSERT( mbedtls_cipher_get_operation( &invalid_ctx ) ==
                 MBEDTLS_OPERATION_NONE );

    /* mbedtls_cipher_setkey() */
    TEST_ASSERT(
        mbedtls_cipher_setkey( &invalid_ctx,
                               valid_buffer,
                               valid_bitlen,
                               valid_operation ) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    /* mbedtls_cipher_set_iv() */
    TEST_ASSERT(
        mbedtls_cipher_set_iv( &invalid_ctx,
                               valid_buffer,
                               valid_size ) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    /* mbedtls_cipher_reset() */
    TEST_ASSERT( mbedtls_cipher_reset( &invalid_ctx ) ==
                 MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    /* mbedtls_cipher_update_ad() */
    TEST_ASSERT(
        mbedtls_cipher_update_ad( &invalid_ctx,
                                  valid_buffer,
                                  valid_size ) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
#endif /* defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C) */

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    /* mbedtls_cipher_set_padding_mode() */
    TEST_ASSERT( mbedtls_cipher_set_padding_mode( &invalid_ctx, valid_mode ) ==
                 MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
#endif

    /* mbedtls_cipher_update() */
    TEST_ASSERT(
        mbedtls_cipher_update( &invalid_ctx,
                               valid_buffer,
                               valid_size,
                               valid_buffer,
                               &size_t_var ) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    /* mbedtls_cipher_finish() */
    TEST_ASSERT(
        mbedtls_cipher_finish( &invalid_ctx,
                               valid_buffer,
                               &size_t_var ) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    /* mbedtls_cipher_write_tag() */
    TEST_ASSERT(
        mbedtls_cipher_write_tag( &invalid_ctx,
                                  valid_buffer,
                                  valid_size ) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    /* mbedtls_cipher_check_tag() */
    TEST_ASSERT(
        mbedtls_cipher_check_tag( &invalid_ctx,
                                  valid_buffer,
                                  valid_size ) ==
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
#endif /* defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C) */

exit:
    mbedtls_cipher_free( &invalid_ctx );
    mbedtls_cipher_free( &valid_ctx );
}

void test_cipher_invalid_param_unconditional_wrapper( void ** params )
{
    (void)params;

    test_cipher_invalid_param_unconditional(  );
}
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_cipher_invalid_param_conditional( )
{
    mbedtls_cipher_context_t valid_ctx;

    mbedtls_operation_t valid_operation = MBEDTLS_ENCRYPT;
    mbedtls_operation_t invalid_operation = 100;
    mbedtls_cipher_padding_t valid_mode = MBEDTLS_PADDING_ZEROS;
    unsigned char valid_buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    int valid_size = sizeof(valid_buffer);
    int valid_bitlen = valid_size * 8;
    const mbedtls_cipher_info_t *valid_info = mbedtls_cipher_info_from_type(
        *( mbedtls_cipher_list() ) );

    size_t size_t_var;

    (void)valid_mode; /* In some configurations this is unused */

    /* mbedtls_cipher_init() */
    TEST_VALID_PARAM( mbedtls_cipher_init( &valid_ctx ) );
    TEST_INVALID_PARAM( mbedtls_cipher_init( NULL ) );

    /* mbedtls_cipher_setup() */
    TEST_VALID_PARAM( mbedtls_cipher_setup( &valid_ctx, valid_info ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_setup( NULL, valid_info ) );

    /* mbedtls_cipher_get_block_size() */
    TEST_INVALID_PARAM_RET( 0, mbedtls_cipher_get_block_size( NULL ) );

    /* mbedtls_cipher_get_cipher_mode() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_MODE_NONE,
        mbedtls_cipher_get_cipher_mode( NULL ) );

    /* mbedtls_cipher_get_iv_size() */
    TEST_INVALID_PARAM_RET( 0, mbedtls_cipher_get_iv_size( NULL ) );

    /* mbedtls_cipher_get_type() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_CIPHER_NONE,
        mbedtls_cipher_get_type( NULL ) );

    /* mbedtls_cipher_get_name() */
    TEST_INVALID_PARAM_RET( 0, mbedtls_cipher_get_name( NULL ) );

    /* mbedtls_cipher_get_key_bitlen() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_KEY_LENGTH_NONE,
        mbedtls_cipher_get_key_bitlen( NULL ) );

    /* mbedtls_cipher_get_operation() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_OPERATION_NONE,
        mbedtls_cipher_get_operation( NULL ) );

    /* mbedtls_cipher_setkey() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_setkey( NULL,
                               valid_buffer,
                               valid_bitlen,
                               valid_operation ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_setkey( &valid_ctx,
                               NULL,
                               valid_bitlen,
                               valid_operation ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_setkey( &valid_ctx,
                               valid_buffer,
                               valid_bitlen,
                               invalid_operation ) );

    /* mbedtls_cipher_set_iv() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_set_iv( NULL,
                               valid_buffer,
                               valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_set_iv( &valid_ctx,
                               NULL,
                               valid_size ) );

    /* mbedtls_cipher_reset() */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
                            mbedtls_cipher_reset( NULL ) );

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    /* mbedtls_cipher_update_ad() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_update_ad( NULL,
                                  valid_buffer,
                                  valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_update_ad( &valid_ctx,
                                  NULL,
                                  valid_size ) );
#endif /* defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C) */

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    /* mbedtls_cipher_set_padding_mode() */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
                            mbedtls_cipher_set_padding_mode( NULL, valid_mode ) );
#endif

    /* mbedtls_cipher_update() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_update( NULL,
                               valid_buffer,
                               valid_size,
                               valid_buffer,
                               &size_t_var ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_update( &valid_ctx,
                               NULL, valid_size,
                               valid_buffer,
                               &size_t_var ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_update( &valid_ctx,
                               valid_buffer, valid_size,
                               NULL,
                               &size_t_var ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_update( &valid_ctx,
                               valid_buffer, valid_size,
                               valid_buffer,
                               NULL ) );

    /* mbedtls_cipher_finish() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_finish( NULL,
                               valid_buffer,
                               &size_t_var ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_finish( &valid_ctx,
                               NULL,
                               &size_t_var ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_finish( &valid_ctx,
                               valid_buffer,
                               NULL ) );

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    /* mbedtls_cipher_write_tag() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_write_tag( NULL,
                                  valid_buffer,
                                  valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_write_tag( &valid_ctx,
                                  NULL,
                                  valid_size ) );

    /* mbedtls_cipher_check_tag() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_check_tag( NULL,
                                  valid_buffer,
                                  valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_check_tag( &valid_ctx,
                                  NULL,
                                  valid_size ) );
#endif /* defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C) */

    /* mbedtls_cipher_crypt() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_crypt( NULL,
                              valid_buffer, valid_size,
                              valid_buffer, valid_size,
                              valid_buffer, &size_t_var ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_crypt( &valid_ctx,
                              NULL, valid_size,
                              valid_buffer, valid_size,
                              valid_buffer, &size_t_var ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_crypt( &valid_ctx,
                              valid_buffer, valid_size,
                              NULL, valid_size,
                              valid_buffer, &size_t_var ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_crypt( &valid_ctx,
                              valid_buffer, valid_size,
                              valid_buffer, valid_size,
                              NULL, &size_t_var ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_crypt( &valid_ctx,
                              valid_buffer, valid_size,
                              valid_buffer, valid_size,
                              valid_buffer, NULL ) );

#if defined(MBEDTLS_CIPHER_MODE_AEAD)
    /* mbedtls_cipher_auth_encrypt() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt( NULL,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, &size_t_var,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt( &valid_ctx,
                                     NULL, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, &size_t_var,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt( &valid_ctx,
                                     valid_buffer, valid_size,
                                     NULL, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, &size_t_var,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt( &valid_ctx,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     NULL, valid_size,
                                     valid_buffer, &size_t_var,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt( &valid_ctx,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     NULL, &size_t_var,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt( &valid_ctx,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, NULL,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt( &valid_ctx,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, &size_t_var,
                                     NULL, valid_size ) );

    /* mbedtls_cipher_auth_decrypt() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt( NULL,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, &size_t_var,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt( &valid_ctx,
                                     NULL, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, &size_t_var,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt( &valid_ctx,
                                     valid_buffer, valid_size,
                                     NULL, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, &size_t_var,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt( &valid_ctx,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     NULL, valid_size,
                                     valid_buffer, &size_t_var,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt( &valid_ctx,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     NULL, &size_t_var,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt( &valid_ctx,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, NULL,
                                     valid_buffer, valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt( &valid_ctx,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, valid_size,
                                     valid_buffer, &size_t_var,
                                     NULL, valid_size ) );
#endif /* defined(MBEDTLS_CIPHER_MODE_AEAD) */

#if defined(MBEDTLS_CIPHER_MODE_AEAD) || defined(MBEDTLS_NIST_KW_C)
    /* mbedtls_cipher_auth_encrypt_ext */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt_ext( NULL,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size, &size_t_var,
                                         valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt_ext( &valid_ctx,
                                         NULL, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size, &size_t_var,
                                         valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt_ext( &valid_ctx,
                                         valid_buffer, valid_size,
                                         NULL, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size, &size_t_var,
                                         valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt_ext( &valid_ctx,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         NULL, valid_size,
                                         valid_buffer, valid_size, &size_t_var,
                                         valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt_ext( &valid_ctx,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         NULL, valid_size, &size_t_var,
                                         valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_encrypt_ext( &valid_ctx,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size, NULL,
                                         valid_size ) );

    /* mbedtls_cipher_auth_decrypt_ext */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt_ext( NULL,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size, &size_t_var,
                                         valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt_ext( &valid_ctx,
                                         NULL, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size, &size_t_var,
                                         valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt_ext( &valid_ctx,
                                         valid_buffer, valid_size,
                                         NULL, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size, &size_t_var,
                                         valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt_ext( &valid_ctx,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         NULL, valid_size,
                                         valid_buffer, valid_size, &size_t_var,
                                         valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt_ext( &valid_ctx,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         NULL, valid_size, &size_t_var,
                                         valid_size ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA,
        mbedtls_cipher_auth_decrypt_ext( &valid_ctx,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size,
                                         valid_buffer, valid_size, NULL,
                                         valid_size ) );
#endif /* MBEDTLS_CIPHER_MODE_AEAD || MBEDTLS_NIST_KW_C */

    /* mbedtls_cipher_free() */
    TEST_VALID_PARAM( mbedtls_cipher_free( NULL ) );
exit:
    TEST_VALID_PARAM( mbedtls_cipher_free( &valid_ctx ) );
}

void test_cipher_invalid_param_conditional_wrapper( void ** params )
{
    (void)params;

    test_cipher_invalid_param_conditional(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
#if defined(MBEDTLS_AES_C)
void test_cipher_special_behaviours(  )
{
    const mbedtls_cipher_info_t *cipher_info;
    mbedtls_cipher_context_t ctx;
    unsigned char input[32];
    unsigned char output[32];
#if defined (MBEDTLS_CIPHER_MODE_CBC)
    unsigned char iv[32];
#endif
    size_t olen = 0;

    mbedtls_cipher_init( &ctx );
    mbedtls_platform_zeroize( input, sizeof( input ) );
    mbedtls_platform_zeroize( output, sizeof( output ) );
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    memset( iv, 0, sizeof( iv ) );

    /* Check and get info structures */
    cipher_info = mbedtls_cipher_info_from_type( MBEDTLS_CIPHER_AES_128_CBC );
    TEST_ASSERT( NULL != cipher_info );

    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx, cipher_info ) );

    /* IV too big */
    TEST_ASSERT( mbedtls_cipher_set_iv( &ctx, iv, MBEDTLS_MAX_IV_LENGTH + 1 )
                 == MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE );

    /* IV too small */
    TEST_ASSERT( mbedtls_cipher_set_iv( &ctx, iv, 0 )
                 == MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    mbedtls_cipher_free( &ctx );
    mbedtls_cipher_init( &ctx );
#endif /* MBEDTLS_CIPHER_MODE_CBC */
    cipher_info = mbedtls_cipher_info_from_type( MBEDTLS_CIPHER_AES_128_ECB );
    TEST_ASSERT( NULL != cipher_info );

    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx, cipher_info ) );

    /* Update ECB with partial block */
    TEST_ASSERT( mbedtls_cipher_update( &ctx, input, 1, output, &olen )
                 == MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED );

exit:
    mbedtls_cipher_free( &ctx );
}

void test_cipher_special_behaviours_wrapper( void ** params )
{
    (void)params;

    test_cipher_special_behaviours(  );
}
#endif /* MBEDTLS_AES_C */
void test_enc_dec_buf( int cipher_id, char * cipher_string, int key_len,
                  int length_val, int pad_mode )
{
    size_t length = length_val, outlen, total_len, i, block_size;
    unsigned char key[64];
    unsigned char iv[16];
    unsigned char ad[13];
    unsigned char tag[16];
    unsigned char inbuf[64];
    unsigned char encbuf[64];
    unsigned char decbuf[64];

    const mbedtls_cipher_info_t *cipher_info;
    mbedtls_cipher_context_t ctx_dec;
    mbedtls_cipher_context_t ctx_enc;

    /*
     * Prepare contexts
     */
    mbedtls_cipher_init( &ctx_dec );
    mbedtls_cipher_init( &ctx_enc );

    memset( key, 0x2a, sizeof( key ) );

    /* Check and get info structures */
    cipher_info = mbedtls_cipher_info_from_type( cipher_id );
    TEST_ASSERT( NULL != cipher_info );
    TEST_ASSERT( mbedtls_cipher_info_from_string( cipher_string ) == cipher_info );

    /* Initialise enc and dec contexts */
    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx_dec, cipher_info ) );
    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx_enc, cipher_info ) );

    TEST_ASSERT( 0 == mbedtls_cipher_setkey( &ctx_dec, key, key_len, MBEDTLS_DECRYPT ) );
    TEST_ASSERT( 0 == mbedtls_cipher_setkey( &ctx_enc, key, key_len, MBEDTLS_ENCRYPT ) );

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    if( -1 != pad_mode )
    {
        TEST_ASSERT( 0 == mbedtls_cipher_set_padding_mode( &ctx_dec, pad_mode ) );
        TEST_ASSERT( 0 == mbedtls_cipher_set_padding_mode( &ctx_enc, pad_mode ) );
    }
#else
    (void) pad_mode;
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */

    /*
     * Do a few encode/decode cycles
     */
    for( i = 0; i < 3; i++ )
    {
    memset( iv , 0x00 + i, sizeof( iv ) );
    memset( ad, 0x10 + i, sizeof( ad ) );
    memset( inbuf, 0x20 + i, sizeof( inbuf ) );

    memset( encbuf, 0, sizeof( encbuf ) );
    memset( decbuf, 0, sizeof( decbuf ) );
    memset( tag, 0, sizeof( tag ) );

    TEST_ASSERT( 0 == mbedtls_cipher_set_iv( &ctx_dec, iv, sizeof( iv ) ) );
    TEST_ASSERT( 0 == mbedtls_cipher_set_iv( &ctx_enc, iv, sizeof( iv ) ) );

    TEST_ASSERT( 0 == mbedtls_cipher_reset( &ctx_dec ) );
    TEST_ASSERT( 0 == mbedtls_cipher_reset( &ctx_enc ) );

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    TEST_ASSERT( 0 == mbedtls_cipher_update_ad( &ctx_dec, ad, sizeof( ad ) - i ) );
    TEST_ASSERT( 0 == mbedtls_cipher_update_ad( &ctx_enc, ad, sizeof( ad ) - i ) );
#endif

    block_size = mbedtls_cipher_get_block_size( &ctx_enc );
    TEST_ASSERT( block_size != 0 );

    /* encode length number of bytes from inbuf */
    TEST_ASSERT( 0 == mbedtls_cipher_update( &ctx_enc, inbuf, length, encbuf, &outlen ) );
    total_len = outlen;

    TEST_ASSERT( total_len == length ||
                 ( total_len % block_size == 0 &&
                   total_len < length &&
                   total_len + block_size > length ) );

    TEST_ASSERT( 0 == mbedtls_cipher_finish( &ctx_enc, encbuf + outlen, &outlen ) );
    total_len += outlen;

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    TEST_ASSERT( 0 == mbedtls_cipher_write_tag( &ctx_enc, tag, sizeof( tag ) ) );
#endif

    TEST_ASSERT( total_len == length ||
                 ( total_len % block_size == 0 &&
                   total_len > length &&
                   total_len <= length + block_size ) );

    /* decode the previously encoded string */
    TEST_ASSERT( 0 == mbedtls_cipher_update( &ctx_dec, encbuf, total_len, decbuf, &outlen ) );
    total_len = outlen;

    TEST_ASSERT( total_len == length ||
                 ( total_len % block_size == 0 &&
                   total_len < length &&
                   total_len + block_size >= length ) );

    TEST_ASSERT( 0 == mbedtls_cipher_finish( &ctx_dec, decbuf + outlen, &outlen ) );
    total_len += outlen;

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    TEST_ASSERT( 0 == mbedtls_cipher_check_tag( &ctx_dec, tag, sizeof( tag ) ) );
#endif

    /* check result */
    TEST_ASSERT( total_len == length );
    TEST_ASSERT( 0 == timingsafe_bcmp(inbuf, decbuf, length) );
    }

    /*
     * Done
     */
exit:
    mbedtls_cipher_free( &ctx_dec );
    mbedtls_cipher_free( &ctx_enc );
}

void test_enc_dec_buf_wrapper( void ** params )
{

    test_enc_dec_buf( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
void test_enc_fail( int cipher_id, int pad_mode, int key_len, int length_val,
               int ret )
{
    size_t length = length_val;
    unsigned char key[32];
    unsigned char iv[16];

    const mbedtls_cipher_info_t *cipher_info;
    mbedtls_cipher_context_t ctx;

    unsigned char inbuf[64];
    unsigned char encbuf[64];

    size_t outlen = 0;

    memset( key, 0, 32 );
    memset( iv , 0, 16 );

    mbedtls_cipher_init( &ctx );

    memset( inbuf, 5, 64 );
    memset( encbuf, 0, 64 );

    /* Check and get info structures */
    cipher_info = mbedtls_cipher_info_from_type( cipher_id );
    TEST_ASSERT( NULL != cipher_info );

    /* Initialise context */
    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx, cipher_info ) );
    TEST_ASSERT( 0 == mbedtls_cipher_setkey( &ctx, key, key_len, MBEDTLS_ENCRYPT ) );
#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    TEST_ASSERT( 0 == mbedtls_cipher_set_padding_mode( &ctx, pad_mode ) );
#else
    (void) pad_mode;
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */
    TEST_ASSERT( 0 == mbedtls_cipher_set_iv( &ctx, iv, 16 ) );
    TEST_ASSERT( 0 == mbedtls_cipher_reset( &ctx ) );
#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    TEST_ASSERT( 0 == mbedtls_cipher_update_ad( &ctx, NULL, 0 ) );
#endif

    /* encode length number of bytes from inbuf */
    TEST_ASSERT( 0 == mbedtls_cipher_update( &ctx, inbuf, length, encbuf, &outlen ) );
    TEST_ASSERT( ret == mbedtls_cipher_finish( &ctx, encbuf + outlen, &outlen ) );

    /* done */
exit:
    mbedtls_cipher_free( &ctx );
}

void test_enc_fail_wrapper( void ** params )
{

    test_enc_fail( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
void test_dec_empty_buf( int cipher,
                    int expected_update_ret,
                    int expected_finish_ret )
{
    unsigned char key[32];
    unsigned char iv[16];

    mbedtls_cipher_context_t ctx_dec;
    const mbedtls_cipher_info_t *cipher_info;

    unsigned char encbuf[64];
    unsigned char decbuf[64];

    size_t outlen = 0;

    memset( key, 0, 32 );
    memset( iv , 0, 16 );

    mbedtls_cipher_init( &ctx_dec );

    memset( encbuf, 0, 64 );
    memset( decbuf, 0, 64 );

    /* Initialise context */
    cipher_info = mbedtls_cipher_info_from_type( cipher );
    TEST_ASSERT( NULL != cipher_info);
    TEST_ASSERT( sizeof(key) * 8 >= cipher_info->key_bitlen );

    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx_dec, cipher_info ) );

    TEST_ASSERT( 0 == mbedtls_cipher_setkey( &ctx_dec,
                                             key, cipher_info->key_bitlen,
                                             MBEDTLS_DECRYPT ) );

    TEST_ASSERT( 0 == mbedtls_cipher_set_iv( &ctx_dec, iv, 16 ) );

    TEST_ASSERT( 0 == mbedtls_cipher_reset( &ctx_dec ) );

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    TEST_ASSERT( 0 == mbedtls_cipher_update_ad( &ctx_dec, NULL, 0 ) );
#endif

    /* decode 0-byte string */
    TEST_ASSERT( expected_update_ret ==
                 mbedtls_cipher_update( &ctx_dec, encbuf, 0, decbuf, &outlen ) );
    TEST_ASSERT( 0 == outlen );

    if ( expected_finish_ret == 0 &&
         ( cipher_info->mode == MBEDTLS_MODE_CBC ||
           cipher_info->mode == MBEDTLS_MODE_ECB ) )
    {
        /* Non-CBC and non-ECB ciphers are OK with decrypting empty buffers and
         * return success, not MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED, when
         * decrypting an empty buffer.
         * On the other hand, CBC and ECB ciphers need a full block of input.
         */
        expected_finish_ret = MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED;
    }

    TEST_ASSERT( expected_finish_ret == mbedtls_cipher_finish(
                                        &ctx_dec, decbuf + outlen, &outlen ) );
    TEST_ASSERT( 0 == outlen );

exit:
    mbedtls_cipher_free( &ctx_dec );
}

void test_dec_empty_buf_wrapper( void ** params )
{

    test_dec_empty_buf( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
void test_enc_dec_buf_multipart( int cipher_id, int key_len, int first_length_val,
                            int second_length_val, int pad_mode,
                            int first_encrypt_output_len, int second_encrypt_output_len,
                            int first_decrypt_output_len, int second_decrypt_output_len )
{
    size_t first_length = first_length_val;
    size_t second_length = second_length_val;
    size_t length = first_length + second_length;
    size_t block_size;
    unsigned char key[32];
    unsigned char iv[16];

    mbedtls_cipher_context_t ctx_dec;
    mbedtls_cipher_context_t ctx_enc;
    const mbedtls_cipher_info_t *cipher_info;

    unsigned char inbuf[64];
    unsigned char encbuf[64];
    unsigned char decbuf[64];

    size_t outlen = 0;
    size_t totaloutlen = 0;

    memset( key, 0, 32 );
    memset( iv , 0, 16 );

    mbedtls_cipher_init( &ctx_dec );
    mbedtls_cipher_init( &ctx_enc );

    memset( inbuf, 5, 64 );
    memset( encbuf, 0, 64 );
    memset( decbuf, 0, 64 );

    /* Initialise enc and dec contexts */
    cipher_info = mbedtls_cipher_info_from_type( cipher_id );
    TEST_ASSERT( NULL != cipher_info);

    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx_dec, cipher_info ) );
    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx_enc, cipher_info ) );

    TEST_ASSERT( 0 == mbedtls_cipher_setkey( &ctx_dec, key, key_len, MBEDTLS_DECRYPT ) );
    TEST_ASSERT( 0 == mbedtls_cipher_setkey( &ctx_enc, key, key_len, MBEDTLS_ENCRYPT ) );

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    if( -1 != pad_mode )
    {
        TEST_ASSERT( 0 == mbedtls_cipher_set_padding_mode( &ctx_dec, pad_mode ) );
        TEST_ASSERT( 0 == mbedtls_cipher_set_padding_mode( &ctx_enc, pad_mode ) );
    }
#else
    (void) pad_mode;
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */

    TEST_ASSERT( 0 == mbedtls_cipher_set_iv( &ctx_dec, iv, 16 ) );
    TEST_ASSERT( 0 == mbedtls_cipher_set_iv( &ctx_enc, iv, 16 ) );

    TEST_ASSERT( 0 == mbedtls_cipher_reset( &ctx_dec ) );
    TEST_ASSERT( 0 == mbedtls_cipher_reset( &ctx_enc ) );

#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    TEST_ASSERT( 0 == mbedtls_cipher_update_ad( &ctx_dec, NULL, 0 ) );
    TEST_ASSERT( 0 == mbedtls_cipher_update_ad( &ctx_enc, NULL, 0 ) );
#endif

    block_size = mbedtls_cipher_get_block_size( &ctx_enc );
    TEST_ASSERT( block_size != 0 );

    /* encode length number of bytes from inbuf */
    TEST_ASSERT( 0 == mbedtls_cipher_update( &ctx_enc, inbuf, first_length, encbuf, &outlen ) );
    TEST_ASSERT( (size_t)first_encrypt_output_len == outlen );
    totaloutlen = outlen;
    TEST_ASSERT( 0 == mbedtls_cipher_update( &ctx_enc, inbuf + first_length, second_length, encbuf + totaloutlen, &outlen ) );
    TEST_ASSERT( (size_t)second_encrypt_output_len == outlen );
    totaloutlen += outlen;
    TEST_ASSERT( totaloutlen == length ||
                 ( totaloutlen % block_size == 0 &&
                   totaloutlen < length &&
                   totaloutlen + block_size > length ) );

    TEST_ASSERT( 0 == mbedtls_cipher_finish( &ctx_enc, encbuf + totaloutlen, &outlen ) );
    totaloutlen += outlen;
    TEST_ASSERT( totaloutlen == length ||
                 ( totaloutlen % block_size == 0 &&
                   totaloutlen > length &&
                   totaloutlen <= length + block_size ) );

    /* decode the previously encoded string */
    second_length = totaloutlen - first_length;
    TEST_ASSERT( 0 == mbedtls_cipher_update( &ctx_dec, encbuf, first_length, decbuf, &outlen ) );
    TEST_ASSERT( (size_t)first_decrypt_output_len == outlen );
    totaloutlen = outlen;
    TEST_ASSERT( 0 == mbedtls_cipher_update( &ctx_dec, encbuf + first_length, second_length, decbuf + totaloutlen, &outlen ) );
    TEST_ASSERT( (size_t)second_decrypt_output_len == outlen );
    totaloutlen += outlen;

    TEST_ASSERT( totaloutlen == length ||
                 ( totaloutlen % block_size == 0 &&
                   totaloutlen < length &&
                   totaloutlen + block_size >= length ) );

    TEST_ASSERT( 0 == mbedtls_cipher_finish( &ctx_dec, decbuf + totaloutlen, &outlen ) );
    totaloutlen += outlen;

    TEST_ASSERT( totaloutlen == length );

    TEST_ASSERT( 0 == timingsafe_bcmp(inbuf, decbuf, length) );

exit:
    mbedtls_cipher_free( &ctx_dec );
    mbedtls_cipher_free( &ctx_enc );
}

void test_enc_dec_buf_multipart_wrapper( void ** params )
{

    test_enc_dec_buf_multipart( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ) );
}
void test_decrypt_test_vec( int cipher_id, int pad_mode, data_t * key,
                       data_t * iv, data_t * cipher,
                       data_t * clear, data_t * ad, data_t * tag,
                       int finish_result, int tag_result )
{
    unsigned char output[265];
    mbedtls_cipher_context_t ctx;
    size_t outlen, total_len;

    mbedtls_cipher_init( &ctx );

    memset( output, 0x00, sizeof( output ) );

#if !defined(MBEDTLS_GCM_C) && !defined(MBEDTLS_CHACHAPOLY_C)
    ((void) ad);
    ((void) tag);
#endif

    /* Prepare context */
    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx,
                                       mbedtls_cipher_info_from_type( cipher_id ) ) );
    TEST_ASSERT( 0 == mbedtls_cipher_setkey( &ctx, key->x, 8 * key->len, MBEDTLS_DECRYPT ) );
#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    if( pad_mode != -1 )
        TEST_ASSERT( 0 == mbedtls_cipher_set_padding_mode( &ctx, pad_mode ) );
#else
    (void) pad_mode;
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */
    TEST_ASSERT( 0 == mbedtls_cipher_set_iv( &ctx, iv->x, iv->len ) );
    TEST_ASSERT( 0 == mbedtls_cipher_reset( &ctx ) );
#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    TEST_ASSERT( 0 == mbedtls_cipher_update_ad( &ctx, ad->x, ad->len ) );
#endif

    /* decode buffer and check tag->x */
    total_len = 0;
    TEST_ASSERT( 0 == mbedtls_cipher_update( &ctx, cipher->x, cipher->len, output, &outlen ) );
    total_len += outlen;
    TEST_ASSERT( finish_result == mbedtls_cipher_finish( &ctx, output + outlen,
                                                 &outlen ) );
    total_len += outlen;
#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CHACHAPOLY_C)
    TEST_ASSERT( tag_result == mbedtls_cipher_check_tag( &ctx, tag->x, tag->len ) );
#endif

    /* check plaintext only if everything went fine */
    if( 0 == finish_result && 0 == tag_result )
    {
        TEST_ASSERT( total_len == clear->len );
        TEST_ASSERT( 0 == timingsafe_bcmp( output, clear->x, clear->len ) );
    }

exit:
    mbedtls_cipher_free( &ctx );
}

void test_decrypt_test_vec_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_decrypt_test_vec( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8, &data10, &data12, *( (int *) params[14] ), *( (int *) params[15] ) );
}
#if defined(MBEDTLS_CIPHER_AUTH_CRYPT)
void test_auth_crypt_tv( int cipher_id, data_t * key, data_t * iv,
                    data_t * ad, data_t * cipher, data_t * tag,
                    char * result, data_t * clear, int use_psa )
{
    /*
     * Take an AEAD ciphertext + tag and perform a pair
     * of AEAD decryption and AEAD encryption. Check that
     * this results in the expected plaintext, and that
     * decryption and encryption are inverse to one another.
     *
     * Do that twice:
     * - once with legacy functions auth_decrypt/auth_encrypt
     * - once with new functions auth_decrypt_ext/auth_encrypt_ext
     * This allows testing both without duplicating test cases.
     */

    int ret;
    int using_nist_kw, using_nist_kw_padding;

    mbedtls_cipher_context_t ctx;
    size_t outlen;

    unsigned char *cipher_plus_tag = NULL;
    size_t cipher_plus_tag_len;
    unsigned char *decrypt_buf = NULL;
    size_t decrypt_buf_len = 0;
    unsigned char *encrypt_buf = NULL;
    size_t encrypt_buf_len = 0;

#if !defined(MBEDTLS_DEPRECATED_WARNING) && \
    !defined(MBEDTLS_DEPRECATED_REMOVED)
    unsigned char *tmp_tag    = NULL;
    unsigned char *tmp_cipher = NULL;
    unsigned char *tag_buf = NULL;
#endif /* !MBEDTLS_DEPRECATED_WARNING && !MBEDTLS_DEPRECATED_REMOVED */

    /* Null pointers are documented as valid for inputs of length 0.
     * The test framework passes non-null pointers, so set them to NULL.
     * key, cipher and tag can't be empty. */
    if( iv->len == 0 )
        iv->x = NULL;
    if( ad->len == 0 )
        ad->x = NULL;
    if( clear->len == 0 )
        clear->x = NULL;

    mbedtls_cipher_init( &ctx );

    /*
     * Are we using NIST_KW? with padding?
     */
    using_nist_kw_padding = cipher_id == MBEDTLS_CIPHER_AES_128_KWP ||
                            cipher_id == MBEDTLS_CIPHER_AES_192_KWP ||
                            cipher_id == MBEDTLS_CIPHER_AES_256_KWP;
    using_nist_kw = cipher_id == MBEDTLS_CIPHER_AES_128_KW ||
                    cipher_id == MBEDTLS_CIPHER_AES_192_KW ||
                    cipher_id == MBEDTLS_CIPHER_AES_256_KW ||
                    using_nist_kw_padding;

    /****************************************************************
     *                                                              *
     *  Part 1: non-deprecated API                                  *
     *                                                              *
     ****************************************************************/

    /*
     * Prepare context for decryption
     */
    if( ! cipher_reset_key( &ctx, cipher_id, use_psa, tag->len, key,
                            MBEDTLS_DECRYPT ) )
        goto exit;

    /*
     * prepare buffer for decryption
     * (we need the tag appended to the ciphertext)
     */
    cipher_plus_tag_len = cipher->len + tag->len;
    ASSERT_ALLOC( cipher_plus_tag, cipher_plus_tag_len );
    memcpy( cipher_plus_tag, cipher->x, cipher->len );
    memcpy( cipher_plus_tag + cipher->len, tag->x, tag->len );

    /*
     * Compute length of output buffer according to the documentation
     */
    if( using_nist_kw )
        decrypt_buf_len = cipher_plus_tag_len - 8;
    else
        decrypt_buf_len = cipher_plus_tag_len - tag->len;


    /*
     * Try decrypting to a buffer that's 1B too small
     */
    if( decrypt_buf_len != 0 )
    {
        ASSERT_ALLOC( decrypt_buf, decrypt_buf_len - 1 );

        outlen = 0;
        ret = mbedtls_cipher_auth_decrypt_ext( &ctx, iv->x, iv->len,
                ad->x, ad->len, cipher_plus_tag, cipher_plus_tag_len,
                decrypt_buf, decrypt_buf_len - 1, &outlen, tag->len );
        TEST_ASSERT( ret == MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

        mbedtls_free( decrypt_buf );
        decrypt_buf = NULL;
    }

    /*
     * Authenticate and decrypt, and check result
     */
    ASSERT_ALLOC( decrypt_buf, decrypt_buf_len );

    outlen = 0;
    ret = mbedtls_cipher_auth_decrypt_ext( &ctx, iv->x, iv->len,
            ad->x, ad->len, cipher_plus_tag, cipher_plus_tag_len,
            decrypt_buf, decrypt_buf_len, &outlen, tag->len );

    if( strcmp( result, "FAIL" ) == 0 )
    {
        TEST_ASSERT( ret == MBEDTLS_ERR_CIPHER_AUTH_FAILED );
        TEST_ASSERT( buffer_is_all_zero( decrypt_buf, decrypt_buf_len ) );
    }
    else
    {
        TEST_ASSERT( ret == 0 );
        ASSERT_COMPARE( decrypt_buf, outlen, clear->x, clear->len );
    }

    /* Free this, but keep cipher_plus_tag for deprecated function with PSA */
    mbedtls_free( decrypt_buf );
    decrypt_buf = NULL;

    /*
     * Encrypt back if test data was authentic
     */
    if( strcmp( result, "FAIL" ) != 0 )
    {
        /* prepare context for encryption */
        if( ! cipher_reset_key( &ctx, cipher_id, use_psa, tag->len, key,
                                MBEDTLS_ENCRYPT ) )
            goto exit;

        /*
         * Compute size of output buffer according to documentation
         */
        if( using_nist_kw )
        {
            encrypt_buf_len = clear->len + 8;
            if( using_nist_kw_padding && encrypt_buf_len % 8 != 0 )
                encrypt_buf_len += 8 - encrypt_buf_len % 8;
        }
        else
        {
            encrypt_buf_len = clear->len + tag->len;
        }

        /*
         * Try encrypting with an output buffer that's 1B too small
         */
        ASSERT_ALLOC( encrypt_buf, encrypt_buf_len - 1 );

        outlen = 0;
        ret = mbedtls_cipher_auth_encrypt_ext( &ctx, iv->x, iv->len,
                ad->x, ad->len, clear->x, clear->len,
                encrypt_buf, encrypt_buf_len - 1, &outlen, tag->len );
        TEST_ASSERT( ret != 0 );

        mbedtls_free( encrypt_buf );
        encrypt_buf = NULL;

        /*
         * Encrypt and check the result
         */
        ASSERT_ALLOC( encrypt_buf, encrypt_buf_len );

        outlen = 0;
        ret = mbedtls_cipher_auth_encrypt_ext( &ctx, iv->x, iv->len,
                ad->x, ad->len, clear->x, clear->len,
                encrypt_buf, encrypt_buf_len, &outlen, tag->len );
        TEST_ASSERT( ret == 0 );

        TEST_ASSERT( outlen == cipher->len + tag->len );
        TEST_ASSERT( timingsafe_bcmp( encrypt_buf, cipher->x, cipher->len ) == 0 );
        TEST_ASSERT( timingsafe_bcmp( encrypt_buf + cipher->len,
                             tag->x, tag->len ) == 0 );

        mbedtls_free( encrypt_buf );
        encrypt_buf = NULL;
    }

    /****************************************************************
     *                                                              *
     *  Part 2: deprecated API                                      *
     *                                                              *
     ****************************************************************/

#if !defined(MBEDTLS_DEPRECATED_WARNING) && \
    !defined(MBEDTLS_DEPRECATED_REMOVED)

    /*
     * Prepare context for decryption
     */
    if( ! cipher_reset_key( &ctx, cipher_id, use_psa, tag->len, key,
                            MBEDTLS_DECRYPT ) )
        goto exit;

    /*
     * Prepare pointers for decryption
     */
    tmp_cipher = cipher->x;
    tmp_tag = tag->x;

    /*
     * Authenticate and decrypt, and check result
     */

    ASSERT_ALLOC( decrypt_buf, cipher->len );
    outlen = 0;
    ret = mbedtls_cipher_auth_decrypt( &ctx, iv->x, iv->len, ad->x, ad->len,
                               tmp_cipher, cipher->len, decrypt_buf, &outlen,
                               tmp_tag, tag->len );

    if( using_nist_kw )
    {
        /* NIST_KW with legacy API */
        TEST_ASSERT( ret == MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE );
    }
    else if( strcmp( result, "FAIL" ) == 0 )
    {
        /* unauthentic message */
        TEST_ASSERT( ret == MBEDTLS_ERR_CIPHER_AUTH_FAILED );
        TEST_ASSERT( buffer_is_all_zero( decrypt_buf, cipher->len ) );
    }
    else
    {
        /* authentic message: is the plaintext correct? */
        TEST_ASSERT( ret == 0 );
        ASSERT_COMPARE(  decrypt_buf, outlen, clear->x, clear->len );
    }

    mbedtls_free( decrypt_buf );
    decrypt_buf = NULL;
    mbedtls_free( cipher_plus_tag );
    cipher_plus_tag = NULL;

    /*
     * Encrypt back if test data was authentic
     */
    if( strcmp( result, "FAIL" ) != 0 )
    {
        /* prepare context for encryption */
        if( ! cipher_reset_key( &ctx, cipher_id, use_psa, tag->len, key,
                                MBEDTLS_ENCRYPT ) )
            goto exit;

        /* prepare buffers for encryption */
        ASSERT_ALLOC( encrypt_buf, cipher->len );
        ASSERT_ALLOC( tag_buf, tag->len );
        tmp_cipher = encrypt_buf;
        tmp_tag = tag_buf;

        /*
         * Encrypt and check the result
         */
        outlen = 0;
        ret = mbedtls_cipher_auth_encrypt( &ctx, iv->x, iv->len, ad->x, ad->len,
                                   clear->x, clear->len, tmp_cipher, &outlen,
                                   tmp_tag, tag->len );

        if( using_nist_kw )
        {
            TEST_ASSERT( ret == MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE );
        }
        else
        {
            TEST_ASSERT( ret == 0 );

            TEST_ASSERT( outlen == cipher->len );
            if( cipher->len != 0 )
                TEST_ASSERT( timingsafe_bcmp( tmp_cipher, cipher->x, cipher->len ) == 0 );
            TEST_ASSERT( timingsafe_bcmp( tmp_tag, tag->x, tag->len ) == 0 );
        }
    }

#endif /* !MBEDTLS_DEPRECATED_WARNING && !MBEDTLS_DEPRECATED_REMOVED */

exit:

    mbedtls_cipher_free( &ctx );
    mbedtls_free( decrypt_buf );
    mbedtls_free( encrypt_buf );
    mbedtls_free( cipher_plus_tag );
#if !defined(MBEDTLS_DEPRECATED_WARNING) && \
    !defined(MBEDTLS_DEPRECATED_REMOVED)
    mbedtls_free( tag_buf );
#endif /* !MBEDTLS_DEPRECATED_WARNING && !MBEDTLS_DEPRECATED_REMOVED */
}

void test_auth_crypt_tv_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_auth_crypt_tv( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9, (char *) params[11], &data12, *( (int *) params[14] ) );
}
#endif /* MBEDTLS_CIPHER_AUTH_CRYPT */
void test_test_vec_ecb( int cipher_id, int operation, data_t * key,
                   data_t * input, data_t * result, int finish_result
                   )
{
    mbedtls_cipher_context_t ctx;
    unsigned char output[32];
    size_t outlen;

    mbedtls_cipher_init( &ctx );

    memset( output, 0x00, sizeof( output ) );

    /* Prepare context */
    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx,
                                       mbedtls_cipher_info_from_type( cipher_id ) ) );


    TEST_ASSERT( 0 == mbedtls_cipher_setkey( &ctx, key->x, 8 * key->len, operation ) );

    TEST_ASSERT( 0 == mbedtls_cipher_update( &ctx, input->x,
                                     mbedtls_cipher_get_block_size( &ctx ),
                                     output, &outlen ) );
    TEST_ASSERT( outlen == mbedtls_cipher_get_block_size( &ctx ) );
    TEST_ASSERT( finish_result == mbedtls_cipher_finish( &ctx, output + outlen,
                                                 &outlen ) );
    TEST_ASSERT( 0 == outlen );

    /* check plaintext only if everything went fine */
    if( 0 == finish_result )
        TEST_ASSERT( 0 == timingsafe_bcmp( output, result->x,
                                  mbedtls_cipher_get_block_size( &ctx ) ) );

exit:
    mbedtls_cipher_free( &ctx );
}

void test_test_vec_ecb_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_test_vec_ecb( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, *( (int *) params[8] ) );
}
#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
void test_test_vec_crypt( int cipher_id, int operation, data_t *key,
                     data_t *iv, data_t *input, data_t *result,
                     int finish_result, int use_psa )
{
    mbedtls_cipher_context_t ctx;
    unsigned char output[32];
    size_t outlen;

    mbedtls_cipher_init( &ctx );

    memset( output, 0x00, sizeof( output ) );

    /* Prepare context */
    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx,
                              mbedtls_cipher_info_from_type( cipher_id ) ) );

    TEST_ASSERT( 0 == mbedtls_cipher_setkey( &ctx, key->x, 8 * key->len, operation ) );
    if( MBEDTLS_MODE_CBC == ctx.cipher_info->mode )
        TEST_ASSERT( 0 == mbedtls_cipher_set_padding_mode( &ctx, MBEDTLS_PADDING_NONE ) );

    TEST_ASSERT( finish_result == mbedtls_cipher_crypt( &ctx, iv->len ? iv->x : NULL,
                                                        iv->len, input->x, input->len,
                                                        output, &outlen ) );
    TEST_ASSERT( result->len == outlen );
    /* check plaintext only if everything went fine */
    if( 0 == finish_result )
        TEST_ASSERT( 0 == timingsafe_bcmp( output, result->x, outlen ) );

exit:
    mbedtls_cipher_free( &ctx );
}

void test_test_vec_crypt_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_test_vec_crypt( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */
#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
void test_set_padding( int cipher_id, int pad_mode, int ret )
{
    const mbedtls_cipher_info_t *cipher_info;
    mbedtls_cipher_context_t ctx;

    mbedtls_cipher_init( &ctx );

    cipher_info = mbedtls_cipher_info_from_type( cipher_id );
    TEST_ASSERT( NULL != cipher_info );
    TEST_ASSERT( 0 == mbedtls_cipher_setup( &ctx, cipher_info ) );

    TEST_ASSERT( ret == mbedtls_cipher_set_padding_mode( &ctx, pad_mode ) );

exit:
    mbedtls_cipher_free( &ctx );
}

void test_set_padding_wrapper( void ** params )
{

    test_set_padding( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */
#if defined(MBEDTLS_CIPHER_MODE_CBC)
void test_check_padding( int pad_mode, data_t * input, int ret, int dlen_check
                    )
{
    mbedtls_cipher_info_t cipher_info;
    mbedtls_cipher_context_t ctx;
    size_t dlen;

    /* build a fake context just for getting access to get_padding */
    mbedtls_cipher_init( &ctx );
    cipher_info.mode = MBEDTLS_MODE_CBC;
    ctx.cipher_info = &cipher_info;

    TEST_ASSERT( 0 == mbedtls_cipher_set_padding_mode( &ctx, pad_mode ) );


    TEST_ASSERT( ret == ctx.get_padding( input->x, input->len, &dlen ) );
    if( 0 == ret )
        TEST_ASSERT( dlen == (size_t) dlen_check );
exit:
    ;
}

void test_check_padding_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_check_padding( *( (int *) params[0] ), &data1, *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#endif /* MBEDTLS_CIPHER_C */

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

#if defined(MBEDTLS_CIPHER_C)

        case 0:
            {
                *out_value = MBEDTLS_CIPHER_DES_CBC;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_CIPHER_DES_EDE_CBC;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_CIPHER_DES_EDE3_CBC;
            }
            break;
        case 3:
            {
                *out_value = -1;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_PADDING_ONE_AND_ZEROS;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_PADDING_ZEROS_AND_LEN;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_PADDING_ZEROS;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_PADDING_NONE;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED;
            }
            break;
        case 9:
            {
                *out_value = MBEDTLS_CIPHER_DES_ECB;
            }
            break;
        case 10:
            {
                *out_value = MBEDTLS_ENCRYPT;
            }
            break;
        case 11:
            {
                *out_value = MBEDTLS_DECRYPT;
            }
            break;
        case 12:
            {
                *out_value = MBEDTLS_CIPHER_DES_EDE_ECB;
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

#if defined(MBEDTLS_CIPHER_C)

        case 0:
            {
#if defined(MBEDTLS_DES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_CIPHER_MODE_CBC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_CIPHER_PADDING_PKCS7)
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

#if defined(MBEDTLS_CIPHER_C)
    test_mbedtls_cipher_list_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_CIPHER_C)
    test_cipher_invalid_param_unconditional_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_cipher_invalid_param_conditional_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_AES_C)
    test_cipher_special_behaviours_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_CIPHER_C)
    test_enc_dec_buf_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_CIPHER_C)
    test_enc_fail_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_CIPHER_C)
    test_dec_empty_buf_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_CIPHER_C)
    test_enc_dec_buf_multipart_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_CIPHER_C)
    test_decrypt_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_CIPHER_AUTH_CRYPT)
    test_auth_crypt_tv_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_CIPHER_C)
    test_test_vec_ecb_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    test_test_vec_crypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    test_set_padding_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_CIPHER_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
    test_check_padding_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_cipher.des.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
