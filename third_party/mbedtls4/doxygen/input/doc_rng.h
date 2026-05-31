/**
 * \file doc_rng.h
 *
 * \brief Random number generator (RNG) module documentation file.
 */
/*
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/**
 * @addtogroup rng_module Random number generator (RNG) module
 *
 * The Random number generator (RNG) module provides random number
 * generation, see \c mbedtls_ctr_drbg_random().
 *
 * The block-cipher counter-mode based deterministic random
 * bit generator (CTR_DBRG) as specified in NIST SP800-90. It needs an external
 * source of entropy. For these purposes \c mbedtls_entropy_func() can be used.
 * This is an implementation based on a simple entropy accumulator design.
 *
 * Meaning that there seems to be no practical algorithm that can guess
 * the next bit with a probability larger than 1/2 in an output sequence.
 *
 * This module can be used to generate random numbers.
 */
