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
#include "libc/log/log.h"
#include "third_party/mbedtls/test/test.inc"
/*
 * *** THIS FILE WAS MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * Test file      : ./test_suite_ecp.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_ecp.function
 *      Test suite data     : suites/test_suite_ecp.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_ECP_C)
#include "third_party/mbedtls/ecp.h"

#define ECP_PF_UNKNOWN     -1

#define ECP_PT_RESET( x )           \
    mbedtls_ecp_point_free( x );    \
    mbedtls_ecp_point_init( x );
void test_ecp_valid_param( )
{
    TEST_VALID_PARAM( mbedtls_ecp_group_free( NULL ) );
    TEST_VALID_PARAM( mbedtls_ecp_keypair_free( NULL ) );
    TEST_VALID_PARAM( mbedtls_ecp_point_free( NULL ) );

#if defined(MBEDTLS_ECP_RESTARTABLE)
    TEST_VALID_PARAM( mbedtls_ecp_restart_free( NULL ) );
#endif /* MBEDTLS_ECP_RESTARTABLE */

exit:
    return;
}

void test_ecp_valid_param_wrapper( void ** params )
{
    (void)params;

    test_ecp_valid_param(  );
}
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_ecp_invalid_param( )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_keypair kp;
    mbedtls_ecp_point P;
    mbedtls_mpi m;
    const char *x = "deadbeef";
    int valid_fmt   = MBEDTLS_ECP_PF_UNCOMPRESSED;
    int invalid_fmt = 42;
    size_t olen;
    unsigned char buf[42] = { 0 };
    const unsigned char *null_buf = NULL;
    mbedtls_ecp_group_id valid_group = MBEDTLS_ECP_DP_SECP192R1;
#if defined(MBEDTLS_ECP_RESTARTABLE)
    mbedtls_ecp_restart_ctx restart_ctx;
#endif /* MBEDTLS_ECP_RESTARTABLE */

    TEST_INVALID_PARAM( mbedtls_ecp_point_init( NULL ) );
    TEST_INVALID_PARAM( mbedtls_ecp_keypair_init( NULL ) );
    TEST_INVALID_PARAM( mbedtls_ecp_group_init( NULL ) );

#if defined(MBEDTLS_ECP_RESTARTABLE)
    TEST_INVALID_PARAM( mbedtls_ecp_restart_init( NULL ) );
    TEST_INVALID_PARAM( mbedtls_ecp_check_budget( NULL, &restart_ctx, 42 ) );
