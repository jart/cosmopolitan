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
 * Test file      : ./test_suite_md.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_md.function
 *      Test suite data     : suites/test_suite_md.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_MD_C)
#include "third_party/mbedtls/md.h"
void test_mbedtls_md_process(  )
{
    const uint8_t *md_type_ptr;
    const mbedtls_md_info_t *info;
    mbedtls_md_context_t ctx;
    unsigned char buf[150];

    mbedtls_md_init( &ctx );

    /*
     * Very minimal testing of mbedtls_md_process, just make sure the various
     * xxx_process_wrap() function pointers are valid. (Testing that they
     * indeed do the right thing whould require messing with the internal
     * state of the underlying mbedtls_md/sha context.)
     *
     * Also tests that mbedtls_md_list() only returns valid MDs.
     */
    for( md_type_ptr = mbedtls_md_list(); *md_type_ptr != 0; md_type_ptr++ )
    {
        info = mbedtls_md_info_from_type( *md_type_ptr );
        TEST_ASSERT( info != NULL );
        TEST_ASSERT( mbedtls_md_setup( &ctx, info, 0 ) == 0 );
        TEST_ASSERT( mbedtls_md_process( &ctx, buf ) == 0 );
        mbedtls_md_free( &ctx );
    }

exit:
    mbedtls_md_free( &ctx );
}

