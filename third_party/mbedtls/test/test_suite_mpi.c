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
 * Test file      : ./test_suite_mpi.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_mpi.function
 *      Test suite data     : suites/test_suite_mpi.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_BIGNUM_C)
#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/entropy.h"

#if MBEDTLS_MPI_MAX_BITS > 792
#define MPI_MAX_BITS_LARGER_THAN_792
#endif

typedef struct mbedtls_test_mpi_random
{
    data_t *data;
    size_t  pos;
    size_t  chunk_len;
} mbedtls_test_mpi_random;

/*
 * This function is called by the Miller-Rabin primality test each time it
 * chooses a random witness. The witnesses (or non-witnesses as provided by the
 * test) are stored in the data member of the state structure. Each number is in
 * the format that mbedtls_mpi_read_string understands and is chunk_len long.
 */
int mbedtls_test_mpi_miller_rabin_determinizer( void* state,
                                                unsigned char* buf,
                                                size_t len )
{
    mbedtls_test_mpi_random *random = (mbedtls_test_mpi_random*) state;

    if( random == NULL || random->data->x == NULL || buf == NULL )
        return( -1 );

    if( random->pos + random->chunk_len > random->data->len
            || random->chunk_len > len )
    {
        return( -1 );
    }

    memset( buf, 0, len );

    /* The witness is written to the end of the buffer, since the buffer is
     * used as big endian, unsigned binary data in mbedtls_mpi_read_binary.
     * Writing the witness to the start of the buffer would result in the
     * buffer being 'witness 000...000', which would be treated as
     * witness * 2^n for some n. */
    memcpy( buf + len - random->chunk_len, &random->data->x[random->pos],
            random->chunk_len );

    random->pos += random->chunk_len;

    return( 0 );
}

/* Random generator that is told how many bytes to return. */
static int f_rng_bytes_left( void *state, unsigned char *buf, size_t len )
{
    size_t *bytes_left = state;
    size_t i;
    for( i = 0; i < len; i++ )
    {
        if( *bytes_left == 0 )
            return( MBEDTLS_ERR_ENTROPY_SOURCE_FAILED );
        buf[i] = *bytes_left & 0xff;
        --( *bytes_left );
    }
    return( 0 );
}

void test_mpi_valid_param( )
{
    TEST_VALID_PARAM( mbedtls_mpi_free( NULL ) );
exit:
    ;
}

