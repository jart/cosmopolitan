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
 * Test file      : ./test_suite_asn1write.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_asn1write.function
 *      Test suite data     : suites/test_suite_asn1write.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_ASN1_WRITE_C)
#include "third_party/mbedtls/asn1write.h"

#define GUARD_LEN 4
#define GUARD_VAL 0x2a

typedef struct
{
    unsigned char *output;
    unsigned char *start;
    unsigned char *end;
    unsigned char *p;
    size_t size;
} generic_write_data_t;

int generic_write_start_step( generic_write_data_t *data )
{
    mbedtls_test_set_step( data->size );
    ASSERT_ALLOC( data->output, data->size == 0 ? 1 : data->size );
    data->end = data->output + data->size;
    data->p = data->end;
    data->start = data->end - data->size;
    return( 1 );
exit:
    return( 0 );
}

int generic_write_finish_step( generic_write_data_t *data,
                               const data_t *expected, int ret )
{
    int ok = 0;

    if( data->size < expected->len )
    {
        TEST_EQUAL( ret, MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );
    }
    else
    {
        TEST_EQUAL( ret, data->end - data->p );
        TEST_ASSERT( data->p >= data->start );
        TEST_ASSERT( data->p <= data->end );
        ASSERT_COMPARE( data->p, (size_t)( data->end - data->p ),
                        expected->x, expected->len );
    }
    ok = 1;

exit:
    mbedtls_free( data->output );
    data->output = NULL;
    return( ok );
}

void test_mbedtls_asn1_write_null( data_t *expected )
{
    generic_write_data_t data = { NULL, NULL, NULL, NULL, 0 };
    int ret;

    for( data.size = 0; data.size < expected->len + 1; data.size++ )
    {
        if( ! generic_write_start_step( &data ) )
            goto exit;
        ret = mbedtls_asn1_write_null( &data.p, data.start );
        if( ! generic_write_finish_step( &data, expected, ret ) )
            goto exit;
    }

exit:
    mbedtls_free( data.output );
}

void test_mbedtls_asn1_write_null_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_mbedtls_asn1_write_null( &data0 );
}
void test_mbedtls_asn1_write_bool( int val, data_t *expected )
{
    generic_write_data_t data = { NULL, NULL, NULL, NULL, 0 };
    int ret;

    for( data.size = 0; data.size < expected->len + 1; data.size++ )
    {
        if( ! generic_write_start_step( &data ) )
            goto exit;
        ret = mbedtls_asn1_write_bool( &data.p, data.start, val );
        if( ! generic_write_finish_step( &data, expected, ret ) )
            goto exit;
    }

exit:
    mbedtls_free( data.output );
}

void test_mbedtls_asn1_write_bool_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_mbedtls_asn1_write_bool( *( (int *) params[0] ), &data1 );
}
void test_mbedtls_asn1_write_int( int val, data_t *expected )
{
    generic_write_data_t data = { NULL, NULL, NULL, NULL, 0 };
    int ret;

    for( data.size = 0; data.size < expected->len + 1; data.size++ )
    {
        if( ! generic_write_start_step( &data ) )
            goto exit;
        ret = mbedtls_asn1_write_int( &data.p, data.start, val );
        if( ! generic_write_finish_step( &data, expected, ret ) )
            goto exit;
    }

exit:
    mbedtls_free( data.output );
}

void test_mbedtls_asn1_write_int_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_mbedtls_asn1_write_int( *( (int *) params[0] ), &data1 );
}
void test_mbedtls_asn1_write_enum( int val, data_t *expected )
{
    generic_write_data_t data = { NULL, NULL, NULL, NULL, 0 };
    int ret;

    for( data.size = 0; data.size < expected->len + 1; data.size++ )
    {
        if( ! generic_write_start_step( &data ) )
            goto exit;
        ret = mbedtls_asn1_write_enum( &data.p, data.start, val );
        if( ! generic_write_finish_step( &data, expected, ret ) )
            goto exit;
    }

exit:
    mbedtls_free( data.output );
}

void test_mbedtls_asn1_write_enum_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_mbedtls_asn1_write_enum( *( (int *) params[0] ), &data1 );
}
#if defined(MBEDTLS_BIGNUM_C)
void test_mbedtls_asn1_write_mpi( data_t *val, data_t *expected )
{
    generic_write_data_t data = { NULL, NULL, NULL, NULL, 0 };
    mbedtls_mpi mpi;
    int ret;

    mbedtls_mpi_init( &mpi );
    TEST_ASSERT( mbedtls_mpi_read_binary( &mpi, val->x, val->len ) == 0 );

    for( data.size = 0; data.size < expected->len + 1; data.size++ )
    {
        if( ! generic_write_start_step( &data ) )
            goto exit;
        ret = mbedtls_asn1_write_mpi( &data.p, data.start, &mpi );
        if( ! generic_write_finish_step( &data, expected, ret ) )
            goto exit;
        if( expected->len > 10 && data.size == 8 )
            data.size = expected->len - 2;
    }

exit:
    mbedtls_mpi_free( &mpi );
    mbedtls_free( data.output );
}

