#ifndef COSMOPOLITAN_THIRD_PARTY_HACLSTAR_BLAKE2B_H_
#define COSMOPOLITAN_THIRD_PARTY_HACLSTAR_BLAKE2B_H_
#include "third_party/haclstar/haclstar.h"
COSMOPOLITAN_C_START_

typedef struct Hacl_Hash_Blake2b_params_and_key_s
{
    Hacl_Hash_Blake2b_blake2_params* fst;
    uint8_t* snd;
} Hacl_Hash_Blake2b_params_and_key;

/* https://tools.ietf.org/html/rfc7693#section-2.7 */
static const uint32_t Hacl_Hash_Blake2b_sigmaTable[160U] = {
    0U,  1U,  2U,  3U,  4U,  5U,  6U,  7U,  8U,  9U,  10U, 11U, 12U, 13U, 14U,
    15U, 14U, 10U, 4U,  8U,  9U,  15U, 13U, 6U,  1U,  12U, 0U,  2U,  11U, 7U,
    5U,  3U,  11U, 8U,  12U, 0U,  5U,  2U,  15U, 13U, 10U, 14U, 3U,  6U,  7U,
    1U,  9U,  4U,  7U,  9U,  3U,  1U,  13U, 12U, 11U, 14U, 2U,  6U,  5U,  10U,
    4U,  0U,  15U, 8U,  9U,  0U,  5U,  7U,  2U,  4U,  10U, 15U, 14U, 1U,  11U,
    12U, 6U,  8U,  3U,  13U, 2U,  12U, 6U,  10U, 0U,  11U, 8U,  3U,  4U,  13U,
    7U,  5U,  15U, 14U, 1U,  9U,  12U, 5U,  1U,  15U, 14U, 13U, 4U,  10U, 0U,
    7U,  6U,  3U,  9U,  2U,  8U,  11U, 13U, 11U, 7U,  14U, 12U, 1U,  3U,  9U,
    5U,  0U,  15U, 4U,  8U,  6U,  2U,  10U, 6U,  15U, 14U, 9U,  11U, 3U,  0U,
    8U,  12U, 2U,  13U, 7U,  1U,  4U,  10U, 5U,  10U, 2U,  8U,  4U,  7U,  6U,
    1U,  5U,  15U, 11U, 9U,  14U, 3U,  12U, 13U
};

/* https://tools.ietf.org/html/rfc7693#section-2.6 */
static const uint64_t Hacl_Hash_Blake2b_ivTable_B[8U] = {
    0x6A09E667F3BCC908ULL, 0xBB67AE8584CAA73BULL, 0x3C6EF372FE94F82BULL,
    0xA54FF53A5F1D36F1ULL, 0x510E527FADE682D1ULL, 0x9B05688C2B3E6C1FULL,
    0x1F83D9ABFB41BD6BULL, 0x5BE0CD19137E2179ULL
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_HACLSTAR_BLAKE2B_H_ */
