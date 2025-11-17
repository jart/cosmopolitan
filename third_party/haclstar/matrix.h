#ifndef COSMOPOLITAN_THIRD_PARTY_HACLSTAR_MATRIX_H_
#define COSMOPOLITAN_THIRD_PARTY_HACLSTAR_MATRIX_H_
#include "libc/serialize.h"
#include "third_party/haclstar/mask.h"
COSMOPOLITAN_C_START_

static inline void
Hacl_Impl_Matrix_mod_pow2(uint32_t n1, uint32_t n2, uint32_t logq, uint16_t* a)
{
    if (logq < 16U) {
        for (uint32_t i0 = 0U; i0 < n1; i0++) {
            for (uint32_t i = 0U; i < n2; i++) {
                a[i0 * n2 + i] = (uint32_t)a[i0 * n2 + i] & ((1U << logq) - 1U);
            }
        }
        return;
    }
}

static inline void
Hacl_Impl_Matrix_matrix_add(uint32_t n1, uint32_t n2, uint16_t* a, uint16_t* b)
{
    for (uint32_t i0 = 0U; i0 < n1; i0++) {
        for (uint32_t i = 0U; i < n2; i++) {
            a[i0 * n2 + i] =
              (uint32_t)a[i0 * n2 + i] + (uint32_t)b[i0 * n2 + i];
        }
    }
}

static inline void
Hacl_Impl_Matrix_matrix_sub(uint32_t n1, uint32_t n2, uint16_t* a, uint16_t* b)
{
    for (uint32_t i0 = 0U; i0 < n1; i0++) {
        for (uint32_t i = 0U; i < n2; i++) {
            b[i0 * n2 + i] =
              (uint32_t)a[i0 * n2 + i] - (uint32_t)b[i0 * n2 + i];
        }
    }
}

static inline void
Hacl_Impl_Matrix_matrix_mul(uint32_t n1,
                            uint32_t n2,
                            uint32_t n3,
                            uint16_t* a,
                            uint16_t* b,
                            uint16_t* c)
{
    for (uint32_t i0 = 0U; i0 < n1; i0++) {
        for (uint32_t i1 = 0U; i1 < n3; i1++) {
            uint16_t res = 0U;
            for (uint32_t i = 0U; i < n2; i++) {
                uint16_t aij = a[i0 * n2 + i];
                uint16_t bjk = b[i * n3 + i1];
                uint16_t res0 = res;
                res = (uint32_t)res0 + (uint32_t)aij * (uint32_t)bjk;
            }
            c[i0 * n3 + i1] = res;
        }
    }
}

static inline void
Hacl_Impl_Matrix_matrix_mul_s(uint32_t n1,
                              uint32_t n2,
                              uint32_t n3,
                              const uint16_t* a,
                              const uint16_t* b,
                              uint16_t* c)
{
    for (uint32_t i0 = 0U; i0 < n1; i0++) {
        for (uint32_t i1 = 0U; i1 < n3; i1++) {
            uint16_t res = 0U;
            for (uint32_t i = 0U; i < n2; i++) {
                uint16_t aij = a[i0 * n2 + i];
                uint16_t bjk = b[i1 * n2 + i];
                uint16_t res0 = res;
                res = (uint32_t)res0 + (uint32_t)aij * (uint32_t)bjk;
            }
            c[i0 * n3 + i1] = res;
        }
    }
}

static inline uint16_t
Hacl_Impl_Matrix_matrix_eq(uint32_t n1,
                           uint32_t n2,
                           const uint16_t* a,
                           const uint16_t* b)
{
    uint16_t res = 0xFFFFU;
    for (uint32_t i = 0U; i < n1 * n2; i++) {
        uint16_t uu____0 = FStar_UInt16_eq_mask(a[i], b[i]);
        res = (uint32_t)uu____0 & (uint32_t)res;
    }
    uint16_t r = res;
    return r;
}

static inline void
Hacl_Impl_Matrix_matrix_to_lbytes(uint32_t n1,
                                  uint32_t n2,
                                  const uint16_t* m,
                                  uint8_t* res)
{
    for (uint32_t i = 0U; i < n1 * n2; i++) {
        WRITE16LE(res + 2U * i, m[i]);
    }
}

static inline void
Hacl_Impl_Matrix_matrix_from_lbytes(uint32_t n1,
                                    uint32_t n2,
                                    const uint8_t* b,
                                    uint16_t* res)
{
    for (uint32_t i = 0U; i < n1 * n2; i++) {
        uint16_t* os = res;
        uint16_t u = READ16LE(b + 2U * i);
        uint16_t x = u;
        os[i] = x;
    }
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_HACLSTAR_MATRIX_H_ */
