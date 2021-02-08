#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_OK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_OK_H_
#include "libc/runtime/symbolic.h"

#define F_OK SYMBOLIC(F_OK)
#define R_OK SYMBOLIC(R_OK)
#define W_OK SYMBOLIC(W_OK)
#define X_OK SYMBOLIC(X_OK)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long F_OK;
extern const long R_OK;
extern const long W_OK;
extern const long X_OK;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_OK_H_ */