void test_mbedtls_md_process_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_md_process(  );
}
void test_md_null_args(  )
{
    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t *info = mbedtls_md_info_from_type( *( mbedtls_md_list() ) );
    unsigned char buf[1] = { 0 };

    mbedtls_md_init( &ctx );

    TEST_ASSERT( mbedtls_md_get_size( NULL ) == 0 );
    TEST_ASSERT( mbedtls_md_get_type( NULL ) == MBEDTLS_MD_NONE );
    TEST_ASSERT( mbedtls_md_get_name( NULL ) == NULL );

    TEST_ASSERT( mbedtls_md_info_from_string( NULL ) == NULL );

    TEST_ASSERT( mbedtls_md_setup( &ctx, NULL, 0 ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );
    TEST_ASSERT( mbedtls_md_setup( NULL, info, 0 ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_md_starts( NULL ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );
    TEST_ASSERT( mbedtls_md_starts( &ctx ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_md_update( NULL, buf, 1 ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );
    TEST_ASSERT( mbedtls_md_update( &ctx, buf, 1 ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_md_finish( NULL, buf ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );
    TEST_ASSERT( mbedtls_md_finish( &ctx, buf ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_md( NULL, buf, 1, buf ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

#if defined(MBEDTLS_FS_IO)
    TEST_ASSERT( mbedtls_md_file( NULL, "", buf ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );
#endif

    TEST_ASSERT( mbedtls_md_hmac_starts( NULL, buf, 1 )
                 == MBEDTLS_ERR_MD_BAD_INPUT_DATA );
    TEST_ASSERT( mbedtls_md_hmac_starts( &ctx, buf, 1 )
                 == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_md_hmac_update( NULL, buf, 1 )
                 == MBEDTLS_ERR_MD_BAD_INPUT_DATA );
    TEST_ASSERT( mbedtls_md_hmac_update( &ctx, buf, 1 )
                 == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_md_hmac_finish( NULL, buf )
                 == MBEDTLS_ERR_MD_BAD_INPUT_DATA );
    TEST_ASSERT( mbedtls_md_hmac_finish( &ctx, buf )
                 == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_md_hmac_reset( NULL ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );
    TEST_ASSERT( mbedtls_md_hmac_reset( &ctx ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_md_hmac( NULL, buf, 1, buf, 1, buf )
                 == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_md_process( NULL, buf ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );
    TEST_ASSERT( mbedtls_md_process( &ctx, buf ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    /* Ok, this is not NULL arg but NULL return... */
    TEST_ASSERT( mbedtls_md_info_from_type( MBEDTLS_MD_NONE ) == NULL );
    TEST_ASSERT( mbedtls_md_info_from_string( "no such md" ) == NULL );
exit:
    ;
}

void test_md_null_args_wrapper( void ** params )
{
    (void)params;

    test_md_null_args(  );
}
void test_md_info( int md_type, char * md_name, int md_size )
{
    const mbedtls_md_info_t *md_info;
    const uint8_t *md_type_ptr;
    int found;

    md_info = mbedtls_md_info_from_type( md_type );
    TEST_ASSERT( md_info != NULL );
    TEST_ASSERT( md_info == mbedtls_md_info_from_string( md_name ) );

    TEST_ASSERT( mbedtls_md_get_type( md_info ) == (mbedtls_md_type_t) md_type );
    TEST_ASSERT( mbedtls_md_get_size( md_info ) == (unsigned char) md_size );
    TEST_ASSERT( strcmp( mbedtls_md_get_name( md_info ), md_name ) == 0 );

    found = 0;
    for( md_type_ptr = mbedtls_md_list(); *md_type_ptr != 0; md_type_ptr++ )
        if( *md_type_ptr == md_type )
            found = 1;
    TEST_ASSERT( found == 1 );
exit:
    ;
}

void test_md_info_wrapper( void ** params )
{

    test_md_info( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ) );
}
void test_md_text( char * text_md_name, char * text_src_string,
              data_t * hash )
{
    char md_name[100];
    unsigned char src_str[1000];
    unsigned char output[100];
    const mbedtls_md_info_t *md_info = NULL;

    memset( md_name, 0x00, 100 );
    memset( src_str, 0x00, 1000 );
    memset( output, 0x00, 100 );

    strncpy( (char *) src_str, text_src_string, sizeof( src_str ) - 1 );
    strncpy( (char *) md_name, text_md_name, sizeof( md_name ) - 1 );
    md_info = mbedtls_md_info_from_string(md_name);
    TEST_ASSERT( md_info != NULL );

    TEST_ASSERT ( 0 == mbedtls_md( md_info, src_str, strlen( (char *) src_str ), output ) );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      mbedtls_md_get_size( md_info ),
                                      hash->len ) == 0 );
exit:
    ;
}

void test_md_text_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_md_text( (char *) params[0], (char *) params[1], &data2 );
}
void test_md_hex( char * text_md_name, data_t * src_str, data_t * hash )
{
    char md_name[100];
    unsigned char output[100];
    const mbedtls_md_info_t *md_info = NULL;

    memset( md_name, 0x00, 100 );
    memset( output, 0x00, 100 );

    strncpy( (char *) md_name, text_md_name, sizeof( md_name ) - 1 );
    md_info = mbedtls_md_info_from_string( md_name );
    TEST_ASSERT( md_info != NULL );

    TEST_ASSERT ( 0 == mbedtls_md( md_info, src_str->x, src_str->len, output ) );


    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      mbedtls_md_get_size( md_info ),
                                      hash->len ) == 0 );
exit:
    ;
}

void test_md_hex_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};

    test_md_hex( (char *) params[0], &data1, &data3 );
}
void test_md_text_multi( char * text_md_name, char * text_src_string,
                    data_t * hash )
{
    char md_name[100];
    unsigned char src_str[1000];
    unsigned char output[100];
    int halfway, len;

    const mbedtls_md_info_t *md_info = NULL;
    mbedtls_md_context_t ctx, ctx_copy;

    mbedtls_md_init( &ctx );
    mbedtls_md_init( &ctx_copy );

    memset( md_name, 0x00, 100 );
    memset( src_str, 0x00, 1000 );
    memset( output, 0x00, 100 );

    strncpy( (char *) src_str, text_src_string, sizeof(src_str) - 1 );
    strncpy( (char *) md_name, text_md_name, sizeof(md_name) - 1 );
    len = strlen( (char *) src_str );
    halfway = len / 2;

    md_info = mbedtls_md_info_from_string(md_name);
    TEST_ASSERT( md_info != NULL );
    TEST_ASSERT ( 0 == mbedtls_md_setup( &ctx, md_info, 0 ) );
    TEST_ASSERT ( 0 == mbedtls_md_setup( &ctx_copy, md_info, 0 ) );

    TEST_ASSERT ( 0 == mbedtls_md_starts( &ctx ) );
    TEST_ASSERT ( ctx.md_ctx != NULL );
    TEST_ASSERT ( 0 == mbedtls_md_update( &ctx, src_str, halfway ) );
    TEST_ASSERT ( 0 == mbedtls_md_clone( &ctx_copy, &ctx ) );

    TEST_ASSERT ( 0 == mbedtls_md_update( &ctx, src_str + halfway, len - halfway ) );
    TEST_ASSERT ( 0 == mbedtls_md_finish( &ctx, output ) );
    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      mbedtls_md_get_size( md_info ),
                                      hash->len) == 0 );

    /* Test clone */
    memset( output, 0x00, 100 );

    TEST_ASSERT ( 0 == mbedtls_md_update( &ctx_copy, src_str + halfway, len - halfway ) );
    TEST_ASSERT ( 0 == mbedtls_md_finish( &ctx_copy, output ) );
    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      mbedtls_md_get_size( md_info ),
                                      hash->len ) == 0 );

exit:
    mbedtls_md_free( &ctx );
    mbedtls_md_free( &ctx_copy );
}

void test_md_text_multi_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_md_text_multi( (char *) params[0], (char *) params[1], &data2 );
}
void test_md_hex_multi( char * text_md_name, data_t * src_str, data_t * hash )
{
    char md_name[100];
    unsigned char output[100];
    const mbedtls_md_info_t *md_info = NULL;
    mbedtls_md_context_t ctx, ctx_copy;
    int halfway;

    mbedtls_md_init( &ctx );
    mbedtls_md_init( &ctx_copy );

    memset( md_name, 0x00, 100 );
    memset( output, 0x00, 100 );

    strncpy( (char *) md_name, text_md_name, sizeof( md_name ) - 1 );
    md_info = mbedtls_md_info_from_string(md_name);
    TEST_ASSERT( md_info != NULL );
    TEST_ASSERT ( 0 == mbedtls_md_setup( &ctx, md_info, 0 ) );
    TEST_ASSERT ( 0 == mbedtls_md_setup( &ctx_copy, md_info, 0 ) );

    halfway = src_str->len / 2;

    TEST_ASSERT ( 0 == mbedtls_md_starts( &ctx ) );
    TEST_ASSERT ( ctx.md_ctx != NULL );
    TEST_ASSERT ( 0 == mbedtls_md_update( &ctx, src_str->x, halfway ) );
    TEST_ASSERT ( 0 == mbedtls_md_clone( &ctx_copy, &ctx ) );

    TEST_ASSERT ( 0 == mbedtls_md_update( &ctx, src_str->x + halfway, src_str->len - halfway) );
    TEST_ASSERT ( 0 == mbedtls_md_finish( &ctx, output ) );
    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      mbedtls_md_get_size( md_info ),
                                      hash->len ) == 0 );

    /* Test clone */
    memset( output, 0x00, 100 );

    TEST_ASSERT ( 0 == mbedtls_md_update( &ctx_copy, src_str->x + halfway, src_str->len - halfway ) );
    TEST_ASSERT ( 0 == mbedtls_md_finish( &ctx_copy, output ) );
    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      mbedtls_md_get_size( md_info ),
                                      hash->len ) == 0 );

