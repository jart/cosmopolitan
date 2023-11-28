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
#include "libc/stdio/stdio.h"
#include "third_party/mbedtls/test/test.inc"
/*
 * *** THIS FILE WAS MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * Test file      : ./test_suite_asn1parse.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_asn1parse.function
 *      Test suite data     : suites/test_suite_asn1parse.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_ASN1_PARSE_C)

#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/asn1.h"
#if defined(MBEDTLS_ASN1_WRITE_C)
#include "third_party/mbedtls/asn1write.h"
#endif

/* Used internally to report an error that indicates a bug in a parsing function. */
#define ERR_PARSE_INCONSISTENCY INT_MAX

/* Use this magic value in some tests to indicate that the expected result
 * should not be checked. */
#define UNPREDICTABLE_RESULT 0x5552

static int nested_parse( unsigned char **const p,
                         const unsigned char *const end )
{
    int ret;
    size_t len = 0;
    size_t len2 = 0;
    unsigned char *const start = *p;
    unsigned char *content_start;
    unsigned char tag;

    /* First get the length, skipping over the tag. */
    content_start = start + 1;
    ret = mbedtls_asn1_get_len( &content_start, end, &len );
    TEST_ASSERT( content_start <= end );
    if( ret != 0 )
        return( ret );

    /* Since we have a valid element start (tag and length), retrieve and
     * check the tag. */
    tag = start[0];
    TEST_EQUAL( mbedtls_asn1_get_tag( p, end, &len2, tag ^ 1 ),
                MBEDTLS_ERR_ASN1_UNEXPECTED_TAG );
    *p = start;
    TEST_EQUAL( mbedtls_asn1_get_tag( p, end, &len2, tag ), 0 );
    TEST_EQUAL( len, len2 );
    TEST_ASSERT( *p == content_start );
    *p = content_start;

    switch( tag & 0x1f )
    {
        case MBEDTLS_ASN1_BOOLEAN:
        {
            int val = -257;
            *p = start;
            ret = mbedtls_asn1_get_bool( p, end, &val );
            if( ret == 0 )
                TEST_ASSERT( val == 0 || val == 1 );
            break;
        }

        case MBEDTLS_ASN1_INTEGER:
        {
#if defined(MBEDTLS_BIGNUM_C)
            mbedtls_mpi mpi;
            mbedtls_mpi_init( &mpi );
            *p = start;
            ret = mbedtls_asn1_get_mpi( p, end, &mpi );
            mbedtls_mpi_free( &mpi );
#else
            *p = start + 1;
            ret = mbedtls_asn1_get_len( p, end, &len );
            *p += len;
#endif
            /* If we're sure that the number fits in an int, also
             * call mbedtls_asn1_get_int(). */
            if( ret == 0 && len < sizeof( int ) )
            {
                int val = -257;
                unsigned char *q = start;
                ret = mbedtls_asn1_get_int( &q, end, &val );
                TEST_ASSERT( *p == q );
            }
            break;
        }

        case MBEDTLS_ASN1_BIT_STRING:
        {
            mbedtls_asn1_bitstring bs;
            *p = start;
            ret = mbedtls_asn1_get_bitstring( p, end, &bs );
            break;
        }

        case MBEDTLS_ASN1_SEQUENCE:
        {
            while( *p <= end && *p < content_start + len && ret == 0 )
                ret = nested_parse( p, content_start + len );
            break;
        }

        case MBEDTLS_ASN1_OCTET_STRING:
        case MBEDTLS_ASN1_NULL:
        case MBEDTLS_ASN1_OID:
        case MBEDTLS_ASN1_UTF8_STRING:
        case MBEDTLS_ASN1_SET:
        case MBEDTLS_ASN1_PRINTABLE_STRING:
        case MBEDTLS_ASN1_T61_STRING:
        case MBEDTLS_ASN1_IA5_STRING:
        case MBEDTLS_ASN1_UTC_TIME:
        case MBEDTLS_ASN1_GENERALIZED_TIME:
        case MBEDTLS_ASN1_UNIVERSAL_STRING:
        case MBEDTLS_ASN1_BMP_STRING:
        default:
            /* No further testing implemented for this tag. */
            *p += len;
            return( 0 );
    }

    TEST_ASSERT( *p <= end );
    return( ret );

exit:
    return( ERR_PARSE_INCONSISTENCY );
}

