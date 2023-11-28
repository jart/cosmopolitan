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
#include "libc/testlib/testlib.h"
#include "third_party/mbedtls/test/test.inc"
/*
 * *** THIS FILE WAS MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * Test file      : ./test_suite_x509parse.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_x509parse.function
 *      Test suite data     : suites/test_suite_x509parse.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_BIGNUM_C)
#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/x509.h"
#include "third_party/mbedtls/x509_crt.h"
#include "third_party/mbedtls/x509_crl.h"
#include "third_party/mbedtls/x509_csr.h"
#include "third_party/mbedtls/pem.h"
#include "third_party/mbedtls/oid.h"
#include "third_party/mbedtls/base64.h"

#if MBEDTLS_X509_MAX_INTERMEDIATE_CA > 19
#error "The value of MBEDTLS_X509_MAX_INTERMEDIATE_C is larger \
than the current threshold 19. To test larger values, please \
adapt the script tests/data_files/dir-max/long.sh."
#endif

/* Test-only profile allowing all digests, PK algorithms, and curves. */
const mbedtls_x509_crt_profile profile_all =
{
    0xFFFFFFFF, /* Any MD        */
    0xFFFFFFFF, /* Any PK alg    */
    0xFFFFFFFF, /* Any curve     */
    1024,
};

/* Profile for backward compatibility. Allows SHA-1, unlike the default
   profile. */
const mbedtls_x509_crt_profile compat_profile =
{
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA1 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_RIPEMD160 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA224 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA256 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA384 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 ),
    0xFFFFFFF, /* Any PK alg    */
    0xFFFFFFF, /* Any curve     */
    1024,
};

const mbedtls_x509_crt_profile profile_rsa3072 =
{
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA256 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA384 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 ),
    MBEDTLS_X509_ID_FLAG( MBEDTLS_PK_RSA ),
    0,
    3072,
};

const mbedtls_x509_crt_profile profile_sha512 =
{
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 ),
    0xFFFFFFF, /* Any PK alg    */
    0xFFFFFFF, /* Any curve     */
    1024,
};

int verify_none( void *data, mbedtls_x509_crt *crt, int certificate_depth, uint32_t *flags )
{
    ((void) data);
    ((void) crt);
    ((void) certificate_depth);
    *flags |= MBEDTLS_X509_BADCERT_OTHER;

    return 0;
}

int verify_all( void *data, mbedtls_x509_crt *crt, int certificate_depth, uint32_t *flags )
{
    ((void) data);
    ((void) crt);
    ((void) certificate_depth);
    *flags = 0;

    return 0;
}

#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
int ca_callback_fail( void *data, mbedtls_x509_crt const *child, mbedtls_x509_crt **candidates )
{
    ((void) data);
    ((void) child);
    ((void) candidates);

    return -1;
}

int ca_callback( void *data, mbedtls_x509_crt const *child,
                 mbedtls_x509_crt **candidates )
{
    int ret = 0;
    mbedtls_x509_crt *ca = (mbedtls_x509_crt *) data;
    mbedtls_x509_crt *first;

    /* This is a test-only implementation of the CA callback
     * which always returns the entire list of trusted certificates.
     * Production implementations managing a large number of CAs
     * should use an efficient presentation and lookup for the
     * set of trusted certificates (such as a hashtable) and only
     * return those trusted certificates which satisfy basic
     * parental checks, such as the matching of child `Issuer`
     * and parent `Subject` field. */
    ((void) child);

    first = mbedtls_calloc( 1, sizeof( mbedtls_x509_crt ) );
    if( first == NULL )
    {
        ret = -1;
        goto exit;
    }
    mbedtls_x509_crt_init( first );

    if( mbedtls_x509_crt_parse_der( first, ca->raw.p, ca->raw.len ) != 0 )
    {
        ret = -1;
        goto exit;
    }

    while( ca->next != NULL )
    {
        ca = ca->next;
        if( mbedtls_x509_crt_parse_der( first, ca->raw.p, ca->raw.len ) != 0 )
        {
            ret = -1;
            goto exit;
        }
    }

exit:

    if( ret != 0 )
    {
        mbedtls_x509_crt_free( first );
        mbedtls_free( first );
        first = NULL;
    }

    *candidates = first;
    return( ret );
}
#endif /* MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK */

int verify_fatal( void *data, mbedtls_x509_crt *crt, int certificate_depth, uint32_t *flags )
{
    int *levels = (int *) data;

    ((void) crt);
    ((void) certificate_depth);

    /* Simulate a fatal error in the callback */
    if( *levels & ( 1 << certificate_depth ) )
    {
        *flags |= ( 1 << certificate_depth );
        return( -1 - certificate_depth );
    }

    return( 0 );
}

/* strsep() not available on Windows */
char *mystrsep(char **stringp, const char *delim)
{
    const char *p;
    char *ret = *stringp;

    if( *stringp == NULL )
        return( NULL );

    for( ; ; (*stringp)++ )
    {
        if( **stringp == '\0' )
        {
            *stringp = NULL;
            goto done;
        }

        for( p = delim; *p != '\0'; p++ )
            if( **stringp == *p )
            {
                **stringp = '\0';
                (*stringp)++;
                goto done;
            }
    }

done:
    return( ret );
}

#if defined(MBEDTLS_X509_CRT_PARSE_C)
typedef struct {
    char buf[512];
    char *p;
} verify_print_context;

void verify_print_init( verify_print_context *ctx )
{
    memset( ctx, 0, sizeof( verify_print_context ) );
    ctx->p = ctx->buf;
}

int verify_print( void *data, mbedtls_x509_crt *crt, int certificate_depth, uint32_t *flags )
{
    int ret;
    verify_print_context *ctx = (verify_print_context *) data;
    char *p = ctx->p;
    size_t n = ctx->buf + sizeof( ctx->buf ) - ctx->p;
    ((void) flags);

    ret = mbedtls_snprintf( p, n, "depth %d - serial ", certificate_depth );
    MBEDTLS_X509_SAFE_SNPRINTF;

    ret = mbedtls_x509_serial_gets( p, n, &crt->serial );
    MBEDTLS_X509_SAFE_SNPRINTF;

    ret = mbedtls_snprintf( p, n, " - subject " );
    MBEDTLS_X509_SAFE_SNPRINTF;

    ret = mbedtls_x509_dn_gets( p, n, &crt->subject );
    MBEDTLS_X509_SAFE_SNPRINTF;

    ret = mbedtls_snprintf( p, n, " - flags 0x%08x\n", *flags );
    MBEDTLS_X509_SAFE_SNPRINTF;

    ctx->p = p;

    return( 0 );
}