void test_mbedtls_asn1_write_mpi_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_mbedtls_asn1_write_mpi( &data0, &data2 );
}
#endif /* MBEDTLS_BIGNUM_C */
void test_mbedtls_asn1_write_string( int tag, data_t *content, data_t *expected )
{
    generic_write_data_t data = { NULL, NULL, NULL, NULL, 0 };
    int ret;

    for( data.size = 0; data.size < expected->len + 1; data.size++ )
    {
        if( ! generic_write_start_step( &data ) )
            goto exit;
        switch( tag )
        {
            case MBEDTLS_ASN1_OCTET_STRING:
                ret = mbedtls_asn1_write_octet_string(
                    &data.p, data.start, content->x, content->len );
                break;
            case MBEDTLS_ASN1_OID:
                ret = mbedtls_asn1_write_oid(
                    &data.p, data.start,
                    (const char *) content->x, content->len );
                break;
            case MBEDTLS_ASN1_UTF8_STRING:
                ret = mbedtls_asn1_write_utf8_string(
                    &data.p, data.start,
                    (const char *) content->x, content->len );
                break;
            case MBEDTLS_ASN1_PRINTABLE_STRING:
                ret = mbedtls_asn1_write_printable_string(
                    &data.p, data.start,
                    (const char *) content->x, content->len );
                break;
            case MBEDTLS_ASN1_IA5_STRING:
                ret = mbedtls_asn1_write_ia5_string(
                    &data.p, data.start,
                    (const char *) content->x, content->len );
                break;
            default:
                ret = mbedtls_asn1_write_tagged_string(
                    &data.p, data.start, tag,
                    (const char *) content->x, content->len );
        }
        if( ! generic_write_finish_step( &data, expected, ret ) )
            goto exit;
        if( expected->len > 10 && data.size == 8 )
            data.size = expected->len - 2;
    }

exit:
    mbedtls_free( data.output );
}

void test_mbedtls_asn1_write_string_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};

    test_mbedtls_asn1_write_string( *( (int *) params[0] ), &data1, &data3 );
}
void test_mbedtls_asn1_write_algorithm_identifier( data_t *oid,
                                              int par_len,
                                              data_t *expected )
{
    generic_write_data_t data = { NULL, NULL, NULL, NULL, 0 };
    int ret;

    for( data.size = 0; data.size < expected->len + 1; data.size++ )
    {
        if( ! generic_write_start_step( &data ) )
            goto exit;
        ret = mbedtls_asn1_write_algorithm_identifier(
            &data.p, data.start,
            (const char *) oid->x, oid->len, par_len );
        /* If params_len != 0, mbedtls_asn1_write_algorithm_identifier()
         * assumes that the parameters are already present in the buffer
         * and returns a length that accounts for this, but our test
         * data omits the parameters. */
        if( ret >= 0 )
            ret -= par_len;
        if( ! generic_write_finish_step( &data, expected, ret ) )
            goto exit;
    }

exit:
    mbedtls_free( data.output );
}

void test_mbedtls_asn1_write_algorithm_identifier_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};

    test_mbedtls_asn1_write_algorithm_identifier( &data0, *( (int *) params[2] ), &data3 );
}
#if defined(MBEDTLS_ASN1_PARSE_C)
void test_mbedtls_asn1_write_len( int len, data_t * asn1, int buf_len,
                             int result )
{
    int ret;
    unsigned char buf[150];
    unsigned char *p;
    size_t i;
    size_t read_len;

    memset( buf, GUARD_VAL, sizeof( buf ) );

    p = buf + GUARD_LEN + buf_len;

    ret = mbedtls_asn1_write_len( &p, buf + GUARD_LEN, (size_t) len );

    TEST_ASSERT( ret == result );

    /* Check for buffer overwrite on both sides */
    for( i = 0; i < GUARD_LEN; i++ )
    {
        TEST_ASSERT( buf[i] == GUARD_VAL );
        TEST_ASSERT( buf[GUARD_LEN + buf_len + i] == GUARD_VAL );
    }

    if( result >= 0 )
    {
        TEST_ASSERT( p + asn1->len == buf + GUARD_LEN + buf_len );

        TEST_ASSERT( memcmp( p, asn1->x, asn1->len ) == 0 );

        /* Read back with mbedtls_asn1_get_len() to check */
        ret = mbedtls_asn1_get_len( &p, buf + GUARD_LEN + buf_len, &read_len );

        if( len == 0 )
        {
            TEST_ASSERT( ret == 0 );
        }
        else
        {
            /* Return will be MBEDTLS_ERR_ASN1_OUT_OF_DATA because the rest of
             * the buffer is missing
             */
            TEST_ASSERT( ret == MBEDTLS_ERR_ASN1_OUT_OF_DATA );
        }
        TEST_ASSERT( read_len == (size_t) len );
        TEST_ASSERT( p == buf + GUARD_LEN + buf_len );
    }
exit:
    ;
}