int get_len_step( const data_t *input, size_t buffer_size,
                  size_t actual_length )
{
    unsigned char *buf = NULL;
    unsigned char *p = NULL;
    unsigned char *end;
    size_t parsed_length;
    int ret;

    mbedtls_test_set_step( buffer_size );
    /* Allocate a new buffer of exactly the length to parse each time.
     * This gives memory sanitizers a chance to catch buffer overreads. */
    if( buffer_size == 0 )
    {
        ASSERT_ALLOC( buf, 1 );
        end = buf + 1;
        p = end;
    }
    else
    {
        ASSERT_ALLOC_WEAK( buf, buffer_size );
        if( buffer_size > input->len )
        {
            memcpy( buf, input->x, input->len );
            memset( buf + input->len, 'A', buffer_size - input->len );
        }
        else
        {
            memcpy( buf, input->x, buffer_size );
        }
        p = buf;
        end = buf + buffer_size;
    }

    ret = mbedtls_asn1_get_len( &p, end, &parsed_length );

    if( buffer_size >= input->len + actual_length )
    {
        TEST_EQUAL( ret, 0 );
        TEST_ASSERT( p == buf + input->len );
        TEST_EQUAL( parsed_length, actual_length );
    }
    else
    {
        TEST_EQUAL( ret, MBEDTLS_ERR_ASN1_OUT_OF_DATA );
    }
    mbedtls_free( buf );
    return( 1 );

exit:
    mbedtls_free( buf );
    return( 0 );
}

typedef struct
{
    const unsigned char *input_start;
    const char *description;
} traverse_state_t;

/* Value returned by traverse_callback if description runs out. */
#define RET_TRAVERSE_STOP 1
/* Value returned by traverse_callback if description has an invalid format
 * (see traverse_sequence_of). */
#define RET_TRAVERSE_ERROR 2


static int traverse_callback( void *ctx, int tag,
                              unsigned char *content, size_t len )
{
    traverse_state_t *state = ctx;
    size_t offset;
    const char *rest = state->description;
    unsigned long n;

    TEST_ASSERT( content > state->input_start );
    offset = content - state->input_start;
    mbedtls_test_set_step( offset );

    if( *rest == 0 )
        return( RET_TRAVERSE_STOP );
    n = strtoul( rest, (char **) &rest, 0 );
    TEST_EQUAL( n, offset );
    TEST_EQUAL( *rest, ',' );
    ++rest;
    n = strtoul( rest, (char **) &rest, 0 );
    TEST_EQUAL( n, (unsigned) tag );
    TEST_EQUAL( *rest, ',' );
    ++rest;
    n = strtoul( rest, (char **) &rest, 0 );
    TEST_EQUAL( n, len );
    if( *rest == ',' )
        ++rest;

    state->description = rest;
    return( 0 );

exit:
    return( RET_TRAVERSE_ERROR );
}

void test_parse_prefixes( const data_t *input,
                     int full_result,
                     int overfull_result )
{
    /* full_result: expected result from parsing the given string. */
    /* overfull_result: expected_result from parsing the given string plus
     * some trailing garbage. This may be UNPREDICTABLE_RESULT to accept
     * any result: use this for invalid inputs that may or may not become
     * valid depending on what the trailing garbage is. */

    unsigned char *buf = NULL;
    unsigned char *p = NULL;
    size_t buffer_size;
    int ret;

    /* Test every prefix of the input, except the empty string.
     * The first byte of the string is the tag. Without a tag byte,
     * we wouldn't know what to parse the input as.
     * Also test the input followed by an extra byte.
     */
    for( buffer_size = 1; buffer_size <= input->len + 1; buffer_size++ )
    {
        mbedtls_test_set_step( buffer_size );
        /* Allocate a new buffer of exactly the length to parse each time.
         * This gives memory sanitizers a chance to catch buffer overreads. */
        ASSERT_ALLOC( buf, buffer_size );
        memcpy( buf, input->x, buffer_size );
        p = buf;
        ret = nested_parse( &p, buf + buffer_size );

        if( ret == ERR_PARSE_INCONSISTENCY )
            goto exit;
        if( buffer_size < input->len )
        {
            TEST_EQUAL( ret, MBEDTLS_ERR_ASN1_OUT_OF_DATA );
        }
        else if( buffer_size == input->len )
        {
            TEST_EQUAL( ret, full_result );
        }
        else /* ( buffer_size > input->len ) */
        {
            if( overfull_result != UNPREDICTABLE_RESULT )
                TEST_EQUAL( ret, overfull_result );
        }
        if( ret == 0 )
            TEST_ASSERT( p == buf + input->len );

        mbedtls_free( buf );
        buf = NULL;
    }

exit:
    mbedtls_free( buf );
}

