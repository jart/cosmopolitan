#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_ILL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_ILL_H_
#include "libc/runtime/symbolic.h"

#define ILL_BADSTK SYMBOLIC(ILL_BADSTK)
#define ILL_COPROC SYMBOLIC(ILL_COPROC)
#define ILL_ILLADR SYMBOLIC(ILL_ILLADR)
#define ILL_ILLOPC SYMBOLIC(ILL_ILLOPC)
#define ILL_ILLOPN SYMBOLIC(ILL_ILLOPN)
#define ILL_ILLTRP SYMBOLIC(ILL_ILLTRP)
#define ILL_PRVOPC SYMBOLIC(ILL_PRVOPC)
#define ILL_PRVREG SYMBOLIC(ILL_PRVREG)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long ILL_BADSTK;
hidden extern const long ILL_COPROC;
hidden extern const long ILL_ILLADR;
hidden extern const long ILL_ILLOPC;
hidden extern const long ILL_ILLOPN;
hidden extern const long ILL_ILLTRP;
hidden extern const long ILL_PRVOPC;
hidden extern const long ILL_PRVREG;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_ILL_H_ */
