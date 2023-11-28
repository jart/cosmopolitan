#ifndef COSMOPOLITAN_DSP_CORE_KSS8_H_
#define COSMOPOLITAN_DSP_CORE_KSS8_H_
#include "libc/limits.h"
#include "libc/macros.internal.h"

/**
 * Performs 16-bit scaled rounded saturated madd w/ eight coefficients or fewer.
 *
 *   (Σᵢ₌₀₋₈𝑘ᵢ𝑥ᵢ + 2ᵐ⁻¹)/2ᵐ
 *
 * @note compiler struggles with this
 */
#define KSS8(M, K1, K2, K3, K4, K5, K6, K7, K8, X1, X2, X3, X4, X5, X6, X7, \
             X8)                                                            \
  ({                                                                        \
    short x1, x2, x3, x4, x5, x6, x7, x8;                                   \
    x1 = X1, x2 = X2, x3 = X3, x4 = X4;                                     \
    x5 = X5, x6 = X6, x7 = X7, x8 = X8;                                     \
    x1 = MIN(SHRT_MAX, MAX(SHRT_MIN, x1 * K1));                             \
    x2 = MIN(SHRT_MAX, MAX(SHRT_MIN, x2 * K2));                             \
    x3 = MIN(SHRT_MAX, MAX(SHRT_MIN, x3 * K3));                             \
    x4 = MIN(SHRT_MAX, MAX(SHRT_MIN, x4 * K4));                             \
    x5 = MIN(SHRT_MAX, MAX(SHRT_MIN, x5 * K5));                             \
    x6 = MIN(SHRT_MAX, MAX(SHRT_MIN, x6 * K6));                             \
    x7 = MIN(SHRT_MAX, MAX(SHRT_MIN, x7 * K7));                             \
    x8 = MIN(SHRT_MAX, MAX(SHRT_MIN, x8 * K8));                             \
    x1 = MIN(SHRT_MAX, MAX(SHRT_MIN, x1 + x2));                             \
    x3 = MIN(SHRT_MAX, MAX(SHRT_MIN, x3 + x4));                             \
    x5 = MIN(SHRT_MAX, MAX(SHRT_MIN, x5 + x6));                             \
    x7 = MIN(SHRT_MAX, MAX(SHRT_MIN, x7 + x8));                             \
    x1 = MIN(SHRT_MAX, MAX(SHRT_MIN, x1 + x3));                             \
    x5 = MIN(SHRT_MAX, MAX(SHRT_MIN, x5 + x7));                             \
    x1 = MIN(SHRT_MAX, MAX(SHRT_MIN, x1 + x5));                             \
    if (M) {                                                                \
      x1 += 1 << MAX(0, M - 1);                                             \
      x1 >>= M;                                                             \
    }                                                                       \
    x1;                                                                     \
  })

#endif /* COSMOPOLITAN_DSP_CORE_KSS8_H_ */
