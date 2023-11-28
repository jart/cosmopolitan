#ifndef COSMOPOLITAN_DSP_CORE_Q_H_
#define COSMOPOLITAN_DSP_CORE_Q_H_
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/math.h"

/**
 * @fileoverview Fixed point arithmetic macros.
 * @see en.wikipedia.org/wiki/Q_(number_format)
 */

#define F2Q(Q, I)  MIN((1 << Q) - 1, roundf((I) * (1.f * ((1 << Q) - 1))))
#define Q2F(Q, I)  ((I) * (1.f / ((1 << Q) - 1)))
#define QRS(Q, X)  (((X) + (1 << (Q - 1))) >> Q)
#define LQRS(Q, X) (((X) + (1L << (Q - 1))) >> Q)

double DifferSumSq(const double[static 6], double, double);
double DifferSumSq8(const double[static 8], double, double);

long GetIntegerCoefficients(long[static 6], const double[static 6], long, long,
                            long);
long GetIntegerCoefficients8(long[static 8], const double[static 8], long, long,
                             long);

#endif /* COSMOPOLITAN_DSP_CORE_Q_H_ */