int verify_parse_san( mbedtls_x509_subject_alternative_name *san,
                      char **buf, size_t *size )
{
    int ret;
    size_t i;
    char *p = *buf;
    size_t n = *size;

    ret = mbedtls_snprintf( p, n, "type : %d", san->type );
    MBEDTLS_X509_SAFE_SNPRINTF;

    switch( san->type )
    {
       case( MBEDTLS_X509_SAN_OTHER_NAME ):
           ret = mbedtls_snprintf( p, n, "\notherName :");
           MBEDTLS_X509_SAFE_SNPRINTF;

           if( MBEDTLS_OID_CMP( MBEDTLS_OID_ON_HW_MODULE_NAME,
                    &san->san.other_name.value.hardware_module_name.oid ) != 0 )
           {
               ret = mbedtls_snprintf( p, n, " hardware module name :" );
               MBEDTLS_X509_SAFE_SNPRINTF;
               ret = mbedtls_snprintf( p, n, " hardware type : " );
               MBEDTLS_X509_SAFE_SNPRINTF;

               ret = mbedtls_oid_get_numeric_string( p, n,
                          &san->san.other_name.value.hardware_module_name.oid );
               MBEDTLS_X509_SAFE_SNPRINTF;

               ret = mbedtls_snprintf( p, n, ", hardware serial number : " );
               MBEDTLS_X509_SAFE_SNPRINTF;

               if( san->san.other_name.value.hardware_module_name.val.len >= n )
               {
                   *p = '\0';
                   return( MBEDTLS_ERR_X509_BUFFER_TOO_SMALL );
               }

               for( i=0; i < san->san.other_name.value.hardware_module_name.val.len; i++ )
               {
                   *p++ = san->san.other_name.value.hardware_module_name.val.p[i];
               }
               n -= san->san.other_name.value.hardware_module_name.val.len;
            }
        break;/* MBEDTLS_OID_ON_HW_MODULE_NAME */
        case(  MBEDTLS_X509_SAN_DNS_NAME ):
            ret = mbedtls_snprintf( p, n, "\ndNSName : " );
            MBEDTLS_X509_SAFE_SNPRINTF;
            if( san->san.unstructured_name.len >= n )
            {
                *p = '\0';
                return( MBEDTLS_ERR_X509_BUFFER_TOO_SMALL );
            }
            n -= san->san.unstructured_name.len;
            for( i = 0; i < san->san.unstructured_name.len; i++ )
                *p++ = san->san.unstructured_name.p[i];
        break;/* MBEDTLS_X509_SAN_DNS_NAME */

        default:
        /*
         * Should not happen.
         */
        return( -1 );
    }
    ret = mbedtls_snprintf( p, n, "\n" );
    MBEDTLS_X509_SAFE_SNPRINTF;

    *size = n;
    *buf = p;

    return( 0 );
}

