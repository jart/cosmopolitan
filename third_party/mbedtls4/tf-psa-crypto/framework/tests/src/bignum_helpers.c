/**
 * \file bignum_helpers.c
 *
 * \brief   This file contains the prototypes of helper functions for
 *          bignum-related testing.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "test_common.h"

#include <test/bignum_helpers.h>

#if defined(MBEDTLS_BIGNUM_C)

#include <stdlib.h>
#include <string.h>

#if !defined(MBEDTLS_VERSION_MAJOR) || MBEDTLS_VERSION_MAJOR >= 4
#include <mbedtls/private/bignum.h>
#else
#include <mbedtls/bignum.h>
#endif

#include <bignum_core.h>
#include <bignum_mod.h>
#include <bignum_mod_raw.h>

#include <test/helpers.h>
#include <test/macros.h>

int mbedtls_test_read_mpi_core(mbedtls_mpi_uint **pX, size_t *plimbs,
                               const char *input)
{
    /* Sanity check */
    if (*pX != NULL) {
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }

    size_t hex_len = strlen(input);
    size_t byte_len = (hex_len + 1) / 2;
    *plimbs = CHARS_TO_LIMBS(byte_len);

    /* A core bignum is not allowed to be empty. Forbid it as test data,
     * this way static analyzers have a chance of knowing we don't expect
     * the bignum functions to support empty inputs. */
    if (*plimbs == 0) {
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }

    *pX = mbedtls_calloc(*plimbs, sizeof(**pX));
    if (*pX == NULL) {
        return MBEDTLS_ERR_MPI_ALLOC_FAILED;
    }

    unsigned char *byte_start = (unsigned char *) *pX;
    if (byte_len % sizeof(mbedtls_mpi_uint) != 0) {
        byte_start += sizeof(mbedtls_mpi_uint) - byte_len % sizeof(mbedtls_mpi_uint);
    }
    if ((hex_len & 1) != 0) {
        /* mbedtls_test_unhexify wants an even number of hex digits */
        TEST_ASSERT(mbedtls_test_ascii2uc(*input, byte_start) == 0);
        ++byte_start;
        ++input;
        --byte_len;
    }
    TEST_ASSERT(mbedtls_test_unhexify(byte_start,
                                      byte_len,
                                      input,
                                      &byte_len) == 0);

    mbedtls_mpi_core_bigendian_to_host(*pX, *plimbs);
    return 0;

exit:
    mbedtls_free(*pX);
    *pX = NULL;
    return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
}

#if defined(MBEDTLS_ECP_WITH_MPI_UINT)
int mbedtls_test_read_mpi_modulus(mbedtls_mpi_mod_modulus *N,
                                  const char *s,
                                  mbedtls_mpi_mod_rep_selector int_rep)
{
    mbedtls_mpi_uint *p = NULL;
    size_t limbs = 0;
    if (N->limbs != 0) {
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }
    int ret = mbedtls_test_read_mpi_core(&p, &limbs, s);
    if (ret != 0) {
        return ret;
    }

    switch (int_rep) {
        case MBEDTLS_MPI_MOD_REP_MONTGOMERY:
            ret = mbedtls_mpi_mod_modulus_setup(N, p, limbs);
            break;
        case MBEDTLS_MPI_MOD_REP_OPT_RED:
            ret = mbedtls_mpi_mod_optred_modulus_setup(N, p, limbs, NULL);
            break;
        default:
            ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            break;
    }
    if (ret != 0) {
        mbedtls_free(p);
    }
    return ret;
}

void mbedtls_test_mpi_mod_modulus_free_with_limbs(mbedtls_mpi_mod_modulus *N)
{
    mbedtls_free((mbedtls_mpi_uint *) N->p);
    mbedtls_mpi_mod_modulus_free(N);
}
#endif /* MBEDTLS_ECP_WITH_MPI_UINT */

int mbedtls_test_read_mpi(mbedtls_mpi *X, const char *s)
{
    int negative = 0;
    /* Always set the sign bit to -1 if the input has a minus sign, even for 0.
     * This creates an invalid representation, which mbedtls_mpi_read_string()
     * avoids but we want to be able to create that in test data. */
    if (s[0] == '-') {
        ++s;
        negative = 1;
    }
    /* mbedtls_mpi_read_string() currently retains leading zeros.
     * It always allocates at least one limb for the value 0. */
    if (s[0] == 0) {
        mbedtls_mpi_free(X);
        return 0;
    }
    int ret = mbedtls_mpi_read_string(X, 16, s);
    if (ret != 0) {
        return ret;
    }
    if (negative) {
        if (mbedtls_mpi_cmp_int(X, 0) == 0) {
            mbedtls_test_increment_case_uses_negative_0();
        }
        X->s = -1;
    }
    return 0;
}

#endif /* MBEDTLS_BIGNUM_C */
