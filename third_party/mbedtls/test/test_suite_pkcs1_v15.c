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
 * Test file      : ./test_suite_pkcs1_v15.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_pkcs1_v15.function
 *      Test suite data     : suites/test_suite_pkcs1_v15.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_SHA1_C)
#include "third_party/mbedtls/rsa.h"
#include "third_party/mbedtls/md.h"
void test_pkcs1_rsaes_v15_encrypt( int mod, int radix_N, char * input_N,
                              int radix_E, char * input_E, int hash,
                              data_t * message_str, data_t * rnd_buf,
                              data_t * result_str, int result )
{
    unsigned char output[128];
    mbedtls_rsa_context ctx;
    mbedtls_test_rnd_buf_info info;
    mbedtls_mpi N, E;

    info.buf = rnd_buf->x;
    info.length = rnd_buf->len;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V15, hash );
    memset( output, 0x00, sizeof( output ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( ( mod + 7 ) / 8 ) );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == 0 );

    if( message_str->len == 0 )
        message_str->x = NULL;
    TEST_ASSERT( mbedtls_rsa_pkcs1_encrypt( &ctx,
                                            &mbedtls_test_rnd_buffer_rand,
                                            &info, MBEDTLS_RSA_PUBLIC,
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

void test_pkcs1_rsaes_v15_encrypt_wrapper( void ** params )
{
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_pkcs1_rsaes_v15_encrypt( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), &data6, &data8, &data10, *( (int *) params[12] ) );
}
void test_pkcs1_rsaes_v15_decrypt( int mod, int radix_P, char * input_P,
                              int radix_Q, char * input_Q, int radix_N,
                              char * input_N, int radix_E, char * input_E,
                              int hash, data_t * result_str,
                              char * seed, data_t * message_str,
                              int result )
{
    unsigned char output[128];
    mbedtls_rsa_context ctx;
    size_t output_len;
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_mpi N, P, Q, E;
    ((void) seed);

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V15, hash );

    memset( output, 0x00, sizeof( output ) );
    memset( &rnd_info, 0, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, &P, &Q, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( ( mod + 7 ) / 8 ) );
    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );
    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == 0 );

    if( result_str->len == 0 )
    {
        TEST_ASSERT( mbedtls_rsa_pkcs1_decrypt( &ctx,
                                                &mbedtls_test_rnd_pseudo_rand,
                                                &rnd_info,
                                                MBEDTLS_RSA_PRIVATE,
                                                &output_len, message_str->x,
                                                NULL, 0 ) == result );
    }
    else
    {
        TEST_ASSERT( mbedtls_rsa_pkcs1_decrypt( &ctx,
                                                &mbedtls_test_rnd_pseudo_rand,
                                                &rnd_info, MBEDTLS_RSA_PRIVATE,
                                                &output_len, message_str->x,
                                                output, 1000 ) == result );
        if( result == 0 )
        {
            TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                              output_len,
                                              result_str->len) == 0 );
        }
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &P );
    mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_pkcs1_rsaes_v15_decrypt_wrapper( void ** params )
{
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data13 = {(uint8_t *) params[13], *( (uint32_t *) params[14] )};

    test_pkcs1_rsaes_v15_decrypt( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ), (char *) params[8], *( (int *) params[9] ), &data10, (char *) params[12], &data13, *( (int *) params[15] ) );
}
void test_pkcs1_v15_decode( int mode,
                       data_t *input,
                       int expected_plaintext_length_arg,
                       int output_size_arg,
                       int expected_result )
{
    size_t expected_plaintext_length = expected_plaintext_length_arg;
    size_t output_size = output_size_arg;
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_mpi Nmpi, Empi, Pmpi, Qmpi;
    mbedtls_rsa_context ctx;
    static unsigned char N[128] = {
        0xc4, 0x79, 0x4c, 0x6d, 0xb2, 0xe9, 0xdf, 0xc5,
        0xe5, 0xd7, 0x55, 0x4b, 0xfb, 0x6c, 0x2e, 0xec,
        0x84, 0xd0, 0x88, 0x12, 0xaf, 0xbf, 0xb4, 0xf5,
        0x47, 0x3c, 0x7e, 0x92, 0x4c, 0x58, 0xc8, 0x73,
        0xfe, 0x8f, 0x2b, 0x8f, 0x8e, 0xc8, 0x5c, 0xf5,
        0x05, 0xeb, 0xfb, 0x0d, 0x7b, 0x2a, 0x93, 0xde,
        0x15, 0x0d, 0xc8, 0x13, 0xcf, 0xd2, 0x6f, 0x0d,
        0x9d, 0xad, 0x30, 0xe5, 0x70, 0x20, 0x92, 0x9e,
        0xb3, 0x6b, 0xba, 0x5c, 0x50, 0x0f, 0xc3, 0xb2,
        0x7e, 0x64, 0x07, 0x94, 0x7e, 0xc9, 0x4e, 0xc1,
        0x65, 0x04, 0xaf, 0xb3, 0x9f, 0xde, 0xa8, 0x46,
        0xfa, 0x6c, 0xf3, 0x03, 0xaf, 0x1c, 0x1b, 0xec,
        0x75, 0x44, 0x66, 0x77, 0xc9, 0xde, 0x51, 0x33,
        0x64, 0x27, 0xb0, 0xd4, 0x8d, 0x31, 0x6a, 0x11,
        0x27, 0x3c, 0x99, 0xd4, 0x22, 0xc0, 0x9d, 0x12,
        0x01, 0xc7, 0x4a, 0x73, 0xac, 0xbf, 0xc2, 0xbb
    };
    static unsigned char E[1] = { 0x03 };
    static unsigned char P[64] = {
        0xe5, 0x53, 0x1f, 0x88, 0x51, 0xee, 0x59, 0xf8,
        0xc1, 0xe4, 0xcc, 0x5b, 0xb3, 0x75, 0x8d, 0xc8,
        0xe8, 0x95, 0x2f, 0xd0, 0xef, 0x37, 0xb4, 0xcd,
        0xd3, 0x9e, 0x48, 0x8b, 0x81, 0x58, 0x60, 0xb9,
        0x27, 0x1d, 0xb6, 0x28, 0x92, 0x64, 0xa3, 0xa5,
        0x64, 0xbd, 0xcc, 0x53, 0x68, 0xdd, 0x3e, 0x55,
        0xea, 0x9d, 0x5e, 0xcd, 0x1f, 0x96, 0x87, 0xf1,
        0x29, 0x75, 0x92, 0x70, 0x8f, 0x28, 0xfb, 0x2b
    };
    static unsigned char Q[64] = {
        0xdb, 0x53, 0xef, 0x74, 0x61, 0xb4, 0x20, 0x3b,
        0x3b, 0x87, 0x76, 0x75, 0x81, 0x56, 0x11, 0x03,
        0x59, 0x31, 0xe3, 0x38, 0x4b, 0x8c, 0x7a, 0x9c,
        0x05, 0xd6, 0x7f, 0x1e, 0x5e, 0x60, 0xf0, 0x4e,
        0x0b, 0xdc, 0x34, 0x54, 0x1c, 0x2e, 0x90, 0x83,
        0x14, 0xef, 0xc0, 0x96, 0x5c, 0x30, 0x10, 0xcc,
        0xc1, 0xba, 0xa0, 0x54, 0x3f, 0x96, 0x24, 0xca,
        0xa3, 0xfb, 0x55, 0xbc, 0x71, 0x29, 0x4e, 0xb1
    };
    unsigned char original[128];
    unsigned char intermediate[128];
    static unsigned char default_content[128] = {
        /* A randomly generated pattern. */
        0x4c, 0x27, 0x54, 0xa0, 0xce, 0x0d, 0x09, 0x4a,
        0x1c, 0x38, 0x8e, 0x2d, 0xa3, 0xc4, 0xe0, 0x19,
        0x4c, 0x99, 0xb2, 0xbf, 0xe6, 0x65, 0x7e, 0x58,
        0xd7, 0xb6, 0x8a, 0x05, 0x2f, 0xa5, 0xec, 0xa4,
        0x35, 0xad, 0x10, 0x36, 0xff, 0x0d, 0x08, 0x50,
        0x74, 0x47, 0xc9, 0x9c, 0x4a, 0xe7, 0xfd, 0xfa,
        0x83, 0x5f, 0x14, 0x5a, 0x1e, 0xe7, 0x35, 0x08,
        0xad, 0xf7, 0x0d, 0x86, 0xdf, 0xb8, 0xd4, 0xcf,
        0x32, 0xb9, 0x5c, 0xbe, 0xa3, 0xd2, 0x89, 0x70,
        0x7b, 0xc6, 0x48, 0x7e, 0x58, 0x4d, 0xf3, 0xef,
        0x34, 0xb7, 0x57, 0x54, 0x79, 0xc5, 0x8e, 0x0a,
        0xa3, 0xbf, 0x6d, 0x42, 0x83, 0x25, 0x13, 0xa2,
        0x95, 0xc0, 0x0d, 0x32, 0xec, 0x77, 0x91, 0x2b,
        0x68, 0xb6, 0x8c, 0x79, 0x15, 0xfb, 0x94, 0xde,
        0xb9, 0x2b, 0x94, 0xb3, 0x28, 0x23, 0x86, 0x3d,
        0x37, 0x00, 0xe6, 0xf1, 0x1f, 0x4e, 0xd4, 0x42
    };
    unsigned char final[128];
    size_t output_length = 0x7EA0;

    memset( &rnd_info, 0, sizeof( mbedtls_test_rnd_pseudo_info ) );
    mbedtls_mpi_init( &Nmpi ); mbedtls_mpi_init( &Empi );
    mbedtls_mpi_init( &Pmpi ); mbedtls_mpi_init( &Qmpi );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V15, 0 );

    TEST_ASSERT( mbedtls_mpi_read_binary( &Nmpi, N, sizeof( N ) ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &Empi, E, sizeof( E ) ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &Pmpi, P, sizeof( P ) ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &Qmpi, Q, sizeof( Q ) ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &Nmpi, &Pmpi, &Qmpi,
                                     NULL, &Empi ) == 0 );
    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );

    TEST_ASSERT( input->len <= sizeof( N ) );
    memcpy( original, input->x, input->len );
    memset( original + input->len, 'd', sizeof( original ) - input->len );
    if( mode == MBEDTLS_RSA_PRIVATE )
        TEST_ASSERT( mbedtls_rsa_public( &ctx, original, intermediate ) == 0 );
    else
        TEST_ASSERT( mbedtls_rsa_private( &ctx, &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info, original,
                                          intermediate ) == 0 );

    memcpy( final, default_content, sizeof( final ) );
    TEST_ASSERT( mbedtls_rsa_pkcs1_decrypt( &ctx,
                                            &mbedtls_test_rnd_pseudo_rand,
                                            &rnd_info, mode, &output_length,
                                            intermediate, final,
                                            output_size ) == expected_result );
    if( expected_result == 0 )
    {
        TEST_ASSERT( output_length == expected_plaintext_length );
        TEST_ASSERT( timingsafe_bcmp( original + sizeof( N ) - output_length,
                                      final, output_length ) == 0 );
    }
    else if( expected_result == MBEDTLS_ERR_RSA_INVALID_PADDING ||
             expected_result == MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE )
    {
        size_t max_payload_length =
            output_size > sizeof( N ) - 11 ? sizeof( N ) - 11 : output_size;
        size_t i;
        size_t count = 0;

#if !defined(MBEDTLS_RSA_ALT)
        /* Check that the output in invalid cases is what the default
         * implementation currently does. Alternative implementations
         * may produce different output, so we only perform these precise
         * checks when using the default implementation. */
        TEST_ASSERT( output_length == max_payload_length );
        for( i = 0; i < max_payload_length; i++ )
            TEST_ASSERT( final[i] == 0 );
#endif
        /* Even in alternative implementations, the outputs must have
         * changed, otherwise it indicates at least a timing vulnerability
         * because no write to the outputs is performed in the bad case. */
        TEST_ASSERT( output_length != 0x7EA0 );
        for( i = 0; i < max_payload_length; i++ )
            count += ( final[i] == default_content[i] );
        /* If more than 16 bytes are unchanged in final, that's evidence
         * that final wasn't overwritten. */
        TEST_ASSERT( count < 16 );
    }

