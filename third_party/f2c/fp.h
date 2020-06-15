#ifndef COSMOPOLITAN_THIRD_PARTY_F2C_FP_H_
#define COSMOPOLITAN_THIRD_PARTY_F2C_FP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#include "libc/math.h"

#define FMAX       40
#define EXPMAXDIGS 8
#define EXPMAX     99999999
/* FMAX = max number of nonzero digits passed to atof() */
/* EXPMAX = 10^EXPMAXDIGS - 1 = largest allowed exponent absolute value */

/* MAXFRACDIGS and MAXINTDIGS are for wrt_F -- bounds (not necessarily
   tight) on the maximum number of digits to the right and left of
 * the decimal point.
 */

/* values that suffice for IEEE double */
#define MAXFRACDIGS 344
#define MAXINTDIGS  DBL_MAX_10_EXP

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_F2C_FP_H_ */
