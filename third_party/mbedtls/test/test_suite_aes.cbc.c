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
#include "libc/log/log.h"
#include "third_party/mbedtls/test/test.inc"
/*
 * *** THIS FILE WAS MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * Test file      : ./test_suite_aes.cbc.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_aes.function
 *      Test suite data     : suites/test_suite_aes.cbc.data
 *
 */

/*----------------------------------------------------------------------------*/
/* Test Suite Code */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_AES_C)
#include "third_party/mbedtls/aes.h"
void test_aes_encrypt_ecb( data_t * key_str, data_t * src_str,
                      data_t * dst, int setkey_result )
{
    unsigned char output[100];
    mbedtls_aes_context ctx;

    memset(output, 0x00, 100);

    mbedtls_aes_init( &ctx );

    TEST_ASSERT( mbedtls_aes_setkey_enc( &ctx, key_str->x, key_str->len * 8 ) == setkey_result );
    if( setkey_result == 0 )
    {
        TEST_ASSERT( mbedtls_aes_crypt_ecb( &ctx, MBEDTLS_AES_ENCRYPT, src_str->x, output ) == 0 );

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, 16, dst->len ) == 0 );
    }

exit:
    mbedtls_aes_free( &ctx );
}

void test_aes_encrypt_ecb_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_aes_encrypt_ecb( &data0, &data2, &data4, *( (int *) params[6] ) );
}
void test_aes_decrypt_ecb( data_t * key_str, data_t * src_str,
                      data_t * dst, int setkey_result )
{
    unsigned char output[100];
    mbedtls_aes_context ctx;

    memset(output, 0x00, 100);

    mbedtls_aes_init( &ctx );

    TEST_ASSERT( mbedtls_aes_setkey_dec( &ctx, key_str->x, key_str->len * 8 ) == setkey_result );
    if( setkey_result == 0 )
    {
        TEST_ASSERT( mbedtls_aes_crypt_ecb( &ctx, MBEDTLS_AES_DECRYPT, src_str->x, output ) == 0 );

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, 16, dst->len ) == 0 );
    }

exit:
    mbedtls_aes_free( &ctx );
}

void test_aes_decrypt_ecb_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_aes_decrypt_ecb( &data0, &data2, &data4, *( (int *) params[6] ) );
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
void test_aes_encrypt_cbc( data_t * key_str, data_t * iv_str,
                      data_t * src_str, data_t * dst,
                      int cbc_result )
{
    unsigned char output[100];
    mbedtls_aes_context ctx;

    memset(output, 0x00, 100);

    mbedtls_aes_init( &ctx );

    mbedtls_aes_setkey_enc( &ctx, key_str->x, key_str->len * 8 );
    TEST_ASSERT( mbedtls_aes_crypt_cbc( &ctx, MBEDTLS_AES_ENCRYPT, src_str->len, iv_str->x, src_str->x, output ) == cbc_result );
    if( cbc_result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x,
                                          src_str->len, dst->len ) == 0 );
    }

exit:
    mbedtls_aes_free( &ctx );
}

void test_aes_encrypt_cbc_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_aes_encrypt_cbc( &data0, &data2, &data4, &data6, *( (int *) params[8] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CBC)
void test_aes_decrypt_cbc( data_t * key_str, data_t * iv_str,
                      data_t * src_str, data_t * dst,
                      int cbc_result )
{
    unsigned char output[100];
    mbedtls_aes_context ctx;

    memset(output, 0x00, 100);
    mbedtls_aes_init( &ctx );

    mbedtls_aes_setkey_dec( &ctx, key_str->x, key_str->len * 8 );
    TEST_ASSERT( mbedtls_aes_crypt_cbc( &ctx, MBEDTLS_AES_DECRYPT, src_str->len, iv_str->x, src_str->x, output ) == cbc_result );
    if( cbc_result == 0)
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x,
                                          src_str->len, dst->len ) == 0 );
    }