void test_mbedtls_asn1_write_len_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_mbedtls_asn1_write_len( *( (int *) params[0] ), &data1, *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_ASN1_PARSE_C */
void test_test_asn1_write_bitstrings( data_t *bitstring, int bits,
                                 data_t *expected, int is_named )
{
    generic_write_data_t data = { NULL, NULL, NULL, NULL, 0 };
    int ret;
    int ( *func )( unsigned char **p, unsigned char *start,
                   const unsigned char *buf, size_t bits ) =
        ( is_named ? mbedtls_asn1_write_named_bitstring :
          mbedtls_asn1_write_bitstring );

    for( data.size = 0; data.size < expected->len + 1; data.size++ )
    {
        if( ! generic_write_start_step( &data ) )
            goto exit;
        ret = ( *func )( &data.p, data.start, bitstring->x, bits );
        if( ! generic_write_finish_step( &data, expected, ret ) )
            goto exit;
    }

exit:
    mbedtls_free( data.output );
}

void test_test_asn1_write_bitstrings_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};

    test_test_asn1_write_bitstrings( &data0, *( (int *) params[2] ), &data3, *( (int *) params[5] ) );
}
void test_store_named_data_find( data_t *oid0, data_t *oid1,
                            data_t *oid2, data_t *oid3,
                            data_t *needle, int from, int position )
{
    data_t *oid[4] = {oid0, oid1, oid2, oid3};
    mbedtls_asn1_named_data nd[] ={
        { {0x06, 0, NULL}, {0, 0, NULL}, NULL, 0 },
        { {0x06, 0, NULL}, {0, 0, NULL}, NULL, 0 },
        { {0x06, 0, NULL}, {0, 0, NULL}, NULL, 0 },
        { {0x06, 0, NULL}, {0, 0, NULL}, NULL, 0 },
    };
    mbedtls_asn1_named_data *pointers[ARRAY_LENGTH( nd ) + 1];
    size_t i;
    mbedtls_asn1_named_data *head = NULL;
    mbedtls_asn1_named_data *found = NULL;

    for( i = 0; i < ARRAY_LENGTH( nd ); i++ )
        pointers[i] = &nd[i];
    pointers[ARRAY_LENGTH( nd )] = NULL;
    for( i = 0; i < ARRAY_LENGTH( nd ); i++ )
    {
        ASSERT_ALLOC( nd[i].oid.p, oid[i]->len );
        memcpy( nd[i].oid.p, oid[i]->x, oid[i]->len );
        nd[i].oid.len = oid[i]->len;
        nd[i].next = pointers[i+1];
    }

    head = pointers[from];
    found = mbedtls_asn1_store_named_data( &head,
                                           (const char *) needle->x,
                                           needle->len,
                                           NULL, 0 );

    /* In any case, the existing list structure must be unchanged. */
    for( i = 0; i < ARRAY_LENGTH( nd ); i++ )
        TEST_ASSERT( nd[i].next == pointers[i+1] );

    if( position >= 0 )
    {
        /* position should have been found and modified. */
        TEST_ASSERT( head == pointers[from] );
        TEST_ASSERT( found == pointers[position] );
    }
    else
    {
        /* A new entry should have been created. */
        TEST_ASSERT( found == head );
        TEST_ASSERT( head->next == pointers[from] );
        for( i = 0; i < ARRAY_LENGTH( nd ); i++ )
            TEST_ASSERT( found != &nd[i] );
    }

exit:
    if( found != NULL && found == head && found != pointers[from] )
    {
        mbedtls_free( found->oid.p );
        mbedtls_free( found );
    }
    for( i = 0; i < ARRAY_LENGTH( nd ); i++ )
        mbedtls_free( nd[i].oid.p );
}

