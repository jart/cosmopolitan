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
 * Test file      : ./test_suite_ecdsa.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_ecdsa.function
 *      Test suite data     : suites/test_suite_ecdsa.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_ECDSA_C)
#include "third_party/mbedtls/ecdsa.h"
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_ecdsa_invalid_param( )
{
    mbedtls_ecdsa_context ctx;
    mbedtls_ecp_keypair key;
    mbedtls_ecp_group grp;
    mbedtls_ecp_group_id valid_group = MBEDTLS_ECP_DP_SECP192R1;
    mbedtls_ecp_point P;
    mbedtls_md_type_t valid_md = MBEDTLS_MD_SHA256;
    mbedtls_mpi m;
    size_t slen;
    unsigned char buf[42] = { 0 };

    TEST_INVALID_PARAM( mbedtls_ecdsa_init( NULL ) );
    TEST_VALID_PARAM( mbedtls_ecdsa_free( NULL ) );

#if defined(MBEDTLS_ECP_RESTARTABLE)
    TEST_INVALID_PARAM( mbedtls_ecdsa_restart_init( NULL ) );
    TEST_VALID_PARAM( mbedtls_ecdsa_restart_free( NULL ) );
#endif /* MBEDTLS_ECP_RESTARTABLE */

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdsa_sign( NULL, &m, &m, &m,
                                                buf, sizeof( buf ),
                                                mbedtls_test_rnd_std_rand,
                                                NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdsa_sign( &grp, NULL, &m, &m,
                                                buf, sizeof( buf ),
                                                mbedtls_test_rnd_std_rand,
                                                NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_sign( &grp, &m, NULL, &m,
                                                buf, sizeof( buf ),
                                                mbedtls_test_rnd_std_rand,
                                                NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_sign( &grp, &m, &m, NULL,
                                                buf, sizeof( buf ),
                                                mbedtls_test_rnd_std_rand,
                                                NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_sign( &grp, &m, &m, &m,
                                                NULL, sizeof( buf ),
                                                mbedtls_test_rnd_std_rand,
                                                NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_sign( &grp, &m, &m, &m,
                                                buf, sizeof( buf ),
                                                NULL, NULL ) );

#if defined(MBEDTLS_ECDSA_DETERMINISTIC)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_sign_det_ext( NULL, &m, &m, &m,
                                                   buf, sizeof( buf ),
                                                   valid_md,
                                                   mbedtls_test_rnd_std_rand,
                                                   NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_sign_det_ext( &grp, NULL, &m, &m,
                                                   buf, sizeof( buf ),
                                                   valid_md,
                                                   mbedtls_test_rnd_std_rand,
                                                   NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_sign_det_ext( &grp, &m, NULL, &m,
                                                   buf, sizeof( buf ),
                                                   valid_md,
                                                   mbedtls_test_rnd_std_rand,
                                                   NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_sign_det_ext( &grp, &m, &m, NULL,
                                                   buf, sizeof( buf ),
                                                   valid_md,
                                                   mbedtls_test_rnd_std_rand,
                                                   NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_sign_det_ext( &grp, &m, &m, &m,
                                                   NULL, sizeof( buf ),
                                                   valid_md,
                                                   mbedtls_test_rnd_std_rand,
                                                   NULL ) );
#endif /* MBEDTLS_ECDSA_DETERMINISTIC */

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_verify( NULL,
                                                  buf, sizeof( buf ),
                                                  &P, &m, &m ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_verify( &grp,
                                                  NULL, sizeof( buf ),
                                                  &P, &m, &m ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_verify( &grp,
                                                  buf, sizeof( buf ),
                                                  NULL, &m, &m ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_verify( &grp,
                                                  buf, sizeof( buf ),
                                                  &P, NULL, &m ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_verify( &grp,
                                                  buf, sizeof( buf ),
                                                  &P, &m, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdsa_write_signature( NULL, valid_md, buf, sizeof( buf ),
                                        buf, &slen, mbedtls_test_rnd_std_rand,
                                        NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdsa_write_signature( &ctx, valid_md, NULL, sizeof( buf ),
                                        buf, &slen, mbedtls_test_rnd_std_rand,
                                        NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdsa_write_signature( &ctx, valid_md, buf, sizeof( buf ),
                                        NULL, &slen, mbedtls_test_rnd_std_rand,
                                        NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdsa_write_signature( &ctx, valid_md, buf, sizeof( buf ),
                                        buf, NULL, mbedtls_test_rnd_std_rand,
                                        NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdsa_write_signature_restartable( NULL, valid_md, buf,
                                                    sizeof( buf ), buf, &slen,
                                                    mbedtls_test_rnd_std_rand,
                                                    NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdsa_write_signature_restartable( &ctx, valid_md, NULL,
                                                    sizeof( buf ), buf, &slen,
                                                    mbedtls_test_rnd_std_rand,
                                                    NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdsa_write_signature_restartable( &ctx, valid_md, buf,
                                                    sizeof( buf ), NULL, &slen,
                                                    mbedtls_test_rnd_std_rand,
                                                    NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdsa_write_signature_restartable( &ctx, valid_md, buf,
                                                    sizeof( buf ), buf, NULL,
                                                    mbedtls_test_rnd_std_rand,
                                                    NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_read_signature( NULL,
                                                        buf, sizeof( buf ),
                                                        buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_read_signature( &ctx,
                                                        NULL, sizeof( buf ),
                                                        buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_read_signature( &ctx,
                                                        buf, sizeof( buf ),
                                                        NULL, sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_read_signature_restartable( NULL,
                                                        buf, sizeof( buf ),
                                                        buf, sizeof( buf ),
                                                        NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_read_signature_restartable( &ctx,
                                                        NULL, sizeof( buf ),
                                                        buf, sizeof( buf ),
                                                        NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_read_signature_restartable( &ctx,
                                                        buf, sizeof( buf ),
                                                        NULL, sizeof( buf ),
                                                        NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_genkey( NULL, valid_group,
                                             mbedtls_test_rnd_std_rand,
                                             NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_genkey( &ctx, valid_group,
                                                  NULL, NULL ) );


    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_from_keypair( NULL, &key ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                       mbedtls_ecdsa_from_keypair( &ctx, NULL ) );

exit:
    return;
}

void test_ecdsa_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_ecdsa_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_ecdsa_prim_random( int id )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point Q;
    mbedtls_mpi d, r, s;
    mbedtls_test_rnd_pseudo_info rnd_info;
    unsigned char buf[MBEDTLS_MD_MAX_SIZE];

    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &Q );
    mbedtls_mpi_init( &d ); mbedtls_mpi_init( &r ); mbedtls_mpi_init( &s );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );
    memset( buf, 0, sizeof( buf ) );

    /* prepare material for signature */
    TEST_ASSERT( mbedtls_test_rnd_pseudo_rand( &rnd_info,
                                               buf, sizeof( buf ) ) == 0 );
    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );
    TEST_ASSERT( mbedtls_ecp_gen_keypair( &grp, &d, &Q,
                                          &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info ) == 0 );

    TEST_ASSERT( mbedtls_ecdsa_sign( &grp, &r, &s, &d, buf, sizeof( buf ),
                                     &mbedtls_test_rnd_pseudo_rand,
                                     &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecdsa_verify( &grp, buf, sizeof( buf ), &Q, &r, &s ) == 0 );

exit:
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &Q );
    mbedtls_mpi_free( &d ); mbedtls_mpi_free( &r ); mbedtls_mpi_free( &s );
}

void test_ecdsa_prim_random_wrapper( void ** params )
{

    test_ecdsa_prim_random( *( (int *) params[0] ) );
}
void test_ecdsa_prim_test_vectors( int id, char * d_str, char * xQ_str,
                              char * yQ_str, data_t * rnd_buf,
                              data_t * hash, char * r_str, char * s_str,
                              int result )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point Q;
    mbedtls_mpi d, r, s, r_check, s_check;
    mbedtls_test_rnd_buf_info rnd_info;

    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &Q );
    mbedtls_mpi_init( &d ); mbedtls_mpi_init( &r ); mbedtls_mpi_init( &s );
    mbedtls_mpi_init( &r_check ); mbedtls_mpi_init( &s_check );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );
    TEST_ASSERT( mbedtls_ecp_point_read_string( &Q, 16, xQ_str, yQ_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &d, 16, d_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &r_check, 16, r_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &s_check, 16, s_str ) == 0 );
    rnd_info.buf = rnd_buf->x;
    rnd_info.length = rnd_buf->len;

    /* Fix rnd_buf->x by shifting it left if necessary */
    if( grp.nbits % 8 != 0 )
    {
        unsigned char shift = 8 - ( grp.nbits % 8 );
        size_t i;

        for( i = 0; i < rnd_info.length - 1; i++ )
            rnd_buf->x[i] = rnd_buf->x[i] << shift | rnd_buf->x[i+1] >> ( 8 - shift );

        rnd_buf->x[rnd_info.length-1] <<= shift;
    }

    TEST_ASSERT( mbedtls_ecdsa_sign( &grp, &r, &s, &d, hash->x, hash->len,
                 mbedtls_test_rnd_buffer_rand, &rnd_info ) == result );

    if ( result == 0)
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &r, &r_check ) == 0 );
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &s, &s_check ) == 0 );

        TEST_ASSERT( mbedtls_ecdsa_verify( &grp, hash->x, hash->len, &Q, &r_check, &s_check ) == 0 );

        TEST_ASSERT( mbedtls_mpi_sub_int( &r, &r, 1 ) == 0 );
        TEST_ASSERT( mbedtls_mpi_add_int( &s, &s, 1 ) == 0 );

        TEST_ASSERT( mbedtls_ecdsa_verify( &grp, hash->x, hash->len,
                     &Q, &r, &s_check ) == MBEDTLS_ERR_ECP_VERIFY_FAILED );
        TEST_ASSERT( mbedtls_ecdsa_verify( &grp, hash->x, hash->len,
                     &Q, &r_check, &s ) == MBEDTLS_ERR_ECP_VERIFY_FAILED );
        TEST_ASSERT( mbedtls_ecdsa_verify( &grp, hash->x, hash->len,
                     &grp.G, &r_check, &s_check ) == MBEDTLS_ERR_ECP_VERIFY_FAILED );
    }

exit:
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &Q );
    mbedtls_mpi_free( &d ); mbedtls_mpi_free( &r ); mbedtls_mpi_free( &s );
    mbedtls_mpi_free( &r_check ); mbedtls_mpi_free( &s_check );
}

void test_ecdsa_prim_test_vectors_wrapper( void ** params )
{
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_ecdsa_prim_test_vectors( *( (int *) params[0] ), (char *) params[1], (char *) params[2], (char *) params[3], &data4, &data6, (char *) params[8], (char *) params[9], *( (int *) params[10] ) );
}
#if defined(MBEDTLS_ECDSA_DETERMINISTIC)
void test_ecdsa_det_test_vectors( int id, char * d_str, int md_alg, char * msg,
                             char * r_str, char * s_str )
{
    mbedtls_ecp_group grp;
    mbedtls_mpi d, r, s, r_check, s_check;
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];
    size_t hlen;
    const mbedtls_md_info_t *md_info;

    mbedtls_ecp_group_init( &grp );
    mbedtls_mpi_init( &d ); mbedtls_mpi_init( &r ); mbedtls_mpi_init( &s );
    mbedtls_mpi_init( &r_check ); mbedtls_mpi_init( &s_check );
    memset( hash, 0, sizeof( hash ) );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &d, 16, d_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &r_check, 16, r_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &s_check, 16, s_str ) == 0 );

    md_info = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md_info != NULL );
    hlen = mbedtls_md_get_size( md_info );
    TEST_ASSERT( mbedtls_md( md_info, (const unsigned char *) msg,
                 strlen( msg ), hash ) == 0 );

    TEST_ASSERT(
                mbedtls_ecdsa_sign_det_ext( &grp, &r, &s, &d, hash, hlen,
                                            md_alg, mbedtls_test_rnd_std_rand,
                                            NULL )
                == 0 );

    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &r, &r_check ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &s, &s_check ) == 0 );

exit:
    mbedtls_ecp_group_free( &grp );
    mbedtls_mpi_free( &d ); mbedtls_mpi_free( &r ); mbedtls_mpi_free( &s );
    mbedtls_mpi_free( &r_check ); mbedtls_mpi_free( &s_check );
}

void test_ecdsa_det_test_vectors_wrapper( void ** params )
{

    test_ecdsa_det_test_vectors( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], (char *) params[4], (char *) params[5] );
}
#endif /* MBEDTLS_ECDSA_DETERMINISTIC */
#if defined(MBEDTLS_SHA256_C)
void test_ecdsa_write_read_random( int id )
{
    mbedtls_ecdsa_context ctx;
    mbedtls_test_rnd_pseudo_info rnd_info;
    unsigned char hash[32];
    unsigned char sig[200];
    size_t sig_len, i;

    mbedtls_ecdsa_init( &ctx );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );
    memset( hash, 0, sizeof( hash ) );
    memset( sig, 0x2a, sizeof( sig ) );

    /* prepare material for signature */
    TEST_ASSERT( mbedtls_test_rnd_pseudo_rand( &rnd_info,
                                               hash, sizeof( hash ) ) == 0 );

    /* generate signing key */
    TEST_ASSERT( mbedtls_ecdsa_genkey( &ctx, id,
                                       &mbedtls_test_rnd_pseudo_rand,
                                       &rnd_info ) == 0 );

    /* generate and write signature, then read and verify it */
    TEST_ASSERT( mbedtls_ecdsa_write_signature( &ctx, MBEDTLS_MD_SHA256,
                 hash, sizeof( hash ),
                 sig, &sig_len, &mbedtls_test_rnd_pseudo_rand,
                 &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecdsa_read_signature( &ctx, hash, sizeof( hash ),
                 sig, sig_len ) == 0 );

    /* check we didn't write past the announced length */
    for( i = sig_len; i < sizeof( sig ); i++ )
        TEST_ASSERT( sig[i] == 0x2a );

    /* try verification with invalid length */
    TEST_ASSERT( mbedtls_ecdsa_read_signature( &ctx, hash, sizeof( hash ),
                 sig, sig_len - 1 ) != 0 );
    TEST_ASSERT( mbedtls_ecdsa_read_signature( &ctx, hash, sizeof( hash ),
                 sig, sig_len + 1 ) != 0 );

    /* try invalid sequence tag */
    sig[0]++;
    TEST_ASSERT( mbedtls_ecdsa_read_signature( &ctx, hash, sizeof( hash ),
                 sig, sig_len ) != 0 );
    sig[0]--;

    /* try modifying r */
    sig[10]++;
    TEST_ASSERT( mbedtls_ecdsa_read_signature( &ctx, hash, sizeof( hash ),
                 sig, sig_len ) == MBEDTLS_ERR_ECP_VERIFY_FAILED );
    sig[10]--;

    /* try modifying s */
    sig[sig_len - 1]++;
    TEST_ASSERT( mbedtls_ecdsa_read_signature( &ctx, hash, sizeof( hash ),
                 sig, sig_len ) == MBEDTLS_ERR_ECP_VERIFY_FAILED );
    sig[sig_len - 1]--;

exit:
    mbedtls_ecdsa_free( &ctx );
}

void test_ecdsa_write_read_random_wrapper( void ** params )
{

    test_ecdsa_write_read_random( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_ECP_RESTARTABLE)
void test_ecdsa_read_restart( int id, data_t *pk, data_t *hash, data_t *sig,
                         int max_ops, int min_restart, int max_restart )
{
    mbedtls_ecdsa_context ctx;
    mbedtls_ecdsa_restart_ctx rs_ctx;
    int ret, cnt_restart;

    mbedtls_ecdsa_init( &ctx );
    mbedtls_ecdsa_restart_init( &rs_ctx );

    TEST_ASSERT( mbedtls_ecp_group_load( &ctx.grp, id ) == 0 );
    TEST_ASSERT( mbedtls_ecp_point_read_binary( &ctx.grp, &ctx.Q,
                                                pk->x, pk->len ) == 0 );

    mbedtls_ecp_set_max_ops( max_ops );

    cnt_restart = 0;
    do {
        ret = mbedtls_ecdsa_read_signature_restartable( &ctx,
                            hash->x, hash->len, sig->x, sig->len, &rs_ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( cnt_restart >= min_restart );
    TEST_ASSERT( cnt_restart <= max_restart );

    /* try modifying r */

    TEST_ASSERT( sig->len > 10 );
    sig->x[10]++;
    do {
        ret = mbedtls_ecdsa_read_signature_restartable( &ctx,
                            hash->x, hash->len, sig->x, sig->len, &rs_ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS );
    TEST_ASSERT( ret == MBEDTLS_ERR_ECP_VERIFY_FAILED );
    sig->x[10]--;

    /* try modifying s */
    sig->x[sig->len - 1]++;
    do {
        ret = mbedtls_ecdsa_read_signature_restartable( &ctx,
                            hash->x, hash->len, sig->x, sig->len, &rs_ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS );
    TEST_ASSERT( ret == MBEDTLS_ERR_ECP_VERIFY_FAILED );
    sig->x[sig->len - 1]--;

    /* Do we leak memory when aborting an operation?
     * This test only makes sense when we actually restart */
    if( min_restart > 0 )
    {
        ret = mbedtls_ecdsa_read_signature_restartable( &ctx,
                            hash->x, hash->len, sig->x, sig->len, &rs_ctx );
        TEST_ASSERT( ret == MBEDTLS_ERR_ECP_IN_PROGRESS );
    }

exit:
    mbedtls_ecdsa_free( &ctx );
    mbedtls_ecdsa_restart_free( &rs_ctx );
}

void test_ecdsa_read_restart_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};

    test_ecdsa_read_restart( *( (int *) params[0] ), &data1, &data3, &data5, *( (int *) params[7] ), *( (int *) params[8] ), *( (int *) params[9] ) );
}
#endif /* MBEDTLS_ECP_RESTARTABLE */
#if defined(MBEDTLS_ECP_RESTARTABLE)
#if defined(MBEDTLS_ECDSA_DETERMINISTIC)
void test_ecdsa_write_restart( int id, char *d_str, int md_alg,
                          char *msg, data_t *sig_check,
                          int max_ops, int min_restart, int max_restart )
{
    int ret, cnt_restart;
    mbedtls_ecdsa_restart_ctx rs_ctx;
    mbedtls_ecdsa_context ctx;
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];
    unsigned char sig[MBEDTLS_ECDSA_MAX_LEN];
    size_t hlen, slen;
    const mbedtls_md_info_t *md_info;

    mbedtls_ecdsa_restart_init( &rs_ctx );
    mbedtls_ecdsa_init( &ctx );
    memset( hash, 0, sizeof( hash ) );
    memset( sig, 0, sizeof( sig ) );

    TEST_ASSERT( mbedtls_ecp_group_load( &ctx.grp, id ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &ctx.d, 16, d_str ) == 0 );

    md_info = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md_info != NULL );

    hlen = mbedtls_md_get_size( md_info );
    TEST_ASSERT( mbedtls_md( md_info,
                             (const unsigned char *) msg, strlen( msg ),
                             hash ) == 0 );

    mbedtls_ecp_set_max_ops( max_ops );

    slen = sizeof( sig );
    cnt_restart = 0;
    do {
        ret = mbedtls_ecdsa_write_signature_restartable( &ctx,
                md_alg, hash, hlen, sig, &slen, NULL, NULL, &rs_ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( slen == sig_check->len );
    TEST_ASSERT( memcmp( sig, sig_check->x, slen ) == 0 );

    TEST_ASSERT( cnt_restart >= min_restart );
    TEST_ASSERT( cnt_restart <= max_restart );

    /* Do we leak memory when aborting an operation?
     * This test only makes sense when we actually restart */
    if( min_restart > 0 )
    {
        ret = mbedtls_ecdsa_write_signature_restartable( &ctx,
                md_alg, hash, hlen, sig, &slen, NULL, NULL, &rs_ctx );
        TEST_ASSERT( ret == MBEDTLS_ERR_ECP_IN_PROGRESS );
    }

exit:
    mbedtls_ecdsa_restart_free( &rs_ctx );
    mbedtls_ecdsa_free( &ctx );
}

void test_ecdsa_write_restart_wrapper( void ** params )
{
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_ecdsa_write_restart( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], &data4, *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ) );
}
#endif /* MBEDTLS_ECDSA_DETERMINISTIC */
#endif /* MBEDTLS_ECP_RESTARTABLE */
#endif /* MBEDTLS_ECDSA_C */

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

#if defined(MBEDTLS_ECDSA_C)

        case 0:
            {
                *out_value = MBEDTLS_ECP_DP_SECP192R1;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ECP_DP_SECP224R1;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ECP_DP_SECP256R1;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ECP_DP_SECP384R1;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ECP_DP_SECP521R1;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_MD_SHA224;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_MD_SHA384;
            }
            break;
        case 9:
            {
                *out_value = MBEDTLS_MD_SHA512;
            }
            break;
        case 10:
            {
                *out_value = MBEDTLS_ERR_ECP_INVALID_KEY;
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

#if defined(MBEDTLS_ECDSA_C)

        case 0:
            {
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
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

#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_ecdsa_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_ECDSA_C)
    test_ecdsa_prim_random_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_ECDSA_C)
    test_ecdsa_prim_test_vectors_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECDSA_DETERMINISTIC)
    test_ecdsa_det_test_vectors_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_SHA256_C)
    test_ecdsa_write_read_random_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    test_ecdsa_read_restart_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE) && defined(MBEDTLS_ECDSA_DETERMINISTIC)
    test_ecdsa_write_restart_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_ecdsa.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
