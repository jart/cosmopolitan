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
 * Test file      : ./test_suite_entropy.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_entropy.function
 *      Test suite data     : suites/test_suite_entropy.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_ENTROPY_C)
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/entropy_poll.h"
#include "third_party/mbedtls/md.h"

typedef enum
{
    DUMMY_CONSTANT_LENGTH, /* Output context->length bytes */
    DUMMY_REQUESTED_LENGTH, /* Output whatever length was requested */
    DUMMY_FAIL, /* Return an error code */
} entropy_dummy_instruction;

typedef struct
{
    entropy_dummy_instruction instruction;
    size_t length; /* Length to return for DUMMY_CONSTANT_LENGTH */
    size_t calls; /* Incremented at each call */
} entropy_dummy_context;

/*
 * Dummy entropy source
 *
 * If data is NULL, write exactly the requested length.
 * Otherwise, write the length indicated by data or error if negative
 */
static int entropy_dummy_source( void *arg, unsigned char *output,
                                 size_t len, size_t *olen )
{
    entropy_dummy_context *context = arg;
    ++context->calls;

    switch( context->instruction )
    {
        case DUMMY_CONSTANT_LENGTH:
            *olen = context->length;
            break;
        case DUMMY_REQUESTED_LENGTH:
            *olen = len;
            break;
        case DUMMY_FAIL:
            return( MBEDTLS_ERR_ENTROPY_SOURCE_FAILED );
    }

    memset( output, 0x2a, *olen );
    return( 0 );
}

/*
 * Ability to clear entropy sources to allow testing with just predefined
 * entropy sources. This function or tests depending on it might break if there
 * are internal changes to how entropy sources are registered.
 *
 * To be called immediately after mbedtls_entropy_init().
 *
 * Just resetting the counter. New sources will overwrite existing ones.
 * This might break memory checks in the future if sources need 'free-ing' then
 * as well.
 */
static void entropy_clear_sources( mbedtls_entropy_context *ctx )
{
    ctx->source_count = 0;
}

#if defined(MBEDTLS_ENTROPY_NV_SEED)
/*
 * NV seed read/write functions that use a buffer instead of a file
 */
static unsigned char buffer_seed[MBEDTLS_ENTROPY_BLOCK_SIZE];

int buffer_nv_seed_read( unsigned char *buf, size_t buf_len )
{
    if( buf_len != MBEDTLS_ENTROPY_BLOCK_SIZE )
        return( -1 );

    memcpy( buf, buffer_seed, MBEDTLS_ENTROPY_BLOCK_SIZE );
    return( 0 );
}

int buffer_nv_seed_write( unsigned char *buf, size_t buf_len )
{
    if( buf_len != MBEDTLS_ENTROPY_BLOCK_SIZE )
        return( -1 );

    memcpy( buffer_seed, buf, MBEDTLS_ENTROPY_BLOCK_SIZE );
    return( 0 );
}

/*
 * NV seed read/write helpers that fill the base seedfile
 */
static int write_nv_seed( unsigned char *buf, size_t buf_len )
{
    FILE *f;

    if( buf_len != MBEDTLS_ENTROPY_BLOCK_SIZE )
        return( -1 );

    if( ( f = fopen( MBEDTLS_PLATFORM_STD_NV_SEED_FILE, "w" ) ) == NULL )
        return( -1 );

    if( fwrite( buf, 1, MBEDTLS_ENTROPY_BLOCK_SIZE, f ) !=
                    MBEDTLS_ENTROPY_BLOCK_SIZE )
        return( -1 );

    fclose( f );

    return( 0 );
}