void test_parse_prefixes_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_parse_prefixes( &data0, *( (int *) params[2] ), *( (int *) params[3] ) );
}
void test_get_len( const data_t *input, int actual_length_arg )
{
    size_t actual_length = actual_length_arg;
    size_t buffer_size;

    /* Test prefixes of a buffer containing the given length string
     * followed by `actual_length` bytes of payload. To save a bit of
     * time, we skip some "boring" prefixes: we don't test prefixes where
     * the payload is truncated more than one byte away from either end,
     * and we only test the empty string on a 1-byte input.
     */
    for( buffer_size = 1; buffer_size <= input->len + 1; buffer_size++ )
    {
        if( ! get_len_step( input, buffer_size, actual_length ) )
            goto exit;
    }
    if( ! get_len_step( input, input->len + actual_length - 1, actual_length ) )
        goto exit;
    if( ! get_len_step( input, input->len + actual_length, actual_length ) )
        goto exit;
exit:
    ;
}

void test_get_len_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_len( &data0, *( (int *) params[2] ) );
}
void test_get_boolean( const data_t *input,
                  int expected_value, int expected_result )
{
    unsigned char *p = input->x;
    int val;
    int ret;
    ret = mbedtls_asn1_get_bool( &p, input->x + input->len, &val );
    TEST_EQUAL( ret, expected_result );
    if( expected_result == 0 )
    {
        TEST_EQUAL( val, expected_value );
        TEST_ASSERT( p == input->x + input->len );
    }
exit:
    ;
}

void test_get_boolean_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_boolean( &data0, *( (int *) params[2] ), *( (int *) params[3] ) );
}
void test_empty_integer( const data_t *input )
{
    unsigned char *p;
#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi actual_mpi;
#endif
    int val;

#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_init( & actual_mpi );
#endif

    /* An INTEGER with no content is not valid. */
    p = input->x;
    TEST_EQUAL( mbedtls_asn1_get_int( &p, input->x + input->len, &val ),
                MBEDTLS_ERR_ASN1_INVALID_LENGTH );

#if defined(MBEDTLS_BIGNUM_C)
    /* INTEGERs are sometimes abused as bitstrings, so the library accepts
     * an INTEGER with empty content and gives it the value 0. */
    p = input->x;
    TEST_EQUAL( mbedtls_asn1_get_mpi( &p, input->x + input->len, &actual_mpi ),
                0 );
    TEST_EQUAL( mbedtls_mpi_cmp_int( &actual_mpi, 0 ), 0 );
#endif

exit:
#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_free( &actual_mpi );
#endif
    /*empty cleanup in some configurations*/ ;
}