exit:
    mbedtls_md_free( &ctx );
    mbedtls_md_free( &ctx_copy );
}

void test_md_hex_multi_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};

    test_md_hex_multi( (char *) params[0], &data1, &data3 );
}
void test_mbedtls_md_hmac( char * text_md_name, int trunc_size,
                      data_t * key_str, data_t * src_str,
                      data_t * hash )
{
    char md_name[100];
    unsigned char output[100];
    const mbedtls_md_info_t *md_info = NULL;

    memset( md_name, 0x00, 100 );
    memset( output, 0x00, 100 );

    strncpy( (char *) md_name, text_md_name, sizeof( md_name ) - 1 );
    md_info = mbedtls_md_info_from_string( md_name );
    TEST_ASSERT( md_info != NULL );


    TEST_ASSERT ( mbedtls_md_hmac( md_info, key_str->x, key_str->len, src_str->x, src_str->len, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      trunc_size, hash->len ) == 0 );
exit:
    ;
}

void test_mbedtls_md_hmac_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mbedtls_md_hmac( (char *) params[0], *( (int *) params[1] ), &data2, &data4, &data6 );
}
void test_md_hmac_multi( char * text_md_name, int trunc_size, data_t * key_str,
                    data_t * src_str, data_t * hash )
{
    char md_name[100];
    unsigned char output[100];
    const mbedtls_md_info_t *md_info = NULL;
    mbedtls_md_context_t ctx;
    int halfway;

    mbedtls_md_init( &ctx );

    memset( md_name, 0x00, 100 );
    memset( output, 0x00, 100 );

    strncpy( (char *) md_name, text_md_name, sizeof( md_name ) - 1 );
    md_info = mbedtls_md_info_from_string( md_name );
    TEST_ASSERT( md_info != NULL );
    TEST_ASSERT ( 0 == mbedtls_md_setup( &ctx, md_info, 1 ) );

    halfway = src_str->len / 2;

    TEST_ASSERT ( 0 == mbedtls_md_hmac_starts( &ctx, key_str->x, key_str->len ) );
    TEST_ASSERT ( ctx.md_ctx != NULL );
    TEST_ASSERT ( 0 == mbedtls_md_hmac_update( &ctx, src_str->x, halfway ) );
    TEST_ASSERT ( 0 == mbedtls_md_hmac_update( &ctx, src_str->x + halfway, src_str->len - halfway ) );
    TEST_ASSERT ( 0 == mbedtls_md_hmac_finish( &ctx, output ) );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      trunc_size, hash->len ) == 0 );

    /* Test again, for reset() */
    memset( output, 0x00, 100 );

    TEST_ASSERT ( 0 == mbedtls_md_hmac_reset( &ctx ) );
    TEST_ASSERT ( 0 == mbedtls_md_hmac_update( &ctx, src_str->x, halfway ) );
    TEST_ASSERT ( 0 == mbedtls_md_hmac_update( &ctx, src_str->x + halfway, src_str->len - halfway ) );
    TEST_ASSERT ( 0 == mbedtls_md_hmac_finish( &ctx, output ) );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      trunc_size, hash->len ) == 0 );

