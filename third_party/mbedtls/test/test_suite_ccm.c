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
 * generate_test_code.py and then mbedtls_test_suite.sh
 *
 * Test file      : ./test_suite_ccm.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_ccm.function
 *      Test suite data     : suites/test_suite_ccm.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_CCM_C)
#include "third_party/mbedtls/ccm.h"
#if defined(MBEDTLS_SELF_TEST)
#if defined(MBEDTLS_AES_C)
void test_mbedtls_ccm_self_test(  )
{
    TEST_ASSERT( mbedtls_ccm_self_test( 1 ) == 0 );
exit:
    ;
}

void test_mbedtls_ccm_self_test_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_ccm_self_test(  );
}
#endif /* MBEDTLS_AES_C */
#endif /* MBEDTLS_SELF_TEST */
void test_mbedtls_ccm_setkey( int cipher_id, int key_size, int result )
{
    mbedtls_ccm_context ctx;
    unsigned char key[32];
    int ret;

    mbedtls_ccm_init( &ctx );

    memset( key, 0x2A, sizeof( key ) );
    TEST_ASSERT( (unsigned) key_size <= 8 * sizeof( key ) );

    ret = mbedtls_ccm_setkey( &ctx, cipher_id, key, key_size );
    TEST_ASSERT( ret == result );

exit:
    mbedtls_ccm_free( &ctx );
}