void test_empty_integer_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_empty_integer( &data0 );
}
void test_get_integer( const data_t *input,
                  const char *expected_hex, int expected_result )
{
    unsigned char *p;
#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi expected_mpi;
    mbedtls_mpi actual_mpi;
    mbedtls_mpi complement;
    int expected_result_for_mpi = expected_result;
#endif
    long expected_value;
    int expected_result_for_int = expected_result;
    int val;
    int ret;

#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_init( &expected_mpi );
    mbedtls_mpi_init( &actual_mpi );
    mbedtls_mpi_init( &complement );
#endif

    errno = 0;
    expected_value = strtol( expected_hex, NULL, 16 );
    if( expected_result == 0 &&
        ( errno == ERANGE
#if LONG_MAX > INT_MAX
          || expected_value > INT_MAX || expected_value < INT_MIN
#endif
            ) )
    {
        /* The library returns the dubious error code INVALID_LENGTH
         * for integers that are out of range. */
        expected_result_for_int = MBEDTLS_ERR_ASN1_INVALID_LENGTH;
    }
    if( expected_result == 0 && expected_value < 0 )
    {
        /* The library does not support negative INTEGERs and
         * returns the dubious error code INVALID_LENGTH.
         * Test that we preserve the historical behavior. If we
         * decide to change the behavior, we'll also change this test. */
        expected_result_for_int = MBEDTLS_ERR_ASN1_INVALID_LENGTH;
    }

    p = input->x;
    ret = mbedtls_asn1_get_int( &p, input->x + input->len, &val );
    TEST_EQUAL( ret, expected_result_for_int );
    if( ret == 0 )
    {
        TEST_EQUAL( val, expected_value );
        TEST_ASSERT( p == input->x + input->len );
    }

#if defined(MBEDTLS_BIGNUM_C)
    ret = mbedtls_mpi_read_string( &expected_mpi, 16, expected_hex );
    TEST_ASSERT( ret == 0 || ret == MBEDTLS_ERR_MPI_BAD_INPUT_DATA );
    if( ret == MBEDTLS_ERR_MPI_BAD_INPUT_DATA )
    {
        /* The data overflows the maximum MPI size. */
        expected_result_for_mpi = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }
    p = input->x;
    ret = mbedtls_asn1_get_mpi( &p, input->x + input->len, &actual_mpi );
    TEST_EQUAL( ret, expected_result_for_mpi );
    if( ret == 0 )
    {
        if( expected_value >= 0 )
        {
            TEST_ASSERT( mbedtls_mpi_cmp_mpi( &actual_mpi,
                                              &expected_mpi ) == 0 );
        }
        else
        {
            /* The library ignores the sign bit in ASN.1 INTEGERs
             * (which makes sense insofar as INTEGERs are sometimes
             * abused as bit strings), so the result of parsing them
             * is a positive integer such that expected_mpi +
             * actual_mpi = 2^n where n is the length of the content
             * of the INTEGER. (Leading ff octets don't matter for the
             * expected value, but they matter for the actual value.)
             * Test that we don't change from this behavior. If we
             * decide to fix the library to change the behavior on
             * negative INTEGERs, we'll fix this test code. */
            unsigned char *q = input->x + 1;
            size_t len;
            TEST_ASSERT( mbedtls_asn1_get_len( &q, input->x + input->len,
                                               &len ) == 0 );
            TEST_ASSERT( mbedtls_mpi_lset( &complement, 1 ) == 0 );
            TEST_ASSERT( mbedtls_mpi_shift_l( &complement, len * 8 ) == 0 );
            TEST_ASSERT( mbedtls_mpi_add_mpi( &complement, &complement,
                                              &expected_mpi ) == 0 );
            TEST_ASSERT( mbedtls_mpi_cmp_mpi( &complement,
                                              &actual_mpi ) == 0 );
        }
        TEST_ASSERT( p == input->x + input->len );
    }
#endif

exit:
#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_mpi_free( &expected_mpi );
    mbedtls_mpi_free( &actual_mpi );
    mbedtls_mpi_free( &complement );
#endif
    /*empty cleanup in some configurations*/ ;
}

void test_get_integer_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_integer( &data0, (char *) params[2], *( (int *) params[3] ) );
}
void test_get_enum( const data_t *input,
               const char *expected_hex, int expected_result )
{
    unsigned char *p;
    long expected_value;
    int expected_result_for_enum = expected_result;
    int val;
    int ret;

    errno = 0;
    expected_value = strtol( expected_hex, NULL, 16 );
    if( expected_result == 0 &&
        ( errno == ERANGE
#if LONG_MAX > INT_MAX
          || expected_value > INT_MAX || expected_value < INT_MIN
#endif
            ) )
    {
        /* The library returns the dubious error code INVALID_LENGTH
         * for integers that are out of range. */
        expected_result_for_enum = MBEDTLS_ERR_ASN1_INVALID_LENGTH;
    }
    if( expected_result == 0 && expected_value < 0 )
    {
        /* The library does not support negative INTEGERs and
         * returns the dubious error code INVALID_LENGTH.
         * Test that we preserve the historical behavior. If we
         * decide to change the behavior, we'll also change this test. */
        expected_result_for_enum = MBEDTLS_ERR_ASN1_INVALID_LENGTH;
    }

    p = input->x;
    ret = mbedtls_asn1_get_enum( &p, input->x + input->len, &val );
    TEST_EQUAL( ret, expected_result_for_enum );
    if( ret == 0 )
    {
        TEST_EQUAL( val, expected_value );
        TEST_ASSERT( p == input->x + input->len );
    }
exit:
    ;
}

