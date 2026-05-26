/**
 * \file random.c
 *
 * \brief   This file contains the helper functions to generate random numbers
 *          for the purpose of testing.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/*
 * for arc4random_buf() from <stdlib.h>
 */
#if defined(__NetBSD__)
#define _NETBSD_SOURCE 1
#elif defined(__OpenBSD__)
#define _BSD_SOURCE 1
#endif

#include "test_common.h"
#include <test/macros.h>
#include <test/random.h>
#include <string.h>

#if !defined(MBEDTLS_VERSION_MAJOR) || MBEDTLS_VERSION_MAJOR >= 4
#include <mbedtls/private/entropy.h>
#else
#include <mbedtls/entropy.h>
#endif
#include <alignment.h>

int mbedtls_test_rnd_std_rand(void *rng_state,
                              unsigned char *output,
                              size_t len)
{
#if !defined(__OpenBSD__) && !defined(__NetBSD__)
    size_t i;

    if (rng_state != NULL) {
        rng_state  = NULL;
    }

    for (i = 0; i < len; ++i) {
        output[i] = rand();
    }
#else
    if (rng_state != NULL) {
        rng_state = NULL;
    }

    arc4random_buf(output, len);
#endif /* !OpenBSD && !NetBSD */

    return 0;
}

int mbedtls_test_rnd_zero_rand(void *rng_state,
                               unsigned char *output,
                               size_t len)
{
    if (rng_state != NULL) {
        rng_state  = NULL;
    }

    memset(output, 0, len);

    return 0;
}

int mbedtls_test_rnd_buffer_rand(void *rng_state,
                                 unsigned char *output,
                                 size_t len)
{
    mbedtls_test_rnd_buf_info *info = (mbedtls_test_rnd_buf_info *) rng_state;
    size_t use_len;

    if (rng_state == NULL) {
        return mbedtls_test_rnd_std_rand(NULL, output, len);
    }

    use_len = len;
    if (len > info->length) {
        use_len = info->length;
    }

    if (use_len) {
        memcpy(output, info->buf, use_len);
        info->buf += use_len;
        info->length -= use_len;
    }

    if (len - use_len > 0) {
        if (info->fallback_f_rng != NULL) {
            return info->fallback_f_rng(info->fallback_p_rng,
                                        output + use_len,
                                        len - use_len);
        } else {
            return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
        }
    }

    return 0;
}

int mbedtls_test_rnd_pseudo_rand(void *rng_state,
                                 unsigned char *output,
                                 size_t len)
{
    mbedtls_test_rnd_pseudo_info *info =
        (mbedtls_test_rnd_pseudo_info *) rng_state;
    uint32_t i, *k, sum, delta = 0x9E3779B9;
    unsigned char result[4], *out = output;

    if (rng_state == NULL) {
        return mbedtls_test_rnd_std_rand(NULL, output, len);
    }

    k = info->key;

    while (len > 0) {
        size_t use_len = (len > 4) ? 4 : len;
        sum = 0;

        for (i = 0; i < 32; i++) {
            info->v0 += (((info->v1 << 4) ^ (info->v1 >> 5))
                         + info->v1) ^ (sum + k[sum & 3]);
            sum += delta;
            info->v1 += (((info->v0 << 4) ^ (info->v0 >> 5))
                         + info->v0) ^ (sum + k[(sum>>11) & 3]);
        }

        MBEDTLS_PUT_UINT32_BE(info->v0, result, 0);
        memcpy(out, result, use_len);
        len -= use_len;
        out += 4;
    }

    return 0;
}