exit:
    mbedtls_aes_free( &ctx );
}

void test_aes_decrypt_cbc_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_aes_decrypt_cbc( &data0, &data2, &data4, &data6, *( (int *) params[8] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_XTS)
void test_aes_encrypt_xts( char *hex_key_string, char *hex_data_unit_string,
                      char *hex_src_string, char *hex_dst_string )
{
    enum { AES_BLOCK_SIZE = 16 };
    unsigned char *data_unit = NULL;
    unsigned char *key = NULL;
    unsigned char *src = NULL;
    unsigned char *dst = NULL;
    unsigned char *output = NULL;
    mbedtls_aes_xts_context ctx;
    size_t key_len, src_len, dst_len, data_unit_len;

    mbedtls_aes_xts_init( &ctx );

    data_unit = mbedtls_test_unhexify_alloc( hex_data_unit_string,
                                             &data_unit_len );
    TEST_ASSERT( data_unit_len == AES_BLOCK_SIZE );

    key = mbedtls_test_unhexify_alloc( hex_key_string, &key_len );
    TEST_ASSERT( key_len % 2 == 0 );

    src = mbedtls_test_unhexify_alloc( hex_src_string, &src_len );
    dst = mbedtls_test_unhexify_alloc( hex_dst_string, &dst_len );
    TEST_ASSERT( src_len == dst_len );

    output = mbedtls_test_zero_alloc( dst_len );

    TEST_ASSERT( mbedtls_aes_xts_setkey_enc( &ctx, key, key_len * 8 ) == 0 );
    TEST_ASSERT( mbedtls_aes_crypt_xts( &ctx, MBEDTLS_AES_ENCRYPT, src_len,
                                        data_unit, src, output ) == 0 );

    TEST_ASSERT( memcmp( output, dst, dst_len ) == 0 );

exit:
    mbedtls_aes_xts_free( &ctx );
    mbedtls_free( data_unit );
    mbedtls_free( key );
    mbedtls_free( src );
    mbedtls_free( dst );
    mbedtls_free( output );
}

void test_aes_encrypt_xts_wrapper( void ** params )
{

    test_aes_encrypt_xts( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3] );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */
#if defined(MBEDTLS_CIPHER_MODE_XTS)
void test_aes_decrypt_xts( char *hex_key_string, char *hex_data_unit_string,
                      char *hex_dst_string, char *hex_src_string )
{
    enum { AES_BLOCK_SIZE = 16 };
    unsigned char *data_unit = NULL;
    unsigned char *key = NULL;
    unsigned char *src = NULL;
    unsigned char *dst = NULL;
    unsigned char *output = NULL;
    mbedtls_aes_xts_context ctx;
    size_t key_len, src_len, dst_len, data_unit_len;

    mbedtls_aes_xts_init( &ctx );

    data_unit = mbedtls_test_unhexify_alloc( hex_data_unit_string,
                                             &data_unit_len );
    TEST_ASSERT( data_unit_len == AES_BLOCK_SIZE );

    key = mbedtls_test_unhexify_alloc( hex_key_string, &key_len );
    TEST_ASSERT( key_len % 2 == 0 );

    src = mbedtls_test_unhexify_alloc( hex_src_string, &src_len );
    dst = mbedtls_test_unhexify_alloc( hex_dst_string, &dst_len );
    TEST_ASSERT( src_len == dst_len );

    output = mbedtls_test_zero_alloc( dst_len );

    TEST_ASSERT( mbedtls_aes_xts_setkey_dec( &ctx, key, key_len * 8 ) == 0 );
    TEST_ASSERT( mbedtls_aes_crypt_xts( &ctx, MBEDTLS_AES_DECRYPT, src_len,
                                        data_unit, src, output ) == 0 );

    TEST_ASSERT( memcmp( output, dst, dst_len ) == 0 );

exit:
    mbedtls_aes_xts_free( &ctx );
    mbedtls_free( data_unit );
    mbedtls_free( key );
    mbedtls_free( src );
    mbedtls_free( dst );
    mbedtls_free( output );
}

void test_aes_decrypt_xts_wrapper( void ** params )
{

    test_aes_decrypt_xts( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3] );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */
#if defined(MBEDTLS_CIPHER_MODE_XTS)
void test_aes_crypt_xts_size( int size, int retval )
{
    mbedtls_aes_xts_context ctx;
    const unsigned char src[16] = { 0 };
    unsigned char output[16];
    unsigned char data_unit[16];
    size_t length = size;

    mbedtls_aes_xts_init( &ctx );
    memset( data_unit, 0x00, sizeof( data_unit ) );


    /* Valid pointers are passed for builds with MBEDTLS_CHECK_PARAMS, as
     * otherwise we wouldn't get to the size check we're interested in. */
    TEST_ASSERT( mbedtls_aes_crypt_xts( &ctx, MBEDTLS_AES_ENCRYPT, length, data_unit, src, output ) == retval );
exit:
    ;
}

void test_aes_crypt_xts_size_wrapper( void ** params )
{

    test_aes_crypt_xts_size( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */
#if defined(MBEDTLS_CIPHER_MODE_XTS)
void test_aes_crypt_xts_keysize( int size, int retval )
{
    mbedtls_aes_xts_context ctx;
    const unsigned char key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    size_t key_len = size;

    mbedtls_aes_xts_init( &ctx );

    TEST_ASSERT( mbedtls_aes_xts_setkey_enc( &ctx, key, key_len * 8 ) == retval );
    TEST_ASSERT( mbedtls_aes_xts_setkey_dec( &ctx, key, key_len * 8 ) == retval );
exit:
    mbedtls_aes_xts_free( &ctx );
}

void test_aes_crypt_xts_keysize_wrapper( void ** params )
{

    test_aes_crypt_xts_keysize( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */
#if defined(MBEDTLS_CIPHER_MODE_CFB)
void test_aes_encrypt_cfb128( data_t * key_str, data_t * iv_str,
                         data_t * src_str, data_t * dst )
{
    unsigned char output[100];
    mbedtls_aes_context ctx;
    size_t iv_offset = 0;

    memset(output, 0x00, 100);
    mbedtls_aes_init( &ctx );


    mbedtls_aes_setkey_enc( &ctx, key_str->x, key_str->len * 8 );
    TEST_ASSERT( mbedtls_aes_crypt_cfb128( &ctx, MBEDTLS_AES_ENCRYPT, 16, &iv_offset, iv_str->x, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, 16, dst->len ) == 0 );

exit:
    mbedtls_aes_free( &ctx );
}

void test_aes_encrypt_cfb128_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_aes_encrypt_cfb128( &data0, &data2, &data4, &data6 );
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */
#if defined(MBEDTLS_CIPHER_MODE_CFB)
void test_aes_decrypt_cfb128( data_t * key_str, data_t * iv_str,
                         data_t * src_str, data_t * dst )
{
    unsigned char output[100];
    mbedtls_aes_context ctx;
    size_t iv_offset = 0;

    memset(output, 0x00, 100);
    mbedtls_aes_init( &ctx );


    mbedtls_aes_setkey_enc( &ctx, key_str->x, key_str->len * 8 );
    TEST_ASSERT( mbedtls_aes_crypt_cfb128( &ctx, MBEDTLS_AES_DECRYPT, 16, &iv_offset, iv_str->x, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, 16, dst->len ) == 0 );

exit:
    mbedtls_aes_free( &ctx );
}

void test_aes_decrypt_cfb128_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_aes_decrypt_cfb128( &data0, &data2, &data4, &data6 );
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */
#if defined(MBEDTLS_CIPHER_MODE_CFB)
void test_aes_encrypt_cfb8( data_t * key_str, data_t * iv_str,
                       data_t * src_str, data_t * dst )
{
    unsigned char output[100];
    mbedtls_aes_context ctx;

    memset(output, 0x00, 100);
    mbedtls_aes_init( &ctx );


    mbedtls_aes_setkey_enc( &ctx, key_str->x, key_str->len * 8 );
    TEST_ASSERT( mbedtls_aes_crypt_cfb8( &ctx, MBEDTLS_AES_ENCRYPT, src_str->len, iv_str->x, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x,
                                      src_str->len, dst->len ) == 0 );

exit:
    mbedtls_aes_free( &ctx );
}

void test_aes_encrypt_cfb8_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_aes_encrypt_cfb8( &data0, &data2, &data4, &data6 );
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
void test_aes_decrypt_cfb8( data_t * key_str, data_t * iv_str,
                       data_t * src_str, data_t * dst )
{
    unsigned char output[100];
    mbedtls_aes_context ctx;

    memset(output, 0x00, 100);
    mbedtls_aes_init( &ctx );


    mbedtls_aes_setkey_enc( &ctx, key_str->x, key_str->len * 8 );
    TEST_ASSERT( mbedtls_aes_crypt_cfb8( &ctx, MBEDTLS_AES_DECRYPT, src_str->len, iv_str->x, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x,
                                      src_str->len, dst->len ) == 0 );

exit:
    mbedtls_aes_free( &ctx );
}

void test_aes_decrypt_cfb8_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_aes_decrypt_cfb8( &data0, &data2, &data4, &data6 );
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
void test_aes_encrypt_ofb( int fragment_size, data_t *key_str,
                      data_t *iv_str, data_t *src_str,
                      data_t *expected_output )
{
    unsigned char output[32];
    mbedtls_aes_context ctx;
    size_t iv_offset = 0;
    int in_buffer_len;
    unsigned char* src_str_next;

    memset( output, 0x00, sizeof( output ) );
    mbedtls_aes_init( &ctx );

    TEST_ASSERT( (size_t)fragment_size < sizeof( output ) );

    TEST_ASSERT( mbedtls_aes_setkey_enc( &ctx, key_str->x,
                                         key_str->len * 8 ) == 0 );
    in_buffer_len = src_str->len;
    src_str_next = src_str->x;

    while( in_buffer_len > 0 )
    {
        TEST_ASSERT( mbedtls_aes_crypt_ofb( &ctx, fragment_size, &iv_offset,
                                            iv_str->x, src_str_next, output ) == 0 );

        TEST_ASSERT( memcmp( output, expected_output->x, fragment_size ) == 0 );

        in_buffer_len -= fragment_size;
        expected_output->x += fragment_size;
        src_str_next += fragment_size;

        if( in_buffer_len < fragment_size )
            fragment_size = in_buffer_len;
    }

exit:
    mbedtls_aes_free( &ctx );
}

void test_aes_encrypt_ofb_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_aes_encrypt_ofb( *( (int *) params[0] ), &data1, &data3, &data5, &data7 );
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_aes_check_params( )
{
    mbedtls_aes_context aes_ctx;
#if defined(MBEDTLS_CIPHER_MODE_XTS)
    mbedtls_aes_xts_context xts_ctx;
#endif
    const unsigned char key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    const unsigned char in[16] = { 0 };
    unsigned char out[16];
    size_t size;
    const int valid_mode = MBEDTLS_AES_ENCRYPT;
    const int invalid_mode = 42;

    (void)size;

    TEST_INVALID_PARAM( mbedtls_aes_init( NULL ) );
#if defined(MBEDTLS_CIPHER_MODE_XTS)
    TEST_INVALID_PARAM( mbedtls_aes_xts_init( NULL ) );
#endif

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_setkey_enc( NULL, key, 128 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_setkey_enc( &aes_ctx, NULL, 128 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_setkey_dec( NULL, key, 128 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_setkey_dec( &aes_ctx, NULL, 128 ) );

#if defined(MBEDTLS_CIPHER_MODE_XTS)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_xts_setkey_enc( NULL, key, 128 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_xts_setkey_enc( &xts_ctx, NULL, 128 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_xts_setkey_dec( NULL, key, 128 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_xts_setkey_dec( &xts_ctx, NULL, 128 ) );
#endif


    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ecb( NULL,
                                                   valid_mode, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ecb( &aes_ctx,
                                                   invalid_mode, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ecb( &aes_ctx,
                                                   valid_mode, NULL, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ecb( &aes_ctx,
                                                   valid_mode, in, NULL ) );

#if defined(MBEDTLS_CIPHER_MODE_CBC)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cbc( NULL,
                                                   valid_mode, 16,
                                                   out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cbc( &aes_ctx,
                                                   invalid_mode, 16,
                                                   out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cbc( &aes_ctx,
                                                   valid_mode, 16,
                                                   NULL, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cbc( &aes_ctx,
                                                   valid_mode, 16,
                                                   out, NULL, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cbc( &aes_ctx,
                                                   valid_mode, 16,
                                                   out, in, NULL ) );
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_XTS)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_xts( NULL,
                                                   valid_mode, 16,
                                                   in, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_xts( &xts_ctx,
                                                   invalid_mode, 16,
                                                   in, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_xts( &xts_ctx,
                                                   valid_mode, 16,
                                                   NULL, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_xts( &xts_ctx,
                                                   valid_mode, 16,
                                                   in, NULL, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_xts( &xts_ctx,
                                                   valid_mode, 16,
                                                   in, in, NULL ) );
#endif /* MBEDTLS_CIPHER_MODE_XTS */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb128( NULL,
                                                      valid_mode, 16,
                                                      &size, out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb128( &aes_ctx,
                                                      invalid_mode, 16,
                                                      &size, out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb128( &aes_ctx,
                                                      valid_mode, 16,
                                                      NULL, out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb128( &aes_ctx,
                                                      valid_mode, 16,
                                                      &size, NULL, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb128( &aes_ctx,
                                                      valid_mode, 16,
                                                      &size, out, NULL, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb128( &aes_ctx,
                                                      valid_mode, 16,
                                                      &size, out, in, NULL ) );


    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb8( NULL,
                                                    valid_mode, 16,
                                                    out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb8( &aes_ctx,
                                                    invalid_mode, 16,
                                                    out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb8( &aes_ctx,
                                                    valid_mode, 16,
                                                    NULL, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb8( &aes_ctx,
                                                    valid_mode, 16,
                                                    out, NULL, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_cfb8( &aes_ctx,
                                                    valid_mode, 16,
                                                    out, in, NULL ) );
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ofb( NULL, 16,
                                                   &size, out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ofb( &aes_ctx, 16,
                                                   NULL, out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ofb( &aes_ctx, 16,
                                                   &size, NULL, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ofb( &aes_ctx, 16,
                                                   &size, out, NULL, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ofb( &aes_ctx, 16,
                                                   &size, out, in, NULL ) );
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ctr( NULL, 16, &size, out,
                                                   out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ctr( &aes_ctx, 16, NULL, out,
                                                   out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ctr( &aes_ctx, 16, &size, NULL,
                                                   out, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ctr( &aes_ctx, 16, &size, out,
                                                   NULL, in, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ctr( &aes_ctx, 16, &size, out,
                                                   out, NULL, out ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_AES_BAD_INPUT_DATA,
                            mbedtls_aes_crypt_ctr( &aes_ctx, 16, &size, out,
                                                   out, in, NULL ) );
#endif /* MBEDTLS_CIPHER_MODE_CTR */
exit:
    ;
}

void test_aes_check_params_wrapper( void ** params )
{
    (void)params;

    test_aes_check_params(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_aes_misc_params( )
{
#if defined(MBEDTLS_CIPHER_MODE_CBC) || \
    defined(MBEDTLS_CIPHER_MODE_XTS) || \
    defined(MBEDTLS_CIPHER_MODE_CFB) || \
    defined(MBEDTLS_CIPHER_MODE_OFB)
    mbedtls_aes_context aes_ctx;
    const unsigned char in[16] = { 0 };
    unsigned char out[16];
#endif
#if defined(MBEDTLS_CIPHER_MODE_XTS)
    mbedtls_aes_xts_context xts_ctx;
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB) || \
    defined(MBEDTLS_CIPHER_MODE_OFB)
    size_t size;
#endif

    /* These calls accept NULL */
    TEST_VALID_PARAM( mbedtls_aes_free( NULL ) );
#if defined(MBEDTLS_CIPHER_MODE_XTS)
    TEST_VALID_PARAM( mbedtls_aes_xts_free( NULL ) );
#endif

#if defined(MBEDTLS_CIPHER_MODE_CBC)
    TEST_ASSERT( mbedtls_aes_crypt_cbc( &aes_ctx, MBEDTLS_AES_ENCRYPT,
                                        15,
                                        out, in, out )
                 == MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    TEST_ASSERT( mbedtls_aes_crypt_cbc( &aes_ctx, MBEDTLS_AES_ENCRYPT,
                                        17,
                                        out, in, out )
                 == MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
#endif

#if defined(MBEDTLS_CIPHER_MODE_XTS)
    TEST_ASSERT( mbedtls_aes_crypt_xts( &xts_ctx, MBEDTLS_AES_ENCRYPT,
                                        15,
                                        in, in, out )
                 == MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
    TEST_ASSERT( mbedtls_aes_crypt_xts( &xts_ctx, MBEDTLS_AES_ENCRYPT,
                                        (1 << 24) + 1,
                                        in, in, out )
                 == MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );
#endif

#if defined(MBEDTLS_CIPHER_MODE_CFB)
    size = 16;
    TEST_ASSERT( mbedtls_aes_crypt_cfb128( &aes_ctx, MBEDTLS_AES_ENCRYPT, 16,
                                           &size, out, in, out )
                 == MBEDTLS_ERR_AES_BAD_INPUT_DATA );
#endif

#if defined(MBEDTLS_CIPHER_MODE_OFB)
    size = 16;
    TEST_ASSERT( mbedtls_aes_crypt_ofb( &aes_ctx, 16, &size, out, in, out )
                 == MBEDTLS_ERR_AES_BAD_INPUT_DATA );
#endif
exit:
    ;
}

void test_aes_misc_params_wrapper( void ** params )
{
    (void)params;

    test_aes_misc_params(  );
}
#if defined(MBEDTLS_SELF_TEST)
void test_aes_selftest(  )
{
    TEST_ASSERT( mbedtls_aes_self_test( 1 ) == 0 );
exit:
    ;
}

void test_aes_selftest_wrapper( void ** params )
{
    (void)params;

    test_aes_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_AES_C */

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

#if defined(MBEDTLS_AES_C)

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

#if defined(MBEDTLS_AES_C)

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

#if defined(MBEDTLS_AES_C)
    test_aes_encrypt_ecb_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_AES_C)
    test_aes_decrypt_ecb_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
    test_aes_encrypt_cbc_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
    test_aes_decrypt_cbc_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_XTS)
    test_aes_encrypt_xts_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_XTS)
    test_aes_decrypt_xts_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_XTS)
    test_aes_crypt_xts_size_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_XTS)
    test_aes_crypt_xts_keysize_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_CFB)
    test_aes_encrypt_cfb128_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_CFB)
    test_aes_decrypt_cfb128_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_CFB)
    test_aes_encrypt_cfb8_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_CFB)
    test_aes_decrypt_cfb8_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_OFB)
    test_aes_encrypt_ofb_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_aes_check_params_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_AES_C)
    test_aes_misc_params_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_SELF_TEST)
    test_aes_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_aes.cbc.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