void test_get_enum_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_enum( &data0, (char *) params[2], *( (int *) params[3] ) );
}
#if defined(MBEDTLS_BIGNUM_C)
void test_get_mpi_too_large( )
{
    unsigned char *buf = NULL;
    unsigned char *p;
    mbedtls_mpi actual_mpi;
    size_t too_many_octets =
        MBEDTLS_MPI_MAX_LIMBS * sizeof(mbedtls_mpi_uint) + 1;
    size_t size = too_many_octets + 6;

    mbedtls_mpi_init( &actual_mpi );

    ASSERT_ALLOC( buf, size );
    buf[0] = 0x02; /* tag: INTEGER */
    buf[1] = 0x84; /* 4-octet length */
    buf[2] = ( too_many_octets >> 24 ) & 0xff;
    buf[3] = ( too_many_octets >> 16 ) & 0xff;
    buf[4] = ( too_many_octets >> 8 ) & 0xff;
    buf[5] = too_many_octets & 0xff;
    buf[6] = 0x01; /* most significant octet */

    p = buf;
    TEST_EQUAL( mbedtls_asn1_get_mpi( &p, buf + size, &actual_mpi ),
                MBEDTLS_ERR_MPI_ALLOC_FAILED );

exit:
    mbedtls_mpi_free( &actual_mpi );
    mbedtls_free( buf );
}

void test_get_mpi_too_large_wrapper( void ** params )
{
    (void)params;

    test_get_mpi_too_large(  );
}
#endif /* MBEDTLS_BIGNUM_C */
void test_get_bitstring( const data_t *input,
                    int expected_length, int expected_unused_bits,
                    int expected_result, int expected_result_null )
{
    mbedtls_asn1_bitstring bs = { 0xdead, 0x21, NULL };
    unsigned char *p = input->x;

    TEST_EQUAL( mbedtls_asn1_get_bitstring( &p, input->x + input->len, &bs ),
                expected_result );
    if( expected_result == 0 )
    {
        TEST_EQUAL( bs.len, (size_t) expected_length );
        TEST_EQUAL( bs.unused_bits, expected_unused_bits );
        TEST_ASSERT( bs.p != NULL );
        TEST_EQUAL( bs.p - input->x + bs.len, input->len );
        TEST_ASSERT( p == input->x + input->len );
    }

    p = input->x;
    TEST_EQUAL( mbedtls_asn1_get_bitstring_null( &p, input->x + input->len,
                                                 &bs.len ),
                expected_result_null );
    if( expected_result_null == 0 )
    {
        TEST_EQUAL( bs.len, (size_t) expected_length );
        if( expected_result == 0 )
            TEST_ASSERT( p == input->x + input->len - bs.len );
    }
exit:
    ;
}