int read_nv_seed( unsigned char *buf, size_t buf_len )
{
    FILE *f;

    if( buf_len != MBEDTLS_ENTROPY_BLOCK_SIZE )
        return( -1 );

    if( ( f = fopen( MBEDTLS_PLATFORM_STD_NV_SEED_FILE, "rb" ) ) == NULL )
        return( -1 );

    if( fread( buf, 1, MBEDTLS_ENTROPY_BLOCK_SIZE, f ) !=
                    MBEDTLS_ENTROPY_BLOCK_SIZE )
        return( -1 );

    fclose( f );

    return( 0 );
}
#endif /* MBEDTLS_ENTROPY_NV_SEED */
void test_entropy_init_free( int reinit )
{
    mbedtls_entropy_context ctx;

    /* Double free is not explicitly documented to work, but it is convenient
     * to call mbedtls_entropy_free() unconditionally on an error path without
     * checking whether it has already been called in the success path. */

    mbedtls_entropy_init( &ctx );
    mbedtls_entropy_free( &ctx );

    if( reinit )
        mbedtls_entropy_init( &ctx );
    mbedtls_entropy_free( &ctx );

    /* This test case always succeeds, functionally speaking. A plausible
     * bug might trigger an invalid pointer dereference or a memory leak. */
    goto exit;
exit:
    ;
}

void test_entropy_init_free_wrapper( void ** params )
{
    test_entropy_init_free( *( (int *) params[0] ) );
}

#if defined(MBEDTLS_ENTROPY_NV_SEED)
#if defined(MBEDTLS_FS_IO)
void test_entropy_seed_file( char * path, int ret )
{
    mbedtls_entropy_context ctx;
    mbedtls_entropy_init( &ctx );
    TEST_ASSERT( mbedtls_entropy_write_seed_file( &ctx, path ) == ret );
    TEST_ASSERT( mbedtls_entropy_update_seed_file( &ctx, path ) == ret );
exit:
    mbedtls_entropy_free( &ctx );
}

void test_entropy_seed_file_wrapper( void ** params )
{

    test_entropy_seed_file( (char *) params[0], *( (int *) params[1] ) );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_ENTROPY_NV_SEED */
void test_entropy_no_sources( )
{
    mbedtls_entropy_context ctx;
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE];

    mbedtls_entropy_init( &ctx );
    entropy_clear_sources( &ctx );
    TEST_EQUAL( mbedtls_entropy_func( &ctx, buf, sizeof( buf ) ),
                MBEDTLS_ERR_ENTROPY_NO_SOURCES_DEFINED );

exit:
    mbedtls_entropy_free( &ctx );
}

void test_entropy_no_sources_wrapper( void ** params )
{
    (void)params;

    test_entropy_no_sources(  );
}
void test_entropy_too_many_sources(  )
{
    mbedtls_entropy_context ctx;
    size_t i;
    entropy_dummy_context dummy = {DUMMY_REQUESTED_LENGTH, 0, 0};

    mbedtls_entropy_init( &ctx );

    /*
     * It's hard to tell precisely when the error will occur,
     * since we don't know how many sources were automatically added.
     */
    for( i = 0; i < MBEDTLS_ENTROPY_MAX_SOURCES; i++ )
        (void) mbedtls_entropy_add_source( &ctx, entropy_dummy_source, &dummy,
                                           16, MBEDTLS_ENTROPY_SOURCE_WEAK );

    TEST_ASSERT( mbedtls_entropy_add_source( &ctx, entropy_dummy_source, &dummy,
                                             16, MBEDTLS_ENTROPY_SOURCE_WEAK )
                 == MBEDTLS_ERR_ENTROPY_MAX_SOURCES );

exit:
    mbedtls_entropy_free( &ctx );
}

void test_entropy_too_many_sources_wrapper( void ** params )
{
    (void)params;

    test_entropy_too_many_sources(  );
}
#if defined(ENTROPY_HAVE_STRONG)
void test_entropy_func_len( int len, int ret )
{
    mbedtls_entropy_context ctx;
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE + 10] = { 0 };
    unsigned char acc[MBEDTLS_ENTROPY_BLOCK_SIZE + 10] = { 0 };
    size_t i, j;

    mbedtls_entropy_init( &ctx );

    /*
     * See comments in mbedtls_entropy_self_test()
     */
    for( i = 0; i < 8; i++ )
    {
        TEST_ASSERT( mbedtls_entropy_func( &ctx, buf, len ) == ret );
        for( j = 0; j < sizeof( buf ); j++ )
            acc[j] |= buf[j];
    }

    if( ret == 0 )
        for( j = 0; j < (size_t) len; j++ )
            TEST_ASSERT( acc[j] != 0 );

    for( j = len; j < sizeof( buf ); j++ )
        TEST_ASSERT( acc[j] == 0 );