void test_mbedtls_ccm_setkey_wrapper( void ** params )
{

    test_mbedtls_ccm_setkey( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#if defined(MBEDTLS_AES_C)
void test_ccm_lengths( int msg_len, int iv_len, int add_len, int tag_len, int res )
{
    mbedtls_ccm_context ctx;
    unsigned char key[16];
    unsigned char msg[10];
    unsigned char iv[14];
    unsigned char *add = NULL;
    unsigned char out[10];
    unsigned char tag[18];
    int decrypt_ret;

    mbedtls_ccm_init( &ctx );

    ASSERT_ALLOC_WEAK( add, add_len );
    memset( key, 0, sizeof( key ) );
    memset( msg, 0, sizeof( msg ) );
    memset( iv, 0, sizeof( iv ) );
    memset( out, 0, sizeof( out ) );
    memset( tag, 0, sizeof( tag ) );

    TEST_ASSERT( mbedtls_ccm_setkey( &ctx, MBEDTLS_CIPHER_ID_AES,
                                 key, 8 * sizeof( key ) ) == 0 );

    TEST_ASSERT( mbedtls_ccm_encrypt_and_tag( &ctx, msg_len, iv, iv_len, add, add_len,
                                      msg, out, tag, tag_len ) == res );

    decrypt_ret = mbedtls_ccm_auth_decrypt( &ctx, msg_len, iv, iv_len, add, add_len,
                                    msg, out, tag, tag_len );

    if( res == 0 )
        TEST_ASSERT( decrypt_ret == MBEDTLS_ERR_CCM_AUTH_FAILED );
    else
        TEST_ASSERT( decrypt_ret == res );

exit:
    mbedtls_free( add );
    mbedtls_ccm_free( &ctx );
}

void test_ccm_lengths_wrapper( void ** params )
{

    test_ccm_lengths( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_AES_C */
#if defined(MBEDTLS_AES_C)
void test_ccm_star_lengths( int msg_len, int iv_len, int add_len, int tag_len,
                       int res )
{
    mbedtls_ccm_context ctx;
    unsigned char key[16];
    unsigned char msg[10];
    unsigned char iv[14];
    unsigned char add[10];
    unsigned char out[10];
    unsigned char tag[18];
    int decrypt_ret;

    mbedtls_ccm_init( &ctx );

    memset( key, 0, sizeof( key ) );
    memset( msg, 0, sizeof( msg ) );
    memset( iv, 0, sizeof( iv ) );
    memset( add, 0, sizeof( add ) );
    memset( out, 0, sizeof( out ) );
    memset( tag, 0, sizeof( tag ) );

    TEST_ASSERT( mbedtls_ccm_setkey( &ctx, MBEDTLS_CIPHER_ID_AES,
                                 key, 8 * sizeof( key ) ) == 0 );

    TEST_ASSERT( mbedtls_ccm_star_encrypt_and_tag( &ctx, msg_len, iv, iv_len,
                 add, add_len, msg, out, tag, tag_len ) == res );

    decrypt_ret = mbedtls_ccm_star_auth_decrypt( &ctx, msg_len, iv, iv_len, add,
                  add_len, msg, out, tag, tag_len );

    if( res == 0 && tag_len != 0 )
        TEST_ASSERT( decrypt_ret == MBEDTLS_ERR_CCM_AUTH_FAILED );
    else
        TEST_ASSERT( decrypt_ret == res );

exit:
    mbedtls_ccm_free( &ctx );
}

void test_ccm_star_lengths_wrapper( void ** params )
{

    test_ccm_star_lengths( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_AES_C */
void test_mbedtls_ccm_encrypt_and_tag( int cipher_id, data_t * key,
                                  data_t * msg, data_t * iv,
                                  data_t * add, data_t * result )
{
    mbedtls_ccm_context ctx;
    size_t tag_len;
    uint8_t * msg_n_tag = (uint8_t *)malloc( result->len + 2 );

    mbedtls_ccm_init( &ctx );

    memset( msg_n_tag, 0, result->len + 2 );
    memcpy( msg_n_tag, msg->x, msg->len );

    tag_len = result->len - msg->len;

    TEST_ASSERT( mbedtls_ccm_setkey( &ctx, cipher_id, key->x, key->len * 8 ) == 0 );

    /* Test with input == output */
    TEST_ASSERT( mbedtls_ccm_encrypt_and_tag( &ctx, msg->len, iv->x, iv->len, add->x, add->len,
                 msg_n_tag, msg_n_tag, msg_n_tag + msg->len, tag_len ) == 0 );

    TEST_ASSERT( memcmp( msg_n_tag, result->x, result->len ) == 0 );

    /* Check we didn't write past the end */
    TEST_ASSERT( msg_n_tag[result->len] == 0 && msg_n_tag[result->len + 1] == 0 );

exit:
    mbedtls_ccm_free( &ctx );
    free( msg_n_tag );
}

void test_mbedtls_ccm_encrypt_and_tag_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_mbedtls_ccm_encrypt_and_tag( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9 );
}
void test_mbedtls_ccm_auth_decrypt( int cipher_id, data_t * key,
                               data_t * msg, data_t * iv,
                               data_t * add, int tag_len, int result,
                               data_t * expected_msg )
{
    unsigned char tag[16];
    mbedtls_ccm_context ctx;

    mbedtls_ccm_init( &ctx );

    memset( tag, 0x00, sizeof( tag ) );

    msg->len -= tag_len;
    memcpy( tag, msg->x + msg->len, tag_len );

    TEST_ASSERT( mbedtls_ccm_setkey( &ctx, cipher_id, key->x, key->len * 8 ) == 0 );

    /* Test with input == output */
    TEST_ASSERT( mbedtls_ccm_auth_decrypt( &ctx, msg->len, iv->x, iv->len, add->x, add->len,
                 msg->x, msg->x, msg->x + msg->len, tag_len ) == result );

    if( result == 0 )
    {
        TEST_ASSERT( memcmp( msg->x, expected_msg->x, expected_msg->len ) == 0 );
    }
    else
    {
        size_t i;

        for( i = 0; i < msg->len; i++ )
            TEST_ASSERT( msg->x[i] == 0 );
    }

    /* Check we didn't write past the end (where the original tag is) */
    TEST_ASSERT( memcmp( msg->x + msg->len, tag, tag_len ) == 0 );

exit:
    mbedtls_ccm_free( &ctx );
}

void test_mbedtls_ccm_auth_decrypt_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};

    test_mbedtls_ccm_auth_decrypt( *( (int *) params[0] ), &data1, &data3, &data5, &data7, *( (int *) params[9] ), *( (int *) params[10] ), &data11 );
}
void test_mbedtls_ccm_star_encrypt_and_tag( int cipher_id,
                            data_t *key, data_t *msg,
                            data_t *source_address, data_t *frame_counter,
                            int sec_level, data_t *add,
                            data_t *expected_result, int output_ret )
{
    unsigned char iv[13];
    unsigned char result[50];
    mbedtls_ccm_context ctx;
    size_t iv_len, tag_len;
    int ret;

    mbedtls_ccm_init( &ctx );

    memset( result, 0x00, sizeof( result ) );

    if( sec_level % 4 == 0)
        tag_len = 0;
    else
        tag_len = 1 << ( sec_level % 4 + 1);

    TEST_ASSERT( source_address->len == 8 );
    TEST_ASSERT( frame_counter->len == 4 );
    memcpy( iv, source_address->x, source_address->len );
    memcpy( iv + source_address->len, frame_counter->x, frame_counter->len );
    iv[source_address->len + frame_counter->len] = sec_level;
    iv_len = sizeof( iv );

    TEST_ASSERT( mbedtls_ccm_setkey( &ctx, cipher_id,
                                     key->x, key->len * 8 ) == 0 );

    ret = mbedtls_ccm_star_encrypt_and_tag( &ctx, msg->len, iv, iv_len,
                                            add->x, add->len, msg->x,
                                            result, result + msg->len, tag_len );

    TEST_ASSERT( ret == output_ret );

    TEST_ASSERT( memcmp( result,
                         expected_result->x, expected_result->len ) == 0 );

    /* Check we didn't write past the end */
    TEST_ASSERT( result[expected_result->len] == 0 &&
                 result[expected_result->len + 1] == 0 );

exit:
    mbedtls_ccm_free( &ctx );
}

void test_mbedtls_ccm_star_encrypt_and_tag_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_mbedtls_ccm_star_encrypt_and_tag( *( (int *) params[0] ), &data1, &data3, &data5, &data7, *( (int *) params[9] ), &data10, &data12, *( (int *) params[14] ) );
}
void test_mbedtls_ccm_star_auth_decrypt( int cipher_id,
                            data_t *key, data_t *msg,
                            data_t *source_address, data_t *frame_counter,
                            int sec_level, data_t *add,
                            data_t *expected_result, int output_ret )
{
    unsigned char iv[13];
    unsigned char result[50];
    mbedtls_ccm_context ctx;
    size_t iv_len, tag_len;
    int ret;

    mbedtls_ccm_init( &ctx );

    memset( iv, 0x00, sizeof( iv ) );
    memset( result, '+', sizeof( result ) );

    if( sec_level % 4 == 0)
        tag_len = 0;
    else
        tag_len = 1 << ( sec_level % 4 + 1);

    TEST_ASSERT( source_address->len == 8 );
    TEST_ASSERT( frame_counter->len == 4 );
    memcpy( iv, source_address->x, source_address->len );
    memcpy( iv + source_address->len, frame_counter->x, frame_counter->len );
    iv[source_address->len + frame_counter->len] = sec_level;
    iv_len = sizeof( iv );

    TEST_ASSERT( mbedtls_ccm_setkey( &ctx, cipher_id, key->x, key->len * 8 ) == 0 );

    ret = mbedtls_ccm_star_auth_decrypt( &ctx, msg->len - tag_len, iv, iv_len,
                                         add->x, add->len, msg->x, result,
                                         msg->x + msg->len - tag_len, tag_len );

    TEST_ASSERT( ret == output_ret );

    TEST_ASSERT( memcmp( result, expected_result->x,
                                 expected_result->len ) == 0 );

    /* Check we didn't write past the end (where the original tag is) */
    TEST_ASSERT( ( msg->len + 2 ) <= sizeof( result ) );
    TEST_EQUAL( result[msg->len], '+' );
    TEST_EQUAL( result[msg->len + 1], '+' );

exit:
    mbedtls_ccm_free( &ctx );
}

void test_mbedtls_ccm_star_auth_decrypt_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_mbedtls_ccm_star_auth_decrypt( *( (int *) params[0] ), &data1, &data3, &data5, &data7, *( (int *) params[9] ), &data10, &data12, *( (int *) params[14] ) );
}
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_ccm_invalid_param( )
{
    struct mbedtls_ccm_context ctx;
    unsigned char valid_buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    mbedtls_cipher_id_t valid_cipher = MBEDTLS_CIPHER_ID_AES;
    int valid_len = sizeof(valid_buffer);
    int valid_bitlen = valid_len * 8;

    mbedtls_ccm_init( &ctx );

    /* mbedtls_ccm_init() */
    TEST_INVALID_PARAM( mbedtls_ccm_init( NULL ) );

    /* mbedtls_ccm_setkey() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_setkey( NULL, valid_cipher, valid_buffer, valid_bitlen ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_setkey( &ctx, valid_cipher, NULL, valid_bitlen ) );

    /* mbedtls_ccm_encrypt_and_tag() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_encrypt_and_tag( NULL, valid_len,
                                     valid_buffer, valid_len,
                                     valid_buffer, valid_len,
                                     valid_buffer, valid_buffer,
                                     valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_encrypt_and_tag( &ctx, valid_len,
                                     NULL, valid_len,
                                     valid_buffer, valid_len,
                                     valid_buffer, valid_buffer,
                                     valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_encrypt_and_tag( &ctx, valid_len,
                                     valid_buffer, valid_len,
                                     NULL, valid_len,
                                     valid_buffer, valid_buffer,
                                     valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_encrypt_and_tag( &ctx, valid_len,
                                     valid_buffer, valid_len,
                                     valid_buffer, valid_len,
                                     NULL, valid_buffer,
                                     valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_encrypt_and_tag( &ctx, valid_len,
                                     valid_buffer, valid_len,
                                     valid_buffer, valid_len,
                                     valid_buffer, NULL,
                                     valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_encrypt_and_tag( &ctx, valid_len,
                                     valid_buffer, valid_len,
                                     valid_buffer, valid_len,
                                     valid_buffer, valid_buffer,
                                     NULL, valid_len ) );

    /* mbedtls_ccm_star_encrypt_and_tag() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_encrypt_and_tag( NULL, valid_len,
                                          valid_buffer, valid_len,
                                          valid_buffer, valid_len,
                                          valid_buffer, valid_buffer,
                                          valid_buffer, valid_len) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_encrypt_and_tag( &ctx, valid_len,
                                          NULL, valid_len,
                                          valid_buffer, valid_len,
                                          valid_buffer, valid_buffer,
                                          valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_encrypt_and_tag( &ctx, valid_len,
                                          valid_buffer, valid_len,
                                          NULL, valid_len,
                                          valid_buffer, valid_buffer,
                                          valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_encrypt_and_tag( &ctx, valid_len,
                                          valid_buffer, valid_len,
                                          valid_buffer, valid_len,
                                          NULL, valid_buffer,
                                          valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_encrypt_and_tag( &ctx, valid_len,
                                          valid_buffer, valid_len,
                                          valid_buffer, valid_len,
                                          valid_buffer, NULL,
                                          valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_encrypt_and_tag( &ctx, valid_len,
                                          valid_buffer, valid_len,
                                          valid_buffer, valid_len,
                                          valid_buffer, valid_buffer,
                                          NULL, valid_len ) );

    /* mbedtls_ccm_auth_decrypt() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_auth_decrypt( NULL, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_buffer,
                                  valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_auth_decrypt( &ctx, valid_len,
                                  NULL, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_buffer,
                                  valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_auth_decrypt( &ctx, valid_len,
                                  valid_buffer, valid_len,
                                  NULL, valid_len,
                                  valid_buffer, valid_buffer,
                                  valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_auth_decrypt( &ctx, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  NULL, valid_buffer,
                                  valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_auth_decrypt( &ctx, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, NULL,
                                  valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_auth_decrypt( &ctx, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_buffer,
                                  NULL, valid_len ) );

    /* mbedtls_ccm_star_auth_decrypt() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_auth_decrypt( NULL, valid_len,
                                       valid_buffer, valid_len,
                                       valid_buffer, valid_len,
                                       valid_buffer, valid_buffer,
                                       valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_auth_decrypt( &ctx, valid_len,
                                       NULL, valid_len,
                                       valid_buffer, valid_len,
                                       valid_buffer, valid_buffer,
                                       valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_auth_decrypt( &ctx, valid_len,
                                       valid_buffer, valid_len,
                                       NULL, valid_len,
                                       valid_buffer, valid_buffer,
                                       valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_auth_decrypt( &ctx, valid_len,
                                       valid_buffer, valid_len,
                                       valid_buffer, valid_len,
                                       NULL, valid_buffer,
                                       valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_auth_decrypt( &ctx, valid_len,
                                       valid_buffer, valid_len,
                                       valid_buffer, valid_len,
                                       valid_buffer, NULL,
                                       valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_CCM_BAD_INPUT,
        mbedtls_ccm_star_auth_decrypt( &ctx, valid_len,
                                       valid_buffer, valid_len,
                                       valid_buffer, valid_len,
                                       valid_buffer, valid_buffer,
                                       NULL, valid_len ) );

exit:
    mbedtls_ccm_free( &ctx );
    return;
}

void test_ccm_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_ccm_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_ccm_valid_param( )
{
    TEST_VALID_PARAM( mbedtls_ccm_free( NULL ) );
exit:
    return;
}

void test_ccm_valid_param_wrapper( void ** params )
{
    (void)params;

    test_ccm_valid_param(  );
}
#endif /* MBEDTLS_CCM_C */

/*----------------------------------------------------------------------------*/
/* Test dispatch code */


/**
 * \brief       Evaluates an expression/macro into its literal integer value.
 *              For optimizing space for embedded targets each expression/macro
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and evaluation code is generated by script:
 *              generate_test_code.py and then mbedtls_test_suite.sh
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

#if defined(MBEDTLS_CCM_C)

        case 0:
            {
                *out_value = MBEDTLS_CIPHER_ID_AES;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_CIPHER_ID_CAMELLIA;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_CCM_BAD_INPUT;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_CIPHER_ID_BLOWFISH;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ERR_CCM_AUTH_FAILED;
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
 *              generate_test_code.py and then mbedtls_test_suite.sh
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

#if defined(MBEDTLS_CCM_C)

        case 0:
            {
#if defined(MBEDTLS_AES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_CAMELLIA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_BLOWFISH_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if !defined(MBEDTLS_CCM_ALT)
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
 *              generate_test_code.py and then mbedtls_test_suite.sh
 *
 */
TestWrapper_t test_funcs[] =
{
/* Function Id: 0 */

#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_AES_C)
    test_mbedtls_ccm_self_test_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_setkey_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_AES_C)
    test_ccm_lengths_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_AES_C)
    test_ccm_star_lengths_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_encrypt_and_tag_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_auth_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_star_encrypt_and_tag_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_CCM_C)
    test_mbedtls_ccm_star_auth_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_ccm_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_CCM_C)
    test_ccm_valid_param_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_ccm.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