exit:
    mbedtls_mpi_free( &Nmpi ); mbedtls_mpi_free( &Empi );
    mbedtls_mpi_free( &Pmpi ); mbedtls_mpi_free( &Qmpi );
    mbedtls_rsa_free( &ctx );
}

void test_pkcs1_v15_decode_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_pkcs1_v15_decode( *( (int *) params[0] ), &data1, *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ) );
}
void test_pkcs1_rsassa_v15_sign( int mod, int radix_P, char * input_P, int radix_Q,
                            char * input_Q, int radix_N, char * input_N,
                            int radix_E, char * input_E, int digest, int hash,
                            data_t * message_str, data_t * rnd_buf,
                            data_t * result_str, int result )
{
    unsigned char hash_result[MBEDTLS_MD_MAX_SIZE];
    unsigned char output[128];
    mbedtls_rsa_context ctx;
    mbedtls_mpi N, P, Q, E;
    mbedtls_test_rnd_buf_info info;

    info.buf = rnd_buf->x;
    info.length = rnd_buf->len;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V15, hash );

    memset( hash_result, 0x00, sizeof( hash_result ) );
    memset( output, 0x00, sizeof( output ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, &P, &Q, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( ( mod + 7 ) / 8 ) );
    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );
    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == 0 );


    if( mbedtls_md_info_from_type( digest ) != NULL )
        TEST_ASSERT( mbedtls_md( mbedtls_md_info_from_type( digest ), message_str->x, message_str->len, hash_result ) == 0 );

    TEST_ASSERT( mbedtls_rsa_pkcs1_sign( &ctx, &mbedtls_test_rnd_buffer_rand,
                                         &info, MBEDTLS_RSA_PRIVATE, digest,
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

void test_pkcs1_rsassa_v15_sign_wrapper( void ** params )
{
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};
    data_t data13 = {(uint8_t *) params[13], *( (uint32_t *) params[14] )};
    data_t data15 = {(uint8_t *) params[15], *( (uint32_t *) params[16] )};

    test_pkcs1_rsassa_v15_sign( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ), (char *) params[8], *( (int *) params[9] ), *( (int *) params[10] ), &data11, &data13, &data15, *( (int *) params[17] ) );
}
void test_pkcs1_rsassa_v15_verify( int mod, int radix_N, char * input_N,
                              int radix_E, char * input_E, int digest,
                              int hash, data_t * message_str, char * salt,
                              data_t * result_str, int result )
{
    unsigned char hash_result[MBEDTLS_MD_MAX_SIZE];
    mbedtls_rsa_context ctx;
    mbedtls_mpi N, E;
    ((void) salt);

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V15, hash );
    memset( hash_result, 0x00, sizeof( hash_result ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( ( mod + 7 ) / 8 ) );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == 0 );


    if( mbedtls_md_info_from_type( digest ) != NULL )
        TEST_ASSERT( mbedtls_md( mbedtls_md_info_from_type( digest ), message_str->x, message_str->len, hash_result ) == 0 );

    TEST_ASSERT( mbedtls_rsa_pkcs1_verify( &ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, digest, 0, hash_result, result_str->x ) == result );

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_pkcs1_rsassa_v15_verify_wrapper( void ** params )
{
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_pkcs1_rsassa_v15_verify( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), *( (int *) params[6] ), &data7, (char *) params[9], &data10, *( (int *) params[12] ) );
}
#endif /* MBEDTLS_SHA1_C */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_PKCS1_V15 */

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

#if defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)

        case 0:
            {
                *out_value = MBEDTLS_MD_NONE;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ERR_RSA_INVALID_PADDING;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_RSA_PRIVATE;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_RSA_PUBLIC;
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

#if defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)

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

#if defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)
    test_pkcs1_rsaes_v15_encrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)
    test_pkcs1_rsaes_v15_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)
    test_pkcs1_v15_decode_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)
    test_pkcs1_rsassa_v15_sign_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)
    test_pkcs1_rsassa_v15_verify_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_pkcs1_v15.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