exit:
    mbedtls_entropy_free( &ctx );
}

void test_entropy_func_len_wrapper( void ** params )
{

    test_entropy_func_len( *( (int *) params[0] ), *( (int *) params[1] ) );
}
#endif /* ENTROPY_HAVE_STRONG */
void test_entropy_source_fail( char * path )
{
    mbedtls_entropy_context ctx;
    unsigned char buf[16];
    entropy_dummy_context dummy = {DUMMY_FAIL, 0, 0};

    mbedtls_entropy_init( &ctx );

    TEST_ASSERT( mbedtls_entropy_add_source( &ctx, entropy_dummy_source,
                                             &dummy, 16,
                                             MBEDTLS_ENTROPY_SOURCE_WEAK )
                 == 0 );

    TEST_ASSERT( mbedtls_entropy_func( &ctx, buf, sizeof( buf ) )
                 == MBEDTLS_ERR_ENTROPY_SOURCE_FAILED );
    TEST_ASSERT( mbedtls_entropy_gather( &ctx )
                 == MBEDTLS_ERR_ENTROPY_SOURCE_FAILED );
#if defined(MBEDTLS_FS_IO) && defined(MBEDTLS_ENTROPY_NV_SEED)
    TEST_ASSERT( mbedtls_entropy_write_seed_file( &ctx, path )
                 == MBEDTLS_ERR_ENTROPY_SOURCE_FAILED );
    TEST_ASSERT( mbedtls_entropy_update_seed_file( &ctx, path )
                 == MBEDTLS_ERR_ENTROPY_SOURCE_FAILED );
#else
    ((void) path);
#endif

exit:
    mbedtls_entropy_free( &ctx );
}

void test_entropy_source_fail_wrapper( void ** params )
{

    test_entropy_source_fail( (char *) params[0] );
}
void test_entropy_threshold( int threshold, int chunk_size, int result )
{
    mbedtls_entropy_context ctx;
    entropy_dummy_context strong =
        {DUMMY_CONSTANT_LENGTH, MBEDTLS_ENTROPY_BLOCK_SIZE, 0};
    entropy_dummy_context weak = {DUMMY_CONSTANT_LENGTH, chunk_size, 0};
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE] = { 0 };
    int ret;

    mbedtls_entropy_init( &ctx );
    entropy_clear_sources( &ctx );

    /* Set strong source that reaches its threshold immediately and
     * a weak source whose threshold is a test parameter. */
    TEST_ASSERT( mbedtls_entropy_add_source( &ctx, entropy_dummy_source,
                                     &strong, 1,
                                     MBEDTLS_ENTROPY_SOURCE_STRONG ) == 0 );
    TEST_ASSERT( mbedtls_entropy_add_source( &ctx, entropy_dummy_source,
                                     &weak, threshold,
                                     MBEDTLS_ENTROPY_SOURCE_WEAK ) == 0 );

    ret = mbedtls_entropy_func( &ctx, buf, sizeof( buf ) );

    if( result >= 0 )
    {
        TEST_ASSERT( ret == 0 );
#if defined(MBEDTLS_ENTROPY_NV_SEED)
        /* If the NV seed functionality is enabled, there are two entropy
         * updates: before and after updating the NV seed. */
        result *= 2;
#endif
        TEST_ASSERT( weak.calls == (size_t) result );
    }
    else
    {
        TEST_ASSERT( ret == result );
    }

exit:
    mbedtls_entropy_free( &ctx );
}

