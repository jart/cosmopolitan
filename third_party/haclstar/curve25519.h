#ifndef COSMOPOLITAN_THIRD_PARTY_HACLSTAR_CURVE25519_H_
#define COSMOPOLITAN_THIRD_PARTY_HACLSTAR_CURVE25519_H_
#include "third_party/haclstar/haclstar.h"
COSMOPOLITAN_C_START_

static inline void
Hacl_Impl_Curve25519_fadd(uint64_t out[static 5],
                          const uint64_t f1[static 5],
                          const uint64_t f2[static 5])
{
    uint64_t f10 = f1[0U];
    uint64_t f20 = f2[0U];
    uint64_t f11 = f1[1U];
    uint64_t f21 = f2[1U];
    uint64_t f12 = f1[2U];
    uint64_t f22 = f2[2U];
    uint64_t f13 = f1[3U];
    uint64_t f23 = f2[3U];
    uint64_t f14 = f1[4U];
    uint64_t f24 = f2[4U];
    out[0U] = f10 + f20;
    out[1U] = f11 + f21;
    out[2U] = f12 + f22;
    out[3U] = f13 + f23;
    out[4U] = f14 + f24;
}

void
Hacl_Impl_Curve25519_fsub(uint64_t[static 5],
                          const uint64_t[static 5],
                          const uint64_t[static 5]);

void
Hacl_Impl_Curve25519_fmul(uint64_t[static 5],
                          const uint64_t[static 5],
                          const uint64_t[static 5]);

void
Hacl_Impl_Curve25519_fmul2(uint64_t[static 10],
                           const uint64_t[static 10],
                           const uint64_t[static 10]);

void
Hacl_Impl_Curve25519_fmul1(uint64_t[static 5],
                           const uint64_t[static 5],
                           uint64_t);

void
Hacl_Impl_Curve25519_fsqr(uint64_t[static 5], const uint64_t[static 5]);

void
Hacl_Impl_Curve25519_fsqr2(uint64_t[static 10], uint64_t[static 10]);

void
Hacl_Impl_Curve25519_store_felem(uint64_t[static 4], const uint64_t[static 5]);

void
Hacl_Impl_Curve25519_cswap2(uint64_t, uint64_t*, uint64_t*);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_HACLSTAR_CURVE25519_H_ */
