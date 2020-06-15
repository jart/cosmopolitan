#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_FE_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_FE_H_
#include "libc/runtime/symbolic.h"

#define FE_ALL_EXCEPT SYMBOLIC(FE_ALL_EXCEPT)
#define FE_DIVBYZERO SYMBOLIC(FE_DIVBYZERO)
#define FE_DOWNWARD SYMBOLIC(FE_DOWNWARD)
#define FE_INEXACT SYMBOLIC(FE_INEXACT)
#define FE_INVALID SYMBOLIC(FE_INVALID)
#define FE_OVERFLOW SYMBOLIC(FE_OVERFLOW)
#define FE_TONEAREST SYMBOLIC(FE_TONEAREST)
#define FE_TOWARDZERO SYMBOLIC(FE_TOWARDZERO)
#define FE_UNDERFLOW SYMBOLIC(FE_UNDERFLOW)
#define FE_UPWARD SYMBOLIC(FE_UPWARD)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long FE_ALL_EXCEPT;
hidden extern const long FE_DIVBYZERO;
hidden extern const long FE_DOWNWARD;
hidden extern const long FE_INEXACT;
hidden extern const long FE_INVALID;
hidden extern const long FE_OVERFLOW;
hidden extern const long FE_TONEAREST;
hidden extern const long FE_TOWARDZERO;
hidden extern const long FE_UNDERFLOW;
hidden extern const long FE_UPWARD;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_FE_H_ */