void test_mpi_valid_param_wrapper( void ** params )
{
    (void)params;

    test_mpi_valid_param(  );
}
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_mpi_invalid_param( )
{
    mbedtls_mpi X;
    const char *s_in = "00101000101010";
    char s_out[16] = { 0 };
    unsigned char u_out[16] = { 0 };
    unsigned char u_in[16] = { 0 };
    size_t olen;
    mbedtls_mpi_uint mpi_uint;

    TEST_INVALID_PARAM( mbedtls_mpi_init( NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_grow( NULL, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_copy( NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_copy( &X, NULL ) );

    TEST_INVALID_PARAM( mbedtls_mpi_swap( NULL, &X ) );
    TEST_INVALID_PARAM( mbedtls_mpi_swap( &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_safe_cond_assign( NULL, &X, 0 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_safe_cond_assign( &X, NULL, 0 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_safe_cond_swap( NULL, &X, 0 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_safe_cond_swap( &X, NULL, 0 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_lset( NULL, 42 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_get_bit( NULL, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_set_bit( NULL, 42, 0 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_read_string( NULL, 2, s_in ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_read_string( &X, 2, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_write_string( NULL, 2,
                                                      s_out, sizeof( s_out ),
                                                      &olen ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_write_string( &X, 2,
                                                      NULL, sizeof( s_out ),
                                                      &olen ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_write_string( &X, 2,
                                                      s_out, sizeof( s_out ),
                                                      NULL ) );

#if defined(MBEDTLS_FS_IO)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_read_file( NULL, 2, stdin ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_read_file( &X, 2, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_write_file( "", NULL, 2, NULL ) );
#endif /* MBEDTLS_FS_IO */

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_read_binary( NULL, u_in,
                                                     sizeof( u_in ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_read_binary( &X, NULL,
                                                     sizeof( u_in ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_write_binary( NULL, u_out,
                                                      sizeof( u_out ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_write_binary( &X, NULL,
                                                      sizeof( u_out ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_shift_l( NULL, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_shift_r( NULL, 42 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_cmp_abs( NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_cmp_abs( &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_cmp_mpi( NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_cmp_mpi( &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_cmp_int( NULL, 42 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_add_abs( NULL, &X, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_add_abs( &X, NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_add_abs( &X, &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_sub_abs( NULL, &X, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_sub_abs( &X, NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_sub_abs( &X, &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_add_mpi( NULL, &X, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_add_mpi( &X, NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_add_mpi( &X, &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_sub_mpi( NULL, &X, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_sub_mpi( &X, NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_sub_mpi( &X, &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_add_int( NULL, &X, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_add_int( &X, NULL, 42 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_sub_int( NULL, &X, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_sub_int( &X, NULL, 42 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_mul_mpi( NULL, &X, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_mul_mpi( &X, NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_mul_mpi( &X, &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_mul_int( NULL, &X, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_mul_int( &X, NULL, 42 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_div_mpi( &X, &X, NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_div_mpi( &X, &X, &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_div_int( &X, &X, NULL, 42 ) );

    TEST_INVALID_PARAM_RET( 0, mbedtls_mpi_lsb( NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_mod_mpi( NULL, &X, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_mod_mpi( &X, NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_mod_mpi( &X, &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_mod_int( NULL, &X, 42 ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_mod_int( &mpi_uint, NULL, 42 ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_exp_mod( NULL, &X, &X, &X, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_exp_mod( &X, NULL, &X, &X, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_exp_mod( &X, &X, NULL, &X, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_exp_mod( &X, &X, &X, NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_fill_random( NULL, 42,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_fill_random( &X, 42, NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_gcd( NULL, &X, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_gcd( &X, NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_gcd( &X, &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_inv_mod( NULL, &X, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_inv_mod( &X, NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_MPI_BAD_INPUT_DATA,
                            mbedtls_mpi_inv_mod( &X, &X, NULL ) );

exit:
    return;
}

void test_mpi_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_mpi_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_mpi_null(  )
{
    mbedtls_mpi X, Y, Z;

    mbedtls_mpi_init( &X );
    mbedtls_mpi_init( &Y );
    mbedtls_mpi_init( &Z );

    TEST_ASSERT( mbedtls_mpi_get_bit( &X, 42 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_lsb( &X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_bitlen( &X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_size( &X ) == 0 );

exit:
    mbedtls_mpi_free( &X );
}

void test_mpi_null_wrapper( void ** params )
{
    (void)params;

    test_mpi_null(  );
}
void test_mpi_read_write_string( int radix_X, char * input_X, int radix_A,
                            char * input_A, int output_size, int result_read,
                            int result_write )
{
    mbedtls_mpi X;
    char str[1000];
    size_t len;

    mbedtls_mpi_init( &X );

    memset( str, '!', sizeof( str ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == result_read );
    if( result_read == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_write_string( &X, radix_A, str, output_size, &len ) == result_write );
        if( result_write == 0 )
        {
            TEST_ASSERT( strcasecmp( str, input_A ) == 0 );
            TEST_ASSERT( str[len] == '!' );
        }
    }

exit:
    mbedtls_mpi_free( &X );
}

void test_mpi_read_write_string_wrapper( void ** params )
{

    test_mpi_read_write_string( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ) );
}
void test_mbedtls_mpi_read_binary( data_t * buf, int radix_A, char * input_A )
{
    mbedtls_mpi X;
    char str[1000];
    size_t len;

    mbedtls_mpi_init( &X );


    TEST_ASSERT( mbedtls_mpi_read_binary( &X, buf->x, buf->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_write_string( &X, radix_A, str, sizeof( str ), &len ) == 0 );
    TEST_ASSERT( strcmp( (char *) str, input_A ) == 0 );

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_read_binary_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_mbedtls_mpi_read_binary( &data0, *( (int *) params[2] ), (char *) params[3] );
}
void test_mbedtls_mpi_read_binary_le( data_t * buf, int radix_A, char * input_A )
{
    mbedtls_mpi X;
    char str[1000];
    size_t len;

    mbedtls_mpi_init( &X );


    TEST_ASSERT( mbedtls_mpi_read_binary_le( &X, buf->x, buf->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_write_string( &X, radix_A, str, sizeof( str ), &len ) == 0 );
    TEST_ASSERT( strcmp( (char *) str, input_A ) == 0 );

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_read_binary_le_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_mbedtls_mpi_read_binary_le( &data0, *( (int *) params[2] ), (char *) params[3] );
}
void test_mbedtls_mpi_write_binary( int radix_X, char * input_X,
                               data_t * input_A, int output_size,
                               int result )
{
    mbedtls_mpi X;
    unsigned char buf[1000];
    size_t buflen;

    memset( buf, 0x00, 1000 );

    mbedtls_mpi_init( &X );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );

    buflen = mbedtls_mpi_size( &X );
    if( buflen > (size_t) output_size )
        buflen = (size_t) output_size;

    TEST_ASSERT( mbedtls_mpi_write_binary( &X, buf, buflen ) == result );
    if( result == 0)
    {

        TEST_ASSERT( mbedtls_test_hexcmp( buf, input_A->x,
                                          buflen, input_A->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_write_binary_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    test_mbedtls_mpi_write_binary( *( (int *) params[0] ), (char *) params[1], &data2, *( (int *) params[4] ), *( (int *) params[5] ) );
}

void test_mbedtls_mpi_write_binary_le( int radix_X, char * input_X,
                                       data_t * input_A, int output_size,
                                       int result )
{
    mbedtls_mpi X;
    unsigned char buf[1000];
    size_t buflen;

    memset( buf, 0x00, 1000 );

    mbedtls_mpi_init( &X );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );

    buflen = mbedtls_mpi_size( &X );
    if( buflen > (size_t) output_size )
        buflen = (size_t) output_size;

    TEST_ASSERT( mbedtls_mpi_write_binary_le( &X, buf, buflen ) == result );
    if( result == 0)
    {

        TEST_ASSERT( mbedtls_test_hexcmp( buf, input_A->x,
                                          buflen, input_A->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_write_binary_le_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_mbedtls_mpi_write_binary_le( *( (int *) params[0] ), (char *) params[1], &data2, *( (int *) params[4] ), *( (int *) params[5] ) );
}
#if defined(MBEDTLS_FS_IO)
void test_mbedtls_mpi_read_file( int radix_X, char * input_file,
                            data_t * input_A, int result )
{
    mbedtls_mpi X;
    unsigned char buf[1000];
    size_t buflen;
    FILE *file;
    int ret;

    memset( buf, 0x00, 1000 );

    mbedtls_mpi_init( &X );

    file = fopen( input_file, "r" );
    TEST_ASSERT( file != NULL );
    ret = mbedtls_mpi_read_file( &X, radix_X, file );
    fclose(file);
    TEST_ASSERT( ret == result );

    if( result == 0 )
    {
        buflen = mbedtls_mpi_size( &X );
        TEST_ASSERT( mbedtls_mpi_write_binary( &X, buf, buflen ) == 0 );


        TEST_ASSERT( mbedtls_test_hexcmp( buf, input_A->x,
                                          buflen, input_A->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_read_file_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_mbedtls_mpi_read_file( *( (int *) params[0] ), (char *) params[1], &data2, *( (int *) params[4] ) );
}
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
void test_mbedtls_mpi_write_file( int radix_X, char * input_X, int output_radix,
                             char * output_file )
{
    mbedtls_mpi X, Y;
    FILE *file_out, *file_in;
    int ret;

    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );

    file_out = fopen( output_file, "w" );
    TEST_ASSERT( file_out != NULL );
    ret = mbedtls_mpi_write_file( NULL, &X, output_radix, file_out );
    fclose(file_out);
    TEST_ASSERT( ret == 0 );

    file_in = fopen( output_file, "r" );
    TEST_ASSERT( file_in != NULL );
    ret = mbedtls_mpi_read_file( &Y, output_radix, file_in );
    fclose(file_in);
    TEST_ASSERT( ret == 0 );

    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &Y ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y );
}

void test_mbedtls_mpi_write_file_wrapper( void ** params )
{

    test_mbedtls_mpi_write_file( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3] );
}
#endif /* MBEDTLS_FS_IO */
void test_mbedtls_mpi_get_bit( int radix_X, char * input_X, int pos, int val )
{
    mbedtls_mpi X;
    mbedtls_mpi_init( &X );
    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_get_bit( &X, pos ) == val );

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_get_bit_wrapper( void ** params )
{

    test_mbedtls_mpi_get_bit( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ) );
}
void test_mbedtls_mpi_set_bit( int radix_X, char * input_X, int pos, int val,
                          int radix_Y, char * output_Y, int result )
{
    mbedtls_mpi X, Y;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, output_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_set_bit( &X, pos, val ) == result );

    if( result == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &Y ) == 0 );
    }

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y );
}

void test_mbedtls_mpi_set_bit_wrapper( void ** params )
{

    test_mbedtls_mpi_set_bit( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ) );
}
void test_mbedtls_mpi_lsb( int radix_X, char * input_X, int nr_bits )
{
    mbedtls_mpi X;
    mbedtls_mpi_init( &X );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_lsb( &X ) == (size_t) nr_bits );

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_lsb_wrapper( void ** params )
{

    test_mbedtls_mpi_lsb( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ) );
}
void test_mbedtls_mpi_bitlen( int radix_X, char * input_X, int nr_bits )
{
    mbedtls_mpi X;
    mbedtls_mpi_init( &X );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_bitlen( &X ) == (size_t) nr_bits );

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_bitlen_wrapper( void ** params )
{

    test_mbedtls_mpi_bitlen( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ) );
}
void test_mbedtls_mpi_gcd( int radix_X, char * input_X, int radix_Y,
                      char * input_Y, int radix_A, char * input_A )
{
    mbedtls_mpi A, X, Y, Z;
    mbedtls_mpi_init( &A ); mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &Z );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_gcd( &Z, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) == 0 );

exit:
    mbedtls_mpi_free( &A ); mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &Z );
}

void test_mbedtls_mpi_gcd_wrapper( void ** params )
{

    test_mbedtls_mpi_gcd( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5] );
}
void test_mbedtls_mpi_cmp_int( int input_X, int input_A, int result_CMP )
{
    mbedtls_mpi X;
    mbedtls_mpi_init( &X  );

    TEST_ASSERT( mbedtls_mpi_lset( &X, input_X ) == 0);
    TEST_ASSERT( mbedtls_mpi_cmp_int( &X, input_A ) == result_CMP);

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_cmp_int_wrapper( void ** params )
{

    test_mbedtls_mpi_cmp_int( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
void test_mbedtls_mpi_cmp_mpi( int radix_X, char * input_X, int radix_Y,
                          char * input_Y, int input_A )
{
    mbedtls_mpi X, Y;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &Y ) == input_A );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y );
}

void test_mbedtls_mpi_cmp_mpi_wrapper( void ** params )
{

    test_mbedtls_mpi_cmp_mpi( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ) );
}
void test_mbedtls_mpi_lt_mpi_ct( int size_X, char * input_X,
                            int size_Y, char * input_Y,
                            int input_ret, int input_err )
{
    unsigned ret = -1;
    unsigned input_uret = input_ret;
    mbedtls_mpi X, Y;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, 16, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, 16, input_Y ) == 0 );

    TEST_ASSERT( mbedtls_mpi_grow( &X, size_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_grow( &Y, size_Y ) == 0 );

    TEST_ASSERT( mbedtls_mpi_lt_mpi_ct( &X, &Y, &ret ) == input_err );
    if( input_err == 0 )
        TEST_ASSERT( ret == input_uret );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y );
}

void test_mbedtls_mpi_lt_mpi_ct_wrapper( void ** params )
{

    test_mbedtls_mpi_lt_mpi_ct( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), *( (int *) params[5] ) );
}
void test_mbedtls_mpi_cmp_abs( int radix_X, char * input_X, int radix_Y,
                          char * input_Y, int input_A )
{
    mbedtls_mpi X, Y;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_abs( &X, &Y ) == input_A );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y );
}

void test_mbedtls_mpi_cmp_abs_wrapper( void ** params )
{

    test_mbedtls_mpi_cmp_abs( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ) );
}
void test_mbedtls_mpi_copy_sint( int input_X, int input_Y )
{
    mbedtls_mpi X, Y;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y );

    TEST_ASSERT( mbedtls_mpi_lset( &X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_lset( &Y, input_Y ) == 0 );

    TEST_ASSERT( mbedtls_mpi_copy( &Y, &X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_int( &X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_int( &Y, input_X ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y );
}

void test_mbedtls_mpi_copy_sint_wrapper( void ** params )
{

    test_mbedtls_mpi_copy_sint( *( (int *) params[0] ), *( (int *) params[1] ) );
}
void test_mbedtls_mpi_copy_binary( data_t *input_X, data_t *input_Y )
{
    mbedtls_mpi X, Y, X0;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &X0 );

    TEST_ASSERT( mbedtls_mpi_read_binary( &X, input_X->x, input_X->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &Y, input_Y->x, input_Y->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &X0, input_X->x, input_X->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &X0 ) == 0 );

    TEST_ASSERT( mbedtls_mpi_copy( &Y, &X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &X0 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Y, &X0 ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &X0 );
}

void test_mbedtls_mpi_copy_binary_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_mbedtls_mpi_copy_binary( &data0, &data2 );
}
void test_mpi_copy_self( int input_X )
{
    mbedtls_mpi X;
    mbedtls_mpi_init( &X );

    TEST_ASSERT( mbedtls_mpi_lset( &X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_copy( &X, &X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_int( &X, input_X ) == 0 );

exit:
    mbedtls_mpi_free( &X );
}

void test_mpi_copy_self_wrapper( void ** params )
{

    test_mpi_copy_self( *( (int *) params[0] ) );
}
void test_mbedtls_mpi_shrink( int before, int used, int min, int after )
{
    mbedtls_mpi X;
    mbedtls_mpi_init( &X );

    TEST_ASSERT( mbedtls_mpi_grow( &X, before ) == 0 );
    TEST_ASSERT( used <= before );
    memset( X.p, 0x2a, used * sizeof( mbedtls_mpi_uint ) );
    TEST_ASSERT( mbedtls_mpi_shrink( &X, min ) == 0 );
    TEST_ASSERT( X.n == (size_t) after );

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_shrink_wrapper( void ** params )
{

    test_mbedtls_mpi_shrink( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
void test_mbedtls_mpi_safe_cond_assign( int x_sign, char * x_str, int y_sign,
                                   char * y_str )
{
    mbedtls_mpi X, Y, XX;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &XX );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, 16, x_str ) == 0 );
    X.s = x_sign;
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, 16, y_str ) == 0 );
    Y.s = y_sign;
    TEST_ASSERT( mbedtls_mpi_copy( &XX, &X ) == 0 );

    TEST_ASSERT( mbedtls_mpi_safe_cond_assign( &X, &Y, 0 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &XX ) == 0 );

    TEST_ASSERT( mbedtls_mpi_safe_cond_assign( &X, &Y, 1 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &Y ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &XX );
}

void test_mbedtls_mpi_safe_cond_assign_wrapper( void ** params )
{

    test_mbedtls_mpi_safe_cond_assign( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3] );
}
void test_mbedtls_mpi_safe_cond_swap( int x_sign, char * x_str, int y_sign,
                                 char * y_str )
{
    mbedtls_mpi X, Y, XX, YY;

    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y );
    mbedtls_mpi_init( &XX ); mbedtls_mpi_init( &YY );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, 16, x_str ) == 0 );
    X.s = x_sign;
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, 16, y_str ) == 0 );
    Y.s = y_sign;

    TEST_ASSERT( mbedtls_mpi_copy( &XX, &X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_copy( &YY, &Y ) == 0 );

    TEST_ASSERT( mbedtls_mpi_safe_cond_swap( &X, &Y, 0 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &XX ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Y, &YY ) == 0 );

    TEST_ASSERT( mbedtls_mpi_safe_cond_swap( &X, &Y, 1 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Y, &XX ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &YY ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y );
    mbedtls_mpi_free( &XX ); mbedtls_mpi_free( &YY );
}

void test_mbedtls_mpi_safe_cond_swap_wrapper( void ** params )
{

    test_mbedtls_mpi_safe_cond_swap( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3] );
}
void test_mbedtls_mpi_swap_sint( int input_X, int input_Y )
{
    mbedtls_mpi X, Y;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y );

    TEST_ASSERT( mbedtls_mpi_lset( &X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_lset( &Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_int( &X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_int( &Y, input_Y ) == 0 );

    mbedtls_mpi_swap( &X, &Y );
    TEST_ASSERT( mbedtls_mpi_cmp_int( &X, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_int( &Y, input_X ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y );
}

void test_mbedtls_mpi_swap_sint_wrapper( void ** params )
{

    test_mbedtls_mpi_swap_sint( *( (int *) params[0] ), *( (int *) params[1] ) );
}
void test_mbedtls_mpi_swap_binary( data_t *input_X, data_t *input_Y )
{
    mbedtls_mpi X, Y, X0, Y0;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y );
    mbedtls_mpi_init( &X0 ); mbedtls_mpi_init( &Y0 );

    TEST_ASSERT( mbedtls_mpi_read_binary( &X, input_X->x, input_X->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &Y, input_Y->x, input_Y->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &X0, input_X->x, input_X->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &Y0, input_Y->x, input_Y->len ) == 0 );

    mbedtls_mpi_swap( &X, &Y );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &Y0 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Y, &X0 ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y );
    mbedtls_mpi_free( &X0 ); mbedtls_mpi_free( &Y0 );
}

void test_mbedtls_mpi_swap_binary_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_mbedtls_mpi_swap_binary( &data0, &data2 );
}
void test_mpi_swap_self( data_t *input_X )
{
    mbedtls_mpi X, X0;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &X0 );

    TEST_ASSERT( mbedtls_mpi_read_binary( &X, input_X->x, input_X->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &X0, input_X->x, input_X->len ) == 0 );

    mbedtls_mpi_swap( &X, &X );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &X0 ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &X0 );
}

void test_mpi_swap_self_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_mpi_swap_self( &data0 );
}
void test_mbedtls_mpi_add_mpi( int radix_X, char * input_X, int radix_Y,
                          char * input_Y, int radix_A, char * input_A )
{
    mbedtls_mpi X, Y, Z, A;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &Z ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_add_mpi( &Z, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) == 0 );

    /* result == first operand */
    TEST_ASSERT( mbedtls_mpi_add_mpi( &X, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );

    /* result == second operand */
    TEST_ASSERT( mbedtls_mpi_add_mpi( &Y, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Y, &A ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &Z ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_add_mpi_wrapper( void ** params )
{

    test_mbedtls_mpi_add_mpi( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5] );
}
void test_mbedtls_mpi_add_mpi_inplace( int radix_X, char * input_X, int radix_A,
                                  char * input_A )
{
    mbedtls_mpi X, A;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_sub_abs( &X, &X, &X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_int( &X, 0 ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_add_abs( &X, &X, &X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &A ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_add_mpi( &X, &X, &X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &A ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_add_mpi_inplace_wrapper( void ** params )
{

    test_mbedtls_mpi_add_mpi_inplace( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3] );
}
void test_mbedtls_mpi_add_abs( int radix_X, char * input_X, int radix_Y,
                          char * input_Y, int radix_A, char * input_A )
{
    mbedtls_mpi X, Y, Z, A;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &Z ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_add_abs( &Z, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) == 0 );

    /* result == first operand */
    TEST_ASSERT( mbedtls_mpi_add_abs( &X, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );

    /* result == second operand */
    TEST_ASSERT( mbedtls_mpi_add_abs( &Y, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Y, &A ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &Z ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_add_abs_wrapper( void ** params )
{

    test_mbedtls_mpi_add_abs( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5] );
}
void test_mbedtls_mpi_add_int( int radix_X, char * input_X, int input_Y,
                          int radix_A, char * input_A )
{
    mbedtls_mpi X, Z, A;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Z ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_add_int( &Z, &X, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Z ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_add_int_wrapper( void ** params )
{

    test_mbedtls_mpi_add_int( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4] );
}
void test_mbedtls_mpi_sub_mpi( int radix_X, char * input_X, int radix_Y,
                          char * input_Y, int radix_A, char * input_A )
{
    mbedtls_mpi X, Y, Z, A;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &Z ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_sub_mpi( &Z, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) == 0 );

    /* result == first operand */
    TEST_ASSERT( mbedtls_mpi_sub_mpi( &X, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );

    /* result == second operand */
    TEST_ASSERT( mbedtls_mpi_sub_mpi( &Y, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Y, &A ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &Z ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_sub_mpi_wrapper( void ** params )
{

    test_mbedtls_mpi_sub_mpi( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5] );
}

void test_mbedtls_mpi_sub_abs( int radix_X, char * input_X, int radix_Y,
                          char * input_Y, int radix_A, char * input_A,
                          int sub_result )
{
    mbedtls_mpi X, Y, Z, A;
    int res;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &Z ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );

    res = mbedtls_mpi_sub_abs( &Z, &X, &Y );
    TEST_ASSERT( res == sub_result );
    if( res == 0 )
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) == 0 );

    /* result == first operand */
    TEST_ASSERT( mbedtls_mpi_sub_abs( &X, &X, &Y ) == sub_result );
    if( sub_result == 0 )
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );

    /* result == second operand */
    TEST_ASSERT( mbedtls_mpi_sub_abs( &Y, &X, &Y ) == sub_result );
    if( sub_result == 0 )
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Y, &A ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &Z ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_sub_abs_wrapper( void ** params )
{

    test_mbedtls_mpi_sub_abs( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ) );
}
void test_mbedtls_mpi_sub_int( int radix_X, char * input_X, int input_Y,
                          int radix_A, char * input_A )
{
    mbedtls_mpi X, Z, A;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Z ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_sub_int( &Z, &X, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Z ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_sub_int_wrapper( void ** params )
{

    test_mbedtls_mpi_sub_int( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4] );
}
void test_mbedtls_mpi_mul_mpi( int radix_X, char * input_X, int radix_Y,
                          char * input_Y, int radix_A, char * input_A )
{
    mbedtls_mpi X, Y, Z, A;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &Z ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_mul_mpi( &Z, &X, &Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &Z ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_mul_mpi_wrapper( void ** params )
{

    test_mbedtls_mpi_mul_mpi( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5] );
}
void test_mbedtls_mpi_mul_int( int radix_X, char * input_X, int input_Y,
                          int radix_A, char * input_A,
                          char * result_comparison )
{
    mbedtls_mpi X, Z, A;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Z ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_mul_int( &Z, &X, input_Y ) == 0 );
    if( strcmp( result_comparison, "==" ) == 0 )
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) == 0 );
    else if( strcmp( result_comparison, "!=" ) == 0 )
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) != 0 );
    else
        TEST_ASSERT( "unknown operator" == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Z ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_mul_int_wrapper( void ** params )
{

    test_mbedtls_mpi_mul_int( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], (char *) params[5] );
}
void test_mbedtls_mpi_div_mpi( int radix_X, char * input_X, int radix_Y,
                          char * input_Y, int radix_A, char * input_A,
                          int radix_B, char * input_B, int div_result )
{
    mbedtls_mpi X, Y, Q, R, A, B;
    int res;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &R );
    mbedtls_mpi_init( &A ); mbedtls_mpi_init( &B );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &B, radix_B, input_B ) == 0 );
    res = mbedtls_mpi_div_mpi( &Q, &R, &X, &Y );
    TEST_ASSERT( res == div_result );
    if( res == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Q, &A ) == 0 );
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R, &B ) == 0 );
    }

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &R );
    mbedtls_mpi_free( &A ); mbedtls_mpi_free( &B );
}

void test_mbedtls_mpi_div_mpi_wrapper( void ** params )
{

    test_mbedtls_mpi_div_mpi( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], *( (int *) params[8] ) );
}
void test_mbedtls_mpi_div_int( int radix_X, char * input_X, int input_Y,
                          int radix_A, char * input_A, int radix_B,
                          char * input_B, int div_result )
{
    mbedtls_mpi X, Q, R, A, B;
    int res;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &R ); mbedtls_mpi_init( &A );
    mbedtls_mpi_init( &B );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &B, radix_B, input_B ) == 0 );
    res = mbedtls_mpi_div_int( &Q, &R, &X, input_Y );
    TEST_ASSERT( res == div_result );
    if( res == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Q, &A ) == 0 );
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R, &B ) == 0 );
    }

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &R ); mbedtls_mpi_free( &A );
    mbedtls_mpi_free( &B );
}

void test_mbedtls_mpi_div_int_wrapper( void ** params )
{

    test_mbedtls_mpi_div_int( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ) );
}
void test_mbedtls_mpi_mod_mpi( int radix_X, char * input_X, int radix_Y,
                          char * input_Y, int radix_A, char * input_A,
                          int div_result )
{
    mbedtls_mpi X, Y, A;
    int res;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    res = mbedtls_mpi_mod_mpi( &X, &X, &Y );
    TEST_ASSERT( res == div_result );
    if( res == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &A ) == 0 );
    }

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_mod_mpi_wrapper( void ** params )
{

    test_mbedtls_mpi_mod_mpi( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ) );
}
void test_mbedtls_mpi_mod_int( int radix_X, char * input_X, int input_Y,
                          int input_A, int div_result )
{
    mbedtls_mpi X;
    int res;
    mbedtls_mpi_uint r;
    mbedtls_mpi_init( &X );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    res = mbedtls_mpi_mod_int( &r, &X, input_Y );
    TEST_ASSERT( res == div_result );
    if( res == 0 )
    {
        TEST_ASSERT( r == (mbedtls_mpi_uint) input_A );
    }

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_mod_int_wrapper( void ** params )
{

    test_mbedtls_mpi_mod_int( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
void test_mbedtls_mpi_exp_mod( int radix_A, char * input_A, int radix_E,
                          char * input_E, int radix_N, char * input_N,
                          int radix_RR, char * input_RR, int radix_X,
                          char * input_X, int div_result )
{
    mbedtls_mpi A, E, N, RR, Z, X;
    int res;
    mbedtls_mpi_init( &A  ); mbedtls_mpi_init( &E ); mbedtls_mpi_init( &N );
    mbedtls_mpi_init( &RR ); mbedtls_mpi_init( &Z ); mbedtls_mpi_init( &X );

    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );

    if( strlen( input_RR ) )
        TEST_ASSERT( mbedtls_mpi_read_string( &RR, radix_RR, input_RR ) == 0 );

    res = mbedtls_mpi_exp_mod( &Z, &A, &E, &N, &RR );
    TEST_ASSERT( res == div_result );
    if( res == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &X ) == 0 );
    }

exit:
    mbedtls_mpi_free( &A  ); mbedtls_mpi_free( &E ); mbedtls_mpi_free( &N );
    mbedtls_mpi_free( &RR ); mbedtls_mpi_free( &Z ); mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_exp_mod_wrapper( void ** params )
{

    test_mbedtls_mpi_exp_mod( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], *( (int *) params[8] ), (char *) params[9], *( (int *) params[10] ) );
}
void test_mbedtls_mpi_exp_mod_size( int A_bytes, int E_bytes, int N_bytes,
                               int radix_RR, char * input_RR, int exp_result )
{
    mbedtls_mpi A, E, N, RR, Z;
    mbedtls_mpi_init( &A  ); mbedtls_mpi_init( &E ); mbedtls_mpi_init( &N );
    mbedtls_mpi_init( &RR ); mbedtls_mpi_init( &Z );

    /* Set A to 2^(A_bytes - 1) + 1 */
    TEST_ASSERT( mbedtls_mpi_lset( &A, 1 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_shift_l( &A, ( A_bytes * 8 ) - 1 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_set_bit( &A, 0, 1 ) == 0 );

    /* Set E to 2^(E_bytes - 1) + 1 */
    TEST_ASSERT( mbedtls_mpi_lset( &E, 1 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_shift_l( &E, ( E_bytes * 8 ) - 1 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_set_bit( &E, 0, 1 ) == 0 );

    /* Set N to 2^(N_bytes - 1) + 1 */
    TEST_ASSERT( mbedtls_mpi_lset( &N, 1 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_shift_l( &N, ( N_bytes * 8 ) - 1 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_set_bit( &N, 0, 1 ) == 0 );

    if( strlen( input_RR ) )
        TEST_ASSERT( mbedtls_mpi_read_string( &RR, radix_RR, input_RR ) == 0 );

    TEST_ASSERT( mbedtls_mpi_exp_mod( &Z, &A, &E, &N, &RR ) == exp_result );

exit:
    mbedtls_mpi_free( &A  ); mbedtls_mpi_free( &E ); mbedtls_mpi_free( &N );
    mbedtls_mpi_free( &RR ); mbedtls_mpi_free( &Z );
}

void test_mbedtls_mpi_exp_mod_size_wrapper( void ** params )
{

    test_mbedtls_mpi_exp_mod_size( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ) );
}
void test_mbedtls_mpi_inv_mod( int radix_X, char * input_X, int radix_Y,
                          char * input_Y, int radix_A, char * input_A,
                          int div_result )
{
    mbedtls_mpi X, Y, Z, A;
    int res;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &Z ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, radix_Y, input_Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    res = mbedtls_mpi_inv_mod( &Z, &X, &Y );
    TEST_ASSERT( res == div_result );
    if( res == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &Z, &A ) == 0 );
    }

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &Z ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_inv_mod_wrapper( void ** params )
{

    test_mbedtls_mpi_inv_mod( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ) );
}
#if defined(MBEDTLS_GENPRIME)
void test_mbedtls_mpi_is_prime( int radix_X, char * input_X, int div_result )
{
    mbedtls_mpi X;
    int res;
    mbedtls_mpi_init( &X );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    res = mbedtls_mpi_is_prime_ext( &X, 40, mbedtls_test_rnd_std_rand, NULL );
    TEST_ASSERT( res == div_result );

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_is_prime_wrapper( void ** params )
{

    test_mbedtls_mpi_is_prime( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ) );
}
#endif /* MBEDTLS_GENPRIME */
#if defined(MBEDTLS_GENPRIME)
void test_mbedtls_mpi_is_prime_det( data_t * input_X, data_t * witnesses,
                               int chunk_len, int rounds )
{
    mbedtls_mpi X;
    int res;
    mbedtls_test_mpi_random rand;

    mbedtls_mpi_init( &X );
    rand.data = witnesses;
    rand.pos = 0;
    rand.chunk_len = chunk_len;

    TEST_ASSERT( mbedtls_mpi_read_binary( &X, input_X->x, input_X->len ) == 0 );
    res = mbedtls_mpi_is_prime_ext( &X, rounds - 1,
                                    mbedtls_test_mpi_miller_rabin_determinizer,
                                    &rand );
    TEST_ASSERT( res == 0 );

    rand.data = witnesses;
    rand.pos = 0;
    rand.chunk_len = chunk_len;

    res = mbedtls_mpi_is_prime_ext( &X, rounds,
                                    mbedtls_test_mpi_miller_rabin_determinizer,
                                    &rand );
    TEST_ASSERT( res == MBEDTLS_ERR_MPI_NOT_ACCEPTABLE );

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_is_prime_det_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_mbedtls_mpi_is_prime_det( &data0, &data2, *( (int *) params[4] ), *( (int *) params[5] ) );
}
#endif /* MBEDTLS_GENPRIME */
#if defined(MBEDTLS_GENPRIME)
void test_mbedtls_mpi_gen_prime( int bits, int flags, int ref_ret )
{
    mbedtls_mpi X;
    int my_ret;

    mbedtls_mpi_init( &X );

    my_ret = mbedtls_mpi_gen_prime( &X, bits, flags,
                                    mbedtls_test_rnd_std_rand, NULL );
    TEST_ASSERT( my_ret == ref_ret );

    if( ref_ret == 0 )
    {
        size_t actual_bits = mbedtls_mpi_bitlen( &X );

        TEST_ASSERT( actual_bits >= (size_t) bits );
        TEST_ASSERT( actual_bits <= (size_t) bits + 1 );

        TEST_ASSERT( mbedtls_mpi_is_prime_ext( &X, 40,
                                               mbedtls_test_rnd_std_rand,
                                               NULL ) == 0 );
        if( flags & MBEDTLS_MPI_GEN_PRIME_FLAG_DH )
        {
            /* X = ( X - 1 ) / 2 */
            TEST_ASSERT( mbedtls_mpi_shift_r( &X, 1 ) == 0 );
            TEST_ASSERT( mbedtls_mpi_is_prime_ext( &X, 40,
                                                   mbedtls_test_rnd_std_rand,
                                                   NULL ) == 0 );
        }
    }

exit:
    mbedtls_mpi_free( &X );
}

void test_mbedtls_mpi_gen_prime_wrapper( void ** params )
{

    test_mbedtls_mpi_gen_prime( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_GENPRIME */
void test_mbedtls_mpi_shift_l( int radix_X, char * input_X, int shift_X,
                          int radix_A, char * input_A )
{
    mbedtls_mpi X, A;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_shift_l( &X, shift_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &A ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_shift_l_wrapper( void ** params )
{

    test_mbedtls_mpi_shift_l( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4] );
}
void test_mbedtls_mpi_shift_r( int radix_X, char * input_X, int shift_X,
                          int radix_A, char * input_A )
{
    mbedtls_mpi X, A;
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &A );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, radix_X, input_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &A, radix_A, input_A ) == 0 );
    TEST_ASSERT( mbedtls_mpi_shift_r( &X, shift_X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &X, &A ) == 0 );

exit:
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &A );
}

void test_mbedtls_mpi_shift_r_wrapper( void ** params )
{

    test_mbedtls_mpi_shift_r( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4] );
}
void test_mpi_fill_random( int wanted_bytes, int rng_bytes, int expected_ret )
{
    mbedtls_mpi X;
    int ret;
    size_t bytes_left = rng_bytes;
    mbedtls_mpi_init( &X );

    ret = mbedtls_mpi_fill_random( &X, wanted_bytes,
                                   f_rng_bytes_left, &bytes_left );
    TEST_ASSERT( ret == expected_ret );

    if( expected_ret == 0 )
    {
        /* mbedtls_mpi_fill_random is documented to use bytes from the RNG
         * as a big-endian representation of the number. We know when
         * our RNG function returns null bytes, so we know how many
         * leading zero bytes the number has. */
        size_t leading_zeros = 0;
        if( wanted_bytes > 0 && rng_bytes % 256 == 0 )
            leading_zeros = 1;
        TEST_ASSERT( mbedtls_mpi_size( &X ) + leading_zeros ==
                     (size_t) wanted_bytes );
        TEST_ASSERT( (int) bytes_left == rng_bytes - wanted_bytes );
    }

exit:
    mbedtls_mpi_free( &X );
}

void test_mpi_fill_random_wrapper( void ** params )
{

    test_mpi_fill_random( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#if defined(MBEDTLS_SELF_TEST)
void test_mpi_selftest(  )
{
    TEST_ASSERT( mbedtls_mpi_self_test( 1 ) == 0 );
exit:
    ;
}

void test_mpi_selftest_wrapper( void ** params )
{
    (void)params;

    test_mpi_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_BIGNUM_C */

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

#if defined(MBEDTLS_BIGNUM_C)

        case 0:
            {
                *out_value = MBEDTLS_ERR_MPI_INVALID_CHARACTER;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_MPI_BUFFER_TOO_SMALL;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ERR_MPI_FILE_IO_ERROR;
            }
            break;
        case 4:
            {
                *out_value = -1;
            }
            break;
        case 5:
            {
                *out_value = -2;
            }
            break;
        case 6:
            {
                *out_value = -3;
            }
            break;
        case 7:
            {
                *out_value = -1500;
            }
            break;
        case 8:
            {
                *out_value = -42;
            }
            break;
        case 9:
            {
                *out_value = +1;
            }
            break;
        case 10:
            {
                *out_value = -9871232;
            }
            break;
        case 11:
            {
                *out_value = MBEDTLS_ERR_MPI_NEGATIVE_VALUE;
            }
            break;
        case 12:
            {
                *out_value = MBEDTLS_ERR_MPI_DIVISION_BY_ZERO;
            }
            break;
        case 13:
            {
                *out_value = -13;
            }
            break;
        case 14:
            {
                *out_value = -34;
            }
            break;
        case 15:
            {
                *out_value = MBEDTLS_MPI_MAX_SIZE;
            }
            break;
        case 16:
            {
                *out_value = MBEDTLS_MPI_MAX_SIZE + 1;
            }
            break;
        case 17:
            {
                *out_value = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
            }
            break;
        case 18:
            {
                *out_value = MBEDTLS_MPI_GEN_PRIME_FLAG_DH;
            }
            break;
        case 19:
            {
                *out_value = MBEDTLS_MPI_GEN_PRIME_FLAG_DH | MBEDTLS_MPI_GEN_PRIME_FLAG_LOW_ERR;
            }
            break;
        case 20:
            {
                *out_value = MBEDTLS_MPI_GEN_PRIME_FLAG_LOW_ERR;
            }
            break;
        case 21:
            {
                *out_value = MBEDTLS_MPI_MAX_SIZE - 7;
            }
            break;
        case 22:
            {
                *out_value = MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
            }
            break;
        case 23:
            {
                *out_value = MBEDTLS_MPI_MAX_SIZE-1;
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

#if defined(MBEDTLS_BIGNUM_C)

        case 0:
            {
#if defined(MPI_MAX_BITS_LARGER_THAN_792)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_GENPRIME)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
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

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_valid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_mpi_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_null_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_read_write_string_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_read_binary_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_read_binary_le_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_write_binary_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_write_binary_le_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO)
    test_mbedtls_mpi_read_file_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO)
    test_mbedtls_mpi_write_file_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_get_bit_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_set_bit_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_lsb_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_bitlen_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_gcd_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_cmp_int_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_cmp_mpi_wrapper,
#else
    NULL,
#endif
/* Function Id: 17 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_lt_mpi_ct_wrapper,
#else
    NULL,
#endif
/* Function Id: 18 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_cmp_abs_wrapper,
#else
    NULL,
#endif
/* Function Id: 19 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_copy_sint_wrapper,
#else
    NULL,
#endif
/* Function Id: 20 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_copy_binary_wrapper,
#else
    NULL,
#endif
/* Function Id: 21 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_copy_self_wrapper,
#else
    NULL,
#endif
/* Function Id: 22 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_shrink_wrapper,
#else
    NULL,
#endif
/* Function Id: 23 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_safe_cond_assign_wrapper,
#else
    NULL,
#endif
/* Function Id: 24 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_safe_cond_swap_wrapper,
#else
    NULL,
#endif
/* Function Id: 25 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_swap_sint_wrapper,
#else
    NULL,
#endif
/* Function Id: 26 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_swap_binary_wrapper,
#else
    NULL,
#endif
/* Function Id: 27 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_swap_self_wrapper,
#else
    NULL,
#endif
/* Function Id: 28 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_add_mpi_wrapper,
#else
    NULL,
#endif
/* Function Id: 29 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_add_mpi_inplace_wrapper,
#else
    NULL,
#endif
/* Function Id: 30 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_add_abs_wrapper,
#else
    NULL,
#endif
/* Function Id: 31 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_add_int_wrapper,
#else
    NULL,
#endif
/* Function Id: 32 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_sub_mpi_wrapper,
#else
    NULL,
#endif
/* Function Id: 33 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_sub_abs_wrapper,
#else
    NULL,
#endif
/* Function Id: 34 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_sub_int_wrapper,
#else
    NULL,
#endif
/* Function Id: 35 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_mul_mpi_wrapper,
#else
    NULL,
#endif
/* Function Id: 36 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_mul_int_wrapper,
#else
    NULL,
#endif
/* Function Id: 37 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_div_mpi_wrapper,
#else
    NULL,
#endif
/* Function Id: 38 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_div_int_wrapper,
#else
    NULL,
#endif
/* Function Id: 39 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_mod_mpi_wrapper,
#else
    NULL,
#endif
/* Function Id: 40 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_mod_int_wrapper,
#else
    NULL,
#endif
/* Function Id: 41 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_exp_mod_wrapper,
#else
    NULL,
#endif
/* Function Id: 42 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_exp_mod_size_wrapper,
#else
    NULL,
#endif
/* Function Id: 43 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_inv_mod_wrapper,
#else
    NULL,
#endif
/* Function Id: 44 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_mpi_is_prime_wrapper,
#else
    NULL,
#endif
/* Function Id: 45 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_mpi_is_prime_det_wrapper,
#else
    NULL,
#endif
/* Function Id: 46 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_GENPRIME)
    test_mbedtls_mpi_gen_prime_wrapper,
#else
    NULL,
#endif
/* Function Id: 47 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_shift_l_wrapper,
#else
    NULL,
#endif
/* Function Id: 48 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_mpi_shift_r_wrapper,
#else
    NULL,
#endif
/* Function Id: 49 */

#if defined(MBEDTLS_BIGNUM_C)
    test_mpi_fill_random_wrapper,
#else
    NULL,
#endif
/* Function Id: 50 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_SELF_TEST)
    test_mpi_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_mpi.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