#endif /* MBEDTLS_ECP_RESTARTABLE */

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_copy( NULL, &P ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_copy( &P, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_group_copy( NULL, &grp ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_group_copy( &grp, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_gen_privkey( NULL,
                                                     &m,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_gen_privkey( &grp,
                                                     NULL,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_gen_privkey( &grp,
                                                     &m,
                                                     NULL,
                                                     NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_set_zero( NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_is_zero( NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_cmp( NULL, &P ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_cmp( &P, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_read_string( NULL, 2,
                                                           x, x ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_read_string( &P, 2,
                                                           NULL, x ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_read_string( &P, 2,
                                                           x, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_write_binary( NULL, &P,
                                                      valid_fmt,
                                                      &olen,
                                                      buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_write_binary( &grp, NULL,
                                                      valid_fmt,
                                                      &olen,
                                                      buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_write_binary( &grp, &P,
                                                      invalid_fmt,
                                                      &olen,
                                                      buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_write_binary( &grp, &P,
                                                      valid_fmt,
                                                      NULL,
                                                      buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_write_binary( &grp, &P,
                                                      valid_fmt,
                                                      &olen,
                                                      NULL, sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_read_binary( NULL, &P, buf,
                                                     sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_read_binary( &grp, NULL, buf,
                                                     sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_point_read_binary( &grp, &P, NULL,
                                                     sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_read_point( NULL, &P,
                                                 (const unsigned char **) &buf,
                                                 sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_read_point( &grp, NULL,
                                                 (const unsigned char **) &buf,
                                                 sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_read_point( &grp, &P, &null_buf,
                                                        sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_read_point( &grp, &P, NULL,
                                                    sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_write_point( NULL, &P,
                                                     valid_fmt,
                                                     &olen,
                                                     buf,
                                                     sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_write_point( &grp, NULL,
                                                     valid_fmt,
                                                     &olen,
                                                     buf,
                                                     sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_write_point( &grp, &P,
                                                     invalid_fmt,
                                                     &olen,
                                                     buf,
                                                     sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_write_point( &grp, &P,
                                                     valid_fmt,
                                                     NULL,
                                                     buf,
                                                     sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_write_point( &grp, &P,
                                                     valid_fmt,
                                                     &olen,
                                                     NULL,
                                                     sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_group_load( NULL, valid_group ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_read_group( NULL,
                                                 (const unsigned char **) &buf,
                                                 sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_read_group( &grp, NULL,
                                                        sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_read_group( &grp, &null_buf,
                                                        sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_read_group_id( NULL,
                                                 (const unsigned char **) &buf,
                                                 sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_read_group_id( &valid_group, NULL,
                                                        sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_read_group_id( &valid_group,
                                                           &null_buf,
                                                           sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_write_group( NULL, &olen,
                                                       buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_write_group( &grp, NULL,
                                                       buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_tls_write_group( &grp, &olen,
                                                       NULL, sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_mul( NULL, &P, &m, &P,
                                             mbedtls_test_rnd_std_rand,
                                             NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_mul( &grp, NULL, &m, &P,
                                             mbedtls_test_rnd_std_rand,
                                             NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_mul( &grp, &P, NULL, &P,
                                             mbedtls_test_rnd_std_rand,
                                             NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_mul( &grp, &P, &m, NULL,
                                             mbedtls_test_rnd_std_rand,
                                             NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_mul_restartable( NULL, &P, &m, &P,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL , NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_mul_restartable( &grp, NULL, &m, &P,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL , NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_mul_restartable( &grp, &P, NULL, &P,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL , NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_mul_restartable( &grp, &P, &m, NULL,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL , NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd( NULL, &P, &m, &P,
                                                &m, &P ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd( &grp, NULL, &m, &P,
                                                &m, &P ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd( &grp, &P, NULL, &P,
                                                &m, &P ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd( &grp, &P, &m, NULL,
                                                &m, &P ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd( &grp, &P, &m, &P,
                                                NULL, &P ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd( &grp, &P, &m, &P,
                                                &m, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd_restartable( NULL, &P, &m, &P,
                                                            &m, &P, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd_restartable( &grp, NULL, &m, &P,
                                                            &m, &P, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd_restartable( &grp, &P, NULL, &P,
                                                            &m, &P, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd_restartable( &grp, &P, &m, NULL,
                                                            &m, &P, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd_restartable( &grp, &P, &m, &P,
                                                            NULL, &P, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_muladd_restartable( &grp, &P, &m, &P,
                                                            &m, NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_check_pubkey( NULL, &P ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_check_pubkey( &grp, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_check_pub_priv( NULL, &kp ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_check_pub_priv( &kp, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_check_privkey( NULL, &m ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_check_privkey( &grp, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecp_gen_keypair_base( NULL, &P, &m, &P,
                                      mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecp_gen_keypair_base( &grp, NULL, &m, &P,
                                      mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecp_gen_keypair_base( &grp, &P, NULL, &P,
                                      mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecp_gen_keypair_base( &grp, &P, &m, NULL,
                                      mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecp_gen_keypair_base( &grp, &P, &m, &P, NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_gen_keypair( NULL,
                                                     &m, &P,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_gen_keypair( &grp,
                                                     NULL, &P,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_gen_keypair( &grp,
                                                     &m, NULL,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_gen_keypair( &grp,
                                                     &m, &P,
                                                     NULL,
                                                     NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_gen_key( valid_group, NULL,
                                                 mbedtls_test_rnd_std_rand,
                                                 NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecp_gen_key( valid_group, &kp,
                                                 NULL, NULL ) );

exit:
    return;
}

void test_ecp_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_ecp_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_mbedtls_ecp_curve_info( int id, int tls_id, int size, char * name )
{
    const mbedtls_ecp_curve_info *by_id, *by_tls, *by_name;

    by_id   = mbedtls_ecp_curve_info_from_grp_id( id     );
    by_tls  = mbedtls_ecp_curve_info_from_tls_id( tls_id );
    by_name = mbedtls_ecp_curve_info_from_name(   name   );
    TEST_ASSERT( by_id   != NULL );
    TEST_ASSERT( by_tls  != NULL );
    TEST_ASSERT( by_name != NULL );

    TEST_ASSERT( by_id == by_tls  );
    TEST_ASSERT( by_id == by_name );

    TEST_ASSERT( by_id->bit_size == size );
exit:
    ;
}

void test_mbedtls_ecp_curve_info_wrapper( void ** params )
{

    test_mbedtls_ecp_curve_info( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), (char *) params[3] );
}

void test_ecp_check_pub( int grp_id, char * x_hex, char * y_hex, char * z_hex,
                         int ret )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point P;
    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &P );
    TEST_ASSERT( mbedtls_ecp_group_load( &grp, grp_id ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &P.X, 16, x_hex ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &P.Y, 16, y_hex ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &P.Z, 16, z_hex ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &P ) == ret );
exit:
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &P );
}

void test_ecp_check_pub_wrapper( void ** params )
{

    test_ecp_check_pub( *( (int *) params[0] ), (char *) params[1], (char *) params[2], (char *) params[3], *( (int *) params[4] ) );
}
#if defined(MBEDTLS_ECP_RESTARTABLE)
void test_ecp_test_vect_restart( int id,
                                 char *dA_str, char *xA_str, char *yA_str,
                                 char *dB_str,  char *xZ_str, char *yZ_str,
                                 int max_ops, int min_restarts, int max_restarts )
{
    /*
     * Test for early restart. Based on test vectors like ecp_test_vect(),
     * but for the sake of simplicity only does half of each side. It's
     * important to test both base point and random point, though, as memory
     * management is different in each case.
     *
     * Don't try using too precise bounds for restarts as the exact number
     * will depend on settings such as MBEDTLS_ECP_FIXED_POINT_OPTIM and
     * MBEDTLS_ECP_WINDOW_SIZE, as well as implementation details that may
     * change in the future. A factor 2 is a minimum safety margin.
     *
     * For reference, with mbed TLS 2.4 and default settings, for P-256:
     * - Random point mult:     ~3250M
     * - Cold base point mult:  ~3300M
     * - Hot base point mult:   ~1100M
     * With MBEDTLS_ECP_WINDOW_SIZE set to 2 (minimum):
     * - Random point mult:     ~3850M
     */
    mbedtls_ecp_restart_ctx ctx;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point R, P;
    mbedtls_mpi dA, xA, yA, dB, xZ, yZ;
    int cnt_restarts;
    int ret;

    mbedtls_ecp_restart_init( &ctx );
    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &R ); mbedtls_ecp_point_init( &P );
    mbedtls_mpi_init( &dA ); mbedtls_mpi_init( &xA ); mbedtls_mpi_init( &yA );
    mbedtls_mpi_init( &dB ); mbedtls_mpi_init( &xZ ); mbedtls_mpi_init( &yZ );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &dA, 16, dA_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &xA, 16, xA_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &yA, 16, yA_str ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &dB, 16, dB_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &xZ, 16, xZ_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &yZ, 16, yZ_str ) == 0 );

    mbedtls_ecp_set_max_ops( (unsigned) max_ops );

    /* Base point case */
    cnt_restarts = 0;
    do {
        ECP_PT_RESET( &R );
        ret = mbedtls_ecp_mul_restartable( &grp, &R, &dA, &grp.G, NULL, NULL, &ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restarts );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xA ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.Y, &yA ) == 0 );

    TEST_ASSERT( cnt_restarts >= min_restarts );
    TEST_ASSERT( cnt_restarts <= max_restarts );

    /* Non-base point case */
    mbedtls_ecp_copy( &P, &R );
    cnt_restarts = 0;
    do {
        ECP_PT_RESET( &R );
        ret = mbedtls_ecp_mul_restartable( &grp, &R, &dB, &P, NULL, NULL, &ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restarts );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xZ ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.Y, &yZ ) == 0 );

    TEST_ASSERT( cnt_restarts >= min_restarts );
    TEST_ASSERT( cnt_restarts <= max_restarts );

    /* Do we leak memory when aborting an operation?
     * This test only makes sense when we actually restart */
    if( min_restarts > 0 )
    {
        ret = mbedtls_ecp_mul_restartable( &grp, &R, &dB, &P, NULL, NULL, &ctx );
        TEST_ASSERT( ret == MBEDTLS_ERR_ECP_IN_PROGRESS );
    }

exit:
    mbedtls_ecp_restart_free( &ctx );
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &R ); mbedtls_ecp_point_free( &P );
    mbedtls_mpi_free( &dA ); mbedtls_mpi_free( &xA ); mbedtls_mpi_free( &yA );
    mbedtls_mpi_free( &dB ); mbedtls_mpi_free( &xZ ); mbedtls_mpi_free( &yZ );
}

void test_ecp_test_vect_restart_wrapper( void ** params )
{

    test_ecp_test_vect_restart( *( (int *) params[0] ), (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6], *( (int *) params[7] ), *( (int *) params[8] ), *( (int *) params[9] ) );
}
#endif /* MBEDTLS_ECP_RESTARTABLE */
#if defined(MBEDTLS_ECP_RESTARTABLE)
void test_ecp_muladd_restart( int id, char *xR_str, char *yR_str,
                         char *u1_str, char *u2_str,
                         char *xQ_str, char *yQ_str,
                         int max_ops, int min_restarts, int max_restarts )
{
    /*
     * Compute R = u1 * G + u2 * Q
     * (test vectors mostly taken from ECDSA intermediate results)
     *
     * See comments at the top of ecp_test_vect_restart()
     */
    mbedtls_ecp_restart_ctx ctx;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point R, Q;
    mbedtls_mpi u1, u2, xR, yR;
    int cnt_restarts;
    int ret;

    mbedtls_ecp_restart_init( &ctx );
    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &R );
    mbedtls_ecp_point_init( &Q );
    mbedtls_mpi_init( &u1 ); mbedtls_mpi_init( &u2 );
    mbedtls_mpi_init( &xR ); mbedtls_mpi_init( &yR );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &u1, 16, u1_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &u2, 16, u2_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &xR, 16, xR_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &yR, 16, yR_str ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &Q.X, 16, xQ_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q.Y, 16, yQ_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_lset( &Q.Z, 1 ) == 0 );

    mbedtls_ecp_set_max_ops( (unsigned) max_ops );

    cnt_restarts = 0;
    do {
        ECP_PT_RESET( &R );
        ret = mbedtls_ecp_muladd_restartable( &grp, &R,
                                              &u1, &grp.G, &u2, &Q, &ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restarts );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xR ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.Y, &yR ) == 0 );

    TEST_ASSERT( cnt_restarts >= min_restarts );
    TEST_ASSERT( cnt_restarts <= max_restarts );

    /* Do we leak memory when aborting an operation?
     * This test only makes sense when we actually restart */
    if( min_restarts > 0 )
    {
        ret = mbedtls_ecp_muladd_restartable( &grp, &R,
                                              &u1, &grp.G, &u2, &Q, &ctx );
        TEST_ASSERT( ret == MBEDTLS_ERR_ECP_IN_PROGRESS );
    }

exit:
    mbedtls_ecp_restart_free( &ctx );
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &R );
    mbedtls_ecp_point_free( &Q );
    mbedtls_mpi_free( &u1 ); mbedtls_mpi_free( &u2 );
    mbedtls_mpi_free( &xR ); mbedtls_mpi_free( &yR );
}

void test_ecp_muladd_restart_wrapper( void ** params )
{

    test_ecp_muladd_restart( *( (int *) params[0] ), (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6], *( (int *) params[7] ), *( (int *) params[8] ), *( (int *) params[9] ) );
}
#endif /* MBEDTLS_ECP_RESTARTABLE */

void test_ecp_test_vect( int id, char * dA_str, char * xA_str, char * yA_str,
                    char * dB_str, char * xB_str, char * yB_str,
                    char * xZ_str, char * yZ_str )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point R;
    mbedtls_mpi dA, xA, yA, dB, xB, yB, xZ, yZ;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_ecp_group_init( &grp ); mbedtls_ecp_point_init( &R );
    mbedtls_mpi_init( &dA ); mbedtls_mpi_init( &xA ); mbedtls_mpi_init( &yA ); mbedtls_mpi_init( &dB );
    mbedtls_mpi_init( &xB ); mbedtls_mpi_init( &yB ); mbedtls_mpi_init( &xZ ); mbedtls_mpi_init( &yZ );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &grp.G ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &dA, 16, dA_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &xA, 16, xA_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &yA, 16, yA_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &dB, 16, dB_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &xB, 16, xB_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &yB, 16, yB_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &xZ, 16, xZ_str ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &yZ, 16, yZ_str ) == 0 );

    TEST_ASSERT( mbedtls_ecp_mul( &grp, &R, &dA, &grp.G,
                          &mbedtls_test_rnd_pseudo_rand, &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xA ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.Y, &yA ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &R ) == 0 );
    TEST_ASSERT( mbedtls_ecp_mul( &grp, &R, &dB, &R, NULL, NULL ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xZ ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.Y, &yZ ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &R ) == 0 );

    TEST_ASSERT( mbedtls_ecp_mul( &grp, &R, &dB, &grp.G, NULL, NULL ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xB ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.Y, &yB ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &R ) == 0 );
    TEST_ASSERT( mbedtls_ecp_mul( &grp, &R, &dA, &R,
                          &mbedtls_test_rnd_pseudo_rand, &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xZ ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.Y, &yZ ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &R ) == 0 );

exit:
    mbedtls_ecp_group_free( &grp ); mbedtls_ecp_point_free( &R );
    mbedtls_mpi_free( &dA ); mbedtls_mpi_free( &xA ); mbedtls_mpi_free( &yA ); mbedtls_mpi_free( &dB );
    mbedtls_mpi_free( &xB ); mbedtls_mpi_free( &yB ); mbedtls_mpi_free( &xZ ); mbedtls_mpi_free( &yZ );
}

void test_ecp_test_vect_wrapper( void ** params )
{

    test_ecp_test_vect( *( (int *) params[0] ), (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6], (char *) params[7], (char *) params[8] );
}
void test_ecp_test_vec_x( int id, char * dA_hex, char * xA_hex, char * dB_hex,
                     char * xB_hex, char * xS_hex )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point R;
    mbedtls_mpi dA, xA, dB, xB, xS;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_ecp_group_init( &grp ); mbedtls_ecp_point_init( &R );
    mbedtls_mpi_init( &dA ); mbedtls_mpi_init( &xA );
    mbedtls_mpi_init( &dB ); mbedtls_mpi_init( &xB );
    mbedtls_mpi_init( &xS );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &grp.G ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &dA, 16, dA_hex ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &dB, 16, dB_hex ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &xA, 16, xA_hex ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &xB, 16, xB_hex ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &xS, 16, xS_hex ) == 0 );

    TEST_ASSERT( mbedtls_ecp_mul( &grp, &R, &dA, &grp.G,
                          &mbedtls_test_rnd_pseudo_rand, &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &R ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xA ) == 0 );

    TEST_ASSERT( mbedtls_ecp_mul( &grp, &R, &dB, &R,
                          &mbedtls_test_rnd_pseudo_rand, &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &R ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xS ) == 0 );

    TEST_ASSERT( mbedtls_ecp_mul( &grp, &R, &dB, &grp.G, NULL, NULL ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &R ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xB ) == 0 );

    TEST_ASSERT( mbedtls_ecp_mul( &grp, &R, &dA, &R, NULL, NULL ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &R ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &R.X, &xS ) == 0 );

exit:
    mbedtls_ecp_group_free( &grp ); mbedtls_ecp_point_free( &R );
    mbedtls_mpi_free( &dA ); mbedtls_mpi_free( &xA );
    mbedtls_mpi_free( &dB ); mbedtls_mpi_free( &xB );
    mbedtls_mpi_free( &xS );
}

void test_ecp_test_vec_x_wrapper( void ** params )
{

    test_ecp_test_vec_x( *( (int *) params[0] ), (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], (char *) params[5] );
}
void test_ecp_test_mul( int id, data_t * n_hex,
                   data_t *  Px_hex, data_t *  Py_hex, data_t *  Pz_hex,
                   data_t * nPx_hex, data_t * nPy_hex, data_t * nPz_hex,
                   int expected_ret )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point P, nP, R;
    mbedtls_mpi n;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_ecp_group_init( &grp ); mbedtls_ecp_point_init( &R );
    mbedtls_ecp_point_init( &P ); mbedtls_ecp_point_init( &nP );
    mbedtls_mpi_init( &n );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &grp.G ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_binary( &n, n_hex->x, n_hex->len ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_binary( &P.X, Px_hex->x, Px_hex->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &P.Y, Py_hex->x, Py_hex->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &P.Z, Pz_hex->x, Pz_hex->len ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &nP.X, nPx_hex->x, nPx_hex->len )
                 == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &nP.Y, nPy_hex->x, nPy_hex->len )
                 == 0 );
    TEST_ASSERT( mbedtls_mpi_read_binary( &nP.Z, nPz_hex->x, nPz_hex->len )
                 == 0 );

    TEST_ASSERT( mbedtls_ecp_mul( &grp, &R, &n, &P,
                                  &mbedtls_test_rnd_pseudo_rand, &rnd_info )
                 == expected_ret );

    if( expected_ret == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &nP.X, &R.X ) == 0 );
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &nP.Y, &R.Y ) == 0 );
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &nP.Z, &R.Z ) == 0 );
    }

exit:
    mbedtls_ecp_group_free( &grp ); mbedtls_ecp_point_free( &R );
    mbedtls_ecp_point_free( &P ); mbedtls_ecp_point_free( &nP );
    mbedtls_mpi_free( &n );
}

void test_ecp_test_mul_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};
    data_t data13 = {(uint8_t *) params[13], *( (uint32_t *) params[14] )};

    test_ecp_test_mul( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9, &data11, &data13, *( (int *) params[15] ) );
}
void test_ecp_test_mul_rng( int id, data_t * d_hex)
{
    mbedtls_ecp_group grp;
    mbedtls_mpi d;
    mbedtls_ecp_point Q;

    mbedtls_ecp_group_init( &grp ); mbedtls_mpi_init( &d );
    mbedtls_ecp_point_init( &Q );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &grp.G ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_binary( &d, d_hex->x, d_hex->len ) == 0 );

    TEST_ASSERT( mbedtls_ecp_mul( &grp, &Q, &d, &grp.G,
                                  &mbedtls_test_rnd_zero_rand, NULL )
                 == MBEDTLS_ERR_ECP_RANDOM_FAILED );

exit:
    mbedtls_ecp_group_free( &grp ); mbedtls_mpi_free( &d );
    mbedtls_ecp_point_free( &Q );
}

void test_ecp_test_mul_rng_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_ecp_test_mul_rng( *( (int *) params[0] ), &data1 );
}
void test_ecp_fast_mod( int id, char * N_str )
{
    mbedtls_ecp_group grp;
    mbedtls_mpi N, R;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &R );
    mbedtls_ecp_group_init( &grp );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, 16, N_str ) == 0 );
    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );
    TEST_ASSERT( grp.modp != NULL );

    /*
     * Store correct result before we touch N
     */
    TEST_ASSERT( mbedtls_mpi_mod_mpi( &R, &N, &grp.P ) == 0 );

    TEST_ASSERT( grp.modp( &N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_bitlen( &N ) <= grp.pbits + 3 );

    /*
     * Use mod rather than addition/subtraction in case previous test fails
     */
    TEST_ASSERT( mbedtls_mpi_mod_mpi( &N, &N, &grp.P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &N, &R ) == 0 );

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &R );
    mbedtls_ecp_group_free( &grp );
}

void test_ecp_fast_mod_wrapper( void ** params )
{
    test_ecp_fast_mod( *( (int *) params[0] ), (char *) params[1] );
}
void test_ecp_write_binary( int id, char * x, char * y, char * z, int format,
                       data_t * out, int blen, int ret )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point P;
    unsigned char buf[256];
    size_t olen;

    memset( buf, 0, sizeof( buf ) );

    mbedtls_ecp_group_init( &grp ); mbedtls_ecp_point_init( &P );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &P.X, 16, x ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &P.Y, 16, y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &P.Z, 16, z ) == 0 );

    TEST_ASSERT( mbedtls_ecp_point_write_binary( &grp, &P, format,
                                   &olen, buf, blen ) == ret );

    if( ret == 0 )
    {
        TEST_ASSERT( mbedtls_test_hexcmp( buf, out->x, olen, out->len ) == 0 );
    }

exit:
    mbedtls_ecp_group_free( &grp ); mbedtls_ecp_point_free( &P );
}

void test_ecp_write_binary_wrapper( void ** params )
{
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};

    test_ecp_write_binary( *( (int *) params[0] ), (char *) params[1], (char *) params[2], (char *) params[3], *( (int *) params[4] ), &data5, *( (int *) params[7] ), *( (int *) params[8] ) );
}
void test_ecp_read_binary( int id, data_t * buf, char * x, char * y, char * z,
                      int ret )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point P;
    mbedtls_mpi X, Y, Z;


    mbedtls_ecp_group_init( &grp ); mbedtls_ecp_point_init( &P );
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &Z );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, 16, x ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, 16, y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Z, 16, z ) == 0 );

    TEST_ASSERT( mbedtls_ecp_point_read_binary( &grp, &P, buf->x, buf->len ) == ret );

    if( ret == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &P.X, &X ) == 0 );
        if( mbedtls_ecp_get_type( &grp ) == MBEDTLS_ECP_TYPE_MONTGOMERY )
        {
            TEST_ASSERT( mbedtls_mpi_is_zero( &Y ) );
            TEST_ASSERT( P.Y.p == NULL );
            TEST_ASSERT( mbedtls_mpi_is_one( &Z ) );
            TEST_ASSERT( mbedtls_mpi_is_one( &P.Z ) );
        }
        else
        {
            TEST_ASSERT( mbedtls_mpi_cmp_mpi( &P.Y, &Y ) == 0 );
            TEST_ASSERT( mbedtls_mpi_cmp_mpi( &P.Z, &Z ) == 0 );
        }
    }

exit:
    mbedtls_ecp_group_free( &grp ); mbedtls_ecp_point_free( &P );
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &Z );
}

void test_ecp_read_binary_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_ecp_read_binary( *( (int *) params[0] ), &data1, (char *) params[3], (char *) params[4], (char *) params[5], *( (int *) params[6] ) );
}
void test_mbedtls_ecp_tls_read_point( int id, data_t * buf, char * x, char * y,
                                 char * z, int ret )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point P;
    mbedtls_mpi X, Y, Z;
    const unsigned char *vbuf = buf->x;


    mbedtls_ecp_group_init( &grp ); mbedtls_ecp_point_init( &P );
    mbedtls_mpi_init( &X ); mbedtls_mpi_init( &Y ); mbedtls_mpi_init( &Z );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &X, 16, x ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Y, 16, y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Z, 16, z ) == 0 );

    TEST_ASSERT( mbedtls_ecp_tls_read_point( &grp, &P, &vbuf, buf->len ) == ret );

    if( ret == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &P.X, &X ) == 0 );
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &P.Y, &Y ) == 0 );
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &P.Z, &Z ) == 0 );
        TEST_ASSERT( (uint32_t)( vbuf - buf->x ) == buf->len );
    }

exit:
    mbedtls_ecp_group_free( &grp ); mbedtls_ecp_point_free( &P );
    mbedtls_mpi_free( &X ); mbedtls_mpi_free( &Y ); mbedtls_mpi_free( &Z );
}

void test_mbedtls_ecp_tls_read_point_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_mbedtls_ecp_tls_read_point( *( (int *) params[0] ), &data1, (char *) params[3], (char *) params[4], (char *) params[5], *( (int *) params[6] ) );
}
void test_ecp_tls_write_read_point( int id )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point pt;
    unsigned char buf[256];
    const unsigned char *vbuf;
    size_t olen;

    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &pt );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    memset( buf, 0x00, sizeof( buf ) ); vbuf = buf;
    TEST_ASSERT( mbedtls_ecp_tls_write_point( &grp, &grp.G,
                    MBEDTLS_ECP_PF_COMPRESSED, &olen, buf, 256 ) == 0 );
    TEST_ASSERT( mbedtls_ecp_tls_read_point( &grp, &pt, &vbuf, olen )
                 == MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE );
    TEST_ASSERT( vbuf == buf + olen );

    memset( buf, 0x00, sizeof( buf ) ); vbuf = buf;
    TEST_ASSERT( mbedtls_ecp_tls_write_point( &grp, &grp.G,
                    MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, buf, 256 ) == 0 );
    TEST_ASSERT( mbedtls_ecp_tls_read_point( &grp, &pt, &vbuf, olen ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &grp.G.X, &pt.X ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &grp.G.Y, &pt.Y ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &grp.G.Z, &pt.Z ) == 0 );
    TEST_ASSERT( vbuf == buf + olen );

    memset( buf, 0x00, sizeof( buf ) ); vbuf = buf;
    TEST_ASSERT( mbedtls_ecp_set_zero( &pt ) == 0 );
    TEST_ASSERT( mbedtls_ecp_tls_write_point( &grp, &pt,
                    MBEDTLS_ECP_PF_COMPRESSED, &olen, buf, 256 ) == 0 );
    TEST_ASSERT( mbedtls_ecp_tls_read_point( &grp, &pt, &vbuf, olen ) == 0 );
    TEST_ASSERT( mbedtls_ecp_is_zero( &pt ) );
    TEST_ASSERT( vbuf == buf + olen );

    memset( buf, 0x00, sizeof( buf ) ); vbuf = buf;
    TEST_ASSERT( mbedtls_ecp_set_zero( &pt ) == 0 );
    TEST_ASSERT( mbedtls_ecp_tls_write_point( &grp, &pt,
                    MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, buf, 256 ) == 0 );
    TEST_ASSERT( mbedtls_ecp_tls_read_point( &grp, &pt, &vbuf, olen ) == 0 );
    TEST_ASSERT( mbedtls_ecp_is_zero( &pt ) );
    TEST_ASSERT( vbuf == buf + olen );

exit:
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &pt );
}

void test_ecp_tls_write_read_point_wrapper( void ** params )
{

    test_ecp_tls_write_read_point( *( (int *) params[0] ) );
}
void test_mbedtls_ecp_tls_read_group( data_t * buf, int result, int bits,
                                 int record_len )
{
    mbedtls_ecp_group grp;
    const unsigned char *vbuf = buf->x;
    int ret;

    mbedtls_ecp_group_init( &grp );

    ret = mbedtls_ecp_tls_read_group( &grp, &vbuf, buf->len );

    TEST_ASSERT( ret == result );
    if( ret == 0)
    {
        TEST_ASSERT( mbedtls_mpi_bitlen( &grp.P ) == (size_t) bits );
        TEST_ASSERT( vbuf - buf->x ==  record_len);
    }

exit:
    mbedtls_ecp_group_free( &grp );
}

void test_mbedtls_ecp_tls_read_group_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_mbedtls_ecp_tls_read_group( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
void test_ecp_tls_write_read_group( int id )
{
    mbedtls_ecp_group grp1, grp2;
    unsigned char buf[10];
    const unsigned char *vbuf = buf;
    size_t len;
    int ret;

    mbedtls_ecp_group_init( &grp1 );
    mbedtls_ecp_group_init( &grp2 );
    memset( buf, 0x00, sizeof( buf ) );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp1, id ) == 0 );

    TEST_ASSERT( mbedtls_ecp_tls_write_group( &grp1, &len, buf, 10 ) == 0 );
    ret = mbedtls_ecp_tls_read_group( &grp2, &vbuf, len );
    TEST_ASSERT( ret == 0 );

    if( ret == 0 )
    {
        TEST_ASSERT( mbedtls_mpi_cmp_mpi( &grp1.N, &grp2.N ) == 0 );
        TEST_ASSERT( grp1.id == grp2.id );
    }

exit:
    mbedtls_ecp_group_free( &grp1 );
    mbedtls_ecp_group_free( &grp2 );
}

void test_ecp_tls_write_read_group_wrapper( void ** params )
{

    test_ecp_tls_write_read_group( *( (int *) params[0] ) );
}
void test_mbedtls_ecp_check_privkey( int id, char * key_hex, int ret )
{
    mbedtls_ecp_group grp;
    mbedtls_mpi d;

    mbedtls_ecp_group_init( &grp );
    mbedtls_mpi_init( &d );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &d, 16, key_hex ) == 0 );

    TEST_ASSERT( mbedtls_ecp_check_privkey( &grp, &d ) == ret );

exit:
    mbedtls_ecp_group_free( &grp );
    mbedtls_mpi_free( &d );
}

void test_mbedtls_ecp_check_privkey_wrapper( void ** params )
{

    test_mbedtls_ecp_check_privkey( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ) );
}

void test_mbedtls_ecp_check_pub_priv( int id_pub, char * Qx_pub, char * Qy_pub,
                                      int id, char * d, char * Qx, char * Qy,
                                      int ret )
{
    mbedtls_ecp_keypair pub, prv;

    mbedtls_ecp_keypair_init( &pub );
    mbedtls_ecp_keypair_init( &prv );

    if( id_pub != MBEDTLS_ECP_DP_NONE )
        TEST_ASSERT( mbedtls_ecp_group_load( &pub.grp, id_pub ) == 0 );
    TEST_ASSERT( mbedtls_ecp_point_read_string( &pub.Q, 16, Qx_pub, Qy_pub ) == 0 );

    if( id != MBEDTLS_ECP_DP_NONE )
        TEST_ASSERT( mbedtls_ecp_group_load( &prv.grp, id ) == 0 );
    TEST_ASSERT( mbedtls_ecp_point_read_string( &prv.Q, 16, Qx, Qy ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &prv.d, 16, d ) == 0 );

    TEST_ASSERT( mbedtls_ecp_check_pub_priv( &pub, &prv ) == ret );

exit:
    mbedtls_ecp_keypair_free( &pub );
    mbedtls_ecp_keypair_free( &prv );
}

void test_mbedtls_ecp_check_pub_priv_wrapper( void ** params )
{

    test_mbedtls_ecp_check_pub_priv( *( (int *) params[0] ), (char *) params[1], (char *) params[2], *( (int *) params[3] ), (char *) params[4], (char *) params[5], (char *) params[6], *( (int *) params[7] ) );
}
void test_mbedtls_ecp_gen_keypair( int id )
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point Q;
    mbedtls_mpi d;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_ecp_group_init( &grp );
    mbedtls_ecp_point_init( &Q );
    mbedtls_mpi_init( &d );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_ecp_group_load( &grp, id ) == 0 );

    TEST_ASSERT( mbedtls_ecp_gen_keypair( &grp, &d, &Q,
                                          &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info ) == 0 );

    TEST_ASSERT( mbedtls_ecp_check_pubkey( &grp, &Q ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_privkey( &grp, &d ) == 0 );

exit:
    mbedtls_ecp_group_free( &grp );
    mbedtls_ecp_point_free( &Q );
    mbedtls_mpi_free( &d );
}

void test_mbedtls_ecp_gen_keypair_wrapper( void ** params )
{

    test_mbedtls_ecp_gen_keypair( *( (int *) params[0] ) );
}
void test_mbedtls_ecp_gen_key( int id )
{
    mbedtls_ecp_keypair key;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_ecp_keypair_init( &key );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_ecp_gen_key( id, &key,
                                      &mbedtls_test_rnd_pseudo_rand,
                                      &rnd_info ) == 0 );

    TEST_ASSERT( mbedtls_ecp_check_pubkey( &key.grp, &key.Q ) == 0 );
    TEST_ASSERT( mbedtls_ecp_check_privkey( &key.grp, &key.d ) == 0 );

exit:
    mbedtls_ecp_keypair_free( &key );
}

void test_mbedtls_ecp_gen_key_wrapper( void ** params )
{

    test_mbedtls_ecp_gen_key( *( (int *) params[0] ) );
}
void test_mbedtls_ecp_read_key( int grp_id, data_t* in_key, int expected, int canonical )
{
    int ret = 0;
    mbedtls_ecp_keypair key;
    mbedtls_ecp_keypair key2;

    mbedtls_ecp_keypair_init( &key );
    mbedtls_ecp_keypair_init( &key2 );

    ret = mbedtls_ecp_read_key( grp_id, &key, in_key->x, in_key->len );
    TEST_ASSERT( ret == expected );

    if( expected == 0 )
    {
        ret = mbedtls_ecp_check_privkey( &key.grp, &key.d );
        TEST_ASSERT( ret == 0 );

        if( canonical )
        {
            unsigned char buf[MBEDTLS_ECP_MAX_BYTES];

            ret = mbedtls_ecp_write_key( &key, buf, in_key->len );
            TEST_ASSERT( ret == 0 );

            ASSERT_COMPARE( in_key->x, in_key->len,
                            buf, in_key->len );
        }
        else
        {
            unsigned char export1[MBEDTLS_ECP_MAX_BYTES];
            unsigned char export2[MBEDTLS_ECP_MAX_BYTES];

            ret = mbedtls_ecp_write_key( &key, export1, in_key->len );
            TEST_ASSERT( ret == 0 );

            ret = mbedtls_ecp_read_key( grp_id, &key2, export1, in_key->len );
            TEST_ASSERT( ret == expected );

            ret = mbedtls_ecp_write_key( &key2, export2, in_key->len );
            TEST_ASSERT( ret == 0 );

            ASSERT_COMPARE( export1, in_key->len,
                            export2, in_key->len );
        }
    }

exit:
    mbedtls_ecp_keypair_free( &key );
    mbedtls_ecp_keypair_free( &key2 );
}

void test_mbedtls_ecp_read_key_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_mbedtls_ecp_read_key( *( (int *) params[0] ), &data1, *( (int *) params[3] ), *( (int *) params[4] ) );
}
#if defined(MBEDTLS_SELF_TEST)
void test_ecp_selftest(  )
{
    TEST_ASSERT( mbedtls_ecp_self_test( 1 ) == 0 );
exit:
    ;
}

void test_ecp_selftest_wrapper( void ** params )
{
    (void)params;

    test_ecp_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_ECP_C */

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

#if defined(MBEDTLS_ECP_C)

        case 0:
            {
                *out_value = MBEDTLS_ECP_DP_BP512R1;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ECP_DP_BP384R1;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ECP_DP_BP256R1;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ECP_DP_SECP521R1;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ECP_DP_SECP384R1;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ECP_DP_SECP256R1;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_ECP_DP_SECP224R1;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_ECP_DP_SECP192R1;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_ECP_DP_CURVE25519;
            }
            break;
        case 9:
            {
                *out_value = MBEDTLS_ERR_ECP_INVALID_KEY;
            }
            break;
        case 10:
            {
                *out_value = MBEDTLS_ECP_DP_SECP224K1;
            }
            break;
        case 11:
            {
                *out_value = MBEDTLS_ECP_PF_UNCOMPRESSED;
            }
            break;
        case 12:
            {
                *out_value = MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;
            }
            break;
        case 13:
            {
                *out_value = MBEDTLS_ECP_PF_COMPRESSED;
            }
            break;
        case 14:
            {
                *out_value = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
            }
            break;
        case 15:
            {
                *out_value = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
            }
            break;
        case 16:
            {
                *out_value = MBEDTLS_ECP_DP_NONE;
            }
            break;
        case 17:
            {
                *out_value = MBEDTLS_ECP_DP_CURVE448;
            }
            break;
        case 18:
            {
                *out_value = INT_MAX;
            }
            break;
        case 19:
            {
                *out_value = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
            }
            break;
        case 20:
            {
                *out_value = MBEDTLS_ECP_DP_SECP192K1;
            }
            break;
        case 21:
            {
                *out_value = MBEDTLS_ECP_DP_SECP256K1;
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

#if defined(MBEDTLS_ECP_C)

        case 0:
            {
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if !defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 13:
            {
#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
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

#if defined(MBEDTLS_ECP_C)
    test_ecp_valid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_ECP_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_ecp_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_ECP_C)
    test_mbedtls_ecp_curve_info_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_ECP_C)
    test_ecp_check_pub_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_ECP_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    test_ecp_test_vect_restart_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_ECP_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    test_ecp_muladd_restart_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_ECP_C)
    test_ecp_test_vect_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_ECP_C)
    test_ecp_test_vec_x_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_ECP_C)
    test_ecp_test_mul_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_ECP_C)
    test_ecp_test_mul_rng_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_ECP_C)
    test_ecp_fast_mod_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_ECP_C)
    test_ecp_write_binary_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_ECP_C)
    test_ecp_read_binary_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_ECP_C)
    test_mbedtls_ecp_tls_read_point_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_ECP_C)
    test_ecp_tls_write_read_point_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_ECP_C)
    test_mbedtls_ecp_tls_read_group_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_ECP_C)
    test_ecp_tls_write_read_group_wrapper,
#else
    NULL,
#endif
/* Function Id: 17 */

#if defined(MBEDTLS_ECP_C)
    test_mbedtls_ecp_check_privkey_wrapper,
#else
    NULL,
#endif
/* Function Id: 18 */

#if defined(MBEDTLS_ECP_C)
    test_mbedtls_ecp_check_pub_priv_wrapper,
#else
    NULL,
#endif
/* Function Id: 19 */

#if defined(MBEDTLS_ECP_C)
    test_mbedtls_ecp_gen_keypair_wrapper,
#else
    NULL,
#endif
/* Function Id: 20 */

#if defined(MBEDTLS_ECP_C)
    test_mbedtls_ecp_gen_key_wrapper,
#else
    NULL,
#endif
/* Function Id: 21 */

#if defined(MBEDTLS_ECP_C)
    test_mbedtls_ecp_read_key_wrapper,
#else
    NULL,
#endif
/* Function Id: 22 */

#if defined(MBEDTLS_ECP_C) && defined(MBEDTLS_SELF_TEST)
    test_ecp_selftest_wrapper,
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
    /* ++ftrace; */
    /* ftrace_install(); */
    mbedtls_test_platform_setup();
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_ecp.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