void test_get_bitstring_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_bitstring( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ) );
}
void test_get_sequence_of( const data_t *input, int tag,
                      const char *description,
                      int expected_result )
{
    /* The description string is a comma-separated list of integers.
     * For each element in the SEQUENCE in input, description contains
     * two integers: the offset of the element (offset from the start
     * of input to the tag of the element) and the length of the
     * element's contents.
     * "offset1,length1,..." */

    mbedtls_asn1_sequence head = { { 0, 0, NULL }, NULL };
    mbedtls_asn1_sequence *cur;
    unsigned char *p = input->x;
    const char *rest = description;
    unsigned long n;
    unsigned int step = 0;

    TEST_EQUAL( mbedtls_asn1_get_sequence_of( &p, input->x + input->len,
                                              &head, tag ),
                expected_result );
    if( expected_result == 0 )
    {
        TEST_ASSERT( p == input->x + input->len );

        if( ! *rest )
        {
            TEST_EQUAL( head.buf.tag, 0 );
            TEST_ASSERT( head.buf.p == NULL );
            TEST_EQUAL( head.buf.len, 0 );
            TEST_ASSERT( head.next == NULL );
        }
        else
        {
            cur = &head;
            while( *rest )
            {
                mbedtls_test_set_step( step );
                TEST_ASSERT( cur != NULL );
                TEST_EQUAL( cur->buf.tag, tag );
                n = strtoul( rest, (char **) &rest, 0 );
                TEST_EQUAL( n, (size_t)( cur->buf.p - input->x ) );
                ++rest;
                n = strtoul( rest, (char **) &rest, 0 );
                TEST_EQUAL( n, cur->buf.len );
                if( *rest )
                    ++rest;
                cur = cur->next;
                ++step;
            }
            TEST_ASSERT( cur == NULL );
        }
    }

exit:
    mbedtls_asn1_sequence_free( head.next );
}

void test_get_sequence_of_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_sequence_of( &data0, *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ) );
}
void test_traverse_sequence_of( const data_t *input,
                           int tag_must_mask, int tag_must_val,
                           int tag_may_mask, int tag_may_val,
                           const char *description,
                           int expected_result )
{
    /* The description string is a comma-separated list of integers.
     * For each element in the SEQUENCE in input, description contains
     * three integers: the offset of the element's content (offset from
     * the start of input to the content of the element), the element's tag,
     * and the length of the element's contents.
     * "offset1,tag1,length1,..." */

    unsigned char *p = input->x;
    traverse_state_t traverse_state = {input->x, description};
    int ret;

    ret = mbedtls_asn1_traverse_sequence_of( &p, input->x + input->len,
                                             (uint8_t) tag_must_mask, (uint8_t) tag_must_val,
                                             (uint8_t) tag_may_mask, (uint8_t) tag_may_val,
                                             traverse_callback, &traverse_state );
    if( ret == RET_TRAVERSE_ERROR )
        goto exit;
    TEST_EQUAL( ret, expected_result );
    TEST_EQUAL( *traverse_state.description, 0 );
exit:
    ;
}

void test_traverse_sequence_of_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_traverse_sequence_of( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ) );
}
void test_get_alg( const data_t *input,
              int oid_offset, int oid_length,
              int params_tag, int params_offset, int params_length,
              int total_length,
              int expected_result )
{
    mbedtls_asn1_buf oid = { -1, 0, NULL };
    mbedtls_asn1_buf params = { -1, 0, NULL };
    unsigned char *p = input->x;
    int ret;

    TEST_EQUAL( mbedtls_asn1_get_alg( &p, input->x + input->len,
                                      &oid, &params ),
                expected_result );
    if( expected_result == 0 )
    {
        TEST_EQUAL( oid.tag, MBEDTLS_ASN1_OID );
        TEST_EQUAL( oid.p - input->x, oid_offset );
        TEST_EQUAL( oid.len, (size_t) oid_length );
        TEST_EQUAL( params.tag, params_tag );
        if( params_offset != 0 )
            TEST_EQUAL( params.p - input->x, params_offset );
        else
            TEST_ASSERT( params.p == NULL );
        TEST_EQUAL( params.len, (size_t) params_length );
        TEST_EQUAL( p - input->x, total_length );
    }

    ret = mbedtls_asn1_get_alg_null( &p, input->x + input->len, &oid );
    if( expected_result == 0 && params_offset == 0 )
    {
        TEST_EQUAL( oid.tag, MBEDTLS_ASN1_OID );
        TEST_EQUAL( oid.p - input->x, oid_offset );
        TEST_EQUAL( oid.len, (size_t) oid_length );
        TEST_EQUAL( p - input->x, total_length );
    }
    else
        TEST_ASSERT( ret != 0 );
exit:
    ;
}

