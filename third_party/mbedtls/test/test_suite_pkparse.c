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
 * Test file      : ./test_suite_pkparse.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_pkparse.function
 *      Test suite data     : suites/test_suite_pkparse.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_PK_PARSE_C)
#if defined(MBEDTLS_BIGNUM_C)
#include "third_party/mbedtls/pk.h"
#include "third_party/mbedtls/pem.h"
#include "third_party/mbedtls/oid.h"
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_FS_IO)
void test_pk_parse_keyfile_rsa( char * key_file, char * password, int result )
{
    mbedtls_pk_context ctx;
    int res;
    char *pwd = password;

    mbedtls_pk_init( &ctx );

    if( strcmp( pwd, "NULL" ) == 0 )
        pwd = NULL;

    res = mbedtls_pk_parse_keyfile( &ctx, key_file, pwd );

    TEST_ASSERT( res == result );

    if( res == 0 )
    {
        mbedtls_rsa_context *rsa;
        TEST_ASSERT( mbedtls_pk_can_do( &ctx, MBEDTLS_PK_RSA ) );
        rsa = mbedtls_pk_rsa( ctx );
        TEST_ASSERT( mbedtls_rsa_check_privkey( rsa ) == 0 );
    }

exit:
    mbedtls_pk_free( &ctx );
}

void test_pk_parse_keyfile_rsa_wrapper( void ** params )
{

    test_pk_parse_keyfile_rsa( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_FS_IO)
void test_pk_parse_public_keyfile_rsa( char * key_file, int result )
{
    mbedtls_pk_context ctx;
    int res;

    mbedtls_pk_init( &ctx );

    res = mbedtls_pk_parse_public_keyfile( &ctx, key_file );

    TEST_ASSERT( res == result );

    if( res == 0 )
    {
        mbedtls_rsa_context *rsa;
        TEST_ASSERT( mbedtls_pk_can_do( &ctx, MBEDTLS_PK_RSA ) );
        rsa = mbedtls_pk_rsa( ctx );
        TEST_ASSERT( mbedtls_rsa_check_pubkey( rsa ) == 0 );
    }

exit:
    mbedtls_pk_free( &ctx );
}

void test_pk_parse_public_keyfile_rsa_wrapper( void ** params )
{

    test_pk_parse_public_keyfile_rsa( (char *) params[0], *( (int *) params[1] ) );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_ECP_C)
void test_pk_parse_public_keyfile_ec( char * key_file, int result )
{
    mbedtls_pk_context ctx;
    int res;

    mbedtls_pk_init( &ctx );

    res = mbedtls_pk_parse_public_keyfile( &ctx, key_file );

    TEST_ASSERT( res == result );

    if( res == 0 )
    {
        mbedtls_ecp_keypair *eckey;
        TEST_ASSERT( mbedtls_pk_can_do( &ctx, MBEDTLS_PK_ECKEY ) );
        eckey = mbedtls_pk_ec( ctx );
        TEST_ASSERT( mbedtls_ecp_check_pubkey( &eckey->grp, &eckey->Q ) == 0 );
    }

exit:
    mbedtls_pk_free( &ctx );
}

void test_pk_parse_public_keyfile_ec_wrapper( void ** params )
{

    test_pk_parse_public_keyfile_ec( (char *) params[0], *( (int *) params[1] ) );
}
#endif /* MBEDTLS_ECP_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_ECP_C)
void test_pk_parse_keyfile_ec( char * key_file, char * password, int result )
{
    mbedtls_pk_context ctx;
    int res;

    mbedtls_pk_init( &ctx );

    res = mbedtls_pk_parse_keyfile( &ctx, key_file, password );

    TEST_ASSERT( res == result );

    if( res == 0 )
    {
        mbedtls_ecp_keypair *eckey;
        TEST_ASSERT( mbedtls_pk_can_do( &ctx, MBEDTLS_PK_ECKEY ) );
        eckey = mbedtls_pk_ec( ctx );
        TEST_ASSERT( mbedtls_ecp_check_privkey( &eckey->grp, &eckey->d ) == 0 );
    }

exit:
    mbedtls_pk_free( &ctx );
}

void test_pk_parse_keyfile_ec_wrapper( void ** params )
{

    test_pk_parse_keyfile_ec( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#endif /* MBEDTLS_ECP_C */
#endif /* MBEDTLS_FS_IO */
void test_pk_parse_key( data_t * buf, int result )
{
    mbedtls_pk_context pk;

    mbedtls_pk_init( &pk );

    TEST_ASSERT( mbedtls_pk_parse_key( &pk, buf->x, buf->len, NULL, 0 ) == result );

exit:
    mbedtls_pk_free( &pk );
}

void test_pk_parse_key_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_pk_parse_key( &data0, *( (int *) params[2] ) );
}
#endif /* MBEDTLS_BIGNUM_C */
#endif /* MBEDTLS_PK_PARSE_C */

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

#if defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_BIGNUM_C)

        case 0:
            {
                *out_value = MBEDTLS_ERR_PK_PASSWORD_REQUIRED;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ERR_PK_PASSWORD_MISMATCH;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_PK_KEY_INVALID_FORMAT;
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

#if defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_BIGNUM_C)

        case 0:
            {
#if defined(MBEDTLS_MD5_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_PEM_PARSE_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_CIPHER_MODE_CBC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_DES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_AES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_SHA1_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_PKCS12_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_CIPHER_PADDING_PKCS7)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(MBEDTLS_ARC4_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(MBEDTLS_PKCS5_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(MBEDTLS_SHA256_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(MBEDTLS_SHA512_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if !defined(MBEDTLS_SHA512_NO_SHA384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 13:
            {
#if defined(MBEDTLS_ECP_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 14:
            {
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 15:
            {
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 16:
            {
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 17:
            {
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 18:
            {
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 19:
            {
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 20:
            {
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 21:
            {
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 22:
            {
#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 23:
            {
#if defined(MBEDTLS_PK_PARSE_EC_EXTENDED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 24:
            {
#if defined(MBEDTLS_RSA_C)
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

#if defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_FS_IO)
    test_pk_parse_keyfile_rsa_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_FS_IO)
    test_pk_parse_public_keyfile_rsa_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_ECP_C)
    test_pk_parse_public_keyfile_ec_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_ECP_C)
    test_pk_parse_keyfile_ec_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_BIGNUM_C)
    test_pk_parse_key_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_pkparse.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
