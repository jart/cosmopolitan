#include "tf-psa-crypto/build_info.h"

#if defined(MBEDTLS_HAVE_TIME)
#include "mbedtls/platform_time.h"
#endif
#include <stddef.h>
#include <stdint.h>

typedef struct fuzzBufferOffset {
    const uint8_t *Data;
    size_t Size;
    size_t Offset;
} fuzzBufferOffset_t;

#if defined(MBEDTLS_HAVE_TIME)
mbedtls_time_t dummy_constant_time(mbedtls_time_t *time);
#endif
void dummy_init(void);

int dummy_send(void *ctx, const unsigned char *buf, size_t len);
int fuzz_recv(void *ctx, unsigned char *buf, size_t len);
int dummy_random(void *p_rng, unsigned char *output, size_t output_len);
int dummy_entropy(void *data, unsigned char *output, size_t len);
int fuzz_recv_timeout(void *ctx, unsigned char *buf, size_t len,
                      uint32_t timeout);

/* Implemented in the fuzz_*.c sources and required by fuzz_onefile.c */
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size);