void test_entropy_threshold_wrapper( void ** params )
{

    test_entropy_threshold( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
void test_entropy_calls( int strength1, int strength2,
                    int threshold, int chunk_size,
                    int result )
{
    /*
     * if result >= 0: result = expected number of calls to source 1
     * if result < 0: result = expected return code from mbedtls_entropy_func()
     */

    mbedtls_entropy_context ctx;
    entropy_dummy_context dummy1 = {DUMMY_CONSTANT_LENGTH, chunk_size, 0};
    entropy_dummy_context dummy2 = {DUMMY_CONSTANT_LENGTH, chunk_size, 0};
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE] = { 0 };
    int ret;

    mbedtls_entropy_init( &ctx );
    entropy_clear_sources( &ctx );

    TEST_ASSERT( mbedtls_entropy_add_source( &ctx, entropy_dummy_source,
                                             &dummy1, threshold,
                                             strength1 ) == 0 );
    TEST_ASSERT( mbedtls_entropy_add_source( &ctx, entropy_dummy_source,
                                             &dummy2, threshold,
                                             strength2 ) == 0 );

    ret = mbedtls_entropy_func( &ctx, buf, sizeof( buf ) );

    if( result >= 0 )
    {
        TEST_ASSERT( ret == 0 );
#if defined(MBEDTLS_ENTROPY_NV_SEED)
        /* If the NV seed functionality is enabled, there are two entropy
         * updates: before and after updating the NV seed. */
        result *= 2;
#endif
        TEST_ASSERT( dummy1.calls == (size_t) result );
    }
    else
    {
        TEST_ASSERT( ret == result );
    }

exit:
    mbedtls_entropy_free( &ctx );
}

void test_entropy_calls_wrapper( void ** params )
{

    test_entropy_calls( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#if defined(MBEDTLS_ENTROPY_NV_SEED)
#if defined(MBEDTLS_FS_IO)
void test_nv_seed_file_create(  )
{
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE];

    memset( buf, 0, MBEDTLS_ENTROPY_BLOCK_SIZE );

    TEST_ASSERT( write_nv_seed( buf, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );
exit:
    ;
}

void test_nv_seed_file_create_wrapper( void ** params )
{
    (void)params;

    test_nv_seed_file_create(  );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_ENTROPY_NV_SEED */
#if defined(MBEDTLS_ENTROPY_NV_SEED)
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
void test_entropy_nv_seed_std_io(  )
{
    unsigned char io_seed[MBEDTLS_ENTROPY_BLOCK_SIZE];
    unsigned char check_seed[MBEDTLS_ENTROPY_BLOCK_SIZE];

    memset( io_seed, 1, MBEDTLS_ENTROPY_BLOCK_SIZE );
    memset( check_seed, 0, MBEDTLS_ENTROPY_BLOCK_SIZE );

    mbedtls_platform_set_nv_seed( mbedtls_platform_std_nv_seed_read,
                                  mbedtls_platform_std_nv_seed_write );

    /* Check if platform NV read and write manipulate the same data */
    TEST_ASSERT( write_nv_seed( io_seed, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );
    TEST_ASSERT( mbedtls_nv_seed_read( check_seed, MBEDTLS_ENTROPY_BLOCK_SIZE ) ==
                    MBEDTLS_ENTROPY_BLOCK_SIZE );

    TEST_ASSERT( memcmp( io_seed, check_seed, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );

    memset( check_seed, 0, MBEDTLS_ENTROPY_BLOCK_SIZE );

    /* Check if platform NV write and raw read manipulate the same data */
    TEST_ASSERT( mbedtls_nv_seed_write( io_seed, MBEDTLS_ENTROPY_BLOCK_SIZE ) ==
                    MBEDTLS_ENTROPY_BLOCK_SIZE );
    TEST_ASSERT( read_nv_seed( check_seed, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );

    TEST_ASSERT( memcmp( io_seed, check_seed, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );
exit:
    ;
}

void test_entropy_nv_seed_std_io_wrapper( void ** params )
{
    (void)params;

    test_entropy_nv_seed_std_io(  );
}
#endif /* MBEDTLS_PLATFORM_NV_SEED_ALT */
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_ENTROPY_NV_SEED */
#if defined(MBEDTLS_MD_C)
#if defined(MBEDTLS_ENTROPY_NV_SEED)
#if defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
void test_entropy_nv_seed( data_t * read_seed )
{
#if defined(MBEDTLS_ENTROPY_SHA512_ACCUMULATOR)
    const mbedtls_md_info_t *md_info =
        mbedtls_md_info_from_type( MBEDTLS_MD_SHA512 );
#elif defined(MBEDTLS_ENTROPY_SHA256_ACCUMULATOR)
    const mbedtls_md_info_t *md_info =
        mbedtls_md_info_from_type( MBEDTLS_MD_SHA256 );
#else
#error "Unsupported entropy accumulator"
#endif
    mbedtls_md_context_t accumulator;
    mbedtls_entropy_context ctx;
    int (*original_mbedtls_nv_seed_read)( unsigned char *buf, size_t buf_len ) =
        mbedtls_nv_seed_read;
    int (*original_mbedtls_nv_seed_write)( unsigned char *buf, size_t buf_len ) =
        mbedtls_nv_seed_write;

    unsigned char header[2];
    unsigned char entropy[MBEDTLS_ENTROPY_BLOCK_SIZE];
    unsigned char buf[MBEDTLS_ENTROPY_BLOCK_SIZE];
    unsigned char empty[MBEDTLS_ENTROPY_BLOCK_SIZE];
    unsigned char check_seed[MBEDTLS_ENTROPY_BLOCK_SIZE];
    unsigned char check_entropy[MBEDTLS_ENTROPY_BLOCK_SIZE];

    memset( entropy, 0, MBEDTLS_ENTROPY_BLOCK_SIZE );
    memset( buf, 0, MBEDTLS_ENTROPY_BLOCK_SIZE );
    memset( empty, 0, MBEDTLS_ENTROPY_BLOCK_SIZE );
    memset( check_seed, 2, MBEDTLS_ENTROPY_BLOCK_SIZE );
    memset( check_entropy, 3, MBEDTLS_ENTROPY_BLOCK_SIZE );

    // Make sure we read/write NV seed from our buffers
    mbedtls_platform_set_nv_seed( buffer_nv_seed_read, buffer_nv_seed_write );

    mbedtls_md_init( &accumulator );
    mbedtls_entropy_init( &ctx );
    entropy_clear_sources( &ctx );

    TEST_ASSERT( mbedtls_entropy_add_source( &ctx, mbedtls_nv_seed_poll, NULL,
                                             MBEDTLS_ENTROPY_BLOCK_SIZE,
                                             MBEDTLS_ENTROPY_SOURCE_STRONG ) == 0 );

    // Set the initial NV seed to read
    TEST_ASSERT( read_seed->len >= MBEDTLS_ENTROPY_BLOCK_SIZE );
    memcpy( buffer_seed, read_seed->x, MBEDTLS_ENTROPY_BLOCK_SIZE );

    // Do an entropy run
    TEST_ASSERT( mbedtls_entropy_func( &ctx, entropy, sizeof( entropy ) ) == 0 );
    // Determine what should have happened with manual entropy internal logic

    // Init accumulator
    header[1] = MBEDTLS_ENTROPY_BLOCK_SIZE;
    TEST_ASSERT( mbedtls_md_setup( &accumulator, md_info, 0 ) == 0 );

    // First run for updating write_seed
    header[0] = 0;
    TEST_ASSERT( mbedtls_md_starts( &accumulator ) == 0 );
    TEST_ASSERT( mbedtls_md_update( &accumulator, header, 2 ) == 0 );
    TEST_ASSERT( mbedtls_md_update( &accumulator,
                                    read_seed->x, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );
    TEST_ASSERT( mbedtls_md_finish( &accumulator, buf ) == 0 );

    TEST_ASSERT( mbedtls_md_starts( &accumulator ) == 0 );
    TEST_ASSERT( mbedtls_md_update( &accumulator,
                                    buf, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );

    TEST_ASSERT( mbedtls_md( md_info, buf, MBEDTLS_ENTROPY_BLOCK_SIZE,
                             check_seed ) == 0 );

    // Second run for actual entropy (triggers mbedtls_entropy_update_nv_seed)
    header[0] = MBEDTLS_ENTROPY_SOURCE_MANUAL;
    TEST_ASSERT( mbedtls_md_update( &accumulator, header, 2 ) == 0 );
    TEST_ASSERT( mbedtls_md_update( &accumulator,
                                    empty, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );

    header[0] = 0;
    TEST_ASSERT( mbedtls_md_update( &accumulator, header, 2 ) == 0 );
    TEST_ASSERT( mbedtls_md_update( &accumulator,
                                    check_seed, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );
    TEST_ASSERT( mbedtls_md_finish( &accumulator, buf ) == 0 );

    TEST_ASSERT( mbedtls_md( md_info, buf, MBEDTLS_ENTROPY_BLOCK_SIZE,
                             check_entropy ) == 0 );

    // Check result of both NV file and entropy received with the manual calculations
    TEST_ASSERT( memcmp( check_seed, buffer_seed, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );
    TEST_ASSERT( memcmp( check_entropy, entropy, MBEDTLS_ENTROPY_BLOCK_SIZE ) == 0 );

exit:
    mbedtls_md_free( &accumulator );
    mbedtls_entropy_free( &ctx );
    mbedtls_nv_seed_read = original_mbedtls_nv_seed_read;
    mbedtls_nv_seed_write = original_mbedtls_nv_seed_write;
}

void test_entropy_nv_seed_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_entropy_nv_seed( &data0 );
}
#endif /* MBEDTLS_PLATFORM_NV_SEED_ALT */
#endif /* MBEDTLS_ENTROPY_NV_SEED */
#endif /* MBEDTLS_MD_C */
#if defined(ENTROPY_HAVE_STRONG)
#if defined(MBEDTLS_SELF_TEST)
void test_entropy_selftest( int result )
{
    TEST_ASSERT( mbedtls_entropy_self_test( 1 ) == result );
exit:
    ;
}

void test_entropy_selftest_wrapper( void ** params )
{

    test_entropy_selftest( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* ENTROPY_HAVE_STRONG */
#endif /* MBEDTLS_ENTROPY_C */

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

#if defined(MBEDTLS_ENTROPY_C)

        case 0:
            {
                *out_value = MBEDTLS_ERR_ENTROPY_FILE_IO_ERROR;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ENTROPY_SOURCE_WEAK;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ENTROPY_BLOCK_SIZE;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ERR_ENTROPY_NO_STRONG_SOURCE;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ENTROPY_SOURCE_STRONG;
            }
            break;
        case 6:
            {
                *out_value = (MBEDTLS_ENTROPY_BLOCK_SIZE+1)/2;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_ENTROPY_BLOCK_SIZE+1;
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

#if defined(MBEDTLS_ENTROPY_C)

        case 0:
            {
#if !defined(MBEDTLS_TEST_NULL_ENTROPY)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_TEST_NULL_ENTROPY)
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

#if defined(MBEDTLS_ENTROPY_C)
    test_entropy_init_free_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_ENTROPY_NV_SEED) && defined(MBEDTLS_FS_IO)
    test_entropy_seed_file_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_ENTROPY_C)
    test_entropy_no_sources_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_ENTROPY_C)
    test_entropy_too_many_sources_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_ENTROPY_C) && defined(ENTROPY_HAVE_STRONG)
    test_entropy_func_len_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_ENTROPY_C)
    test_entropy_source_fail_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_ENTROPY_C)
    test_entropy_threshold_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_ENTROPY_C)
    test_entropy_calls_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_ENTROPY_NV_SEED) && defined(MBEDTLS_FS_IO)
    test_nv_seed_file_create_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_ENTROPY_NV_SEED) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
    test_entropy_nv_seed_std_io_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_MD_C) && defined(MBEDTLS_ENTROPY_NV_SEED) && defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
    test_entropy_nv_seed_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_ENTROPY_C) && defined(ENTROPY_HAVE_STRONG) && defined(MBEDTLS_SELF_TEST)
    test_entropy_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_entropy.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
