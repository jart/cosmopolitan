#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_
#include "libc/runtime/symbolic.h"

#define POLL_ERR SYMBOLIC(POLL_ERR)
#define POLL_HUP SYMBOLIC(POLL_HUP)
#define POLL_IN SYMBOLIC(POLL_IN)
#define POLL_MSG SYMBOLIC(POLL_MSG)
#define POLL_OUT SYMBOLIC(POLL_OUT)
#define POLL_PRI SYMBOLIC(POLL_PRI)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long POLL_ERR;
extern const long POLL_HUP;
extern const long POLL_IN;
extern const long POLL_MSG;
extern const long POLL_OUT;
extern const long POLL_PRI;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_POLL_H_ */
