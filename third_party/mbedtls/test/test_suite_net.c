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
 * Test file      : ./test_suite_net.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_net.function
 *      Test suite data     : suites/test_suite_net.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_NET_C)


#if defined(unix) || defined(__unix__) || defined(__unix) || \
    defined(__APPLE__) || defined(__QNXNTO__) || \
    defined(__HAIKU__) || defined(__midipix__)
#define MBEDTLS_PLATFORM_IS_UNIXLIKE
#endif

#if defined(MBEDTLS_PLATFORM_IS_UNIXLIKE)
#endif


#if defined(MBEDTLS_PLATFORM_IS_UNIXLIKE)
/** Open a file on the given file descriptor.
 *
 * This is disruptive if there is already something open on that descriptor.
 * Caller beware.
 *
 * \param ctx           An initialized, but unopened socket context.
 *                      On success, it refers to the opened file (\p wanted_fd).
 * \param wanted_fd     The desired file descriptor.
 *
 * \return              \c 0 on succes, a negative error code on error.
 */
static int open_file_on_fd( mbedtls_net_context *ctx, int wanted_fd )
{
    int got_fd = open( "/dev/null", O_RDONLY );
    TEST_ASSERT( got_fd >= 0 );
    if( got_fd != wanted_fd )
    {
        TEST_ASSERT( dup2( got_fd, wanted_fd ) >= 0 );
        TEST_ASSERT( close( got_fd ) >= 0 );
    }
    ctx->fd = wanted_fd;
    return( 0 );
exit:
    return( -1 );
}
#endif /* MBEDTLS_PLATFORM_IS_UNIXLIKE */

void test_context_init_free( int reinit )
{
    mbedtls_net_context ctx;

    mbedtls_net_init( &ctx );
    mbedtls_net_free( &ctx );

    if( reinit )
        mbedtls_net_init( &ctx );
    mbedtls_net_free( &ctx );

    /* This test case always succeeds, functionally speaking. A plausible
     * bug might trigger an invalid pointer dereference or a memory leak. */
    goto exit;
exit:
    ;
}

void test_context_init_free_wrapper( void ** params )
{

    test_context_init_free( *( (int *) params[0] ) );
}
#if defined(MBEDTLS_PLATFORM_IS_UNIXLIKE)
void test_poll_beyond_fd_setsize( )
{
    /* Test that mbedtls_net_poll does not misbehave when given a file
     * descriptor greater or equal to FD_SETSIZE. This code is specific to
     * platforms with a Unix-like select() function, which is where
     * FD_SETSIZE is a concern. */

    struct rlimit rlim_nofile;
    int restore_rlim_nofile = 0;
    int ret;
    mbedtls_net_context ctx;
    uint8_t buf[1];

    mbedtls_net_init( &ctx );

    /* On many systems, by default, the maximum permitted file descriptor
     * number is less than FD_SETSIZE. If so, raise the limit if
     * possible.
     *
     * If the limit can't be raised, a file descriptor opened by the
     * net_sockets module will be less than FD_SETSIZE, so the test
     * is not necessary and we mark it as skipped.
     * A file descriptor could still be higher than FD_SETSIZE if it was
     * opened before the limit was lowered (which is something an application
     * might do); but we don't do such things in our test code, so the unit
     * test will run if it can.
     */
    TEST_ASSERT( getrlimit( RLIMIT_NOFILE, &rlim_nofile ) == 0 );
    if( rlim_nofile.rlim_cur < FD_SETSIZE + 1 )
    {
        rlim_t old_rlim_cur = rlim_nofile.rlim_cur;
        rlim_nofile.rlim_cur = FD_SETSIZE + 1;
        TEST_ASSUME( setrlimit( RLIMIT_NOFILE, &rlim_nofile ) == 0 );
        rlim_nofile.rlim_cur = old_rlim_cur;
        restore_rlim_nofile = 1;
    }

    TEST_ASSERT( open_file_on_fd( &ctx, FD_SETSIZE ) == 0 );

    /* In principle, mbedtls_net_poll() with valid arguments should succeed.
     * However, we know that on Unix-like platforms (and others), this function
     * is implemented on top of select() and fd_set, which do not support
     * file descriptors greater or equal to FD_SETSIZE. So we expect to hit
     * this platform limitation.
     *
     * If mbedtls_net_poll() does not proprely check that ctx.fd is in range,
     * it may still happen to return the expected failure code, but if this
     * is problematic on the particular platform where the code is running,
     * a memory sanitizer such as UBSan should catch it.
     */
    ret = mbedtls_net_poll( &ctx, MBEDTLS_NET_POLL_READ, 0 );
    TEST_EQUAL( ret, MBEDTLS_ERR_NET_POLL_FAILED );

    /* mbedtls_net_recv_timeout() uses select() and fd_set in the same way. */
    ret = mbedtls_net_recv_timeout( &ctx, buf, sizeof( buf ), 0 );
    TEST_EQUAL( ret, MBEDTLS_ERR_NET_POLL_FAILED );

exit:
    mbedtls_net_free( &ctx );
    if( restore_rlim_nofile )
        setrlimit( RLIMIT_NOFILE, &rlim_nofile );
}

void test_poll_beyond_fd_setsize_wrapper( void ** params )
{
    (void)params;

    test_poll_beyond_fd_setsize(  );
}
#endif /* MBEDTLS_PLATFORM_IS_UNIXLIKE */
#endif /* MBEDTLS_NET_C */

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

#if defined(MBEDTLS_NET_C)

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

#if defined(MBEDTLS_NET_C)

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

#if defined(MBEDTLS_NET_C)
    test_context_init_free_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_NET_C) && defined(MBEDTLS_PLATFORM_IS_UNIXLIKE)
    test_poll_beyond_fd_setsize_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_net.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
