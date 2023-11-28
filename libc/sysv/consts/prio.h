#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_PRIO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_PRIO_H_
COSMOPOLITAN_C_START_

extern const int PRIO_MAX;
extern const int PRIO_MIN;
extern const int PRIO_PGRP;
extern const int PRIO_PROCESS;
extern const int PRIO_USER;

#define PRIO_PROCESS 0
#define PRIO_PGRP    1
#define PRIO_USER    2
#define PRIO_MIN     -20
#define PRIO_MAX     20

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_PRIO_H_ */
