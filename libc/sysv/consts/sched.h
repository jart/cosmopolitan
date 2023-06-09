#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SCHED_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SCHED_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int SCHED_BATCH;
extern const int SCHED_DEADLINE;
extern const int SCHED_FIFO;
extern const int SCHED_IDLE;
extern const int SCHED_OTHER;
extern const int SCHED_RESET_ON_FORK;
extern const int SCHED_RR;

#define SCHED_BATCH         SCHED_BATCH
#define SCHED_DEADLINE      SCHED_DEADLINE
#define SCHED_FIFO          SCHED_FIFO
#define SCHED_IDLE          SCHED_IDLE
#define SCHED_NORMAL        SCHED_OTHER
#define SCHED_OTHER         SCHED_OTHER
#define SCHED_RESET_ON_FORK SCHED_RESET_ON_FORK
#define SCHED_RR            SCHED_RR

#define __tmpcosmo_SCHED_BATCH         -394281437
#define __tmpcosmo_SCHED_DEADLINE      2088435783
#define __tmpcosmo_SCHED_FIFO          -1450130825
#define __tmpcosmo_SCHED_IDLE          -501938008
#define __tmpcosmo_SCHED_NORMAL        -679334447
#define __tmpcosmo_SCHED_OTHER         -1797889952
#define __tmpcosmo_SCHED_RESET_ON_FORK -1017505393
#define __tmpcosmo_SCHED_RR            -1204010265

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SCHED_H_ */
