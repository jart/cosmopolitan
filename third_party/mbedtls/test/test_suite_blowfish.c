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
 * Test file      : ./test_suite_blowfish.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_blowfish.function
 *      Test suite data     : suites/test_suite_blowfish.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_BLOWFISH_C)
void test_blowfish_valid_param( )
{
    TEST_VALID_PARAM( mbedtls_blowfish_free( NULL ) );
exit:
    ;
}

void test_blowfish_valid_param_wrapper( void ** params )
{
    (void)params;

    test_blowfish_valid_param(  );
}
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_blowfish_invalid_param( )
{
    mbedtls_blowfish_context ctx;
    unsigned char buf[16] = { 0 };
    size_t const valid_keylength = sizeof( buf ) * 8;
    size_t valid_mode = MBEDTLS_BLOWFISH_ENCRYPT;
    size_t invalid_mode = 42;
    size_t off;
    ((void) off);

    TEST_INVALID_PARAM( mbedtls_blowfish_init( NULL ) );
    TEST_VALID_PARAM( mbedtls_blowfish_free( NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_setkey( NULL,
                                                     buf,
                                                     valid_keylength ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_setkey( &ctx,
                                                     NULL,
                                                     valid_keylength ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_ecb( NULL,
                                                     valid_mode,
                                                     buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_ecb( &ctx,
                                                        invalid_mode,
                                                        buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_ecb( &ctx,
                                                        valid_mode,
                                                        NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_ecb( &ctx,
                                                        valid_mode,
                                                        buf, NULL ) );

#if defined(MBEDTLS_CIPHER_MODE_CBC)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cbc( NULL,
                                                        valid_mode,
                                                        sizeof( buf ),
                                                        buf, buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cbc( &ctx,
                                                        invalid_mode,
                                                        sizeof( buf ),
                                                        buf, buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cbc( &ctx,
                                                        valid_mode,
                                                        sizeof( buf ),
                                                        NULL, buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cbc( &ctx,
                                                        valid_mode,
                                                        sizeof( buf ),
                                                        buf, NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cbc( &ctx,
                                                        valid_mode,
                                                        sizeof( buf ),
                                                        buf, buf, NULL ) );
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cfb64( NULL,
                                                          valid_mode,
                                                          sizeof( buf ),
                                                          &off, buf,
                                                          buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cfb64( &ctx,
                                                          invalid_mode,
                                                          sizeof( buf ),
                                                          &off, buf,
                                                          buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cfb64( &ctx,
                                                          valid_mode,
                                                          sizeof( buf ),
                                                          NULL, buf,
                                                          buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cfb64( &ctx,
                                                          valid_mode,
                                                          sizeof( buf ),
                                                          &off, NULL,
                                                          buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cfb64( &ctx,
                                                          valid_mode,
                                                          sizeof( buf ),
                                                          &off, buf,
                                                          NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_cfb64( &ctx,
                                                          valid_mode,
                                                          sizeof( buf ),
                                                          &off, buf,
                                                          buf, NULL ) );
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_ctr( NULL,
                                                        sizeof( buf ),
                                                        &off,
                                                        buf, buf,
                                                        buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_ctr( &ctx,
                                                        sizeof( buf ),
                                                        NULL,
                                                        buf, buf,
                                                        buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_ctr( &ctx,
                                                        sizeof( buf ),
                                                        &off,
                                                        NULL, buf,
                                                        buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_ctr( &ctx,
                                                        sizeof( buf ),
                                                        &off,
                                                        buf, NULL,
                                                        buf, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_ctr( &ctx,
                                                        sizeof( buf ),
                                                        &off,
                                                        buf, buf,
                                                        NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA,
                            mbedtls_blowfish_crypt_ctr( &ctx,
                                                        sizeof( buf ),
                                                        &off,
                                                        buf, buf,
                                                        buf, NULL ) );
#endif /* MBEDTLS_CIPHER_MODE_CTR */

exit:
    return;
}

void test_blowfish_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_blowfish_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_blowfish_encrypt_ecb( data_t * key_str, data_t * src_str,
                           data_t * dst, int setkey_result )
{
    unsigned char output[100];
    mbedtls_blowfish_context ctx;

    memset(output, 0x00, 100);
    mbedtls_blowfish_init( &ctx );


    TEST_ASSERT( mbedtls_blowfish_setkey( &ctx, key_str->x, key_str->len * 8 ) == setkey_result );
    if( setkey_result == 0 )
    {
        TEST_ASSERT( mbedtls_blowfish_crypt_ecb( &ctx, MBEDTLS_BLOWFISH_ENCRYPT, src_str->x, output ) == 0 );

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, 8, dst->len ) == 0 );
    }

exit:
    mbedtls_blowfish_free( &ctx );
}

void test_blowfish_encrypt_ecb_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_blowfish_encrypt_ecb( &data0, &data2, &data4, *( (int *) params[6] ) );
}
void test_blowfish_decrypt_ecb( data_t * key_str, data_t * src_str,
                           data_t * dst, int setkey_result )
{
    unsigned char output[100];
    mbedtls_blowfish_context ctx;

    memset(output, 0x00, 100);
    mbedtls_blowfish_init( &ctx );


    TEST_ASSERT( mbedtls_blowfish_setkey( &ctx, key_str->x, key_str->len * 8 ) == setkey_result );
    if( setkey_result == 0 )
    {
        TEST_ASSERT( mbedtls_blowfish_crypt_ecb( &ctx, MBEDTLS_BLOWFISH_DECRYPT, src_str->x, output ) == 0 );

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, 8, dst->len ) == 0 );
    }

exit:
    mbedtls_blowfish_free( &ctx );
}

void test_blowfish_decrypt_ecb_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_blowfish_decrypt_ecb( &data0, &data2, &data4, *( (int *) params[6] ) );
}
#if defined(MBEDTLS_CIPHER_MODE_CBC)
void test_blowfish_encrypt_cbc( data_t * key_str, data_t * iv_str,
                           data_t * src_str, data_t * dst,
                           int cbc_result )
{
    unsigned char output[100];
    mbedtls_blowfish_context ctx;

    memset(output, 0x00, 100);
    mbedtls_blowfish_init( &ctx );


    mbedtls_blowfish_setkey( &ctx, key_str->x, key_str->len * 8 );

    TEST_ASSERT( mbedtls_blowfish_crypt_cbc( &ctx, MBEDTLS_BLOWFISH_ENCRYPT, src_str->len , iv_str->x, src_str->x, output ) == cbc_result );
    if( cbc_result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x,
                                          src_str->len, dst->len ) == 0 );
    }

exit:
    mbedtls_blowfish_free( &ctx );
}

void test_blowfish_encrypt_cbc_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_blowfish_encrypt_cbc( &data0, &data2, &data4, &data6, *( (int *) params[8] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#if defined(MBEDTLS_CIPHER_MODE_CBC)
void test_blowfish_decrypt_cbc( data_t * key_str, data_t * iv_str,
                           data_t * src_str, data_t * dst,
                           int cbc_result )
{
    unsigned char output[100];
    mbedtls_blowfish_context ctx;

    memset(output, 0x00, 100);
    mbedtls_blowfish_init( &ctx );


    mbedtls_blowfish_setkey( &ctx, key_str->x, key_str->len * 8 );
    TEST_ASSERT( mbedtls_blowfish_crypt_cbc( &ctx, MBEDTLS_BLOWFISH_DECRYPT, src_str->len , iv_str->x, src_str->x, output ) == cbc_result );
    if( cbc_result == 0)
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, src_str->len,
                                          dst->len ) == 0 );
    }

exit:
    mbedtls_blowfish_free( &ctx );
}

void test_blowfish_decrypt_cbc_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_blowfish_decrypt_cbc( &data0, &data2, &data4, &data6, *( (int *) params[8] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#if defined(MBEDTLS_CIPHER_MODE_CFB)
void test_blowfish_encrypt_cfb64( data_t * key_str, data_t * iv_str,
                             data_t * src_str, data_t * dst )
{
    unsigned char output[100];
    mbedtls_blowfish_context ctx;
    size_t iv_offset = 0;

    memset(output, 0x00, 100);
    mbedtls_blowfish_init( &ctx );


    mbedtls_blowfish_setkey( &ctx, key_str->x, key_str->len * 8 );
    TEST_ASSERT( mbedtls_blowfish_crypt_cfb64( &ctx, MBEDTLS_BLOWFISH_ENCRYPT, src_str->len, &iv_offset, iv_str->x, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, src_str->len,
                                      dst->len ) == 0 );

exit:
    mbedtls_blowfish_free( &ctx );
}

void test_blowfish_encrypt_cfb64_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_blowfish_encrypt_cfb64( &data0, &data2, &data4, &data6 );
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */
#if defined(MBEDTLS_CIPHER_MODE_CFB)
void test_blowfish_decrypt_cfb64( data_t * key_str, data_t * iv_str,
                             data_t * src_str, data_t * dst )
{
    unsigned char output[100];
    mbedtls_blowfish_context ctx;
    size_t iv_offset = 0;

    memset(output, 0x00, 100);
    mbedtls_blowfish_init( &ctx );


    mbedtls_blowfish_setkey( &ctx, key_str->x, key_str->len * 8 );
    TEST_ASSERT( mbedtls_blowfish_crypt_cfb64( &ctx, MBEDTLS_BLOWFISH_DECRYPT, src_str->len, &iv_offset, iv_str->x, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, src_str->len,
                                      dst->len ) == 0 );

exit:
    mbedtls_blowfish_free( &ctx );
}

void test_blowfish_decrypt_cfb64_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_blowfish_decrypt_cfb64( &data0, &data2, &data4, &data6 );
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */
#if defined(MBEDTLS_CIPHER_MODE_CTR)
void test_blowfish_encrypt_ctr( data_t * key_str, data_t * iv_str,
                           data_t * src_str, data_t * dst )
{
    unsigned char stream_str[100];
    unsigned char output[100];
    mbedtls_blowfish_context ctx;
    size_t iv_offset = 0;

    memset(stream_str, 0x00, 100);
    memset(output, 0x00, 100);
    mbedtls_blowfish_init( &ctx );


    mbedtls_blowfish_setkey( &ctx, key_str->x, key_str->len * 8 );
    TEST_ASSERT( mbedtls_blowfish_crypt_ctr( &ctx, src_str->len, &iv_offset, iv_str->x, stream_str, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, src_str->len,
                                      dst->len ) == 0 );

exit:
    mbedtls_blowfish_free( &ctx );
}

void test_blowfish_encrypt_ctr_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_blowfish_encrypt_ctr( &data0, &data2, &data4, &data6 );
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */
#endif /* MBEDTLS_BLOWFISH_C */

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

#if defined(MBEDTLS_BLOWFISH_C)

        case 0:
            {
                *out_value = MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ERR_BLOWFISH_INVALID_INPUT_LENGTH;
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

#if defined(MBEDTLS_BLOWFISH_C)

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

#if defined(MBEDTLS_BLOWFISH_C)
    test_blowfish_valid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_BLOWFISH_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_blowfish_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_BLOWFISH_C)
    test_blowfish_encrypt_ecb_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_BLOWFISH_C)
    test_blowfish_decrypt_ecb_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_BLOWFISH_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
    test_blowfish_encrypt_cbc_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_BLOWFISH_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
    test_blowfish_decrypt_cbc_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_BLOWFISH_C) && defined(MBEDTLS_CIPHER_MODE_CFB)
    test_blowfish_encrypt_cfb64_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_BLOWFISH_C) && defined(MBEDTLS_CIPHER_MODE_CFB)
    test_blowfish_decrypt_cfb64_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_BLOWFISH_C) && defined(MBEDTLS_CIPHER_MODE_CTR)
    test_blowfish_encrypt_ctr_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_blowfish.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
