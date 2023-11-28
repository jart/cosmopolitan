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
 * Test file      : ./test_suite_memory_buffer_alloc.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_memory_buffer_alloc.function
 *      Test suite data     : suites/test_suite_memory_buffer_alloc.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#include "third_party/mbedtls/memory_buffer_alloc.h"
#define TEST_SUITE_MEMORY_BUFFER_ALLOC

static int check_pointer( void *p )
{
    if( p == NULL )
        return( -1 );

    if( (size_t) p % MBEDTLS_MEMORY_ALIGN_MULTIPLE != 0 )
        return( -1 );

    return( 0 );
}
#if defined(MBEDTLS_SELF_TEST)
void test_mbedtls_memory_buffer_alloc_self_test(  )
{
    TEST_ASSERT( mbedtls_memory_buffer_alloc_self_test( 1 ) == 0 );
exit:
    ;
}

void test_mbedtls_memory_buffer_alloc_self_test_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_memory_buffer_alloc_self_test(  );
}
#endif /* MBEDTLS_SELF_TEST */
void test_memory_buffer_alloc_free_alloc( int a_bytes, int b_bytes, int c_bytes,
                                     int d_bytes, int free_a, int free_b,
                                     int free_c, int free_d, int e_bytes,
                                     int f_bytes )
{
    unsigned char buf[1024];
    unsigned char *ptr_a = NULL, *ptr_b = NULL, *ptr_c = NULL, *ptr_d = NULL,
                    *ptr_e = NULL, *ptr_f = NULL;

#if defined(MBEDTLS_MEMORY_DEBUG)
    size_t reported_blocks;
    size_t reported_bytes;
#endif
    size_t allocated_bytes = 0;

    mbedtls_memory_buffer_alloc_init( buf, sizeof( buf ) );

    mbedtls_memory_buffer_set_verify( MBEDTLS_MEMORY_VERIFY_ALWAYS );

    if( a_bytes > 0 )
    {
        ptr_a = mbedtls_calloc( a_bytes, sizeof(char) );
        TEST_ASSERT( check_pointer( ptr_a ) == 0 );

        allocated_bytes += a_bytes * sizeof(char);
    }

    if( b_bytes > 0 )
    {
        ptr_b = mbedtls_calloc( b_bytes, sizeof(char) );
        TEST_ASSERT( check_pointer( ptr_b ) == 0 );

        allocated_bytes += b_bytes * sizeof(char);
    }

    if( c_bytes > 0 )
    {
        ptr_c = mbedtls_calloc( c_bytes, sizeof(char) );
        TEST_ASSERT( check_pointer( ptr_c ) == 0 );

        allocated_bytes += c_bytes * sizeof(char);
    }

    if( d_bytes > 0 )
    {
        ptr_d = mbedtls_calloc( d_bytes, sizeof(char) );
        TEST_ASSERT( check_pointer( ptr_d ) == 0 );

        allocated_bytes += d_bytes * sizeof(char);
    }

#if defined(MBEDTLS_MEMORY_DEBUG)
    mbedtls_memory_buffer_alloc_cur_get( &reported_bytes, &reported_blocks );
    TEST_ASSERT( reported_bytes == allocated_bytes );
#endif

    if( free_a )
    {
        mbedtls_free( ptr_a );
        ptr_a = NULL;
        TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );

        allocated_bytes -= a_bytes * sizeof(char);
    }

    if( free_b )
    {
        mbedtls_free( ptr_b );
        ptr_b = NULL;
        TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );

        allocated_bytes -= b_bytes * sizeof(char);
    }

    if( free_c )
    {
        mbedtls_free( ptr_c );
        ptr_c = NULL;
        TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );

        allocated_bytes -= c_bytes * sizeof(char);
    }

    if( free_d )
    {
        mbedtls_free( ptr_d );
        ptr_d = NULL;
        TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );

        allocated_bytes -= d_bytes * sizeof(char);
    }

#if defined(MBEDTLS_MEMORY_DEBUG)
    mbedtls_memory_buffer_alloc_cur_get( &reported_bytes, &reported_blocks );
    TEST_ASSERT( reported_bytes == allocated_bytes );
