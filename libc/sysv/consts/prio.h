#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_PRIO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_PRIO_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long PRIO_MAX;
extern const long PRIO_MIN;
extern const long PRIO_PGRP;
extern const long PRIO_PROCESS;
extern const long PRIO_USER;
extern const long NZERO;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define PRIO_PROCESS LITERALLY(0)
#define PRIO_PGRP    LITERALLY(1)
#define PRIO_USER    LITERALLY(2)
#define PRIO_MIN     LITERALLY(-20)
#define PRIO_MAX     LITERALLY(20)
#define NZERO        LITERALLY(20)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_PRIO_H_ */
