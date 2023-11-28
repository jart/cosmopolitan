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
 * Test file      : ./test_suite_ecdh.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_ecdh.function
 *      Test suite data     : suites/test_suite_ecdh.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_ECDH_C)
#include "third_party/mbedtls/ecdh.h"

static int load_public_key( int grp_id, data_t *point,
                            mbedtls_ecp_keypair *ecp )
{
    int ok = 0;
    TEST_ASSERT( mbedtls_ecp_group_load( &ecp->grp, grp_id ) == 0 );
    TEST_ASSERT( mbedtls_ecp_point_read_binary( &ecp->grp,
                                                &ecp->Q,
                                                point->x,
                                                point->len ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_pubkey( &ecp->grp,
                                           &ecp->Q ) == 0 );
    ok = 1;
exit:
    return( ok );
}

static int load_private_key( int grp_id, data_t *private_key,
                             mbedtls_ecp_keypair *ecp,
                             mbedtls_test_rnd_pseudo_info *rnd_info )
{
    int ok = 0;
    TEST_ASSERT( mbedtls_ecp_read_key( grp_id, ecp,
                                       private_key->x,
                                       private_key->len ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_privkey( &ecp->grp, &ecp->d ) == 0 );
    /* Calculate the public key from the private key. */
    TEST_ASSERT( mbedtls_ecp_mul( &ecp->grp, &ecp->Q, &ecp->d,
                                  &ecp->grp.G,
                                  &mbedtls_test_rnd_pseudo_rand,
                                  rnd_info ) == 0 );
    ok = 1;
exit:
    return( ok );
}

void test_ecdh_valid_param( )
{
    TEST_VALID_PARAM( mbedtls_ecdh_free( NULL ) );
exit:
    ;
}

void test_ecdh_valid_param_wrapper( void ** params )
{
    (void)params;

    test_ecdh_valid_param(  );
}
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_ecdh_invalid_param( )
{
    mbedtls_ecp_group grp;
    mbedtls_ecdh_context ctx;
    mbedtls_mpi m;
    mbedtls_ecp_point P;
    mbedtls_ecp_keypair kp;
    size_t olen;
    unsigned char buf[42] = { 0 };
    const unsigned char *buf_null = NULL;
    size_t const buflen = sizeof( buf );
    int invalid_side = 42;
    mbedtls_ecp_group_id valid_grp = MBEDTLS_ECP_DP_SECP192R1;

    TEST_INVALID_PARAM( mbedtls_ecdh_init( NULL ) );

#if defined(MBEDTLS_ECP_RESTARTABLE)
    TEST_INVALID_PARAM( mbedtls_ecdh_enable_restart( NULL ) );
#endif /* MBEDTLS_ECP_RESTARTABLE */

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_gen_public( NULL, &m, &P,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_gen_public( &grp, NULL, &P,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_gen_public( &grp, &m, NULL,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_gen_public( &grp, &m, &P,
                                                     NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_compute_shared( NULL, &m, &P, &m,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_compute_shared( &grp, NULL, &P, &m,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_compute_shared( &grp, &m, NULL, &m,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_compute_shared( &grp, &m, &P, NULL,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_setup( NULL, valid_grp ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_make_params( NULL, &olen, buf, buflen,
                                   mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_make_params( &ctx, NULL, buf, buflen,
                                   mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_make_params( &ctx, &olen, NULL, buflen,
                                   mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_make_params( &ctx, &olen, buf, buflen, NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_read_params( NULL,
                                                  (const unsigned char**) &buf,
                                                  buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_read_params( &ctx, &buf_null,
                                                      buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_read_params( &ctx, NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_read_params( &ctx,
                                                  (const unsigned char**) &buf,
                                                  NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_get_params( NULL, &kp,
                                                     MBEDTLS_ECDH_OURS ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_get_params( &ctx, NULL,
                                                     MBEDTLS_ECDH_OURS ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_get_params( &ctx, &kp,
                                                     invalid_side ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_make_public( NULL, &olen, buf, buflen,
                                   mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_make_public( &ctx, NULL, buf, buflen,
                                   mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_make_public( &ctx, &olen, NULL, buflen,
                                   mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_make_public( &ctx, &olen, buf, buflen, NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_read_public( NULL, buf, buflen ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecdh_read_public( &ctx, NULL, buflen ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_calc_secret( NULL, &olen, buf, buflen,
                                   mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_calc_secret( &ctx, NULL, buf, buflen,
                                   mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
         mbedtls_ecdh_calc_secret( &ctx, &olen, NULL, buflen,
                                   mbedtls_test_rnd_std_rand, NULL ) );

exit:
    return;
}

void test_ecdh_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_ecdh_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_ecdh_primitive_random( int id )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point qA, qB;
    mbedtls_mpi dA, dB, zA, zB;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &qA ); mbedtls_ecp_point_init( &qB );
    mbedtls_mpi_init( &dA ); mbedtls_mpi_init( &dB );
    mbedtls_mpi_init( &zA ); mbedtls_mpi_init( &zB );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_EQUAL( 0, mbedtls_ecdh_gen_public( &grp, &dA, &qA,
                                            &mbedtls_test_rnd_pseudo_rand,
                                            &rnd_info ) );
    TEST_ASSERT( mbedtls_ecdh_gen_public( &grp, &dB, &qB,
                                          &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecdh_compute_shared( &grp, &zA, &qB, &dA,
                                              &mbedtls_test_rnd_pseudo_rand,
                                              &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecdh_compute_shared( &grp, &zB, &qA, &dB,
                                      NULL, NULL ) == 0 );

    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &zA, &zB ) == 0 );

exit:
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &qA ); mbedtls_ecp_point_free( &qB );
    mbedtls_mpi_free( &dA ); mbedtls_mpi_free( &dB );
    mbedtls_mpi_free( &zA ); mbedtls_mpi_free( &zB );
}

void test_ecdh_primitive_random_wrapper( void ** params )
{

    test_ecdh_primitive_random( *( (int *) params[0] ) );
}
void test_ecdh_primitive_testvec( int id, data_t * rnd_buf_A, char * xA_str,
                             char * yA_str, data_t * rnd_buf_B,
                             char * xB_str, char * yB_str, char * z_str )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point qA, qB;
    mbedtls_mpi dA, dB, zA, zB, check;
    mbedtls_test_rnd_buf_info rnd_info_A, rnd_info_B;

    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &qA ); mbedtls_ecp_point_init( &qB );
    mbedtls_mpi_init( &dA ); mbedtls_mpi_init( &dB );
    mbedtls_mpi_init( &zA ); mbedtls_mpi_init( &zB ); mbedtls_mpi_init( &check );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    rnd_info_A.buf = rnd_buf_A->x;
    rnd_info_A.length = rnd_buf_A->len;

    /* Fix rnd_buf_A->x by shifting it left if necessary */
    if( grp.nbits % 8 != 0 )
    {
        unsigned char shift = 8 - ( grp.nbits % 8 );
        size_t i;

        for( i = 0; i < rnd_info_A.length - 1; i++ )
            rnd_buf_A->x[i] = rnd_buf_A->x[i] << shift
                         | rnd_buf_A->x[i+1] >> ( 8 - shift );

        rnd_buf_A->x[rnd_info_A.length-1] <<= shift;
    }

    rnd_info_B.buf = rnd_buf_B->x;
    rnd_info_B.length = rnd_buf_B->len;

    /* Fix rnd_buf_B->x by shifting it left if necessary */
    if( grp.nbits % 8 != 0 )
    {
        unsigned char shift = 8 - ( grp.nbits % 8 );
        size_t i;

        for( i = 0; i < rnd_info_B.length - 1; i++ )
            rnd_buf_B->x[i] = rnd_buf_B->x[i] << shift
                         | rnd_buf_B->x[i+1] >> ( 8 - shift );

        rnd_buf_B->x[rnd_info_B.length-1] <<= shift;
    }

    TEST_ASSERT( mbedtls_ecdh_gen_public( &grp, &dA, &qA,
                                          mbedtls_test_rnd_buffer_rand,
                                          &rnd_info_A ) == 0 );
    TEST_ASSERT( !mbedtls_ecp_is_zero( &qA ) );
    TEST_ASSERT( mbedtls_mpi_read_string( &check, 16, xA_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &qA.X, &check ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &check, 16, yA_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &qA.Y, &check ) == 0 );

    TEST_ASSERT( mbedtls_ecdh_gen_public( &grp, &dB, &qB,
                                          mbedtls_test_rnd_buffer_rand,
                                          &rnd_info_B ) == 0 );
    TEST_ASSERT( !mbedtls_ecp_is_zero( &qB ) );
    TEST_ASSERT( mbedtls_mpi_read_string( &check, 16, xB_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &qB.X, &check ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &check, 16, yB_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &qB.Y, &check ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &check, 16, z_str ) == 0 );
    TEST_ASSERT( mbedtls_ecdh_compute_shared( &grp, &zA, &qB, &dA, NULL, NULL ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &zA, &check ) == 0 );
    TEST_ASSERT( mbedtls_ecdh_compute_shared( &grp, &zB, &qA, &dB, NULL, NULL ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &zB, &check ) == 0 );

exit:
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &qA ); mbedtls_ecp_point_free( &qB );
    mbedtls_mpi_free( &dA ); mbedtls_mpi_free( &dB );
    mbedtls_mpi_free( &zA ); mbedtls_mpi_free( &zB ); mbedtls_mpi_free( &check );
}

void test_ecdh_primitive_testvec_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};

    test_ecdh_primitive_testvec( *( (int *) params[0] ), &data1, (char *) params[3], (char *) params[4], &data5, (char *) params[7], (char *) params[8], (char *) params[9] );
}
void test_ecdh_exchange( int id )
{
    mbedtls_ecdh_context srv, cli;
    unsigned char buf[1000];
    const unsigned char *vbuf;
    size_t len;
    mbedtls_test_rnd_pseudo_info rnd_info;
    unsigned char res_buf[1000];
    size_t res_len;

    mbedtls_ecdh_init( &srv );
    mbedtls_ecdh_init( &cli );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_ecdh_setup( &srv, id ) == 0 );

    memset( buf, 0x00, sizeof( buf ) ); vbuf = buf;
    TEST_ASSERT( mbedtls_ecdh_make_params( &srv, &len, buf, 1000,
                                           &mbedtls_test_rnd_pseudo_rand,
                                           &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecdh_read_params( &cli, &vbuf, buf + len ) == 0 );

    memset( buf, 0x00, sizeof( buf ) );
    TEST_ASSERT( mbedtls_ecdh_make_public( &cli, &len, buf, 1000,
                                           &mbedtls_test_rnd_pseudo_rand,
                                           &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecdh_read_public( &srv, buf, len ) == 0 );

    TEST_ASSERT( mbedtls_ecdh_calc_secret( &srv, &len, buf, 1000,
                                           &mbedtls_test_rnd_pseudo_rand,
                                           &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecdh_calc_secret( &cli, &res_len, res_buf, 1000,
                                           NULL, NULL ) == 0 );
    TEST_ASSERT( len == res_len );
    TEST_ASSERT( memcmp( buf, res_buf, len ) == 0 );

exit:
    mbedtls_ecdh_free( &srv );
    mbedtls_ecdh_free( &cli );
}

void test_ecdh_exchange_wrapper( void ** params )
{

    test_ecdh_exchange( *( (int *) params[0] ) );
}
#if defined(MBEDTLS_ECP_RESTARTABLE)
void test_ecdh_restart( int id, data_t *dA, data_t *dB, data_t *z,
                   int enable, int max_ops, int min_restart, int max_restart )
{
    int ret;
    mbedtls_ecdh_context srv, cli;
    unsigned char buf[1000];
    const unsigned char *vbuf;
    size_t len;
    mbedtls_test_rnd_buf_info rnd_info_A, rnd_info_B;
    int cnt_restart;
    mbedtls_ecp_group grp;

    mbedtls_ecp_group_init( &grp );
    mbedtls_ecdh_init( &srv );
    mbedtls_ecdh_init( &cli );

    rnd_info_A.buf = dA->x;
    rnd_info_A.length = dA->len;

    rnd_info_B.buf = dB->x;
    rnd_info_B.length = dB->len;

    /* The ECDH context is not guaranteed ot have an mbedtls_ecp_group structure
     * in every configuration, therefore we load it separately. */
    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    /* Otherwise we would have to fix the random buffer,
     * as in ecdh_primitive_testvec. */
    TEST_ASSERT( grp.nbits % 8 == 0 );

    TEST_ASSERT( mbedtls_ecdh_setup( &srv, id ) == 0 );

    /* set up restart parameters */
    mbedtls_ecp_set_max_ops( max_ops );

    if( enable )
    {
        mbedtls_ecdh_enable_restart( &srv );
        mbedtls_ecdh_enable_restart( &cli );
    }

    /* server writes its parameters */
    memset( buf, 0x00, sizeof( buf ) );
    len = 0;

    cnt_restart = 0;
    do {
        ret = mbedtls_ecdh_make_params( &srv, &len, buf, sizeof( buf ),
                                        mbedtls_test_rnd_buffer_rand,
                                        &rnd_info_A );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( cnt_restart >= min_restart );
    TEST_ASSERT( cnt_restart <= max_restart );

    /* client read server params */
    vbuf = buf;
    TEST_ASSERT( mbedtls_ecdh_read_params( &cli, &vbuf, buf + len ) == 0 );

    /* client writes its key share */
    memset( buf, 0x00, sizeof( buf ) );
    len = 0;

    cnt_restart = 0;
    do {
        ret = mbedtls_ecdh_make_public( &cli, &len, buf, sizeof( buf ),
                                        mbedtls_test_rnd_buffer_rand,
                                        &rnd_info_B );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( cnt_restart >= min_restart );
    TEST_ASSERT( cnt_restart <= max_restart );

    /* server reads client key share */
    TEST_ASSERT( mbedtls_ecdh_read_public( &srv, buf, len ) == 0 );

    /* server computes shared secret */
    memset( buf, 0, sizeof( buf ) );
    len = 0;

    cnt_restart = 0;
    do {
        ret = mbedtls_ecdh_calc_secret( &srv, &len, buf, sizeof( buf ),
                                              NULL, NULL );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( cnt_restart >= min_restart );
    TEST_ASSERT( cnt_restart <= max_restart );

    TEST_ASSERT( len == z->len );
    TEST_ASSERT( memcmp( buf, z->x, len ) == 0 );

    /* client computes shared secret */
    memset( buf, 0, sizeof( buf ) );
    len = 0;

    cnt_restart = 0;
    do {
        ret = mbedtls_ecdh_calc_secret( &cli, &len, buf, sizeof( buf ),
                                              NULL, NULL );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( cnt_restart >= min_restart );
    TEST_ASSERT( cnt_restart <= max_restart );

    TEST_ASSERT( len == z->len );
    TEST_ASSERT( memcmp( buf, z->x, len ) == 0 );

exit:
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecdh_free( &srv );
    mbedtls_ecdh_free( &cli );
}

void test_ecdh_restart_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};

    test_ecdh_restart( *( (int *) params[0] ), &data1, &data3, &data5, *( (int *) params[7] ), *( (int *) params[8] ), *( (int *) params[9] ), *( (int *) params[10] ) );
}
#endif /* MBEDTLS_ECP_RESTARTABLE */
#if defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
void test_ecdh_exchange_legacy( int id )
{
    mbedtls_ecdh_context srv, cli;
    unsigned char buf[1000];
    const unsigned char *vbuf;
    size_t len;

    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_ecdh_init( &srv );
    mbedtls_ecdh_init( &cli );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_ecp_group_load( &srv.grp, id ) == 0 );

    memset( buf, 0x00, sizeof( buf ) ); vbuf = buf;
    TEST_ASSERT( mbedtls_ecdh_make_params( &srv, &len, buf, 1000,
                                           &mbedtls_test_rnd_pseudo_rand,
                                           &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecdh_read_params( &cli, &vbuf, buf + len ) == 0 );

    memset( buf, 0x00, sizeof( buf ) );
    TEST_ASSERT( mbedtls_ecdh_make_public( &cli, &len, buf, 1000,
                                           &mbedtls_test_rnd_pseudo_rand,
                                           &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecdh_read_public( &srv, buf, len ) == 0 );

    TEST_ASSERT( mbedtls_ecdh_calc_secret( &srv, &len, buf, 1000,
                                           &mbedtls_test_rnd_pseudo_rand,
                                           &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecdh_calc_secret( &cli, &len, buf, 1000, NULL,
                                           NULL ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &srv.z, &cli.z ) == 0 );

exit:
    mbedtls_ecdh_free( &srv );
    mbedtls_ecdh_free( &cli );
}

void test_ecdh_exchange_legacy_wrapper( void ** params )
{

    test_ecdh_exchange_legacy( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_ECDH_LEGACY_CONTEXT */
void test_ecdh_exchange_calc_secret( int grp_id,
                                data_t *our_private_key,
                                data_t *their_point,
                                int ours_first,
                                data_t *expected )
{
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_ecp_keypair our_key;
    mbedtls_ecp_keypair their_key;
    mbedtls_ecdh_context ecdh;
    unsigned char shared_secret[MBEDTLS_ECP_MAX_BYTES];
    size_t shared_secret_length = 0;

    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );
    mbedtls_ecdh_init( &ecdh );
    mbedtls_ecp_keypair_init( &our_key );
    mbedtls_ecp_keypair_init( &their_key );

    if( ! load_private_key( grp_id, our_private_key, &our_key, &rnd_info ) )
        goto exit;
    if( ! load_public_key( grp_id, their_point, &their_key ) )
        goto exit;

    /* Import the keys to the ECDH calculation. */
    if( ours_first )
    {
        TEST_ASSERT( mbedtls_ecdh_get_params(
                         &ecdh, &our_key, MBEDTLS_ECDH_OURS ) == 0 );
        TEST_ASSERT( mbedtls_ecdh_get_params(
                         &ecdh, &their_key, MBEDTLS_ECDH_THEIRS ) == 0 );
    }
    else
    {
        TEST_ASSERT( mbedtls_ecdh_get_params(
                         &ecdh, &their_key, MBEDTLS_ECDH_THEIRS ) == 0 );
        TEST_ASSERT( mbedtls_ecdh_get_params(
                         &ecdh, &our_key, MBEDTLS_ECDH_OURS ) == 0 );
    }

    /* Perform the ECDH calculation. */
    TEST_ASSERT( mbedtls_ecdh_calc_secret(
                     &ecdh,
                     &shared_secret_length,
                     shared_secret, sizeof( shared_secret ),
                     &mbedtls_test_rnd_pseudo_rand, &rnd_info ) == 0 );
    TEST_ASSERT( shared_secret_length == expected->len );
    TEST_ASSERT( memcmp( expected->x, shared_secret,
                         shared_secret_length ) == 0 );

exit:
    mbedtls_ecdh_free( &ecdh );
    mbedtls_ecp_keypair_free( &our_key );
    mbedtls_ecp_keypair_free( &their_key );
}

void test_ecdh_exchange_calc_secret_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_ecdh_exchange_calc_secret( *( (int *) params[0] ), &data1, &data3, *( (int *) params[5] ), &data6 );
}
void test_ecdh_exchange_get_params_fail( int our_grp_id,
                                    data_t *our_private_key,
                                    int their_grp_id,
                                    data_t *their_point,
                                    int ours_first,
                                    int expected_ret )
{
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_ecp_keypair our_key;
    mbedtls_ecp_keypair their_key;
    mbedtls_ecdh_context ecdh;

    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );
    mbedtls_ecdh_init( &ecdh );
    mbedtls_ecp_keypair_init( &our_key );
    mbedtls_ecp_keypair_init( &their_key );

    if( ! load_private_key( our_grp_id, our_private_key, &our_key, &rnd_info ) )
        goto exit;
    if( ! load_public_key( their_grp_id, their_point, &their_key ) )
        goto exit;

    if( ours_first )
    {
        TEST_ASSERT( mbedtls_ecdh_get_params(
                         &ecdh, &our_key, MBEDTLS_ECDH_OURS ) == 0 );
        TEST_ASSERT( mbedtls_ecdh_get_params(
                         &ecdh, &their_key, MBEDTLS_ECDH_THEIRS ) ==
                     expected_ret );
    }
    else
    {
        TEST_ASSERT( mbedtls_ecdh_get_params(
                         &ecdh, &their_key, MBEDTLS_ECDH_THEIRS ) == 0 );
        TEST_ASSERT( mbedtls_ecdh_get_params(
                         &ecdh, &our_key, MBEDTLS_ECDH_OURS ) ==
                     expected_ret );
    }

exit:
    mbedtls_ecdh_free( &ecdh );
    mbedtls_ecp_keypair_free( &our_key );
    mbedtls_ecp_keypair_free( &their_key );
}

void test_ecdh_exchange_get_params_fail_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_ecdh_exchange_get_params_fail( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, *( (int *) params[6] ), *( (int *) params[7] ) );
}
#endif /* MBEDTLS_ECDH_C */

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

#if defined(MBEDTLS_ECDH_C)

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
                *out_value = MBEDTLS_ECP_DP_CURVE25519;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_ECP_DP_BP256R1;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
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

#if defined(MBEDTLS_ECDH_C)

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
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
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

#if defined(MBEDTLS_ECDH_C)
    test_ecdh_valid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_ECDH_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_ecdh_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_ECDH_C)
    test_ecdh_primitive_random_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_ECDH_C)
    test_ecdh_primitive_testvec_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_ECDH_C)
    test_ecdh_exchange_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_ECDH_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    test_ecdh_restart_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_ECDH_C) && defined(MBEDTLS_ECDH_LEGACY_CONTEXT)
    test_ecdh_exchange_legacy_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_ECDH_C)
    test_ecdh_exchange_calc_secret_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_ECDH_C)
    test_ecdh_exchange_get_params_fail_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_ecdh.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
