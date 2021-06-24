#ifndef MBEDTLS_SHA1_H_
#define MBEDTLS_SHA1_H_
#include "third_party/mbedtls/config.h"
COSMOPOLITAN_C_START_
/* clang-format off */

/* MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED is deprecated and should not be used. */
#define MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED  -0x0035  /**< SHA-1 hardware accelerator failed */
#define MBEDTLS_ERR_SHA1_BAD_INPUT_DATA   -0x0073  /**< SHA-1 input data was malformed. */

/**
 * \brief          The SHA-1 context structure.
 *
 * \warning        SHA-1 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
typedef struct mbedtls_sha1_context
{
    uint32_t total[2];    /*!< The number of Bytes processed.  */
    uint32_t state[5];    /*!< The intermediate digest state.  */
    uint8_t buffer[64];   /*!< The data block being processed. */
}
mbedtls_sha1_context;

void mbedtls_sha1_init( mbedtls_sha1_context * );
void mbedtls_sha1_free( mbedtls_sha1_context * );
void mbedtls_sha1_clone( mbedtls_sha1_context *, const mbedtls_sha1_context * );
int mbedtls_sha1_starts_ret( mbedtls_sha1_context * );
int mbedtls_sha1_update_ret( mbedtls_sha1_context *, const unsigned char *, size_t );
int mbedtls_sha1_finish_ret( mbedtls_sha1_context *, unsigned char[20] );
int mbedtls_internal_sha1_process( mbedtls_sha1_context *, const unsigned char[64] );
int mbedtls_sha1_ret( const unsigned char *, size_t, unsigned char[20] );
int mbedtls_sha1_self_test( int );

COSMOPOLITAN_C_END_
#endif /* MBEDTLS_SHA1_H_ */
