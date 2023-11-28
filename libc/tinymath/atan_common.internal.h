#ifndef COSMOPOLITAN_LIBC_TINYMATH_ATAN_COMMON_H_
#define COSMOPOLITAN_LIBC_TINYMATH_ATAN_COMMON_H_
#include "libc/tinymath/atan_data.internal.h"
#include "libc/tinymath/estrin_wrap.internal.h"
#include "libc/tinymath/horner.internal.h"
COSMOPOLITAN_C_START_
// clang-format off

/*
 * Double-precision polynomial evaluation function for scalar and vector atan(x)
 * and atan2(y,x).
 *
 * Copyright (c) 2021-2023, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */

#if WANT_VMATH

#define DBL_T float64x2_t
#define P(i) v_f64 (__atan_poly_data.poly[i])

#else

#define DBL_T double
#define P(i) __atan_poly_data.poly[i]

#endif

/* Polynomial used in fast atan(x) and atan2(y,x) implementations
   The order 19 polynomial P approximates (atan(sqrt(x))-sqrt(x))/x^(3/2).  */
static inline DBL_T
eval_poly (DBL_T z, DBL_T az, DBL_T shift)
{
  /* Use split Estrin scheme for P(z^2) with deg(P)=19. Use split instead of
     full scheme to avoid underflow in x^16.  */
  DBL_T z2 = z * z;
  DBL_T x2 = z2 * z2;
  DBL_T x4 = x2 * x2;
  DBL_T x8 = x4 * x4;
  DBL_T y
    = FMA (ESTRIN_11_ (z2, x2, x4, x8, P, 8), x8, ESTRIN_7 (z2, x2, x4, P));

  /* Finalize. y = shift + z + z^3 * P(z^2).  */
  y = FMA (y, z2 * az, az);
  y = y + shift;

  return y;
}

#undef DBL_T
#undef FMA
#undef P

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_ATAN_COMMON_H_ */