void test_get_alg_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_get_alg( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ) );
}
void test_find_named_data( data_t *oid0, data_t *oid1, data_t *oid2, data_t *oid3,
                      data_t *needle, int from, int position )
{
    mbedtls_asn1_named_data nd[] ={
        { {0x06, oid0->len, oid0->x}, {0, 0, NULL}, NULL, 0 },
        { {0x06, oid1->len, oid1->x}, {0, 0, NULL}, NULL, 0 },
        { {0x06, oid2->len, oid2->x}, {0, 0, NULL}, NULL, 0 },
        { {0x06, oid3->len, oid3->x}, {0, 0, NULL}, NULL, 0 },
    };
    mbedtls_asn1_named_data *pointers[ARRAY_LENGTH( nd ) + 1];
    size_t i;
    mbedtls_asn1_named_data *found;

    for( i = 0; i < ARRAY_LENGTH( nd ); i++ )
        pointers[i] = &nd[i];
    pointers[ARRAY_LENGTH( nd )] = NULL;
    for( i = 0; i < ARRAY_LENGTH( nd ); i++ )
        nd[i].next = pointers[i+1];

    found = mbedtls_asn1_find_named_data( pointers[from],
                                          (const char *) needle->x,
                                          needle->len );
    TEST_ASSERT( found == pointers[position] );
exit:
    ;
}

void test_find_named_data_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_find_named_data( &data0, &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ) );
}
void test_free_named_data_null( )
{
    mbedtls_asn1_free_named_data( NULL );
    goto exit; /* Silence unused label warning */
exit:
    ;
}

void test_free_named_data_null_wrapper( void ** params )
{
    (void)params;

    test_free_named_data_null(  );
}
void test_free_named_data( int with_oid, int with_val, int with_next )
{
    mbedtls_asn1_named_data next =
        { {0x06, 0, NULL}, {0, 0xcafe, NULL}, NULL, 0 };
    mbedtls_asn1_named_data head =
        { {0x06, 0, NULL}, {0, 0, NULL}, NULL, 0 };

    if( with_oid )
        ASSERT_ALLOC( head.oid.p, 1 );
    if( with_val )
        ASSERT_ALLOC( head.val.p, 1 );
    if( with_next )
        head.next = &next;

    mbedtls_asn1_free_named_data( &head );
    TEST_ASSERT( head.oid.p == NULL );
    TEST_ASSERT( head.val.p == NULL );
    TEST_ASSERT( head.next == NULL );
    TEST_ASSERT( next.val.len == 0xcafe );

exit:
    mbedtls_free( head.oid.p );
    mbedtls_free( head.val.p );
}

void test_free_named_data_wrapper( void ** params )
{

    test_free_named_data( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
void test_free_named_data_list( int length )
{
    mbedtls_asn1_named_data *head = NULL;
    int i;

    for( i = 0; i < length; i++ )
    {
        mbedtls_asn1_named_data *new = NULL;
        ASSERT_ALLOC( new, sizeof( mbedtls_asn1_named_data ) );
        new->next = head;
        head = new;
    }

    mbedtls_asn1_free_named_data_list( &head );
    TEST_ASSERT( head == NULL );
    /* Most of the point of the test is that it doesn't leak memory.
     * So this test is only really useful under a memory leak detection
     * framework. */
exit:
    mbedtls_asn1_free_named_data_list( &head );
}

void test_free_named_data_list_wrapper( void ** params )
{

    test_free_named_data_list( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_ASN1_PARSE_C */

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

#if defined(MBEDTLS_ASN1_PARSE_C)

        case 0:
            {
                *out_value = MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 1:
            {
                *out_value = UNPREDICTABLE_RESULT;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ERR_ASN1_INVALID_DATA;
            }
            break;
        case 6:
            {
                *out_value = RET_TRAVERSE_STOP;
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

#if defined(MBEDTLS_ASN1_PARSE_C)

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

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_parse_prefixes_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_len_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_boolean_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_empty_integer_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_integer_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_enum_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_ASN1_PARSE_C) && defined(MBEDTLS_BIGNUM_C)
    test_get_mpi_too_large_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_bitstring_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_sequence_of_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_traverse_sequence_of_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_get_alg_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_find_named_data_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_free_named_data_null_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_free_named_data_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_ASN1_PARSE_C)
    test_free_named_data_list_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_asn1parse.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
