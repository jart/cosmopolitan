#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_CLD_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_CLD_H_
#include "libc/runtime/symbolic.h"

#define CLD_CONTINUED SYMBOLIC(CLD_CONTINUED)
#define CLD_DUMPED SYMBOLIC(CLD_DUMPED)
#define CLD_EXITED SYMBOLIC(CLD_EXITED)
#define CLD_KILLED SYMBOLIC(CLD_KILLED)
#define CLD_STOPPED SYMBOLIC(CLD_STOPPED)
#define CLD_TRAPPED SYMBOLIC(CLD_TRAPPED)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long CLD_CONTINUED;
hidden extern const long CLD_DUMPED;
hidden extern const long CLD_EXITED;
hidden extern const long CLD_KILLED;
hidden extern const long CLD_STOPPED;
hidden extern const long CLD_TRAPPED;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_CLD_H_ */