void test_store_named_data_find_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_store_named_data_find( &data0, &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ) );
}
void test_store_named_data_val_found( int old_len, int new_len )
{
    mbedtls_asn1_named_data nd =
        { {0x06, 3, (unsigned char *) "OID"}, {0, 0, NULL}, NULL, 0 };
    mbedtls_asn1_named_data *head = &nd;
    mbedtls_asn1_named_data *found = NULL;
    unsigned char *old_val = NULL;
    unsigned char *new_val = (unsigned char *) "new value";

    if( old_len != 0 )
    {
        ASSERT_ALLOC( nd.val.p, (size_t) old_len );
        old_val = nd.val.p;
        nd.val.len = old_len;
        memset( old_val, 'x', old_len );
    }
    if( new_len <= 0 )
    {
        new_len = - new_len;
        new_val = NULL;
    }

    found = mbedtls_asn1_store_named_data( &head, "OID", 3,
                                           new_val, new_len );
    TEST_ASSERT( head == &nd );
    TEST_ASSERT( found == head );

    if( new_val != NULL)
        ASSERT_COMPARE( found->val.p, found->val.len,
                        new_val, (size_t) new_len );
    if( new_len == 0)
        TEST_ASSERT( found->val.p == NULL );
    else if( new_len == old_len )
        TEST_ASSERT( found->val.p == old_val );
    else
        TEST_ASSERT( found->val.p != old_val );

exit:
    mbedtls_free( nd.val.p );
}

void test_store_named_data_val_found_wrapper( void ** params )
{

    test_store_named_data_val_found( *( (int *) params[0] ), *( (int *) params[1] ) );
}
void test_store_named_data_val_new( int new_len )
{
    mbedtls_asn1_named_data *head = NULL;
    mbedtls_asn1_named_data *found = NULL;
    const unsigned char *oid = (unsigned char *) "OID";
    size_t oid_len = strlen( (const char *) oid );
    const unsigned char *new_val = (unsigned char *) "new value";

    if( new_len <= 0 )
        new_val = NULL;
    if( new_len < 0 )
        new_len = - new_len;

    found = mbedtls_asn1_store_named_data( &head,
                                           (const char *) oid, oid_len,
                                           new_val, (size_t) new_len );
    TEST_ASSERT( found != NULL );
    TEST_ASSERT( found == head );
    TEST_ASSERT( found->oid.p != oid );
    ASSERT_COMPARE( found->oid.p, found->oid.len, oid, oid_len );
    if( new_len == 0 )
        TEST_ASSERT( found->val.p == NULL );
    else if( new_val == NULL )
        TEST_ASSERT( found->val.p != NULL );
    else
    {
        TEST_ASSERT( found->val.p != new_val );
        ASSERT_COMPARE( found->val.p, found->val.len,
                        new_val, (size_t) new_len );
    }

exit:
    if( found != NULL )
    {
        mbedtls_free( found->oid.p );
        mbedtls_free( found->val.p );
    }
    mbedtls_free( found );
}

void test_store_named_data_val_new_wrapper( void ** params )
{

    test_store_named_data_val_new( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_ASN1_WRITE_C */

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

#if defined(MBEDTLS_ASN1_WRITE_C)

        case 0:
            {
                *out_value = MBEDTLS_ASN1_OCTET_STRING;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ASN1_UTF8_STRING;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ASN1_PRINTABLE_STRING;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ASN1_IA5_STRING;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ASN1_IA5_STRING | MBEDTLS_ASN1_CONTEXT_SPECIFIC;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ASN1_OID;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_ERR_ASN1_BUF_TOO_SMALL;
            }
            break;
        case 7:
            {
                *out_value = -1;
            }
            break;
        case 8:
            {
                *out_value = -4;
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

#if defined(MBEDTLS_ASN1_WRITE_C)

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

#if defined(MBEDTLS_ASN1_WRITE_C)
    test_mbedtls_asn1_write_null_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_ASN1_WRITE_C)
    test_mbedtls_asn1_write_bool_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_ASN1_WRITE_C)
    test_mbedtls_asn1_write_int_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_ASN1_WRITE_C)
    test_mbedtls_asn1_write_enum_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_ASN1_WRITE_C) && defined(MBEDTLS_BIGNUM_C)
    test_mbedtls_asn1_write_mpi_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_ASN1_WRITE_C)
    test_mbedtls_asn1_write_string_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_ASN1_WRITE_C)
    test_mbedtls_asn1_write_algorithm_identifier_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_ASN1_WRITE_C) && defined(MBEDTLS_ASN1_PARSE_C)
    test_mbedtls_asn1_write_len_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_ASN1_WRITE_C)
    test_test_asn1_write_bitstrings_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_ASN1_WRITE_C)
    test_store_named_data_find_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_ASN1_WRITE_C)
    test_store_named_data_val_found_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_ASN1_WRITE_C)
    test_store_named_data_val_new_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_asn1write.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