exit:
    mbedtls_md_free( &ctx );
}

void test_md_hmac_multi_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_md_hmac_multi( (char *) params[0], *( (int *) params[1] ), &data2, &data4, &data6 );
}
#if defined(MBEDTLS_FS_IO)
void test_mbedtls_md_file( char * text_md_name, char * filename,
                      data_t * hash )
{
    char md_name[100];
    unsigned char output[100];
    const mbedtls_md_info_t *md_info = NULL;

    memset( md_name, 0x00, 100 );
    memset( output, 0x00, 100 );

    strncpy( (char *) md_name, text_md_name, sizeof( md_name ) - 1 );
    md_info = mbedtls_md_info_from_string( md_name );
    TEST_ASSERT( md_info != NULL );

    TEST_ASSERT( mbedtls_md_file( md_info, filename, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      mbedtls_md_get_size( md_info ),
                                      hash->len ) == 0 );
exit:
    ;
}

void test_mbedtls_md_file_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_mbedtls_md_file( (char *) params[0], (char *) params[1], &data2 );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_MD_C */

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

#if defined(MBEDTLS_MD_C)

        case 0:
            {
                *out_value = MBEDTLS_MD_MD2;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_MD_MD4;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_MD_MD5;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_MD_RIPEMD160;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_MD_SHA224;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_MD_SHA384;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_MD_SHA512;
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

#if defined(MBEDTLS_MD_C)

        case 0:
            {
#if defined(MBEDTLS_MD2_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_MD4_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_MD5_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_RIPEMD160_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_SHA1_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_SHA256_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_SHA512_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if !defined(MBEDTLS_SHA512_NO_SHA384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(MBEDTLS_MD_C)
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

#if defined(MBEDTLS_MD_C)
    test_mbedtls_md_process_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_MD_C)
    test_md_null_args_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_MD_C)
    test_md_info_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_MD_C)
    test_md_text_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_MD_C)
    test_md_hex_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_MD_C)
    test_md_text_multi_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_MD_C)
    test_md_hex_multi_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_MD_C)
    test_mbedtls_md_hmac_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_MD_C)
    test_md_hmac_multi_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_MD_C) && defined(MBEDTLS_FS_IO)
    test_mbedtls_md_file_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_md.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