#endif

    if( e_bytes > 0 )
    {
        ptr_e = mbedtls_calloc( e_bytes, sizeof(char) );
        TEST_ASSERT( check_pointer( ptr_e ) == 0 );
    }

    if( f_bytes > 0 )
    {
        ptr_f = mbedtls_calloc( f_bytes, sizeof(char) );
        TEST_ASSERT( check_pointer( ptr_f ) == 0 );
    }

    /* Once blocks are reallocated, the block allocated to the memory request
     * may be bigger than the request itself, which is indicated by the reported
     * bytes, and makes it hard to know what the reported size will be, so
     * we don't check the size after blocks have been reallocated. */

    if( ptr_a != NULL )
    {
        mbedtls_free( ptr_a );
        ptr_a = NULL;
        TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );
    }

    if( ptr_b != NULL )
    {
        mbedtls_free( ptr_b );
        ptr_b = NULL;
        TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );
    }

    if( ptr_c != NULL )
    {
        mbedtls_free( ptr_c );
        ptr_c = NULL;
        TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );
    }

    if( ptr_d != NULL )
    {
        mbedtls_free( ptr_d );
        ptr_d = NULL;
        TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );
    }

    if( ptr_e != NULL )
    {
        mbedtls_free( ptr_e );
        ptr_e = NULL;
        TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );
    }

    if( ptr_f != NULL )
    {
        mbedtls_free( ptr_f );
        ptr_f = NULL;
    }

#if defined(MBEDTLS_MEMORY_DEBUG)
    mbedtls_memory_buffer_alloc_cur_get( &reported_bytes, &reported_blocks );
    TEST_ASSERT( reported_bytes == 0 );
#endif

    TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );

exit:
    mbedtls_memory_buffer_alloc_free( );
}

void test_memory_buffer_alloc_free_alloc_wrapper( void ** params )
{

    test_memory_buffer_alloc_free_alloc( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ), *( (int *) params[9] ) );
}
void test_memory_buffer_alloc_oom_test(  )
{
    unsigned char buf[1024];
    unsigned char *ptr_a = NULL, *ptr_b = NULL, *ptr_c = NULL;
#if defined(MBEDTLS_MEMORY_DEBUG)
    size_t reported_blocks, reported_bytes;
#endif

    (void)ptr_c;

    mbedtls_memory_buffer_alloc_init( buf, sizeof( buf ) );

    mbedtls_memory_buffer_set_verify( MBEDTLS_MEMORY_VERIFY_ALWAYS );

    ptr_a = mbedtls_calloc( 432, sizeof(char) );
    TEST_ASSERT( check_pointer( ptr_a ) == 0 );

    ptr_b = mbedtls_calloc( 432, sizeof(char) );
    TEST_ASSERT( check_pointer( ptr_b ) == 0 );

    ptr_c = mbedtls_calloc( 431, sizeof(char) );
    TEST_ASSERT( ptr_c == NULL );

#if defined(MBEDTLS_MEMORY_DEBUG)
    mbedtls_memory_buffer_alloc_cur_get( &reported_bytes, &reported_blocks );
    TEST_ASSERT( reported_bytes >= 864 && reported_bytes <= sizeof(buf) );
#endif

    mbedtls_free( ptr_a );
    ptr_a = NULL;
    TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );

    mbedtls_free( ptr_b );
    ptr_b = NULL;
    TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );

#if defined(MBEDTLS_MEMORY_DEBUG)
    mbedtls_memory_buffer_alloc_cur_get( &reported_bytes, &reported_blocks );
    TEST_ASSERT( reported_bytes == 0 );
#endif

    TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );

exit:
    mbedtls_memory_buffer_alloc_free( );
}

void test_memory_buffer_alloc_oom_test_wrapper( void ** params )
{
    (void)params;

    test_memory_buffer_alloc_oom_test(  );
}
void test_memory_buffer_heap_too_small( )
{
    unsigned char buf[1];

    mbedtls_memory_buffer_alloc_init( buf, sizeof( buf ) );
    /* With MBEDTLS_MEMORY_DEBUG enabled, this prints a message
     * "FATAL: verification of first header failed".
     */
    TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() != 0 );
exit:
    ;
}

void test_memory_buffer_heap_too_small_wrapper( void ** params )
{
    (void)params;

    test_memory_buffer_heap_too_small(  );
}
void test_memory_buffer_underalloc( )
{
    unsigned char buf[100];
    size_t i;

    mbedtls_memory_buffer_alloc_init( buf, sizeof( buf ) );
    for( i = 1; i < MBEDTLS_MEMORY_ALIGN_MULTIPLE; i++ )
    {
        TEST_ASSERT( mbedtls_calloc( 1,
                     (size_t)-( MBEDTLS_MEMORY_ALIGN_MULTIPLE - i ) ) == NULL );
        TEST_ASSERT( mbedtls_memory_buffer_alloc_verify() == 0 );
    }

exit:
    mbedtls_memory_buffer_alloc_free();
}

void test_memory_buffer_underalloc_wrapper( void ** params )
{
    (void)params;

    test_memory_buffer_underalloc(  );
}
#endif /* MBEDTLS_MEMORY_BUFFER_ALLOC_C */

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

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)

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

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)

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

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C) && defined(MBEDTLS_SELF_TEST)
    test_mbedtls_memory_buffer_alloc_self_test_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    test_memory_buffer_alloc_free_alloc_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    test_memory_buffer_alloc_oom_test_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    test_memory_buffer_heap_too_small_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    test_memory_buffer_underalloc_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_memory_buffer_alloc.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