int parse_crt_ext_cb( void *p_ctx, mbedtls_x509_crt const *crt, mbedtls_x509_buf const *oid,
                      int critical, const unsigned char *cp, const unsigned char *end )
{
    ( void ) crt;
    ( void ) critical;
    mbedtls_x509_buf *new_oid = (mbedtls_x509_buf *)p_ctx;
    if( oid->tag == MBEDTLS_ASN1_OID &&
        MBEDTLS_OID_CMP( MBEDTLS_OID_CERTIFICATE_POLICIES, oid ) == 0 )
    {
        /* Handle unknown certificate policy */
        int ret, parse_ret = 0;
        size_t len;
        unsigned char **p = (unsigned char **)&cp;

        /* Get main sequence tag */
        ret = mbedtls_asn1_get_tag( p, end, &len,
                                 MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE );
        if( ret != 0 )
            return( MBEDTLS_ERR_X509_INVALID_EXTENSIONS + ret );

        if( *p + len != end )
            return( MBEDTLS_ERR_X509_INVALID_EXTENSIONS +
                    MBEDTLS_ERR_ASN1_LENGTH_MISMATCH );

        /*
         * Cannot be an empty sequence.
         */
        if( len == 0 )
            return( MBEDTLS_ERR_X509_INVALID_EXTENSIONS +
                    MBEDTLS_ERR_ASN1_LENGTH_MISMATCH );

        while( *p < end )
        {
            const unsigned char *policy_end;

            /*
             * Get the policy sequence
             */
            if( ( ret = mbedtls_asn1_get_tag( p, end, &len,
                    MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
                return( MBEDTLS_ERR_X509_INVALID_EXTENSIONS + ret );

            policy_end = *p + len;

            if( ( ret = mbedtls_asn1_get_tag( p, policy_end, &len,
                                              MBEDTLS_ASN1_OID ) ) != 0 )
                return( MBEDTLS_ERR_X509_INVALID_EXTENSIONS + ret );

            /*
             * Recognize exclusively the policy with OID 1
             */
            if( len != 1 || *p[0] != 1 )
                parse_ret = MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE;

            *p += len;

           /*
            * If there is an optional qualifier, then *p < policy_end
            * Check the Qualifier len to verify it doesn't exceed policy_end.
            */
            if( *p < policy_end )
            {
                if( ( ret = mbedtls_asn1_get_tag( p, policy_end, &len,
                         MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
                    return( MBEDTLS_ERR_X509_INVALID_EXTENSIONS + ret );
                /*
                 * Skip the optional policy qualifiers.
                 */
                *p += len;
            }

            if( *p != policy_end )
                return( MBEDTLS_ERR_X509_INVALID_EXTENSIONS +
                        MBEDTLS_ERR_ASN1_LENGTH_MISMATCH );
        }

        if( *p != end )
            return( MBEDTLS_ERR_X509_INVALID_EXTENSIONS +
                    MBEDTLS_ERR_ASN1_LENGTH_MISMATCH );

        return( parse_ret );
    }
    else if( new_oid != NULL && new_oid->tag == oid->tag && new_oid->len == oid->len &&
             timingsafe_bcmp( new_oid->p, oid->p, oid->len ) == 0 )
        return( 0 );
    else
        return( MBEDTLS_ERR_X509_INVALID_EXTENSIONS + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_x509_parse_san( char * crt_file, char * result_str )
{
    int ret;
    mbedtls_x509_crt   crt;
    mbedtls_x509_subject_alternative_name san;
    mbedtls_x509_sequence *cur = NULL;
    char buf[2000];
    char *p = buf;
    size_t n = sizeof( buf );

    mbedtls_x509_crt_init( &crt );
    memset( buf, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );

    if( crt.ext_types & MBEDTLS_X509_EXT_SUBJECT_ALT_NAME )
    {
        cur = &crt.subject_alt_names;
        while( cur != NULL )
        {
            ret = mbedtls_x509_parse_subject_alt_name( &cur->buf, &san );
            TEST_ASSERT( ret == 0 || ret == MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE );
            /*
             * If san type not supported, ignore.
             */
            if( ret == 0)
                TEST_ASSERT( verify_parse_san( &san, &p, &n ) == 0 );
            cur = cur->next;
        }
    }

    TEST_ASSERT_STREQ( buf, result_str );

exit:

    mbedtls_x509_crt_free( &crt );
}

void test_x509_parse_san_wrapper( void ** params )
{

    test_x509_parse_san( (char *) params[0], (char *) params[1] );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */

#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_x509_cert_info( char * crt_file, char * result_str )
{
    mbedtls_x509_crt   crt;
    char buf[2000];
    int res;

    mbedtls_x509_crt_init( &crt );
    memset( buf, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );
    res = mbedtls_x509_crt_info( buf, 2000, "", &crt );

    TEST_ASSERT( res != -1 );
    TEST_ASSERT( res != -2 );

    TEST_ASSERT_STREQ( buf, result_str );

exit:
    mbedtls_x509_crt_free( &crt );
}

void test_x509_cert_info_wrapper( void ** params )
{

    test_x509_cert_info( (char *) params[0], (char *) params[1] );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRL_PARSE_C)
void test_mbedtls_x509_crl_info( char * crl_file, char * result_str )
{
    mbedtls_x509_crl   crl;
    char buf[2000];
    int res;

    mbedtls_x509_crl_init( &crl );
    memset( buf, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crl_parse_file( &crl, crl_file ) == 0 );
    res = mbedtls_x509_crl_info( buf, 2000, "", &crl );

    TEST_ASSERT( res != -1 );
    TEST_ASSERT( res != -2 );

    TEST_ASSERT_STREQ( buf, result_str );

exit:
    mbedtls_x509_crl_free( &crl );
}

void test_mbedtls_x509_crl_info_wrapper( void ** params )
{

    test_mbedtls_x509_crl_info( (char *) params[0], (char *) params[1] );
}
#endif /* MBEDTLS_X509_CRL_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRL_PARSE_C)
void test_mbedtls_x509_crl_parse( char * crl_file, int result )
{
    mbedtls_x509_crl   crl;
    char buf[2000];

    mbedtls_x509_crl_init( &crl );
    memset( buf, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crl_parse_file( &crl, crl_file ) == result );

exit:
    mbedtls_x509_crl_free( &crl );
}

void test_mbedtls_x509_crl_parse_wrapper( void ** params )
{

    test_mbedtls_x509_crl_parse( (char *) params[0], *( (int *) params[1] ) );
}
#endif /* MBEDTLS_X509_CRL_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CSR_PARSE_C)
void test_mbedtls_x509_csr_info( char * csr_file, char * result_str )
{
    mbedtls_x509_csr   csr;
    char buf[2000];
    int res;

    mbedtls_x509_csr_init( &csr );
    memset( buf, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_csr_parse_file( &csr, csr_file ) == 0 );
    res = mbedtls_x509_csr_info( buf, 2000, "", &csr );

    TEST_ASSERT( res != -1 );
    TEST_ASSERT( res != -2 );

    TEST_ASSERT_STREQ( buf, result_str );

exit:
    mbedtls_x509_csr_free( &csr );
}

void test_mbedtls_x509_csr_info_wrapper( void ** params )
{

    test_mbedtls_x509_csr_info( (char *) params[0], (char *) params[1] );
}
#endif /* MBEDTLS_X509_CSR_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_x509_verify_info( int flags, char * prefix, char * result_str )
{
    char buf[2000];
    int res;

    memset( buf, 0, sizeof( buf ) );

    res = mbedtls_x509_crt_verify_info( buf, sizeof( buf ), prefix, flags );

    TEST_ASSERT( res >= 0 );

    TEST_ASSERT_STREQ( buf, result_str );
exit:
    ;
}

void test_x509_verify_info_wrapper( void ** params )
{

    test_x509_verify_info( *( (int *) params[0] ), (char *) params[1], (char *) params[2] );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_X509_CRL_PARSE_C)
#if defined(MBEDTLS_ECP_RESTARTABLE)
#if defined(MBEDTLS_ECDSA_C)
void test_x509_verify_restart( char *crt_file, char *ca_file,
                          int result, int flags_result,
                          int max_ops, int min_restart, int max_restart )
{
    int ret, cnt_restart;
    mbedtls_x509_crt_restart_ctx rs_ctx;
    mbedtls_x509_crt crt;
    mbedtls_x509_crt ca;
    uint32_t flags = 0;

    /*
     * See comments on ecp_test_vect_restart() for op count precision.
     *
     * For reference, with mbed TLS 2.6 and default settings:
     * - ecdsa_verify() for P-256:  ~  6700
     * - ecdsa_verify() for P-384:  ~ 18800
     * - x509_verify() for server5 -> test-ca2:             ~ 18800
     * - x509_verify() for server10 -> int-ca3 -> int-ca2:  ~ 25500
     */

    mbedtls_x509_crt_restart_init( &rs_ctx );
    mbedtls_x509_crt_init( &crt );
    mbedtls_x509_crt_init( &ca );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );
    TEST_ASSERT( mbedtls_x509_crt_parse_file( &ca, ca_file ) == 0 );

    mbedtls_ecp_set_max_ops( max_ops );

    cnt_restart = 0;
    do {
        ret = mbedtls_x509_crt_verify_restartable( &crt, &ca, NULL,
                &mbedtls_x509_crt_profile_default, NULL, &flags,
                NULL, NULL, &rs_ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart );

    TEST_ASSERT( ret == result );
    TEST_ASSERT( flags == (uint32_t) flags_result );

    TEST_ASSERT( cnt_restart >= min_restart );
    TEST_ASSERT( cnt_restart <= max_restart );

    /* Do we leak memory when aborting? */
    ret = mbedtls_x509_crt_verify_restartable( &crt, &ca, NULL,
            &mbedtls_x509_crt_profile_default, NULL, &flags,
            NULL, NULL, &rs_ctx );
    TEST_ASSERT( ret == result || ret == MBEDTLS_ERR_ECP_IN_PROGRESS );

exit:
    mbedtls_x509_crt_restart_free( &rs_ctx );
    mbedtls_x509_crt_free( &crt );
    mbedtls_x509_crt_free( &ca );
}

void test_x509_verify_restart_wrapper( void ** params )
{

    test_x509_verify_restart( (char *) params[0], (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ) );
}
#endif /* MBEDTLS_ECDSA_C */
#endif /* MBEDTLS_ECP_RESTARTABLE */
#endif /* MBEDTLS_X509_CRL_PARSE_C */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_X509_CRL_PARSE_C)
void test_x509_verify( char *crt_file, char *ca_file, char *crl_file,
                  char *cn_name_str, int result, int flags_result,
                  char *profile_str,
                  char *verify_callback )
{
    mbedtls_x509_crt   crt;
    mbedtls_x509_crt   ca;
    mbedtls_x509_crl    crl;
    uint32_t         flags = 0;
    int         res;
    int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *) = NULL;
    char *      cn_name = NULL;
    const mbedtls_x509_crt_profile *profile;

    mbedtls_x509_crt_init( &crt );
    mbedtls_x509_crt_init( &ca );
    mbedtls_x509_crl_init( &crl );

    USE_PSA_INIT( );

    if( strcmp( cn_name_str, "NULL" ) != 0 )
        cn_name = cn_name_str;

    if( strcmp( profile_str, "" ) == 0 )
        profile = &mbedtls_x509_crt_profile_default;
    else if( strcmp( profile_str, "next" ) == 0 )
        profile = &mbedtls_x509_crt_profile_next;
    else if( strcmp( profile_str, "suite_b" ) == 0 )
        profile = &mbedtls_x509_crt_profile_suiteb;
    else if( strcmp( profile_str, "compat" ) == 0 )
        profile = &compat_profile;
    else if( strcmp( profile_str, "all" ) == 0 )
        profile = &profile_all;
    else
        TEST_ASSERT( "Unknown algorithm profile" == 0 );

    if( strcmp( verify_callback, "NULL" ) == 0 )
        f_vrfy = NULL;
    else if( strcmp( verify_callback, "verify_none" ) == 0 )
        f_vrfy = verify_none;
    else if( strcmp( verify_callback, "verify_all" ) == 0 )
        f_vrfy = verify_all;
    else
        TEST_ASSERT( "No known verify callback selected" == 0 );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );
    TEST_ASSERT( mbedtls_x509_crt_parse_file( &ca, ca_file ) == 0 );
    TEST_ASSERT( mbedtls_x509_crl_parse_file( &crl, crl_file ) == 0 );

    res = mbedtls_x509_crt_verify_with_profile( &crt, &ca, &crl, profile, cn_name, &flags, f_vrfy, NULL );

    TEST_EQUAL( res, result );
    TEST_ASSERT( flags == (uint32_t)( flags_result ) );

#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    /* CRLs aren't supported with CA callbacks, so skip the CA callback
     * version of the test if CRLs are in use. */
    if( crl_file == NULL || strcmp( crl_file, "" ) == 0 )
    {
        flags = 0;

        res = mbedtls_x509_crt_verify_with_ca_cb( &crt, ca_callback, &ca, profile, cn_name, &flags, f_vrfy, NULL );

        TEST_ASSERT( res == ( result ) );
        TEST_ASSERT( flags == (uint32_t)( flags_result ) );
    }
#endif /* MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK */
exit:
    mbedtls_x509_crt_free( &crt );
    mbedtls_x509_crt_free( &ca );
    mbedtls_x509_crl_free( &crl );
    USE_PSA_DONE( );
}

void test_x509_verify_wrapper( void ** params )
{

    test_x509_verify( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], *( (int *) params[4] ), *( (int *) params[5] ), (char *) params[6], (char *) params[7] );
}
#endif /* MBEDTLS_X509_CRL_PARSE_C */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */

#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_X509_CRL_PARSE_C)
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
void test_x509_verify_ca_cb_failure( char *crt_file, char *ca_file, char *name,
                                int exp_ret )
{
    int ret;
    mbedtls_x509_crt crt;
    mbedtls_x509_crt ca;
    uint32_t flags = 0;

    mbedtls_x509_crt_init( &crt );
    mbedtls_x509_crt_init( &ca );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );
    TEST_ASSERT( mbedtls_x509_crt_parse_file( &ca, ca_file ) == 0 );

    if( strcmp( name, "NULL" ) == 0 )
        name = NULL;

    ret = mbedtls_x509_crt_verify_with_ca_cb( &crt, ca_callback_fail, &ca,
                                              &compat_profile, name, &flags,
                                              NULL, NULL );

    TEST_ASSERT( ret == exp_ret );
    TEST_ASSERT( flags == (uint32_t)( -1 ) );
exit:
    mbedtls_x509_crt_free( &crt );
    mbedtls_x509_crt_free( &ca );
}

void test_x509_verify_ca_cb_failure_wrapper( void ** params )
{

    test_x509_verify_ca_cb_failure( (char *) params[0], (char *) params[1], (char *) params[2], *( (int *) params[3] ) );
}
#endif /* MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK */
#endif /* MBEDTLS_X509_CRL_PARSE_C */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */

#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_x509_verify_callback( char *crt_file, char *ca_file, char *name,
                           int exp_ret, char *exp_vrfy_out )
{
    int ret;
    mbedtls_x509_crt crt;
    mbedtls_x509_crt ca;
    uint32_t flags = 0;
    verify_print_context vrfy_ctx;

    mbedtls_x509_crt_init( &crt );
    mbedtls_x509_crt_init( &ca );
    verify_print_init( &vrfy_ctx );

    USE_PSA_INIT( );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );
    TEST_ASSERT( mbedtls_x509_crt_parse_file( &ca, ca_file ) == 0 );

    if( strcmp( name, "NULL" ) == 0 )
        name = NULL;

    ret = mbedtls_x509_crt_verify_with_profile( &crt, &ca, NULL,
                                                &compat_profile,
                                                name, &flags,
                                                verify_print, &vrfy_ctx );

    TEST_ASSERT( ret == exp_ret );
    TEST_ASSERT( strcmp( vrfy_ctx.buf, exp_vrfy_out ) == 0 );

exit:
    mbedtls_x509_crt_free( &crt );
    mbedtls_x509_crt_free( &ca );
    USE_PSA_DONE( );
}

void test_x509_verify_callback_wrapper( void ** params )
{

    test_x509_verify_callback( (char *) params[0], (char *) params[1], (char *) params[2], *( (int *) params[3] ), (char *) params[4] );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_mbedtls_x509_dn_gets( char * crt_file, char * entity, char * result_str )
{
    mbedtls_x509_crt   crt;
    char buf[2000];
    int res = 0;

    mbedtls_x509_crt_init( &crt );
    memset( buf, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );
    if( strcmp( entity, "subject" ) == 0 )
        res =  mbedtls_x509_dn_gets( buf, 2000, &crt.subject );
    else if( strcmp( entity, "issuer" ) == 0 )
        res =  mbedtls_x509_dn_gets( buf, 2000, &crt.issuer );
    else
        TEST_ASSERT( "Unknown entity" == 0 );

    TEST_ASSERT( res != -1 );
    TEST_ASSERT( res != -2 );

    TEST_ASSERT_STREQ( buf, result_str );

exit:
    mbedtls_x509_crt_free( &crt );
}

void test_mbedtls_x509_dn_gets_wrapper( void ** params )
{

    test_mbedtls_x509_dn_gets( (char *) params[0], (char *) params[1], (char *) params[2] );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_mbedtls_x509_time_is_past( char * crt_file, char * entity, int result )
{
    mbedtls_x509_crt   crt;

    mbedtls_x509_crt_init( &crt );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );

    if( strcmp( entity, "valid_from" ) == 0 )
        TEST_ASSERT( mbedtls_x509_time_is_past( &crt.valid_from ) == result );
    else if( strcmp( entity, "valid_to" ) == 0 )
        TEST_ASSERT( mbedtls_x509_time_is_past( &crt.valid_to ) == result );
    else
        TEST_ASSERT( "Unknown entity" == 0 );

exit:
    mbedtls_x509_crt_free( &crt );
}

void test_mbedtls_x509_time_is_past_wrapper( void ** params )
{

    test_mbedtls_x509_time_is_past( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_mbedtls_x509_time_is_future( char * crt_file, char * entity, int result )
{
    mbedtls_x509_crt   crt;

    mbedtls_x509_crt_init( &crt );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );

    if( strcmp( entity, "valid_from" ) == 0 )
        TEST_ASSERT( mbedtls_x509_time_is_future( &crt.valid_from ) == result );
    else if( strcmp( entity, "valid_to" ) == 0 )
        TEST_ASSERT( mbedtls_x509_time_is_future( &crt.valid_to ) == result );
    else
        TEST_ASSERT( "Unknown entity" == 0 );

exit:
    mbedtls_x509_crt_free( &crt );
}

void test_mbedtls_x509_time_is_future_wrapper( void ** params )
{

    test_mbedtls_x509_time_is_future( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_FS_IO)
void test_x509parse_crt_file( char * crt_file, int result )
{
    mbedtls_x509_crt crt;

    mbedtls_x509_crt_init( &crt );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == result );

exit:
    mbedtls_x509_crt_free( &crt );
}

void test_x509parse_crt_file_wrapper( void ** params )
{

    test_x509parse_crt_file( (char *) params[0], *( (int *) params[1] ) );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_x509parse_crt( data_t * buf, char * result_str, int result )
{
    mbedtls_x509_crt   crt;
    unsigned char output[2000];
    int res;

    mbedtls_x509_crt_init( &crt );
    memset( output, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crt_parse_der( &crt, buf->x, buf->len ) == ( result ) );
    if( ( result ) == 0 )
    {
        res = mbedtls_x509_crt_info( (char *) output, 2000, "", &crt );

        TEST_ASSERT( res != -1 );
        TEST_ASSERT( res != -2 );

        TEST_ASSERT_STREQ( (char *) output, result_str );
    }

    mbedtls_x509_crt_free( &crt );
    mbedtls_x509_crt_init( &crt );
    memset( output, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crt_parse_der_nocopy( &crt, buf->x, buf->len ) == ( result ) );
    if( ( result ) == 0 )
    {
        res = mbedtls_x509_crt_info( (char *) output, 2000, "", &crt );

        TEST_ASSERT( res != -1 );
        TEST_ASSERT( res != -2 );

        TEST_ASSERT_STREQ( (char *) output, result_str );
    }

    mbedtls_x509_crt_free( &crt );
    mbedtls_x509_crt_init( &crt );
    memset( output, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crt_parse_der_with_ext_cb( &crt, buf->x, buf->len, 0, NULL, NULL ) == ( result ) );
    if( ( result ) == 0 )
    {
        res = mbedtls_x509_crt_info( (char *) output, 2000, "", &crt );

        TEST_ASSERT( res != -1 );
        TEST_ASSERT( res != -2 );

        TEST_ASSERT_STREQ( (char *) output, result_str );
    }

    mbedtls_x509_crt_free( &crt );
    mbedtls_x509_crt_init( &crt );
    memset( output, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crt_parse_der_with_ext_cb( &crt, buf->x, buf->len, 1, NULL, NULL ) == ( result ) );
    if( ( result ) == 0 )
    {
        res = mbedtls_x509_crt_info( (char *) output, 2000, "", &crt );

        TEST_ASSERT( res != -1 );
        TEST_ASSERT( res != -2 );

        TEST_ASSERT_STREQ( (char *) output, result_str );
    }

exit:
    mbedtls_x509_crt_free( &crt );
}

void test_x509parse_crt_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_x509parse_crt( &data0, (char *) params[2], *( (int *) params[3] ) );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_x509parse_crt_cb( data_t * buf, char * result_str, int result )
{
    mbedtls_x509_crt   crt;
    mbedtls_x509_buf   oid;
    unsigned char output[2000];
    int res;

    oid.tag = MBEDTLS_ASN1_OID;
    oid.len = MBEDTLS_OID_SIZE(MBEDTLS_OID_PKIX "\x01\x1F");
    oid.p = (unsigned char *)MBEDTLS_OID_PKIX "\x01\x1F";

    mbedtls_x509_crt_init( &crt );
    memset( output, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crt_parse_der_with_ext_cb( &crt, buf->x, buf->len, 0, parse_crt_ext_cb, &oid ) == ( result ) );
    if( ( result ) == 0 )
    {
        res = mbedtls_x509_crt_info( (char *) output, 2000, "", &crt );

        TEST_ASSERT( res != -1 );
        TEST_ASSERT( res != -2 );

        TEST_ASSERT_STREQ( (char *) output, result_str );
    }

    mbedtls_x509_crt_free( &crt );
    mbedtls_x509_crt_init( &crt );
    memset( output, 0, 2000 );

    TEST_ASSERT( mbedtls_x509_crt_parse_der_with_ext_cb( &crt, buf->x, buf->len, 1, parse_crt_ext_cb, &oid ) == ( result ) );
    if( ( result ) == 0 )
    {
        res = mbedtls_x509_crt_info( (char *) output, 2000, "", &crt );

        TEST_ASSERT( res != -1 );
        TEST_ASSERT( res != -2 );

        TEST_ASSERT_STREQ( (char *) output, result_str );
    }

exit:
    mbedtls_x509_crt_free( &crt );
}

void test_x509parse_crt_cb_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_x509parse_crt_cb( &data0, (char *) params[2], *( (int *) params[3] ) );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRL_PARSE_C)
void test_x509parse_crl( data_t * buf, char * result_str, int result )
{
    mbedtls_x509_crl   crl;
    unsigned char output[2000];
    int res;

    mbedtls_x509_crl_init( &crl );
    memset( output, 0, 2000 );


    TEST_ASSERT( mbedtls_x509_crl_parse( &crl, buf->x, buf->len ) == ( result ) );
    if( ( result ) == 0 )
    {
        res = mbedtls_x509_crl_info( (char *) output, 2000, "", &crl );

        TEST_ASSERT( res != -1 );
        TEST_ASSERT( res != -2 );

        TEST_ASSERT_STREQ( (char *) output, result_str );
    }

exit:
    mbedtls_x509_crl_free( &crl );
}

void test_x509parse_crl_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_x509parse_crl( &data0, (char *) params[2], *( (int *) params[3] ) );
}
#endif /* MBEDTLS_X509_CRL_PARSE_C */
#if defined(MBEDTLS_X509_CSR_PARSE_C)
void test_mbedtls_x509_csr_parse( data_t * csr_der, char * ref_out, int ref_ret )
{
    mbedtls_x509_csr csr;
    char my_out[1000];
    int my_ret;

    mbedtls_x509_csr_init( &csr );
    memset( my_out, 0, sizeof( my_out ) );

    my_ret = mbedtls_x509_csr_parse_der( &csr, csr_der->x, csr_der->len );
    TEST_ASSERT( my_ret == ref_ret );

    if( ref_ret == 0 )
    {
        size_t my_out_len = mbedtls_x509_csr_info( my_out, sizeof( my_out ), "", &csr );
        TEST_ASSERT( my_out_len == strlen( ref_out ) );
        TEST_ASSERT( strcmp( my_out, ref_out ) == 0 );
    }

exit:
    mbedtls_x509_csr_free( &csr );
}

void test_mbedtls_x509_csr_parse_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_mbedtls_x509_csr_parse( &data0, (char *) params[2], *( (int *) params[3] ) );
}
#endif /* MBEDTLS_X509_CSR_PARSE_C */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_mbedtls_x509_crt_parse_path( char * crt_path, int ret, int nb_crt )
{
    mbedtls_x509_crt chain, *cur;
    int i;

    mbedtls_x509_crt_init( &chain );

    TEST_ASSERT( mbedtls_x509_crt_parse_path( &chain, crt_path ) == ret );

    /* Check how many certs we got */
    for( i = 0, cur = &chain; cur != NULL; cur = cur->next )
        if( cur->raw.p != NULL )
            i++;

    TEST_ASSERT( i == nb_crt );

exit:
    mbedtls_x509_crt_free( &chain );
}

void test_mbedtls_x509_crt_parse_path_wrapper( void ** params )
{

    test_mbedtls_x509_crt_parse_path( (char *) params[0], *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_mbedtls_x509_crt_verify_max( char *ca_file, char *chain_dir, int nb_int,
                                       int ret_chk, int flags_chk )
{
    char file_buf[128];
    int ret;
    uint32_t flags;
    mbedtls_x509_crt trusted, chain;

    /*
     * We expect chain_dir to contain certificates 00.crt, 01.crt, etc.
     * with NN.crt signed by NN-1.crt
     */

    mbedtls_x509_crt_init( &trusted );
    mbedtls_x509_crt_init( &chain );

    USE_PSA_INIT( );

    /* Load trusted root */
    TEST_ASSERT( mbedtls_x509_crt_parse_file( &trusted, ca_file ) == 0 );

    /* Load a chain with nb_int intermediates (from 01 to nb_int),
     * plus one "end-entity" cert (nb_int + 1) */
    ret = mbedtls_snprintf( file_buf, sizeof file_buf, "%s/c%02d.pem", chain_dir,
                                                            nb_int + 1 );
    TEST_ASSERT( ret > 0 && (size_t) ret < sizeof file_buf );
    TEST_ASSERT( mbedtls_x509_crt_parse_file( &chain, file_buf ) == 0 );

    /* Try to verify that chain */
    ret = mbedtls_x509_crt_verify( &chain, &trusted, NULL, NULL, &flags,
                                   NULL, NULL );
    TEST_ASSERT( ret == ret_chk );
    TEST_ASSERT( flags == (uint32_t) flags_chk );

exit:
    mbedtls_x509_crt_free( &chain );
    mbedtls_x509_crt_free( &trusted );
    USE_PSA_DONE( );
}

void test_mbedtls_x509_crt_verify_max_wrapper( void ** params )
{

    test_mbedtls_x509_crt_verify_max( (char *) params[0], (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_mbedtls_x509_crt_verify_chain(  char *chain_paths, char *trusted_ca,
                                     int flags_result, int result,
                                     char *profile_name, int vrfy_fatal_lvls )
{
    char* act;
    uint32_t flags;
    int res;
    mbedtls_x509_crt trusted, chain;
    const mbedtls_x509_crt_profile *profile = NULL;

    mbedtls_x509_crt_init( &chain );
    mbedtls_x509_crt_init( &trusted );

    USE_PSA_INIT( );

    while( ( act = mystrsep( &chain_paths, " " ) ) != NULL )
        TEST_ASSERT( mbedtls_x509_crt_parse_file( &chain, act ) == 0 );
    TEST_ASSERT( mbedtls_x509_crt_parse_file( &trusted, trusted_ca ) == 0 );

    if( strcmp( profile_name, "" ) == 0 )
        profile = &mbedtls_x509_crt_profile_default;
    else if( strcmp( profile_name, "next" ) == 0 )
        profile = &mbedtls_x509_crt_profile_next;
    else if( strcmp( profile_name, "suiteb" ) == 0 )
        profile = &mbedtls_x509_crt_profile_suiteb;
    else if( strcmp( profile_name, "rsa3072" ) == 0 )
        profile = &profile_rsa3072;
    else if( strcmp( profile_name, "sha512" ) == 0 )
        profile = &profile_sha512;

    res = mbedtls_x509_crt_verify_with_profile( &chain, &trusted, NULL, profile,
            NULL, &flags, verify_fatal, &vrfy_fatal_lvls );

    TEST_ASSERT( res == ( result ) );
    TEST_ASSERT( flags == (uint32_t)( flags_result ) );

exit:
    mbedtls_x509_crt_free( &trusted );
    mbedtls_x509_crt_free( &chain );
    USE_PSA_DONE( );
}

void test_mbedtls_x509_crt_verify_chain_wrapper( void ** params )
{

    test_mbedtls_x509_crt_verify_chain( (char *) params[0], (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ) );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_X509_USE_C)
void test_x509_oid_desc( data_t * buf, char * ref_desc )
{
    mbedtls_x509_buf oid;
    const char *desc = NULL;
    int ret;


    oid.tag = MBEDTLS_ASN1_OID;
    oid.p   = buf->x;
    oid.len   = buf->len;

    ret = mbedtls_oid_get_extended_key_usage( &oid, &desc );

    if( strcmp( ref_desc, "notfound" ) == 0 )
    {
        TEST_ASSERT( ret != 0 );
        TEST_ASSERT( desc == NULL );
    }
    else
    {
        TEST_ASSERT( ret == 0 );
        TEST_ASSERT( desc != NULL );
        TEST_ASSERT( strcmp( desc, ref_desc ) == 0 );
    }
exit:
    ;
}

void test_x509_oid_desc_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_x509_oid_desc( &data0, (char *) params[2] );
}
#endif /* MBEDTLS_X509_USE_C */
#if defined(MBEDTLS_X509_USE_C)
void test_x509_oid_numstr( data_t * oid_buf, char * numstr, int blen, int ret )
{
    mbedtls_x509_buf oid;
    char num_buf[100];

    memset( num_buf, 0x2a, sizeof num_buf );

    oid.tag = MBEDTLS_ASN1_OID;
    oid.p   = oid_buf->x;
    oid.len   = oid_buf->len;

    TEST_ASSERT( (size_t) blen <= sizeof num_buf );

    TEST_ASSERT( mbedtls_oid_get_numeric_string( num_buf, blen, &oid ) == ret );

    if( ret >= 0 )
    {
        TEST_ASSERT( num_buf[ret] == 0 );
        TEST_ASSERT( strcmp( num_buf, numstr ) == 0 );
    }
exit:
    ;
}

void test_x509_oid_numstr_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_x509_oid_numstr( &data0, (char *) params[2], *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_X509_USE_C */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_X509_CHECK_KEY_USAGE)
void test_x509_check_key_usage( char * crt_file, int usage, int ret )
{
    mbedtls_x509_crt crt;

    mbedtls_x509_crt_init( &crt );

    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );

    TEST_ASSERT( mbedtls_x509_crt_check_key_usage( &crt, usage ) == ret );

exit:
    mbedtls_x509_crt_free( &crt );
}

void test_x509_check_key_usage_wrapper( void ** params )
{

    test_x509_check_key_usage( (char *) params[0], *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_X509_CHECK_KEY_USAGE */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE)
void test_x509_check_extended_key_usage( char * crt_file, data_t * oid, int ret
                                    )
{
    mbedtls_x509_crt crt;

    mbedtls_x509_crt_init( &crt );


    TEST_ASSERT( mbedtls_x509_crt_parse_file( &crt, crt_file ) == 0 );

    TEST_ASSERT( mbedtls_x509_crt_check_extended_key_usage( &crt, (const char *)oid->x, oid->len ) == ret );

exit:
    mbedtls_x509_crt_free( &crt );
}

void test_x509_check_extended_key_usage_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_x509_check_extended_key_usage( (char *) params[0], &data1, *( (int *) params[3] ) );
}
#endif /* MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_X509_USE_C)
void test_x509_get_time( int tag, char * time_str, int ret, int year, int mon,
                    int day, int hour, int min, int sec )
{
    mbedtls_x509_time time;
    unsigned char buf[21];
    unsigned char* start = buf;
    unsigned char* end = buf;

    memset( &time, 0x00, sizeof( time ) );
    *end = (unsigned char)tag; end++;
    *end = strlen( time_str );
    TEST_ASSERT( *end < 20 );
    end++;
    memcpy( end, time_str, (size_t)*(end - 1) );
    end += *(end - 1);

    TEST_ASSERT( mbedtls_x509_get_time( &start, end, &time ) == ret );
    if( ret == 0 )
    {
        TEST_ASSERT( year == time.year );
        TEST_ASSERT( mon  == time.mon  );
        TEST_ASSERT( day  == time.day  );
        TEST_ASSERT( hour == time.hour );
        TEST_ASSERT( min  == time.min  );
        TEST_ASSERT( sec  == time.sec  );
    }
exit:
    ;
}

void test_x509_get_time_wrapper( void ** params )
{

    test_x509_get_time( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ) );
}
#endif /* MBEDTLS_X509_USE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_X509_RSASSA_PSS_SUPPORT)
void test_x509_parse_rsassa_pss_params( data_t * params, int params_tag,
                                   int ref_msg_md, int ref_mgf_md,
                                   int ref_salt_len, int ref_ret )
{
    int my_ret;
    mbedtls_x509_buf buf;
    mbedtls_md_type_t my_msg_md, my_mgf_md;
    int my_salt_len;

    buf.p = params->x;
    buf.len = params->len;
    buf.tag = params_tag;

    my_ret = mbedtls_x509_get_rsassa_pss_params( &buf, &my_msg_md, &my_mgf_md,
                                                 &my_salt_len );

    TEST_ASSERT( my_ret == ref_ret );

    if( ref_ret == 0 )
    {
        TEST_ASSERT( my_msg_md == (mbedtls_md_type_t) ref_msg_md );
        TEST_ASSERT( my_mgf_md == (mbedtls_md_type_t) ref_mgf_md );
        TEST_ASSERT( my_salt_len == ref_salt_len );
    }

exit:
    ;;
}

void test_x509_parse_rsassa_pss_params_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_x509_parse_rsassa_pss_params( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ) );
}
#endif /* MBEDTLS_X509_RSASSA_PSS_SUPPORT */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_SELF_TEST)
void test_x509_selftest(  )
{
    TEST_ASSERT( mbedtls_x509_self_test( 1 ) == 0 );
exit:
    ;
}

void test_x509_selftest_wrapper( void ** params )
{
    (void)params;

    test_x509_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
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
                *out_value = MBEDTLS_ERR_PEM_NO_HEADER_FOOTER_PRESENT;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_EXTENSIONS + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_X509_BADCERT_MISSING;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_X509_BADCERT_EXPIRED | MBEDTLS_X509_BADCRL_EXPIRED;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_X509_BADCERT_OTHER | 0x80000000;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ERR_X509_CERT_VERIFY_FAILED;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_X509_BADCERT_REVOKED | MBEDTLS_X509_BADCRL_EXPIRED;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_X509_BADCERT_REVOKED | MBEDTLS_X509_BADCRL_FUTURE;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_X509_BADCERT_REVOKED | MBEDTLS_X509_BADCRL_EXPIRED | MBEDTLS_X509_BADCERT_CN_MISMATCH;
            }
            break;
        case 9:
            {
                *out_value = MBEDTLS_X509_BADCERT_REVOKED | MBEDTLS_X509_BADCRL_FUTURE | MBEDTLS_X509_BADCERT_CN_MISMATCH;
            }
            break;
        case 10:
            {
                *out_value = MBEDTLS_X509_BADCRL_EXPIRED;
            }
            break;
        case 11:
            {
                *out_value = MBEDTLS_X509_BADCRL_FUTURE;
            }
            break;
        case 12:
            {
                *out_value = MBEDTLS_X509_BADCERT_REVOKED;
            }
            break;
        case 13:
            {
                *out_value = MBEDTLS_X509_BADCERT_REVOKED | MBEDTLS_X509_BADCERT_CN_MISMATCH;
            }
            break;
        case 14:
            {
                *out_value = MBEDTLS_X509_BADCERT_EXPIRED;
            }
            break;
        case 15:
            {
                *out_value = MBEDTLS_X509_BADCERT_FUTURE;
            }
            break;
        case 16:
            {
                *out_value = MBEDTLS_X509_BADCERT_NOT_TRUSTED;
            }
            break;
        case 17:
            {
                *out_value = MBEDTLS_X509_BADCERT_BAD_MD;
            }
            break;
        case 18:
            {
                *out_value = MBEDTLS_X509_BADCRL_BAD_MD | MBEDTLS_X509_BADCERT_BAD_MD;
            }
            break;
        case 19:
            {
                *out_value = MBEDTLS_X509_BADCERT_OTHER;
            }
            break;
        case 20:
            {
                *out_value = MBEDTLS_X509_BADCERT_CN_MISMATCH;
            }
            break;
        case 21:
            {
                *out_value = MBEDTLS_X509_BADCERT_CN_MISMATCH + MBEDTLS_X509_BADCERT_NOT_TRUSTED;
            }
            break;
        case 22:
            {
                *out_value = MBEDTLS_X509_BADCRL_NOT_TRUSTED;
            }
            break;
        case 23:
            {
                *out_value = MBEDTLS_X509_BADCERT_REVOKED|MBEDTLS_X509_BADCRL_FUTURE;
            }
            break;
        case 24:
            {
                *out_value = MBEDTLS_X509_BADCERT_BAD_MD|MBEDTLS_X509_BADCERT_BAD_PK|MBEDTLS_X509_BADCERT_BAD_KEY|MBEDTLS_X509_BADCRL_BAD_MD|MBEDTLS_X509_BADCRL_BAD_PK;
            }
            break;
        case 25:
            {
                *out_value = MBEDTLS_X509_BADCERT_BAD_PK;
            }
            break;
        case 26:
            {
                *out_value = MBEDTLS_X509_BADCERT_BAD_MD|MBEDTLS_X509_BADCRL_BAD_MD;
            }
            break;
        case 27:
            {
                *out_value = MBEDTLS_ERR_X509_FATAL_ERROR;
            }
            break;
        case 28:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_FORMAT;
            }
            break;
        case 29:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_FORMAT + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 30:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_FORMAT + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 31:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_FORMAT + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 32:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_SERIAL + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 33:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_VERSION + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 34:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_VERSION + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 35:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_VERSION + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 36:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_VERSION + MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
            }
            break;
        case 37:
            {
                *out_value = MBEDTLS_ERR_X509_UNKNOWN_VERSION;
            }
            break;
        case 38:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_SERIAL + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 39:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_SERIAL + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 40:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_ALG + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 41:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_ALG + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 42:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_ALG + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 43:
            {
                *out_value = MBEDTLS_ERR_X509_UNKNOWN_SIG_ALG + MBEDTLS_ERR_OID_NOT_FOUND;
            }
            break;
        case 44:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_ALG + MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
            }
            break;
        case 45:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_ALG;
            }
            break;
        case 46:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_NAME + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 47:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_NAME + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 48:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_NAME + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 49:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_NAME + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;;
            }
            break;
        case 50:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_NAME + MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
            }
            break;
        case 51:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_DATE + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 52:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_DATE + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 53:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_DATE + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 54:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_DATE;
            }
            break;
        case 55:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_DATE + MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
            }
            break;
        case 56:
            {
                *out_value = MBEDTLS_ERR_PK_KEY_INVALID_FORMAT + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 57:
            {
                *out_value = MBEDTLS_ERR_PK_KEY_INVALID_FORMAT + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 58:
            {
                *out_value = MBEDTLS_ERR_PK_KEY_INVALID_FORMAT + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 59:
            {
                *out_value = MBEDTLS_ERR_PK_INVALID_ALG + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 60:
            {
                *out_value = MBEDTLS_ERR_PK_INVALID_ALG + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 61:
            {
                *out_value = MBEDTLS_ERR_PK_INVALID_ALG + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 62:
            {
                *out_value = MBEDTLS_ERR_PK_UNKNOWN_PK_ALG;
            }
            break;
        case 63:
            {
                *out_value = MBEDTLS_ERR_PK_INVALID_PUBKEY + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 64:
            {
                *out_value = MBEDTLS_ERR_PK_INVALID_PUBKEY + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 65:
            {
                *out_value = MBEDTLS_ERR_PK_INVALID_PUBKEY + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 66:
            {
                *out_value = MBEDTLS_ERR_PK_INVALID_PUBKEY + MBEDTLS_ERR_ASN1_INVALID_DATA;
            }
            break;
        case 67:
            {
                *out_value = MBEDTLS_ERR_PK_INVALID_PUBKEY + MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
            }
            break;
        case 68:
            {
                *out_value = MBEDTLS_ERR_PK_INVALID_PUBKEY;
            }
            break;
        case 69:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_FORMAT + MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
            }
            break;
        case 70:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_EXTENSIONS + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 71:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_EXTENSIONS + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 72:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_EXTENSIONS + MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
            }
            break;
        case 73:
            {
                *out_value = MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE;
            }
            break;
        case 74:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_EXTENSIONS;
            }
            break;
        case 75:
            {
                *out_value = MBEDTLS_ERR_X509_SIG_MISMATCH;
            }
            break;
        case 76:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_SIGNATURE + MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 77:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_SIGNATURE + MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
            }
            break;
        case 78:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_SIGNATURE + MBEDTLS_ERR_ASN1_INVALID_LENGTH;
            }
            break;
        case 79:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_SIGNATURE + MBEDTLS_ERR_ASN1_INVALID_DATA;
            }
            break;
        case 80:
            {
                *out_value = MBEDTLS_ERR_X509_UNKNOWN_SIG_ALG;
            }
            break;
        case 81:
            {
                *out_value = MBEDTLS_ERR_ASN1_OUT_OF_DATA;
            }
            break;
        case 82:
            {
                *out_value = MBEDTLS_X509_MAX_INTERMEDIATE_CA;
            }
            break;
        case 83:
            {
                *out_value = MBEDTLS_X509_MAX_INTERMEDIATE_CA-1;
            }
            break;
        case 84:
            {
                *out_value = MBEDTLS_X509_MAX_INTERMEDIATE_CA+1;
            }
            break;
        case 85:
            {
                *out_value = -1;
            }
            break;
        case 86:
            {
                *out_value = MBEDTLS_ERR_X509_BAD_INPUT_DATA;
            }
            break;
        case 87:
            {
                *out_value = MBEDTLS_X509_BADCERT_BAD_MD|MBEDTLS_X509_BADCERT_BAD_PK|MBEDTLS_X509_BADCERT_BAD_KEY;
            }
            break;
        case 88:
            {
                *out_value = MBEDTLS_X509_BADCERT_BAD_PK|MBEDTLS_X509_BADCERT_BAD_KEY;
            }
            break;
        case 89:
            {
                *out_value = MBEDTLS_X509_BADCERT_BAD_MD|MBEDTLS_X509_BADCERT_BAD_KEY;
            }
            break;
        case 90:
            {
                *out_value = -2;
            }
            break;
        case 91:
            {
                *out_value = -4;
            }
            break;
        case 92:
            {
                *out_value = -3;
            }
            break;
        case 93:
            {
                *out_value = MBEDTLS_ERR_OID_BUF_TOO_SMALL;
            }
            break;
        case 94:
            {
                *out_value = MBEDTLS_X509_KU_DIGITAL_SIGNATURE|MBEDTLS_X509_KU_KEY_ENCIPHERMENT;
            }
            break;
        case 95:
            {
                *out_value = MBEDTLS_X509_KU_KEY_CERT_SIGN;
            }
            break;
        case 96:
            {
                *out_value = MBEDTLS_X509_KU_DIGITAL_SIGNATURE;
            }
            break;
        case 97:
            {
                *out_value = MBEDTLS_X509_KU_KEY_CERT_SIGN|MBEDTLS_X509_KU_CRL_SIGN;
            }
            break;
        case 98:
            {
                *out_value = MBEDTLS_X509_KU_KEY_ENCIPHERMENT|MBEDTLS_X509_KU_KEY_AGREEMENT;
            }
            break;
        case 99:
            {
                *out_value = MBEDTLS_X509_KU_DIGITAL_SIGNATURE|MBEDTLS_X509_KU_KEY_ENCIPHERMENT|MBEDTLS_X509_KU_DECIPHER_ONLY;
            }
            break;
        case 100:
            {
                *out_value = MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE;
            }
            break;
        case 101:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 102:
            {
                *out_value = MBEDTLS_ASN1_SEQUENCE;
            }
            break;
        case 103:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 104:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_ALG + MBEDTLS_ERR_ASN1_INVALID_DATA;
            }
            break;
        case 105:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_ALG + MBEDTLS_ERR_OID_NOT_FOUND;
            }
            break;
        case 106:
            {
                *out_value = MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE + MBEDTLS_ERR_OID_NOT_FOUND;
            }
            break;
        case 107:
            {
                *out_value = MBEDTLS_ERR_PEM_INVALID_DATA + MBEDTLS_ERR_BASE64_INVALID_CHARACTER;
            }
            break;
        case 108:
            {
                *out_value = MBEDTLS_ASN1_UTC_TIME;
            }
            break;
        case 109:
            {
                *out_value = MBEDTLS_ASN1_GENERALIZED_TIME;
            }
            break;
        case 110:
            {
                *out_value = MBEDTLS_ASN1_CONTEXT_SPECIFIC;
            }
            break;
        case 111:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_DATE+MBEDTLS_ERR_ASN1_UNEXPECTED_TAG;
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
#if defined(MBEDTLS_PEM_PARSE_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_RSA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_SHA1_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_MD2_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_MD4_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_MD5_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_SHA256_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_SHA512_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if !defined(MBEDTLS_SHA512_NO_SHA384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(MBEDTLS_X509_RSASSA_PSS_SUPPORT)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(MBEDTLS_ECDSA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 13:
            {
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 14:
            {
#if defined(MBEDTLS_X509_ALLOW_EXTENSIONS_NON_V3)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 15:
            {
#if defined(MBEDTLS_HAVE_TIME_DATE)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 16:
            {
#if defined(MBEDTLS_PKCS1_V15)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 17:
            {
#if defined(MBEDTLS_TLS_DEFAULT_ALLOW_SHA1_IN_CERTIFICATES)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 18:
            {
#if !defined(MBEDTLS_TLS_DEFAULT_ALLOW_SHA1_IN_CERTIFICATES)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 19:
            {
#if defined(MBEDTLS_X509_CHECK_KEY_USAGE)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 20:
            {
#if defined(MBEDTLS_ECP_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 21:
            {
#if !defined(MBEDTLS_HAVE_TIME_DATE)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 22:
            {
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 23:
            {
#if defined(MBEDTLS_CERTS_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 24:
            {
#if !defined(MBEDTLS_X509_ALLOW_EXTENSIONS_NON_V3)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 25:
            {
#if !defined(MBEDTLS_X509_ALLOW_UNSUPPORTED_CRITICAL_EXTENSION)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 26:
            {
#if defined(MBEDTLS_X509_USE_C)
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

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_x509_parse_san_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_x509_cert_info_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRL_PARSE_C)
    test_mbedtls_x509_crl_info_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRL_PARSE_C)
    test_mbedtls_x509_crl_parse_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CSR_PARSE_C)
    test_mbedtls_x509_csr_info_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_x509_verify_info_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_X509_CRL_PARSE_C) && defined(MBEDTLS_ECP_RESTARTABLE) && defined(MBEDTLS_ECDSA_C)
    test_x509_verify_restart_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_X509_CRL_PARSE_C)
    test_x509_verify_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_X509_CRL_PARSE_C) && defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    test_x509_verify_ca_cb_failure_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_x509_verify_callback_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_mbedtls_x509_dn_gets_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_mbedtls_x509_time_is_past_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_mbedtls_x509_time_is_future_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_FS_IO)
    test_x509parse_crt_file_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_x509parse_crt_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_x509parse_crt_cb_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_CRL_PARSE_C)
    test_x509parse_crl_wrapper,
#else
    NULL,
#endif
/* Function Id: 17 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_CSR_PARSE_C)
    test_mbedtls_x509_csr_parse_wrapper,
#else
    NULL,
#endif
/* Function Id: 18 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_mbedtls_x509_crt_parse_path_wrapper,
#else
    NULL,
#endif
/* Function Id: 19 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_mbedtls_x509_crt_verify_max_wrapper,
#else
    NULL,
#endif
/* Function Id: 20 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_mbedtls_x509_crt_verify_chain_wrapper,
#else
    NULL,
#endif
/* Function Id: 21 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_USE_C)
    test_x509_oid_desc_wrapper,
#else
    NULL,
#endif
/* Function Id: 22 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_USE_C)
    test_x509_oid_numstr_wrapper,
#else
    NULL,
#endif
/* Function Id: 23 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_X509_CHECK_KEY_USAGE)
    test_x509_check_key_usage_wrapper,
#else
    NULL,
#endif
/* Function Id: 24 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE)
    test_x509_check_extended_key_usage_wrapper,
#else
    NULL,
#endif
/* Function Id: 25 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_USE_C)
    test_x509_get_time_wrapper,
#else
    NULL,
#endif
/* Function Id: 26 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_X509_RSASSA_PSS_SUPPORT)
    test_x509_parse_rsassa_pss_params_wrapper,
#else
    NULL,
#endif
/* Function Id: 27 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_SELF_TEST)
    test_x509_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_x509parse.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
